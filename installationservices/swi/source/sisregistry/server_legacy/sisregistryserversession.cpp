/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisRegistrySession class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>

#include "log.h"

#include <swi/sistruststatus.h>
#include <scs/ipcstream.h>
#include "swi/siscontroller.h"
#include "sisregistryserversession.h"
#include "sisregistryserver.h"
#include "sisregistrypackage.h"
#include "sisregistryserverconst.h"
#include "sisregistryobject.h"
#include "sisregistrycache.h"
#include "sisregistryfiledescription.h"
#include "sisregistrydependency.h"
#include "controllerinfo.h"

#include "arrayutils.h"

#include "integrityservices.h"
#include "hashcontainer.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"
#include "sisrevocationmanager.h"
#include "siscontroller.h"
#include "siscertificatechain.h"

#include "cleanuputils.h"
#include "sisregistryutil.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "securitymanager.h"
#include "siscontrollerverifier.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "swi/sisregistrylogversion.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <swi/sisregistrylog.h>
#include "sisinstallblock.h"
#include "sisregistryfiledescription.h"
#include "sisregistrywritablesession.h"
  

using namespace Swi;

template<class T> 
void SendDataL(const RMessage2& aMessage, const T& aProvider, TInt aIpcIndx);
template<class T> 
void SendDataArrayL(const RMessage2& aMessage, const RArray<T> aProvider, TInt aIpcIndx);
template<class T> 
void SendDataPointerArrayL(const RMessage2& aMessage, const RPointerArray<T> aProvider, TInt aIpcIndx);
	
/**
 * Function to panic an offending client
 * @internalTechnology
 * @released
 */
 void CSisRegistrySession::PanicClient(const RMessagePtr2& aMessage, Swi::TSisRegistryPanic aPanic)
	{
	aMessage.Panic(KSisRegistryShortName, aPanic);
	}

void CSisRegistrySession::CreateL()
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	Server().AddSession();
	
	iRevocationManager =
      CSisRevocationManager::NewL(*this);
    
	// Just obtain a handle reference to use 
	iFs = Server().Cache().RFsHandle();
	}

CSisRegistrySession::~CSisRegistrySession()
	{
	Server().DropSession();
    
	delete iRevocationManager;
	delete iSisControllerVerifier;
	iControllerArray.ResetAndDestroy();
	}

void CSisRegistrySession::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
// Leaving is handled by CSisRegistry::ServiceError() which reports
// the error code to the client
//
	{
	DEBUG_PRINTF2(_L8("SIS Registry Server - ServiceL (function %d)"), aMessage.Function());
	switch (aMessage.Function())
		{	
	// general registry	
	case EInstalledUids:
		RequestInstalledUidsL(aMessage);
		break;
	case EInstalledPackages:
		RequestInstalledPackagesL(aMessage);
		break;
	case EUidInstalled:
	    UidInstalledL(aMessage);
	    break;
	case EControllerInstalled:
	    ControllerInstalledL(aMessage);
	    break;
	case ESidToPackage:
		RequestSidToPackageL(aMessage);
	    break;
	case ESidToFileName:
		RequestSidToFileNameL(aMessage);
		break;
	case EModifiable:
		RequestModifiableL(aMessage);
		break;
	case EHash:
		RequestHashL(aMessage);
		break;		
	// general entry 	
	case EOpenRegistryUid:
		OpenRegistryUidEntryL(aMessage);
		break;
	case EOpenRegistryPackage:
		OpenRegistryPackageEntryL(aMessage);
		break;
	case EOpenRegistryNames:
		OpenRegistryNamesEntryL(aMessage);
		break;
	case ECloseRegistryEntry:
		CloseRegistryEntryL(aMessage);
		break;
	case EVersion:
	    RequestVersionRegistryEntryL(aMessage);
		break;
	case EPackageName:
		RequestPackageNameRegistryEntryL(aMessage);
		break;
	case EUniqueVendorName:
		RequestUniqueVendorNameRegistryEntryL(aMessage);
		break;	
	case ELocalizedVendorName:
		RequestLocalizedVendorNameRegistryEntryL(aMessage);
		break;
	case EUid:
		RequestUidRegistryEntryL(aMessage);
		break;
	case ESids:
		RequestSidsRegistryEntryL(aMessage);
		break;
	case ELanguage:
		RequestLanguageRegistryEntryL(aMessage);
		break;
	case EUidPresent:
		UidPresentRegistryEntryL(aMessage);
		break;
	case ESigned:
	    SignedRegistryEntryL(aMessage);
		break;
	case EGetTrust:
		RegistryEntryTrustL(aMessage);
		break;	
	case ETrustTimeStamp:
		TrustTimeStampL(aMessage);
		break;	
	case ETrustStatus:
		TrustStatusEntryL(aMessage);
		break; 
	case EIsSidPresent:
		IsSidPresentL(aMessage);
		break;
	case EShutdownAllApps:
		ShutdownAllAppsL(aMessage);
		break;
	case ERevocationStatus:
		RevocationCheckEntryL(aMessage);
		break;	
	case ECancelRevocationStatus:
		CancelRevocationCheckEntryL(aMessage);
		break;			
	case EInRom:
		InRomRegistryEntryL(aMessage);
		break;
	case EAugmentation:
		AugmentationRegistryEntryL(aMessage);
		break;
	case EControllers:
		RequestControllersL(aMessage);
		break;
	case EProperty:
		RequestPropertyRegistryEntryL(aMessage);
		break;
	case EFiles:
		RequestFileNamesRegistryEntryL(aMessage);
		break;
	case EHashEntry:
		RequestHashRegistryEntryL(aMessage);
		break;	
	case EFileDescriptions:
		RequestFileDescriptionsRegistryEntryL(aMessage);
		break;	
	case ESelectedDrive:
		RequestSelectedDriveRegistryEntryL(aMessage);
		break;
	case EInstalledDrives:
		RequestInstalledDrivesRegistryEntryL(aMessage);
		break;			
	case EPreInstalled:
		PreInstalledRegistryEntryL(aMessage);
		break;
	case EInstallType:
		RequestInstallTypeRegistryEntryL(aMessage);
		break;
	case ECertificateChains:
		RequestCertificateChainsRegistryEntryL(aMessage);
		break;
	case EEmbeddedPackages:
	    RequestEmbeddedPackageRegistryEntryL(aMessage);
	    break; 
	case EEmbeddingPackages:
		RequestEmbeddingPackagesRegistryEntryL(aMessage);
	    break; 
	case EPackageAugmentations:
	    RequestPackageAugmentationsRegistryEntryL(aMessage);
	    break;
	case EPackageAugmentationsNumber:
		RequestPackageAugmentationsNumberL(aMessage);
		break;
	case ESize:
		RequestSizeRegistryEntryL(aMessage);
	    break;
	case EPackage:
		RequestPackageRegistryEntryL(aMessage);
	    break;
	case EAddEntry:
		RegisterEntryL(aMessage, ETrue);
		break;
	case EUpdateEntry:
		RegisterEntryL(aMessage, EFalse);
		break;
	case EDeleteEntry:
		DeleteEntryL(aMessage);
		break;
	case EDependencies:
		RequestDependenciesRegistryEntryL(aMessage);
		break; 
	case EDependentPackages:
		RequestDependentPackagesRegistryEntryL(aMessage);
		break;
	case EGetEntry:
		RequestRegistryEntryL(aMessage);
		break;
	case EAddDrive:
		AddDriveL(aMessage);
		break;
	case ERemoveDrive:
		RemoveDriveL(aMessage);
		break;
 	case ERegenerateCache:
 		RegenerateCacheL(aMessage);	
 		break;
	case EDeletablePreInstalled:
		DeletablePreInstalledRegistryEntryL(aMessage);
		break;
	case EVerifyController:
		{
		VerifyControllerL(aMessage);
		break;
		}
	case ERemoveWithLastDependent:
		{
		RemoveWithLastDependentL(aMessage);
		break;
		} 
	case ESetRemoveWithLastDependent:
		{
		SetRemoveWithLastDependentL(aMessage);
		break;
		} 
	case EloggingFile:
 		LoggingFileInfoL(aMessage);
 		break;
	case ENonRemovable:
		IsRemovableL(aMessage);
		break;
	case ERemovablePackages:
		RequestRemovablePackagesL(aMessage);
		break;
	case EPackageExistsInRom:
		PackageExistsInRomL(aMessage);
		break;	
	case EInitRecovery:
		RecoverL(aMessage);
		break;
	case EStubFileEntries:
		RequestStubFileEntriesL(aMessage);
		break;
	case ESignedBySuCert:
 		SignedBySuCertRegistryEntryL(aMessage);
 		break;
	case EGetMatchingSupportedLanguages:
		RequestMatchingSupportedLanguagesL(aMessage);
	    break;
	case ERegistryFiles:
	    RequestInternalRegistryFilesL(aMessage);
        break;
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CSisRegistrySession::VerifyControllerL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L("CSisRegistry::VerifyControllerSignature"));
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<HBufC8> controllers;

	iControllerArray.ResetAndDestroy();
	Server().Cache().GenerateControllersArrayL(object, iControllerArray);
	
	delete iSisControllerVerifier;
	iSisControllerVerifier = CSisControllerVerifier::NewL(aMessage);

	iSisControllerVerifier->VerifyControllerL(iControllerArray); 
	}

