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
* sisregistry - restricted client registry session interface implementation
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include <s32mem.h>
#include "sisregistrywritablesession.h"
#include "sisregistryobject.h"
#include "sisregistryclientserver.h"
#include "application.h"
#include "siscontroller.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "cleanuputils.h"
#include "arrayutils.h"
#include "installtypes.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swtypereginfo.h"
#include "ipcutil.h"
#include "sisregistrypackage.h"
#endif

using namespace Swi;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisRegistryWritableSession::CreateTransactionL()
	{
	User::LeaveIfError(SendReceive(ECreateTransaction));
	}

EXPORT_C void RSisRegistryWritableSession::CommitTransactionL()
	{
	User::LeaveIfError(SendReceive(ECommitTransaction));
	}

EXPORT_C void RSisRegistryWritableSession::RollbackTransactionL()
	{
	User::LeaveIfError(SendReceive(ERollbackTransaction));
	}
#endif

void RSisRegistryWritableSession::AddEntryImplL(TInt aMessage,
												const CApplication& aApplication,
												const TDesC8& aController, 
												TInt64 aTransactionID,
												TIpcArgs& aIpcArgs)
	{
	CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	CSisRegistryObject* object = CSisRegistryObject::NewLC();
	object->ProcessApplicationL(aApplication, aController);
	
	RBufWriteStream stream(*tempBuffer);
 	CleanupClosePushL(stream);
 	
	object->ExternalizeL(stream);
	
	CleanupStack::PopAndDestroy(2, object); // stream
	
	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
	TPckgC<TInt64> transactionID(aTransactionID);
	
	aIpcArgs.Set(0, &ptr);
	aIpcArgs.Set(1, &transactionID);
	aIpcArgs.Set(2, &aController);
	
	User::LeaveIfError(SendReceive(aMessage, aIpcArgs));
	
	CleanupStack::PopAndDestroy(2, tempBuffer); // buffer
	}

