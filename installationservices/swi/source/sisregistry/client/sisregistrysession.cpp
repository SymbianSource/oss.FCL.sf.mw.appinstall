/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* sisregistry - exported client registry session interface implementation
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#include "log.h"
 
#include<s32mem.h>
#include <hash.h>
#include "sisregistryclientserver.h"
#include "sisregistryfiledescription.h"
#include "sisregistrysession.h"
#include "sisregistrypackage.h"
#include "arrayutils.h"
#include "hashcontainer.h"
#include "dessisdataprovider.h"
#include "siscontroller.h"


using namespace Swi;

TInt StartSisRegistry()
	{
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	RProcess server;
	TInt err = server.Create(KSisRegistryImg, KNullDesC, serverUid);
	if (err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

EXPORT_C TInt RSisRegistrySession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry = 2;
	for (;;) // loop forever, exit relies on internal count and unexpected errors happening
		{
		TInt err = CreateSession(KSisRegistryName, TVersion(1, 0, 0), 2);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry==0)
			{
			return err;
			}
		err = StartSisRegistry();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

EXPORT_C void RSisRegistrySession::InstalledUidsL(RArray<TUid>& aUids)
	{
	HBufC8* buffer = SendReceiveBufferLC(EInstalledUids);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	// reassemble the array from the stream
	InternalizeArrayL(aUids, stream);

	CleanupStack::PopAndDestroy(2, buffer);
	}

EXPORT_C void RSisRegistrySession::InstalledPackagesL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	HBufC8* buffer = SendReceiveBufferLC(EInstalledPackages);

	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	// reassemble the array from the stream
	InternalizePointerArrayL(aPackages, stream);
 
 	CleanupStack::PopAndDestroy(2, buffer);
	}
 
EXPORT_C TBool RSisRegistrySession::IsInstalledL(TUid aUid)
	{
	TPckgC<TUid> uid(aUid);
	TBool installed = EFalse;
	TPckg<TBool> isInstalled(installed);
	User::LeaveIfError(SendReceive(EUidInstalled, TIpcArgs(&uid, &isInstalled)));
	return installed;
	}

EXPORT_C TBool RSisRegistrySession::IsInstalledL(TDesC8& aController)
	{
	TBool installed = EFalse;
	TPckg<TBool> isInstalled(installed);
	CDesDataProvider* desProvider = CDesDataProvider::NewLC(aController);
	Sis::CController* controller = Sis::CController::NewLC(*desProvider, Sis::EAssumeType);
  
 	CMessageDigest* msgDigest = controller->GenerateControllerHashLC(aController);
 	
	TPtrC8 hash = msgDigest->Final();
	CHashContainer* controllerHash = CHashContainer::NewLC(CMessageDigest::ESHA1, hash);
	
	CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	controllerHash->ExternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	
	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
	User::LeaveIfError(SendReceive(EControllerInstalled, TIpcArgs(&ptr, &isInstalled)));

	CleanupStack::PopAndDestroy(6, desProvider);
	return installed;
	}

EXPORT_C TBool RSisRegistrySession::ModifiableL(const TDesC& aFileName)
	{
	TBool modifiable = EFalse;
	TPckg<TBool> isModifiable(modifiable);
	
	User::LeaveIfError(SendReceive(EModifiable, TIpcArgs(&aFileName, &isModifiable)));
	return modifiable;
	}

EXPORT_C CHashContainer* RSisRegistrySession::HashL(const TDesC& aFileName)
	{
	HBufC8* buffer = SendReceiveBufferLC(EHash, aFileName);

	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the hash from the stream
    CHashContainer* hash = CHashContainer::NewLC(stream);
	CleanupStack::Pop(hash);
	
	CleanupStack::PopAndDestroy(2, buffer);
		
	return hash;
	}

EXPORT_C CSisRegistryPackage* RSisRegistrySession::SidToPackageL(TUid aSid)
	{
	TPckgC<TUid> sid(aSid);

	HBufC8* buffer = SendReceiveBufferLC(ESidToPackage, static_cast<TPtrC8>(sid));
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
    CSisRegistryPackage* package = CSisRegistryPackage::NewLC(stream);
	CleanupStack::Pop(package);
	
	CleanupStack::PopAndDestroy(2, buffer);
	return package;
	}

EXPORT_C void RSisRegistrySession::SidToFileNameL(TUid aSid,TDes& aFileName)
	{
	TPckgC<TUid> sid(aSid);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		

	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr(buffer->Des());
	
	User::LeaveIfError(SendReceive(ESidToFileName, TIpcArgs(&sid, &ptr, -1)));
	
	// Get the filename
	aFileName.Copy(*buffer);
	CleanupStack::PopAndDestroy(buffer);
#else
    // -1 is used to indicate no drive check.
	HBufC8* buffer = SendReceiveBufferLC(ESidToFileName, static_cast<TPtrC8>(sid), -1);
	RDesReadStream stream(*buffer);
	stream.PushL();

	// Reassemble the CSisRegistryFileDescription from the stream
	CSisRegistryFileDescription* fileDesc = CSisRegistryFileDescription::NewLC(stream);

	aFileName.Copy(fileDesc->Target());

	CleanupStack::PopAndDestroy(fileDesc);
	CleanupStack::PopAndDestroy(&stream);	
	CleanupStack::PopAndDestroy(buffer);	
 #endif
	}

EXPORT_C void RSisRegistrySession::SidToFileNameL(TUid aSid,TDes& aFileName, TDriveUnit aDrive)
	{
	TPckgC<TUid> sid(aSid);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		

	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr(buffer->Des());
	
	User::LeaveIfError(SendReceive(ESidToFileName, TIpcArgs(&sid, &ptr, aDrive)));
	
	// Get the filename
	aFileName.Copy(*buffer);
	CleanupStack::PopAndDestroy(buffer);
#else	
	HBufC8* buffer = SendReceiveBufferLC(ESidToFileName, static_cast<TPtrC8>(sid), aDrive);

	RDesReadStream stream(*buffer);
	stream.PushL();

	// Reassemble the CSisRegistryFileDescription from the stream
	CSisRegistryFileDescription* fileDesc = CSisRegistryFileDescription::NewLC(stream);

	// Get the filename
	aFileName.Copy(fileDesc->Target());

	CleanupStack::PopAndDestroy(fileDesc);
	CleanupStack::PopAndDestroy(&stream);	
	CleanupStack::PopAndDestroy(buffer);	
#endif
	}


EXPORT_C TBool RSisRegistrySession::IsSidPresentL(TUid aSid)
/**
	Queries whether the supplied SID is associated with
	any package on the device.
	
	@param	aSid			The executable's SID.
	@return					Whether that SID is present in any registered
							package.
 */
	{
	TPckgC<TUid> sid(aSid);
	TPckgBuf<TBool> present;
	
	TInt r = SendReceive(EIsSidPresent, TIpcArgs(&sid, &present));
	User::LeaveIfError(r);
	
	return present();
	}
	
EXPORT_C void RSisRegistrySession::RemovablePackagesL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	HBufC8* buffer = SendReceiveBufferLC(ERemovablePackages);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	// reassemble the array from the stream
	InternalizePointerArrayL(aPackages, stream);
	CleanupStack::PopAndDestroy(2, buffer);
	}
	