void CSisRegistrySession::RequestInstalledUidsL(const RMessage2& aMessage)
	{
	RArray<TUid> uids;
	CleanupClosePushL(uids);
	Server().Cache().UidListL(uids);
	SendDataArrayL(aMessage, uids, EIpcArgument0);
	CleanupStack::PopAndDestroy(&uids);
	}
	
void CSisRegistrySession::RequestInstalledPackagesL(const RMessage2& aMessage)
	{
    RPointerArray<CSisRegistryPackage> packages; 
    CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
    Server().Cache().PackageListL(packages); 
    SendDataPointerArrayL(aMessage, packages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&packages);
	}
	
void CSisRegistrySession::UidInstalledL(const RMessage2& aMessage)
	{
	TBool isRegistered;
	TUid uid; 
	TPckg<TUid> packageUid(uid);
	
	aMessage.ReadL(EIpcArgument0, packageUid);
	
	// check whether is registered
	isRegistered = Server().Cache().IsRegistered(uid); 
	
	DEBUG_CODE_SECTION(
		if (isRegistered)
			{
			DEBUG_PRINTF2(_L8("SIS Registry Server - UID 0x%08x is registered."), uid);
			}
		else
			{
			DEBUG_PRINTF2(_L8("SIS Registry Server - UID 0x%08x is not registered."), uid);
			}
	);
	
	// return the data back
    TPckgC<TBool> packageBool(isRegistered);
	aMessage.WriteL(EIpcArgument1, packageBool);
	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::ControllerInstalledL(const RMessage2& aMessage)
	{
	TBool installed;
	
	TInt srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC8* buffer = HBufC8::NewLC(srcLen);
	TPtr8 dest = buffer->Des();
	
	// read data in the buffer
	aMessage.ReadL(EIpcArgument0, dest, 0);
		
	// lets store all in a stream
	RDesReadStream stream(dest);
    CleanupClosePushL(stream);

	// now create the hash object to look for
	CHashContainer* hash = CHashContainer::NewLC(stream);
  	
  	// is the entry present already?
	installed = Server().Cache().IsRegistered(*hash);
	
	DEBUG_CODE_SECTION(
		if (installed)
			{
			DEBUG_PRINTF(_L8("SIS Registry Server - Controller is installed."));
			}
		else
			{
			DEBUG_PRINTF(_L8("SIS Registry Server - Controller is not installed."));
			}
	);
	
	// return the data back
	TPckgC<TBool> isInstalled(installed);
	aMessage.WriteL(EIpcArgument1, isInstalled);
	
	aMessage.Complete(KErrNone);
	// reclaim memory back
	CleanupStack::PopAndDestroy(3, buffer); // buffer, stream, hash  
	}

void CSisRegistrySession::RegisterEntryL(const RMessage2& aMessage, TBool aNewEntry)
	// register/update new/existing registry entry 
	{
	// get the registry object from client space
	// get the size of the descriptor in client space
	// and allocate sufficient buffer 
	TInt srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC8* buffer = HBufC8::NewLC(srcLen);
	TPtr8 dest = buffer->Des();
	// read data in the buffer
	aMessage.ReadL(EIpcArgument0, dest, 0);
		
	// lets store all in a stream
	RDesReadStream stream(dest);
    CleanupClosePushL(stream);   

	// now create a registry entry from the stream
	CSisRegistryObject* object = CSisRegistryObject::NewL(stream);	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream 
	CleanupStack::PushL(object);
	buffer = NULL;
	
	DEBUG_CODE_SECTION(
		if (aNewEntry)
			{
			DEBUG_PRINTF4(_L("Sis Registry Server - registering entry for object with UID: 0x%08x, Name: %S, Vendor: %S."),
				object->Uid().iUid, &(object->Name()), &(object->Vendor()));
			}
		else
			{
			DEBUG_PRINTF4(_L("Sis Registry Server - updating entry for object with UID: 0x%08x, Name: %S, Vendor: %S."),
				object->Uid().iUid, &(object->Name()), &(object->Vendor()));
			}
	);
	
	TBool isRegistered;
	if(aNewEntry)
		{
		isRegistered = Server().Cache().IsRegistered(*object);
		}
	else
		{
		// We're upgrading an existing entry, if it's not an augmentation upgrade,
		// check for the existance of the base package. Otherwise, check by package
		if (object->InstallType() != Sis::EInstAugmentation)
			{
			isRegistered = Server().Cache().IsRegistered(object->Uid());
			}
		else
			{
			isRegistered = Server().Cache().IsRegistered(*object);
			}
		}

  	// the entry should not be existing - new or
  	// the entry should be existing - upgrade
	if (isRegistered == (!aNewEntry))	
		{			 
		// get this transaction ID from client space, using packaging
		TInt64 transactionID;
		TPckg<TInt64> pkgTransactionID(transactionID);
		aMessage.ReadL(EIpcArgument1, pkgTransactionID);
		
		CIntegrityServices* integrityService = CIntegrityServices::NewLC(transactionID, KIntegrityServicesPath);
		if (!aNewEntry)
			{
			CSisRegistryObject* existingObject = NULL;
			existingObject = Server().Cache().ObjectL(object->Uid(), object->Index());
			CleanupStack::PushL(existingObject);
			
			// DEF085506 fix. remove control files (Previous SA and any of PUs) also while SA upgrading.
			if (object->InstallType() == Sis::EInstInstallation)
				{
				Server().Cache().RemoveRegistryEntryL(*existingObject, *integrityService);	
				}
			else // PartialUpgarde case remove only registry file.
				{
				// Essentially, this is an uninstall except we leave the controller file intact.
				Server().Cache().RemoveRegistryFilesL(*existingObject, *integrityService);	
				}	
								
			CleanupStack::PopAndDestroy(existingObject);
			}
		// get the current controller from client space
		// check the size of the descriptor in client space
		// and allocate accordingly
		TInt srcLenCtrl = aMessage.GetDesLengthL(EIpcArgument2);
		HBufC8* bufferCtrl = HBufC8::NewLC(srcLenCtrl);
		TPtr8 destCtrl = bufferCtrl->Des();

		// read controller content as a binary data
		aMessage.ReadL(EIpcArgument2, destCtrl, 0);
			
		Server().Cache().AddRegistryEntryL(*object, *integrityService, *bufferCtrl);
		
		HBufC* logName = SisRegistryUtil::BuildLogFileNameLC();
 	
 		TDriveUnit sysDrive(RFs::GetSystemDrive());
		TBuf<128> logDir = sysDrive.Name();
		logDir.Append(KLogDir);
 	
 		if( SisRegistryUtil::FileExistsL(iFs,*logName))
 			{
 			TInt sizeOfFile = 0;
 			RFile file;
 			User::LeaveIfError(file.Open(iFs,logDir,EFileRead));
 			CleanupClosePushL(file);
 			User::LeaveIfError(file.Size(sizeOfFile));
 			RBuf8 text;
 			text.CreateL(sizeOfFile);
 			text.CleanupClosePushL();
 			file.Read(text,sizeOfFile);
 			file.Close();
 			integrityService->RemoveL(*logName);
 			User::LeaveIfError(file.Create(iFs,logDir,EFileWrite| EFileShareExclusive |EFileStream));
 			User::LeaveIfError(file.Write(text,sizeOfFile));
 			CleanupStack::PopAndDestroy(2,&file);
 			}
 		integrityService->AddL(*logName);
 		
 		if (!aNewEntry)
 			{
 			Server().Cache().AddLogEntryL(*object,ESwiLogUpgrade);
 			}
 		else
 			{
 			Server().Cache().AddLogEntryL(*object,ESwiLogInstall);
 			}
 
 		CleanupStack::PopAndDestroy(3, integrityService); // integrityService, bufferCtrl, logName		
	
		aMessage.Complete(KErrNone);				
		}  
	else
		{
		DEBUG_CODE_SECTION(
			if (aNewEntry)
				{
				DEBUG_PRINTF(_L8("Sis Registry Server - Error, package already exists!"));
				}
			else
				{
				DEBUG_PRINTF(_L8("Sis Registry Server - Error, package was not found!"));
				}
		);
			
		TInt reason = (aNewEntry)? KErrAlreadyExists:KErrNotFound; 
		aMessage.Complete(reason);				
		}
	
	// need to free up the used memory
	CleanupStack::PopAndDestroy(object); 
	}

void CSisRegistrySession::DeleteEntryL(const RMessage2& aMessage)
// Delete a registry entry keyed by package and vendor names
// uses integrity services and the associated transactionId
	{
	TInt srcLen;
	
	TUid uid;
	TPckg<TUid> packageUid(uid);
	aMessage.ReadL(EIpcArgument0, packageUid, 0);
	
	// Package Name - ipc(1)
	srcLen = aMessage.GetDesLengthL(EIpcArgument1);
	HBufC* packageName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = packageName->Des();
	aMessage.ReadL(EIpcArgument1, namePtr1, 0);	
	
	// Vendor's Name - ipc(2)	
	srcLen = aMessage.GetDesLengthL(EIpcArgument2);
	HBufC* vendorName = HBufC::NewLC(srcLen);
	TPtr namePtr2 = vendorName->Des();
	aMessage.ReadL(EIpcArgument2, namePtr2, 0);
	
	CSisRegistryObject* object = Server().Cache().ObjectL(uid, *packageName, *vendorName);
	CleanupStack::PopAndDestroy(2, packageName); // packageName,vendorName 
	CleanupStack::PushL(object);
	
	DEBUG_PRINTF4(_L("Sis Registry Server - Removing package registry entry for UID: %08x, Name: %S, Vendor %S."),
		object->Uid().iUid, &(object->Name()), &(object->Vendor()));
	
	// transaction ID - ipc(3)
	TInt64 transactionID;
	TPckg<TInt64> pkgTransactionID(transactionID);
	aMessage.ReadL(EIpcArgument3, pkgTransactionID);
	
	// create a integrity service object
    CIntegrityServices* integrityService = CIntegrityServices::NewLC(transactionID, KIntegrityServicesPath);
	Server().Cache().RemoveRegistryEntryL(*object, *integrityService);

	//If removal is for ROM upgrade type, 
	//After removing the existing registry entry set,
	//re generate the Registry Entry Cache. 
	//If any of the ROM based stub doesn't have it's registry set 
	//in appropriate path, it will create them (.reg & . ctl) 
	//from the ROM based stub sis file.	
	if ((object->InstallType() == Sis::EInstInstallation || 
		 object->InstallType() == Sis::EInstPartialUpgrade) &&	
		SisRegistryUtil::RomBasedPackageL(object->Uid()))
		{	
		Server().Cache().RegenerateCacheL();					
		}		
		
	HBufC* logName = SisRegistryUtil::BuildLogFileNameLC();
	
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> logDir = sysDrive.Name();
	logDir.Append(KLogDir);
	
 	if( SisRegistryUtil::FileExistsL(iFs,*logName))
			{
 			TInt sizeOfFile = 0;
 			RFile file;
 			User::LeaveIfError(file.Open(iFs,logDir,EFileRead));
 			CleanupClosePushL(file);
 			User::LeaveIfError(file.Size(sizeOfFile));
 			RBuf8 text;
 			text.CreateL(sizeOfFile);
 			text.CleanupClosePushL();
 			file.Read(text,sizeOfFile);
 			file.Close();
 			integrityService->RemoveL(*logName);
 			User::LeaveIfError(file.Create(iFs,logDir,EFileWrite| EFileShareExclusive |EFileStream));
 			User::LeaveIfError(file.Write(text,sizeOfFile));
 			CleanupStack::PopAndDestroy(2,&file);
 			}			
 	integrityService->AddL(*logName);
 	Server().Cache().AddLogEntryL(*object,ESwiLogUnInstall);	
 	
 	CleanupStack::PopAndDestroy(3, object);// object, integrityService , logName
	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::OpenRegistryUidEntryL(const RMessage2& aMessage)
	{
	// expects a UID as an arg 0
	TUid uid;
	TPckg<TUid> packageUid(uid);
	aMessage.ReadL(EIpcArgument0, packageUid, 0);
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Opening entry subsession for UID: %08x"),
		uid.iUid);
		
	// check uid is present in the list
	if (Server().Cache().IsRegistered(uid))	
		{
		// get the id of the object stored in the cache
	    TUint id;
	    TRAPD(err, Server().Cache().OpenReadHandleL(Server().Cache().PackageL(uid), id));
	    if((err != KErrPathNotFound) && (err != KErrNotFound) && (err != KErrNone) )
	    	{
	    	User::Leave(err);
	    	}
	    // return this handle to identify the subsession
		if((err == KErrPathNotFound) || (err == KErrNotFound))
			{
			err = KErrNotFound;
			DEBUG_PRINTF2(_L8("Sis Registry Server - Could not find UID: %08x."), uid.iUid);
			Server().Cache().RegenerateCacheL();
			}
		else
			{
			TPckgC<TInt> refId(id);
	    	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d."), id);
			aMessage.WriteL(EIpcArgument3, refId);
			}
		aMessage.Complete(err);
		}
	else
		{
		DEBUG_PRINTF(_L8("Sis Registry Server - Failed to open package by UID, does not exist."));
		
		aMessage.Complete(KErrNotFound);		
		}
	}

void CSisRegistrySession::OpenRegistryPackageEntryL(const RMessage2& aMessage)
	{
	// Package Name - ipc(0)
	TInt srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC8* buffer = HBufC8::NewLC(srcLen);
	TPtr8 dest = buffer->Des();
	// read data in the buffer
	aMessage.ReadL(EIpcArgument0, dest, 0);
		
	// lets store all in a stream
	RDesReadStream stream(dest);
    CleanupClosePushL(stream);

	// now create a registry entry
	CSisRegistryPackage* package = CSisRegistryPackage::NewLC(stream);

	DEBUG_PRINTF4(_L("Sis Registry Server - Opening entry subsession for UID: %08x, Name: %S, Vendor: %S"),
		package->Uid().iUid, &(package->Name()), &(package->Vendor()));

	// get the package handle
	TUint id;
	Server().Cache().OpenReadHandleL(*package, id);
	TPckgC<TInt> refId(id);
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d."),
		id);
	
	// return this handle to identify the subsession
	aMessage.WriteL(EIpcArgument3, refId);
	aMessage.Complete(KErrNone);
	
	CleanupStack::PopAndDestroy(3, buffer); // buffer, stream, package
	}
	