EXPORT_C void RSisRegistryWritableSession::AddEntryL(const CApplication& aApplication,
													 const TDesC8& aController, 
													 TInt64 aTransactionID)
	{
	TIpcArgs ipcArgs;
	AddEntryImplL(EAddEntry, aApplication, aController, aTransactionID, ipcArgs);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisRegistryWritableSession::AddEntryL(const CApplication& aApplication,
													 const TDesC8& aController,
													 const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray,
													 TInt64 aTransactionID)
	{
	RBuf8 serializedArray;
	serializedArray.CleanupClosePushL();
	SoftwareTypeRegInfoUtils::SerializeArrayL(aSwTypeRegInfoArray, serializedArray);
	TIpcArgs ipcArgs;
	ipcArgs.Set(3, &serializedArray);

	AddEntryImplL(EAddSoftwareTypeEntry, aApplication, aController, aTransactionID, ipcArgs);

	CleanupStack::PopAndDestroy(&serializedArray);
	}

EXPORT_C void RSisRegistryWritableSession::AddEntryL(const Usif::CApplicationRegistrationData& aApparcRegFileData,
                                                     const CSisRegistryPackage& aSisRegistryPackage)
    {
    TIpcArgs ipcArgs;
    
    Usif::TComponentId componentUid = GetComponentIdForPackageL(aSisRegistryPackage.Name(),aSisRegistryPackage.Vendor()); 
    ipcArgs.Set(0, componentUid);

    TInt objectSize = 0;
    objectSize = GetObjectSizeL(&aApparcRegFileData);
    
    HBufC8* appRegData = HBufC8::NewMaxLC(objectSize);
    TPtr8 appRegDataDes = appRegData->Des();

    RDesWriteStream ipcstream(appRegDataDes);
    CleanupClosePushL(ipcstream);
    
    ipcstream << aApparcRegFileData;
    ipcstream.CommitL();
    ipcArgs.Set(1, &appRegDataDes);
    
    User::LeaveIfError(SendReceive(EAddAppRegEntry, ipcArgs));
    CleanupStack::PopAndDestroy(2, appRegData);
    }
#endif

void RSisRegistryWritableSession::UpdateEntryImplL(TInt aMessage,
												   const CApplication& aApplication, 
												   const TDesC8& aController, 
												   TInt64 aTransactionID,
												   TIpcArgs& aIpcArgs)
	{
	CSisRegistryObject* object;
	
	// create a buffer for the final extenalisation
	CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	// check whether it is full or partial upgrade
	if (aApplication.IsPartialUpgrade())
		{
		TUid uid = aApplication.ControllerL().Info().Uid().Uid();
		TPckg<TUid> packageUid(uid);
		// request the copy of the object across IPC
		HBufC8* buffer = SendReceiveBufferLC(EGetEntry, packageUid);

		// create a stream based on the buffer
		RDesReadStream stream(*buffer);
		CleanupClosePushL(stream);
		
		// reassemble the object from the stream
		object = CSisRegistryObject::NewLC(stream);
		CleanupStack::Pop(object);
		
		CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
		CleanupStack::PushL(object);
		// update the entry using the application stuff
		object->UpgradeApplicationL(aApplication, aController);
		}
	else
		{
		// create a new object
		object = CSisRegistryObject::NewLC();
		object->ProcessApplicationL(aApplication, aController);
		}

	// create a stream to extenalise to
	RBufWriteStream stream(*tempBuffer);
 	CleanupClosePushL(stream);

	object->ExternalizeL(stream);
	
	CleanupStack::PopAndDestroy(2, object); // stream
	
	// create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
	// pass the transasction Id
	TPckgC<TInt64> transactionID(aTransactionID);
	
	aIpcArgs.Set(0, &ptr);
	aIpcArgs.Set(1, &transactionID);
	aIpcArgs.Set(2, &aController);
	
	User::LeaveIfError(SendReceive(aMessage, aIpcArgs));

	CleanupStack::PopAndDestroy(2, tempBuffer); // buffer 
	}

EXPORT_C void RSisRegistryWritableSession::UpdateEntryL(const CApplication& aApplication, 
														const TDesC8& aController, 
														TInt64 aTransactionID)
	{
	TIpcArgs ipcArgs;
	UpdateEntryImplL(EUpdateEntry, aApplication, aController, aTransactionID, ipcArgs);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisRegistryWritableSession::UpdateEntryL(const CApplication& aApplication, 
														const TDesC8& aController,
														const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray,
														TInt64 aTransactionID)
	{
	RBuf8 serializedArray;
	serializedArray.CleanupClosePushL();
	SoftwareTypeRegInfoUtils::SerializeArrayL(aSwTypeRegInfoArray, serializedArray);
	TIpcArgs ipcArgs;
	ipcArgs.Set(3, &serializedArray);
	
	UpdateEntryImplL(EUpdateSoftwareTypeEntry, aApplication, aController, aTransactionID, ipcArgs);
	
	CleanupStack::PopAndDestroy(&serializedArray);
	}

EXPORT_C void RSisRegistryWritableSession::UpdateEntryL(const CApplication& aApplication, 
                                                        const Usif::CApplicationRegistrationData& aApparcRegFileData,
                                                        const CSisRegistryPackage& aSisRegistryPackage) 
    {
    TIpcArgs ipcArgs;
    Usif::TComponentId componentUid = 0;  

    TRAPD(err,componentUid = GetComponentIdForPackageL(aSisRegistryPackage.Name(),aSisRegistryPackage.Vendor());)

	if (err != KErrNone && err != KErrNotFound)
        {
        User::Leave(err);
        }
    else if (err == KErrNotFound) // Possible only in PU and SA.
        {
        componentUid = GetComponentIdForUidL(aApplication.ControllerL().Info().Uid().Uid());
        }

    ipcArgs.Set(0, componentUid);
    
    TInt objectSize = 0;
    objectSize = GetObjectSizeL(&aApparcRegFileData);
    HBufC8* appRegData=HBufC8::NewMaxLC(objectSize);
    TPtr8 appRegDataDes = appRegData->Des();

    RDesWriteStream ipcstream(appRegDataDes);
    CleanupClosePushL(ipcstream);
    
    ipcstream << aApparcRegFileData;
    ipcstream.CommitL();
    ipcArgs.Set(1, &appRegDataDes);
    User::LeaveIfError(SendReceive(EUpdateAppRegEntry, ipcArgs));
    CleanupStack::PopAndDestroy(2, appRegData);
    }
#endif

EXPORT_C void RSisRegistryWritableSession::DeleteEntryL(const CSisRegistryPackage& aPackage, TInt64 aTransactionID)
	{
	TPckgC<TUid> packageUid(aPackage.Uid());
	TPckgC<TInt64> transactionID(aTransactionID);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TPckgC<TInt> packageIndex(aPackage.Index());
	User::LeaveIfError(SendReceive(EDeleteEntry, TIpcArgs(&packageUid, &packageIndex, &transactionID)));
#else
	User::LeaveIfError(SendReceive(EDeleteEntry, TIpcArgs(&packageUid, &aPackage.Name(), &aPackage.Vendor(), &transactionID)));
#endif
	}

EXPORT_C void RSisRegistryWritableSession::AddDriveL(const TInt aDrive)
	{
	TPckgC<TInt> packageDrive(aDrive);
	User::LeaveIfError(SendReceive(EAddDrive, TIpcArgs(&packageDrive)));	
	}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisRegistryWritableSession::RemoveDriveL(const TInt aDrive)
	{
	TPckgC<TInt> packageDrive(aDrive);
	User::LeaveIfError(SendReceive(ERemoveDrive, TIpcArgs(&packageDrive)));	
	}

EXPORT_C void RSisRegistryWritableSession::RegenerateCacheL()
	{
	User::LeaveIfError(SendReceive(ERegenerateCache));
	}
#endif

/**
* Returns ETrue if any ROM stub in the filesystem has the package uid specified
* in aPackageId 
* With the addition of SA upgrades to ROM, a package may be both in ROM, upgraded 
* on another drive and this method can be used to check for the presence 
* of an upgraded ROM package.
*
*
* @param aPackageId		Package Id to be searched in the ROM stub files.
*
* @return ETrue if it can find the aPackageId or aFileName in any of the ROM stub SIS.
*         EFalse otherwise
*/

EXPORT_C TBool RSisRegistryWritableSession::PackageExistsInRomL(const TUid& aPackageId)
	{
	TPckgBuf<TBool> present;	
	TPckgC<TUid> tuid(aPackageId);
	TInt returnCode = SendReceive(EPackageExistsInRom, TIpcArgs(&tuid, &present));
	User::LeaveIfError(returnCode);
		
	return present();
	}


/**
* Gets all the eclipsable file's entries from the ROM stub file of a ROM based package.
*
* @param aUid		Package UId to identify the right stub file.
*
* @param aFiles		A pointer array of file names to be populated.
*
*/
EXPORT_C void RSisRegistryWritableSession::GetFilesForRomApplicationL(const TUid& aUid, RPointerArray<HBufC>& aFiles)
	{
	TInt startingFile = 0;
	TPckgBuf<TInt> filesCount;
	TPckgC<TInt> pckgStartingFile(startingFile);
	TStubExtractionMode operationMode = EGetCount;
	
	// Get the total file count.	
	TPckgC<TUid> packageUid(aUid);
	TPckgC<TStubExtractionMode> packageMode(operationMode);
	
	TInt result = SendReceive(EStubFileEntries, TIpcArgs(&packageUid, &packageMode, &filesCount));
	User::LeaveIfError(result);	
	
	TInt totalFilesCount = filesCount();
	operationMode = EGetFiles;	
	
	do
		{	
		HBufC8* buffer = HBufC8::NewLC(KDefaultBufferSize);
	
		TPtr8 pOutput(buffer->Des());
		
		// Fetch the files.	
		TInt result=SendReceive(EStubFileEntries, TIpcArgs(&packageUid, &packageMode, &pckgStartingFile, &pOutput));
		User::LeaveIfError(result);
		
		// create a stream based on the buffer
		RDesReadStream stream(*buffer);
		CleanupClosePushL(stream);
		
		// reassemble the array from the stream
		InternalizePointerArrayL(aFiles, stream);
		
		CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
		startingFile = aFiles.Count();	
		} while (startingFile < totalFilesCount);	
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisRegistryWritableSession::ActivateComponentL(TComponentId aComponentId)
	{
	SetComponentStateL(aComponentId, Usif::EActivated);
	}

EXPORT_C void RSisRegistryWritableSession::DeactivateComponentL(TComponentId aComponentId)
	{
	SetComponentStateL(aComponentId, Usif::EDeactivated);
	}

void RSisRegistryWritableSession::SetComponentStateL(TComponentId aComponentId, TScomoState aState)
	{
	TPckgC<TComponentId> packageComponentId(aComponentId);
	TPckgC<TScomoState> packageState(aState);	
	TInt returnCode = SendReceive(ESetComponentState, TIpcArgs(&packageComponentId, &packageState));
	User::LeaveIfError(returnCode);
	}

EXPORT_C void RSisRegistryWritableSession::SetComponentPresenceL(TComponentId aComponentId, TBool aState)
    {
    TPckgC<TComponentId> componentId(aComponentId);
    TPckgC<TBool> componentPresence(aState);   
    User::LeaveIfError(SendReceive(ESetComponentPresence, TIpcArgs(&componentId, &componentPresence)));
    }
#endif