EXPORT_C void RSisRegistrySession::RecoverL()
	{
	User::LeaveIfError(SendReceive(EInitRecovery));	
	}

HBufC8* RSisRegistrySession::SendReceiveBufferLC(TInt aMessage) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result = SendReceive(aMessage, TIpcArgs(&pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer after reclaiming memory
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=SendReceive(aMessage, TIpcArgs(&pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}
	
HBufC8* RSisRegistrySession::SendReceiveBufferLC(TInt aMessage, const TDesC& aInputDescriptor) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result = SendReceive(aMessage, TIpcArgs(&aInputDescriptor, &pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded = 0;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=SendReceive(aMessage, TIpcArgs(&aInputDescriptor, &pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}
	
HBufC8* RSisRegistrySession::SendReceiveBufferLC(TInt aMessage, TPtrC8 aInputBuffer) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result=SendReceive(aMessage, TIpcArgs(&aInputBuffer, &pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
 		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=SendReceive(aMessage, TIpcArgs(&aInputBuffer, &pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}
	
EXPORT_C void RSisRegistrySession::RetrieveLogFileL(RPointerArray<CLogEntry>& aLogEntry)
 	{
 	HBufC8* buffer = SendReceiveBufferLC(EloggingFile);
  
 	RDesReadStream stream(*buffer);
 	CleanupClosePushL(stream);
 	TInt err;
 	if(buffer->Length() != 0)
 		{
 		do
	 		{
	 		CLogEntry* log = NULL;
	 		TRAP(err,log = CLogEntry::NewL(stream));
	 		if(err ==KErrEof)
	 			{
				// coverity[memory_leak]
	 			break;
	 			}
	 		else if(err != KErrNone)
				{
				User::Leave(err);	
				}
	 		CleanupStack::PushL(log);
	 		aLogEntry.AppendL(log);
	 		CleanupStack::Pop(log);
	 		}
	 	while(err == KErrNone);
 		}
 	 	
 	CleanupStack::PopAndDestroy(2,buffer);	//buffer,stream
 	}
 
HBufC8* RSisRegistrySession::SendReceiveBufferLC(TInt aMessage, TPtrC8 aInputBuffer, TInt aThirdArgument) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);

	TPtr8 pOutput(output->Des());

	TInt result=SendReceive(aMessage, TIpcArgs(&aInputBuffer, &pOutput, aThirdArgument));

	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);

		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());

		result=SendReceive(aMessage, TIpcArgs(&aInputBuffer, &pResizedOutput, aThirdArgument));
		}
	User::LeaveIfError(result);
	return output;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C TBool RSisRegistrySession::IsFileRegisteredL(const TDesC& aFileName)
	{
	TBool registered = EFalse;
	TPckg<TBool> registeredPckg(registered);
	
	User::LeaveIfError(SendReceive(EIsFileRegistered, TIpcArgs(&aFileName, &registeredPckg)));
	return registered;	
	}
	
EXPORT_C Usif::TComponentId RSisRegistrySession::GetComponentIdForUidL(const TUid& aUid)
	{
	Usif::TComponentId componentId(0);
	TPckg<Usif::TComponentId> componentIdPckg(componentId);	
	User::LeaveIfError(SendReceive(EComponentIdForUid, TIpcArgs(aUid.iUid, &componentIdPckg)));
	return componentId;		
	}

EXPORT_C Usif::TComponentId RSisRegistrySession::GetComponentIdForPackageL(const TDesC& aPackageName, const TDesC& aVendorName) const
    {           
    Usif::TComponentId componentId(0);
    TPckg<Usif::TComponentId> componentIdPckg(componentId);
    User::LeaveIfError(SendReceive(EComponentIdForPackage, TIpcArgs(&aPackageName, &aVendorName, &componentIdPckg)));    
    return componentId;     
    }

EXPORT_C void RSisRegistrySession::GetAppUidsForComponentL(Usif::TComponentId& aCompId, RArray<TUid>& aAppUids)
    {
    TPckgC<Usif::TComponentId> compId(aCompId);    
    HBufC8* buffer = SendReceiveBufferLC(EAppUidsForComponent, static_cast<TPtrC8>(compId));        
    RDesReadStream stream(*buffer);
    CleanupClosePushL(stream);    
    InternalizeArrayL(aAppUids, stream);
    CleanupStack::PopAndDestroy(2, buffer);           
    }

EXPORT_C void RSisRegistrySession::GetComponentIdsForUidL(TUid& aPackageUid, RArray<Usif::TComponentId>& aComponentIds)
    {
    TPckgC<TUid> pkgUid(aPackageUid);    
    HBufC8* buffer = SendReceiveBufferLC(EComponentIdsForPackageUid, static_cast<TPtrC8>(pkgUid));        
    RDesReadStream stream(*buffer);
    CleanupClosePushL(stream);    
    InternalizeArrayL(aComponentIds, stream);
    CleanupStack::PopAndDestroy(2, buffer);           
    }

EXPORT_C void RSisRegistrySession::AddAppRegInfoL(const TDesC& aAppRegFile)
    {
    TInt returnCode = SendReceive(EAddAppRegInfo, TIpcArgs(&aAppRegFile));
    User::LeaveIfError(returnCode);
    }

EXPORT_C void RSisRegistrySession::RemoveAppRegInfoL(const TDesC& aAppRegFile)
    {
    TInt returnCode = SendReceive(ERemoveAppRegInfo, TIpcArgs(&aAppRegFile));
    User::LeaveIfError(returnCode);
    }

#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