void CSisRegistrySession::OpenRegistryNamesEntryL(const RMessage2& aMessage)
	{
	// expects a package name and a vendor name as an arg 0 and 1
	TInt srcLen;
	
	// Package Name - ipc(0)
	srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* packageName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = packageName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);
	
	// Vendor's Name - ipc(1)	
	srcLen = aMessage.GetDesLengthL(EIpcArgument1);
	HBufC* vendorName = HBufC::NewLC(srcLen);
	TPtr namePtr2 = vendorName->Des();
	// read 
	aMessage.ReadL(EIpcArgument1, namePtr2, 0);
	
	DEBUG_PRINTF3(_L("Sis Registry Server - Opening entry subsession by name/vendor. Name: %S, Vendor: %S."),
		packageName, vendorName);
	
	// we cannot use default constructor to create a package
	// so we are going to return pointer/reference to the value 
	// pointer implementation, so it should not be deallocated
	const CSisRegistryPackage& package = Server().Cache().PackageL(*packageName, *vendorName);
	// get the package handle
	TUint id;
	Server().Cache().OpenReadHandleL(package, id);
	TPckgC<TInt> refId(id);
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d"),
		id);
	
	// return this handle to identify the subsession
	aMessage.WriteL(EIpcArgument3, refId);
	aMessage.Complete(KErrNone);
	
	CleanupStack::PopAndDestroy(2, packageName);// packageName, vendorName	
	}

void CSisRegistrySession::CloseRegistryEntryL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Closing open subsession, ID %d."),
		aMessage.Int3());
	
	// the identity of the current entry/subsession is passed in IPC 3!
	// all we have to do is destroy the relevant object 
	Server().Cache().CloseReadHandleL(aMessage.Int3());
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::RequestVersionRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TVersion version;
	version = object.Version();
	TPckg<TVersion> packageVersion(version);
	
	DEBUG_PRINTF5(_L8("Sis Registry Server - Version for subsession ID %d is %d.%d.%d"),
		aMessage.Int3(), version.iMajor, version.iMinor, version.iBuild);
	
	aMessage.WriteL(EIpcArgument0, packageVersion);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestPackageNameRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	DEBUG_PRINTF3(_L("Sis Registry Server - Name for subsession ID %d is '%S'"),
		aMessage.Int3(), &(object.Name()));
			
	aMessage.WriteL(EIpcArgument0, object.Name());	
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestLocalizedVendorNameRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());

	DEBUG_PRINTF3(_L("Sis Registry Server - Vendor for subsession ID %d is '%S'"),
		aMessage.Int3(), &(object.VendorLocalizedName()));

	aMessage.WriteL(EIpcArgument0, object.VendorLocalizedName());
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestUniqueVendorNameRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());

	DEBUG_PRINTF3(_L("Sis Registry Server - Unique Vendor for subsession ID %d is '%S'"),
		aMessage.Int3(), &(object.Vendor()));

	aMessage.WriteL(EIpcArgument0, object.Vendor());	
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestEmbeddedPackageRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());

	RPointerArray<CSisRegistryPackage> embeddedPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddedPackages);
	object.EmbeddedPackagesL(embeddedPackages);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession %d has %d embedded packages."),
		aMessage.Int3(), embeddedPackages.Count());
	
	SendDataPointerArrayL(aMessage, embeddedPackages, EIpcArgument0);

	CleanupStack::PopAndDestroy(&embeddedPackages);
	}

void CSisRegistrySession::RequestHashRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);

	DEBUG_PRINTF3(_L("Sis Registry Server - Subsession ID %d requested hash for file '%S'."),
		aMessage.Int3(), fileName);
	
	SendDataL(aMessage, object.FileDescriptionL(*fileName).Hash(), EIpcArgument1);
	CleanupStack::PopAndDestroy(fileName);	
	}	
	
void CSisRegistrySession::RequestUidRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Subsession ID %d has Package UID 0x%08x."),
		aMessage.Int3(), object.Uid().iUid);
	
	TUid uid = object.Uid();
	TPckg<TUid> packageUid(uid);
	aMessage.WriteL(EIpcArgument0, packageUid);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestLanguageRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Subsession ID %d is localised in language %d."),
		aMessage.Int3(), object.Language());
	
	TLanguage language = object.Language();
	TPckgC<TLanguage> packageLanguage(language);
	aMessage.WriteL(EIpcArgument0, packageLanguage);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::UidPresentRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	// The file object is not updated, but the cache token is
	TBool isPresent = Server().Cache().TokenL(object.Uid(), object.Name(), object.Vendor()).PresentState(); 
	TPckgC<TBool> package(isPresent);
	
	DEBUG_CODE_SECTION(
		if (isPresent)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is completely present."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not completely present."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);		
	}
	
void CSisRegistrySession::SignedRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TBool isSigned = object.IsSigned(); 	
	TPckgC<TBool> package(isSigned);
	
	DEBUG_CODE_SECTION(
		if (isSigned)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is signed."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not signed."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySession::SignedBySuCertRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TBool isSignedBySuCert = object.IsSignedBySuCert(); 	
	TPckgC<TBool> package(isSignedBySuCert);
	
	DEBUG_CODE_SECTION(
		if (isSignedBySuCert)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is signed with RU Cert."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not signed with RU Cert."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySession::RegistryEntryTrustL(const RMessage2& aMessage)
	{
	TUint id = aMessage.Int3();
	CSisRegistryObject& object = Server().Cache().EntryObjectL(id);
	
	TSisPackageTrust trust = object.Trust(); 	
	TPckgC<TSisPackageTrust> package(trust);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package trust status for subsession ID %d is %d."),
		aMessage.Int3(), trust);
	
	aMessage.WriteL(0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySession::TrustTimeStampL(const RMessage2& aMessage)
	{
	TUint id = aMessage.Int3();
	CSisRegistryObject& object = Server().Cache().EntryObjectL(id);
	
	TTime time = object.TrustTimeStamp(); 	
	TPckgC<TTime> package(time);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Last package trust update time for subsession %d was %Ld"),
		aMessage.Int3(), time.Int64());
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySession::TrustStatusEntryL(const RMessage2& aMessage)
    {
	CSisRegistryObject& object = 
	    Server().Cache().EntryObjectL(aMessage.Int3());
	
	TSisTrustStatus status = object.TrustStatus();
	
	DEBUG_PRINTF4(_L8("Sis Registry Server - Package for subsession ID %d has Validation Status: %d, Revocation Status: %d."),
		aMessage.Int3(), status.ValidationStatus(), status.RevocationStatus());
	
	SendDataL(aMessage,status,EIpcArgument0);
    }
    
void CSisRegistrySession::RevocationCheckEntryL(const RMessage2& aMessage)
    {
    TInt    uid       = aMessage.Int3();
    TUint   uriLen    = aMessage.GetDesLengthL(EIpcArgument0);
    HBufC8*  ocspUri   = HBufC8::NewLC(uriLen);
    
    TPtr8    ocspPtr   = ocspUri->Des();	    
    
    aMessage.ReadL(EIpcArgument0, ocspPtr);
    
    CSisRegistryObject& object = 
	    Server().Cache().EntryObjectL(uid);
    
    DEBUG_PRINTF3(_L8("Sis Registry Server - Client has requested a revocation check for package 0x%08x with URI '%S'."),
    	uid, ocspUri);
    
    /*
     * An array which contains the list of controllers. 
     * Note that just the first controller is used here.
     */
    RPointerArray<HBufC8> array;
	CleanupResetAndDestroyPushL(array);
	Server().Cache().GenerateControllersArrayL(object, array);
	
	CDesDataProvider* dataProvider = CDesDataProvider::NewLC(*array[0]);
    
    Sis::CController* controllers = Sis::CController::NewLC(*dataProvider,
                                                            Sis::EAssumeType);	
    
    iRevocationManager->
        RevocationStatusRequestL(
        	array[0], controllers,
                           const_cast<TSisTrustStatus&>(object.TrustStatus()),
                           object.InstallChainIndicies(),
                           ocspPtr,
                           aMessage);

    array.Remove(0); // remove the buffer passed to the revocation manager
    CleanupStack::Pop(controllers); // belongs to revocation manager now
    CleanupStack::PopAndDestroy(3, ocspUri);
	}
	
void CSisRegistrySession::CancelRevocationCheckEntryL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF(_L8("Sis Registry Server - Cancelling revocation check."));
    
	if (iRevocationManager)   	
		{
		iRevocationManager->Cancel();
		}
	aMessage.Complete(KErrNone);	
	}
		
void CSisRegistrySession::InRomRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());	
	
	TBool inRom = object.InRom();
	TPckgC<TBool> package(inRom);
	
	DEBUG_CODE_SECTION(
		if (inRom)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is in ROM."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not in ROM."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::PreInstalledRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());	
		
	TBool preInstalled = object.PreInstalled();	
	TPckgC<TBool> package(preInstalled);
	
	DEBUG_CODE_SECTION(
		if (preInstalled)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is preinstalled."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not preinstalled."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::DeletablePreInstalledRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());	
		
	TBool deletablePreInstalled = object.DeletablePreInstalled();	
	TPckgC<TBool> package(deletablePreInstalled);
	
	DEBUG_CODE_SECTION(
		if (deletablePreInstalled)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is a 'deleteable' presinstalled package."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not a 'deleteable' presinstalled package."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::AugmentationRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());	
		
	TBool aug = (object.InstallType() == Sis::EInstAugmentation);	
	TPckgC<TBool> package(aug);
	
	DEBUG_CODE_SECTION(
		if (aug)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is an augmentation."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d is not an augmentation."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySession::RequestSelectedDriveRegistryEntryL(const RMessage2& aMessage)	
//
// Request the user selected drive
//
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TInt drive = object.SelectedDrive();
	TPckg<TInt> packageDrive(drive);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has selected drive %d"),	
		aMessage.Int3(), drive);
	
	aMessage.WriteL(EIpcArgument0, packageDrive);
	aMessage.Complete(KErrNone);	
	}

/**
* Request the drive bitmask where the files are installed
*/
void CSisRegistrySession::RequestInstalledDrivesRegistryEntryL(const RMessage2& aMessage)	
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TUint drive = object.Drives();
	TPckg<TUint> installedfilesDrive(drive);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has Installed drives bitmask %d"),	
		aMessage.Int3(), drive);
	
	aMessage.WriteL(EIpcArgument0, installedfilesDrive);
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::RequestInstallTypeRegistryEntryL(const RMessage2& aMessage)	
//
// Request the install type
//
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	Sis::TInstallType installType = object.InstallType();
	TPckgC<Sis::TInstallType> packageInstallType(installType);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d is of type %d."),
		aMessage.Int3(), installType);
	
	aMessage.WriteL(EIpcArgument0, packageInstallType);
	aMessage.Complete(KErrNone);	
	}	
	
void CSisRegistrySession::RequestPropertyRegistryEntryL(const RMessage2& aMessage)
//
// Request the value of property, based on specific property key.
// if not found returns to client KErrNotFound.
//
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TInt value = object.Property(aMessage.Int0());

	DEBUG_PRINTF4(_L8("Sis Registry Server - Value of property %d for subsession ID %d is %d"),
		aMessage.Int0(), aMessage.Int3(), value);

	TPckgC<TInt> packageValue(value);
	aMessage.WriteL(EIpcArgument1, packageValue);
	aMessage.Complete(KErrNone);
	}
	
void CSisRegistrySession::RequestSizeRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	TInt64 size = object.SizeL();
	TPckg<TInt64> packageSize(size);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d takes up %Ld bytes."),	
		aMessage.Int3(), size);
	
	aMessage.WriteL(EIpcArgument0, packageSize);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestPackageRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryPackage& package = Server().Cache().EntryObjectL(aMessage.Int3());
	
	DEBUG_PRINTF5(_L("Sis Registry Server - Package for subsession ID %d is UID: 0x%08x, Name: %S, Vendor: %S."),
		aMessage.Int3(), package.Uid().iUid, &(package.Name()), &(package.Vendor()));
	
	SendDataL(aMessage, package, EIpcArgument0);
	}

void CSisRegistrySession::RequestFileNamesRegistryEntryL(const RMessage2& aMessage)
	{
	TStubExtractionMode tMode;
	TPckg<TStubExtractionMode> packageMode(tMode);
	aMessage.ReadL(EIpcArgument0, packageMode, 0);
	
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<CSisRegistryFileDescription>& filesDescription = object.FileDescriptions();
	
	if (tMode == EGetFiles)
		{
		TInt sentFilenamesMemoSize = 0;
		TInt startingFileNo;	
		TPckg<TInt> start(startingFileNo);
		aMessage.ReadL(EIpcArgument1, start, 0);	
		
		RPointerArray<TPtrC> fileNames;
		CleanupResetAndDestroy<RPointerArray<TPtrC> >::PushL(fileNames);

		TInt totalFileDescCount = filesDescription.Count();
		// Populate the files in to a temporary array.
		for(TInt fileCount = startingFileNo; fileCount < totalFileDescCount; ++fileCount )
			{
			sentFilenamesMemoSize += filesDescription[fileCount]->Target().Size();
			// If amount of data red exceeds the client buffer size, break reading.
			if (sentFilenamesMemoSize > KDefaultBufferSize)
				{
				break;
				}
			
			// Only create a TPtrC when we know we have space available
			TPtrC* fileName = new(ELeave) TPtrC(filesDescription[fileCount]->Target());
			CleanupStack::PushL(fileName);
			fileNames.AppendL(fileName);
			CleanupStack::Pop(fileName); 
			}
		// Stream via multiple IPC writes instead of
		// copying to a buffer and streaming from there.
		RIpcWriteStream ipcstream;
		ipcstream.Open(aMessage, EIpcArgument2);
		CleanupClosePushL(ipcstream);
		ExternalizePointerArrayL(fileNames,ipcstream);
		ipcstream.CommitL();
		aMessage.Complete(KErrNone);
		CleanupStack::PopAndDestroy(&ipcstream);
		CleanupStack::PopAndDestroy(&fileNames);
		}
		// If only the count needed, send the stub file's total entries count.
	else if (tMode == EGetCount)
		{
		TPckgBuf<TInt> fileCount(filesDescription.Count());
		
		DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d contains %d file names."),
				aMessage.Int3(), filesDescription.Count());		
		
		aMessage.WriteL(EIpcArgument1, fileCount);
		aMessage.Complete(KErrNone);					
		} 
	else
		{
		// No operation mode specified
		PanicClient(aMessage, EPanicBadOperationMode);
		}
	}

// Gets the total numbr of files in a stub file and all the entries as well.
// The output will be based on the request parameters (operational mode) sent as input.
void CSisRegistrySession::RequestStubFileEntriesL(const RMessage2& aMessage)
	{
	TBool stubNotFound(ETrue);
	
	TUid tUid;	
	TPckg<TUid> packageUid(tUid);
	aMessage.ReadL(EIpcArgument0, packageUid, 0);
	
	TStubExtractionMode tMode;
	TPckg<TStubExtractionMode> packageMode(tMode);
	aMessage.ReadL(EIpcArgument1, packageMode, 0);
	
	// Prepare the stub file path.
	TDriveUnit romDrive(EDriveZ);
	RBuf romRegistryPath;
	romRegistryPath.CreateL(romDrive.Name(), KMaxPath);
	CleanupClosePushL(romRegistryPath);
	romRegistryPath.Append(KPreInstalledPath);	

	RFs tFs;
	CDir* dir;
	
	// Get stub files under the ROM stub directory (Z:\system\install\).
	User::LeaveIfError(tFs.Connect());
	CleanupClosePushL(tFs);
		
	TInt err = tFs.GetDir(romRegistryPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);

	if (err == KErrNone)
		{
		CleanupStack::PushL(dir);	
		TInt count(dir->Count());
		RBuf controllerFileName;
		controllerFileName.CreateL(KMaxFileName);
		CleanupClosePushL(controllerFileName);
		for (TInt index = 0; index < count; ++index)
			{
			controllerFileName = romRegistryPath;
			controllerFileName.Append((*dir)[index].iName);

  			// Read the ROM stub controller
  			CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(tFs, controllerFileName);
  			Swi::Sis::CController* stubController = NULL;
  			TRAPD(errCode, stubController = Swi::Sis::CController::NewL(*fileProvider));
  			if (errCode != KErrNone)
	  			{
	  			// Ignore the broken stub file under the ROM stub directory.
	  			DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to read the stub controller. Error code %d."), errCode);
	  			CleanupStack::PopAndDestroy(fileProvider);
	  			continue;
	  			}
  			CleanupStack::PushL(stubController);

  			// If the UID in a stub file matches the current package's UID,
  			// populate the list of eclipsable files from the same stub file.
  			if ( stubController->Info().Uid().Uid() == tUid )
  				{
  				stubNotFound = EFalse;
  				const RPointerArray<Sis::CFileDescription>& depArray = stubController->InstallBlock().FileDescriptions();
  				// Get as many number of files as possible that can be accomodate in client allocated buffer.
  				if (tMode == EGetFiles)
  					{
  					TInt sizeRed = 0;
  					TInt startingFileNo;	
					TPckg<TInt> start(startingFileNo);
					aMessage.ReadL(EIpcArgument2, start, 0);
  						
  					RPointerArray<TPtrC> fileNames;
  					CleanupResetAndDestroy<RPointerArray<TPtrC> >::PushL(fileNames);

  					TInt totalDepArrayCount = depArray.Count();
  					// Populate the files in to a temporary array.
  					for(TInt fileCount = startingFileNo; fileCount < totalDepArrayCount; ++fileCount )
  						{
  						sizeRed += depArray[fileCount]->Target().Data().Size();
  						// If amount of data red exceeds the client buffer size, break reading.
  						if (sizeRed > KDefaultBufferSize)
  							{
  							break;
  							}
  							
  						// Only create a TPtrC when we know we have space available
  						TPtrC* fileName = new(ELeave) TPtrC(depArray[fileCount]->Target().Data());
  						CleanupStack::PushL(fileName);
  						fileNames.AppendL(fileName);
  						CleanupStack::Pop(fileName);   						
	  					}	  	
  					// Stream via multiple IPC writes instead of
  					// copying to a buffer and streaming from there.
  					RIpcWriteStream ipcstream;
  					ipcstream.Open(aMessage, EIpcArgument3);
  					CleanupClosePushL(ipcstream);
  					ExternalizePointerArrayL(fileNames,ipcstream);
  					ipcstream.CommitL();
  					aMessage.Complete(KErrNone);
  					CleanupStack::PopAndDestroy(&ipcstream);
  					CleanupStack::PopAndDestroy(&fileNames);					
  					}
  				// If only the count needed, send the stub file's total entrie's count.
  				else if (tMode == EGetCount)
  					{
  					TPckgBuf<TInt> fileCount(depArray.Count());
					aMessage.WriteL(EIpcArgument2, fileCount);
					aMessage.Complete(KErrNone);					
  					}  				
	  			CleanupStack::PopAndDestroy(2, fileProvider);
	  			break;	
  				}
  			CleanupStack::PopAndDestroy(2, fileProvider);  		
			}
		CleanupStack::PopAndDestroy(2, dir); // controllerFileName
		// If the stub file itself not found, leave with the same error info'
		if (stubNotFound)
  			{
  			aMessage.Complete(KErrNotFound);
  			}		
		}
	else if(err != KErrPathNotFound)
		{
		aMessage.Complete(err);		
		}
	CleanupStack::PopAndDestroy(2, &romRegistryPath); // tFs
	}

void CSisRegistrySession::RequestFileDescriptionsRegistryEntryL(const RMessage2& aMessage)
	{
	// check that it has the right settings 
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
 
	RPointerArray<CSisRegistryFileDescription>& filesDescription = object.FileDescriptions();
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d contains %d file descriptions."),
		aMessage.Int3(), filesDescription.Count());
	
	// as a reference to the member is returned no deallocation
	SendDataPointerArrayL(aMessage, filesDescription, EIpcArgument0);
	}

void CSisRegistrySession::RequestSidsRegistryEntryL(const RMessage2& aMessage)
	{
	// check that it has the right settings 
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d contains %d SIDs."),
		aMessage.Int3(), object.Sids().Count()); 
	
    SendDataArrayL(aMessage, object.Sids(), EIpcArgument0);
	}
	
void CSisRegistrySession::RequestCertificateChainsRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<HBufC8> chainList;
	CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(chainList);
	
	// only signed entries have certificate chains
	if (object.IsSigned())
		{
		Server().Cache().GenerateChainListL(object, chainList);	 
		}
		
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d is signed with %d certificate chains"),
		aMessage.Int3(), chainList.Count());
		
	SendDataPointerArrayL(aMessage, chainList, EIpcArgument0);
	CleanupStack::PopAndDestroy(&chainList);
	}
	
void CSisRegistrySession::RequestDependenciesRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<CSisRegistryDependency> dependencies;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryDependency> >::PushL(dependencies);
	
	object.DependsOnL(dependencies);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has %d dependencies."),
		aMessage.Int3(), dependencies.Count());
	
	SendDataPointerArrayL(aMessage, dependencies, EIpcArgument0);
	CleanupStack::PopAndDestroy(&dependencies);
	}
	
void CSisRegistrySession::RequestDependentPackagesRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<CSisRegistryPackage> dependents;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(dependents); 
	// allocates memory
	Server().Cache().DependentsPackagesL(object, dependents);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has %d dependencies."),
		aMessage.Int3(), dependents.Count());
	
	// add
    SendDataPointerArrayL(aMessage, dependents, EIpcArgument0);
    // free
    CleanupStack::PopAndDestroy(&dependents);
	}
	

	
void CSisRegistrySession::RequestEmbeddingPackagesRegistryEntryL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<CSisRegistryPackage> embeddingPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddingPackages);
	
	Server().Cache().EmbeddingPackagesL(object, embeddingPackages);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d (and its partial upgrades) was embedded in %d other packages."),
		aMessage.Int3(), embeddingPackages.Count());
    
    SendDataPointerArrayL(aMessage, embeddingPackages, EIpcArgument0);
    
    CleanupStack::PopAndDestroy(&embeddingPackages);
	}

void CSisRegistrySession::RequestControllersL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<HBufC8> controllers;

	CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(controllers);
	Server().Cache().GenerateControllersArrayL(object, controllers);
   
   	DEBUG_PRINTF3(_L8("Sis Registry Server - Package entry for subsession ID %d has %d controllers."),
		aMessage.Int3(), controllers.Count());

	TInt len(0);
   	TPckg<TInt> lenPckg(len);
   	
   	// calculate the total length for the buffer.
   	TInt controllersCount(controllers.Count());
   	TInt total = controllersCount * lenPckg.Length(); // metadata length
   	for (TInt i = 0; i < controllersCount; ++i)
   		{
   		total += controllers[i]->Length();
   		}
   		
   	if (aMessage.GetDesMaxLengthL(EIpcArgument0) < total)
		{
		len = total;
		aMessage.WriteL(EIpcArgument0, lenPckg);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		TInt pos(0);
		for (TInt i = 0; i < controllersCount; ++i)
			{
			len = controllers[i]->Length();
			aMessage.WriteL(EIpcArgument0, lenPckg, pos);
			pos += lenPckg.Length();
			
			aMessage.WriteL(EIpcArgument0, *controllers[i], pos);
			pos += len;
			}
		aMessage.Complete(KErrNone);
		} 
   
	//SendDataPointerArrayL(aMessage, controllers, EIpcArgument0);
	CleanupStack::PopAndDestroy(&controllers);
	}

void CSisRegistrySession::ShutdownAllAppsL(const RMessage2& aMessage)
	{
	TBool shutdownAllApps = EFalse;
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
		
	// check for the shutdown flag in all controllers
	TInt count = object.ControllerInfo().Count();
	TInt i = 0;
	while (!shutdownAllApps && i < count)
		{
		// construct the name using the package and offset
		HBufC* fileName = SisRegistryUtil::BuildControllerFileNameLC(object.Uid(), object.Index(), 
			object.ControllerInfo()[i]->Offset());
		
		// load the controller directly from the file
		// DO NOT TRY TO LOAD THE CONTROLLER FROM A BUFFER BECAUSE 
		// THIS CAN CAUSE SISREGISTRY SERVER TO RUN OUT OF MEMORY.
		CFileSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(iFs, *fileName);
		
		Sis::CController* controller = Sis::CController::NewLC(*dataProvider, Sis::EAssumeType);
		
		shutdownAllApps |= (controller->Info().InstallFlags() & Sis::EInstFlagShutdownApps);
			
		CleanupStack::PopAndDestroy(3, fileName); // controller, dataprovider, fileName
		i++;
		}
	TPckg<TBool> packageShutdownAllApps(shutdownAllApps);
	aMessage.WriteL(EIpcArgument0, packageShutdownAllApps);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestPackageAugmentationsRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	RPointerArray<CSisRegistryPackage> augmentationPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);
	if (object.InstallType() != Sis::EInstAugmentation)
		{
		//augmentationPackages = Server().Cache().PackageAugmentationsL(object.Uid());
		Server().Cache().PackageAugmentationsL(object.Uid(), augmentationPackages);
		}
		
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has %d augmentations."),
		aMessage.Int3(), augmentationPackages.Count());
	
	SendDataPointerArrayL(aMessage, augmentationPackages, EIpcArgument0);
	CleanupStack::PopAndDestroy(&augmentationPackages);
	}
	
void CSisRegistrySession::RequestPackageAugmentationsNumberL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	TInt numAugmentations = 0;
	
	if (object.InstallType() != Sis::EInstAugmentation)
		{
		numAugmentations = Server().Cache().PackageAugmentationsNumber(object.Uid());
		}
	
	TPckgC<TInt> package(numAugmentations);
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);
	}
	
void CSisRegistrySession::RequestRegistryEntryL(const RMessage2& aMessage)
	{
    TUid uid; 
	TPckg<TUid> packageUid(uid);
	
	aMessage.ReadL(EIpcArgument0, packageUid);
    
    // create the entry filename using index 0 (non-augmentation)
    HBufC* name = SisRegistryUtil::BuildEntryFileNameLC(uid, 0); 
    
	RFileReadStream fileStream;
	User::LeaveIfError(fileStream.Open(iFs, *name, EFileRead | EFileShareReadersOnly));
	CleanupStack::PopAndDestroy(name);
	CleanupClosePushL(fileStream);
	
	CSisRegistryObject*	object = CSisRegistryObject::NewLC(fileStream);
	
  	SendDataL(aMessage, *object, EIpcArgument1);
  	
	CleanupStack::PopAndDestroy(2, &fileStream); // fileStream, object
	}

void CSisRegistrySession::AddDriveL(const RMessage2& aMessage)
	{
	TInt drive;
	TPckg<TInt> pkgDrive(drive);
	aMessage.ReadL(EIpcArgument0, pkgDrive);

	DEBUG_PRINTF2(_L8("Sis Registry Server - Removable drive %d added."), drive);

	// update the drives state
	Server().Cache().AddDriveAndRefreshL(drive);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RemoveDriveL(const RMessage2& aMessage)
	{
	TInt drive;
	TPckg<TInt> pkgDrive(drive);
	aMessage.ReadL(EIpcArgument0, pkgDrive);

	DEBUG_PRINTF2(_L8("Sis Registry Server - Removable drive %d removed."), drive);

	// update the drives state
	Server().Cache().RemoveDriveAndRefresh(drive);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestSidToPackageL(const RMessage2& aMessage)
	{
	TUid sid; 
	TPckg<TUid> executableSid(sid);
	
	aMessage.ReadL(EIpcArgument0, executableSid);
	CSisRegistryPackage& package = Server().Cache().SidToPackageL(sid);
	
	DEBUG_PRINTF5(_L("Sis Registry Server - SID 0x%08x is owned by package UID: 0x%08x, Name: %S, Vendor: %S."),
		sid.iUid, package.Uid().iUid, &(package.Name()), &(package.Vendor()));
	
	SendDataL(aMessage, package, EIpcArgument1);
	}

void CSisRegistrySession::RequestSidToFileNameL(const RMessage2& aMessage)
	{
	TBool checkDrive = EFalse;
	TInt thirdParameter; 
	TDriveUnit expectedDrive;
	TUid sid;
	TPckg<TUid> fileSid(sid);
	TInt matchingIndex = 0 ;

	// Obtain the sid from argument slot 0 from the client
	aMessage.ReadL(EIpcArgument0, fileSid);

	// Obtain drive from argument slot 2 from the client.
	// This parameter will be set to -1 if user did not specified a drive with this request.
	thirdParameter = aMessage.Int2(); 
	if(thirdParameter >=0 )
		{
		checkDrive = ETrue;
		expectedDrive = thirdParameter;
		}

	RArray<CSisRegistryPackage> matchingPkgs;
	CleanupClosePushL(matchingPkgs);
	// Get the registry objects matching the sid.
	Server().Cache().SidToPackageL(sid,matchingPkgs);

	CSisRegistryObject* sidObject = NULL;
	if(checkDrive) 
		{
		// Finds sid that matches expected drive.  
		for (TInt index =0; index < matchingPkgs.Count(); index++ )
			{
			CSisRegistryPackage&  singlePkg = matchingPkgs[index];
			sidObject = Server().Cache().ObjectL(singlePkg.Uid(),singlePkg.Name(),singlePkg.Vendor());
			CleanupStack::PushL(sidObject);
			const CSisRegistryFileDescription& fileNameDesc = sidObject->FileDescriptionL(sid);

			TInt drive; 
			User::LeaveIfError(RFs::CharToDrive(fileNameDesc.Target()[0], drive));
			if(drive == expectedDrive )
				{
				break;
				}
			else
				{
				// Destroys object so we continue to next iteration.
				CleanupStack::PopAndDestroy(sidObject);
				}
			}  // End for loop.

		if (!sidObject)
			{
			// No match, something has gone wrong, should not arrive to this point.
			User::Leave(KErrNotFound);
			}
		}

	if (!sidObject ) // Only when no drive was especified, matchingIndex will be zero.
		{
		CSisRegistryPackage& sidPackage =  matchingPkgs[matchingIndex];
		sidObject = Server().Cache().ObjectL(sidPackage.Uid(),sidPackage.Name(),sidPackage.Vendor());
		CleanupStack::PushL(sidObject);
		}

	const CSisRegistryFileDescription& desc = sidObject->FileDescriptionL(sid);

	// Send the filedescription object back to the client in argument slot 1
	DEBUG_PRINTF3(_L("Sis Registry Server - SID 0x%08x maps to file '%S'."),sid.iUid, &(desc.Target()));
	SendDataL(aMessage, desc, EIpcArgument1);		

	CleanupStack::PopAndDestroy(sidObject);
	CleanupStack::PopAndDestroy(&matchingPkgs);
	}

void CSisRegistrySession::RequestModifiableL(const RMessage2& aMessage)
	{
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);
	
	TBool modifiableFile = Server().Cache().ModifiableL(*fileName);

	DEBUG_CODE_SECTION(
		if (modifiableFile)
			{
			DEBUG_PRINTF2(_L("Sis Registry Server - File '%S' is modifiable."),
				fileName);
			}
		else
			{
			DEBUG_PRINTF2(_L("Sis Registry Server - File '%S' is not modifiable."),
				fileName);
			}
	);

	TPckgC<TBool> isModifiable(modifiableFile);
	aMessage.WriteL(EIpcArgument1, isModifiable);
	aMessage.Complete(KErrNone);

	CleanupStack::PopAndDestroy(fileName);	
	}

void CSisRegistrySession::RequestHashL(const RMessage2& aMessage)
	{
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);

	DEBUG_PRINTF2(_L("Sis Registry Server - Client requested the hash of file '%S'."),
		fileName);

	CHashContainer* hashContainer = Server().Cache().HashL(*fileName);
	CleanupStack::PushL(hashContainer);

	SendDataL(aMessage, *hashContainer, EIpcArgument1);

	CleanupStack::PopAndDestroy(hashContainer);
	CleanupStack::PopAndDestroy(fileName);
	}
	
void CSisRegistrySession::ServiceError(const RMessage2& aMessage, TInt aError)
//
// Handle an error from CSisRegistrySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - ServiceL left with code %d."), aError);
	
	if (aError == KErrBadDescriptor)
		{
		PanicClient(aMessage, EPanicBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}

template<class T> 
void SendDataPointerArrayL(const RMessage2& aMessage, const RPointerArray<T> aProvider, TInt aIpcIndx)
	// templated version for a RPointerArray of T objects 
	// aMessage - the message
	// aProvider - the data to be serialised 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	// externalise the pointer array		
	ExternalizePointerArrayL(aProvider, stream);
	stream.CommitL();
	
	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    	
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
        TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
		
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer	
	}
	
template<class T> 
void SendDataArrayL(const RMessage2& aMessage, const RArray<T> aProvider, TInt aIpcIndx)
	// templated version for a an array of objects
	// aMessage - the message
	// aProvider - the data to be serialised 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
		
	// externalise the array of objects		
	ExternalizeArrayL(aProvider, stream);
	stream.CommitL();

	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    	
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
		
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer		
	}	

template<class T> 
void SendDataL(const RMessage2& aMessage, const T& aProvider, TInt aIpcIndx)
	// templated version for a single object 
	// aMessage - the message
	// aProvider - the data to be serialised - object 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);	
	
	// externalise the object		
	aProvider.ExternalizeL(stream);
	stream.CommitL();

	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    	
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer	
	}

void CSisRegistrySession::RegenerateCacheL(const RMessage2& aMessage)
	{
	Server().Cache().RegenerateCacheL();
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::UpdateTrustStatusL(const TUid& uid, const TSisTrustStatus& status )
	{
	Server().Cache().UpdateTrustStatusL(uid, status);
	}

void CSisRegistrySession::IsSidPresentL(const RMessage2& aMessage)
/**
	Handle the EIsSidPresent message by extracting a SID from the
	message and checking whether is in any registered package.
	
	@param	aMessage		Message refers to client-side descriptors
							which contains SID and boolean.
 */
	{
	TPckgBuf<TUid> uid;
	aMessage.ReadL(EIpcArgument0, uid);
	TBool isPresent = Server().Cache().IsSidPresent(uid());
	
	DEBUG_CODE_SECTION(
		if (isPresent)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - SID 0x%08x is present in registry."),
				uid().iUid);
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - SID 0x%08x is not present in registry."),
				uid().iUid);
			}
	);
	
	TPckgBuf<TBool> present(isPresent);
	aMessage.WriteL(EIpcArgument1, present);
	aMessage.Complete(KErrNone);
	}
	
void CSisRegistrySession::PackageExistsInRomL(const RMessage2& aMessage)
/**
	Handle the EPackageExistsInRom message by extracting a pkg uid in the message 
		
	@param	aMessage		Message refers to client-side descriptors
							which contains a pkg uid to be searched on ROM stub files 
							and a boolean which holds the return value (string found or not).
 */
	{
	
    TPckgBuf<TUid> tuid;
	aMessage.ReadL(EIpcArgument0, tuid);
	TBool isInRom = SisRegistryUtil::RomBasedPackageL(tuid());
	
	TPckgBuf<TBool> present(isInRom);
	aMessage.WriteL(EIpcArgument1, present);
	aMessage.Complete(KErrNone);
	}


void CSisRegistrySession::RemoveWithLastDependentL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	TInt isRemoveWithLastDependent = object.RemoveWithLastDependent(); 	
	TPckgC<TInt> package(isRemoveWithLastDependent);
	
	DEBUG_CODE_SECTION(
		if (isRemoveWithLastDependent)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d will be removed by uninstalling the last dependant."),
				aMessage.Int3());
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package for subsession ID %d will not be removed by uninstalling the last dependant."),
				aMessage.Int3());
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}
	
void CSisRegistrySession::SetRemoveWithLastDependentL(const RMessage2& aMessage)
	{
	TPckgBuf<TUid> uid;
	aMessage.ReadL(EIpcArgument0, uid);
	
	Server().Cache().SetRemoveWithLastDependentL(uid());
	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySession::RequestRemovablePackagesL(const RMessage2& aMessage)
    {
    RPointerArray<CSisRegistryPackage> packages; 
    CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
    Server().Cache().RemovablePackageListL(packages);
    SendDataPointerArrayL(aMessage, packages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&packages);
    }
	
void CSisRegistrySession::IsRemovableL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());	
	
	TBool isRemovable = object.IsRemovable();
	TPckgC<TBool> package(isRemovable);
	
	DEBUG_CODE_SECTION(
		if (isRemovable)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package UID 0x%08x is removable."),
				object.Uid().iUid);
			}
		else
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Package UID 0x%08x is non-removable."),
				object.Uid().iUid);
			}
	);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);
	}
	
void CSisRegistrySession::RecoverL(const RMessage2& aMessage)
	{
	Server().Cache().RecoverL();
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::LoggingFileInfoL(const RMessage2& aMessage)
 	{
 	RFile file;
 	
 	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> logDir = sysDrive.Name();
	logDir.Append(KLogDir);
		
 	TInt err = file.Open(iFs,logDir,EFileRead|EFileShareExclusive);
 	if(err == KErrNone)
		{
		CleanupClosePushL(file);
	 	TInt fileSize;
	 	file.Size(fileSize);
	 	
	 	HBufC8* buffer = HBufC8::NewLC(2);
	 	TPtr8 ptr = buffer->Des();
	 	file.Read(ptr,2); 	
	 	RDesReadStream stream(ptr);
	 	CleanupClosePushL(stream);
	 	CLogFileVersion* logVer = CLogFileVersion::NewL(stream);
	 	CleanupStack::PushL(logVer);
	 	
	 	HBufC8* buf = HBufC8::NewLC(fileSize-2);
	 	TPtr8 filePtr = buf->Des();
	 	file.Read(filePtr,fileSize - 2); 
	 	
	 	aMessage.WriteL(EIpcArgument0,filePtr);
	 	CleanupStack::PopAndDestroy(5,&file);	//file,buffer,stream,logVer,buf
	 	aMessage.Complete(KErrNone);
		}
	else 
		{
		aMessage.Complete(KErrNone);
		}		
 	}

void CSisRegistrySession::RequestMatchingSupportedLanguagesL(const RMessage2& aMessage)
	{
	CSisRegistryObject& object = Server().Cache().EntryObjectL(aMessage.Int3());
	
	RArray<TInt>& supportedLanguageIds = object.GetSupportedLanguageIdsArray(); 	
	SendDataArrayL(aMessage, supportedLanguageIds, EIpcArgument0);	
	}

void CSisRegistrySession::RequestInternalRegistryFilesL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF(_L8("Sis Registry Server - Generate the list of registry files"));
    
    TEntry entry;
    TInt err = 0;
    RPointerArray<HBufC> files; 
    CleanupResetAndDestroyPushL(files);
    
    // Get the registry object for the package
    CSisRegistryObject& regObject = Server().Cache().EntryObjectL(aMessage.Int3());
    
    // Form the corresponding registry file name 
    HBufC* regFilename = SisRegistryUtil::BuildEntryFileNameLC(regObject.Uid(), regObject.Index());
    DEBUG_PRINTF3(_L("Sis Registry Server - Reg file not to be removed : %S : %d"), regFilename, regFilename->Length());
    // Check if the registry file is present on the device
    err = iFs.Entry(*regFilename, entry);  
    if (err==KErrNone)
        {
        files.AppendL(regFilename);    
        CleanupStack::Pop(regFilename);
        }
    else
        {
        CleanupStack::PopAndDestroy(regFilename);
        }
        
    const RPointerArray<CControllerInfo>& regControllerInfo = regObject.ControllerInfo();
    TInt ctlCount = regControllerInfo.Count();
    for (TInt j = 0; j < ctlCount; j++)
        {
        // Form the ctl file name for the index 
        HBufC* ctlFilename = SisRegistryUtil::BuildControllerFileNameLC(regObject.Uid(), regObject.Index(), regControllerInfo[j]->Offset());
        DEBUG_PRINTF3(_L("Sis Registry Server - Ctl file not to be removed : %S : %d"), ctlFilename, ctlFilename->Length());
        // Check if the ctl file is present 
        err = iFs.Entry(*ctlFilename, entry);  
        if (err==KErrNone)
            {
            files.AppendL(ctlFilename);                
            CleanupStack::Pop(ctlFilename);
            }
        else
            {
            CleanupStack::PopAndDestroy(ctlFilename);
            }
        }
    
    _LIT(KSysBinPath, "\\sys\\bin\\");    
    _LIT(KSysHashPath, ":\\sys\\hash\\");
    const RPointerArray<CSisRegistryFileDescription>& fileDescriptions = regObject.FileDescriptions();
    TInt fileCount = fileDescriptions.Count();
    for (TInt k = 0; k < fileCount; k++)
        {
        const TDesC& targetPath = fileDescriptions[k]->Target();
        // Check if the target is a binary file
        if (KErrNotFound != targetPath.FindF(KSysBinPath))
            {
            // Create the hash file name for the binary
            TFileName targetHashPath;
            targetHashPath.Append(RFs::GetSystemDriveChar());
            targetHashPath.Append(KSysHashPath);
            targetHashPath.Append(TParsePtrC(targetPath).NameAndExt());
            DEBUG_PRINTF3(_L("Sis Registry Server - Hash file not to be removed : %S : %d"), &targetHashPath, targetHashPath.Length());
            
            // Check if the hash file is present
            err = iFs.Entry(targetHashPath, entry);
            if (err==KErrNone)
                {
                HBufC* hashPtr = targetHashPath.AllocLC();
                files.AppendL(hashPtr);
                CleanupStack::Pop(hashPtr);
                }
            }
        }
        
    SendDataPointerArrayL(aMessage, files, EIpcArgument0);
   
    CleanupStack::PopAndDestroy(&files);
    }
