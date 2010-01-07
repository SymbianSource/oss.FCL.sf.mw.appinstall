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
#include <e32property.h>
#include <connect/sbdefs.h>
#include <miscutil.h>
#include <swi/sistruststatus.h>
#include <swi/sisregistrylog.h>
#include <scs/ipcstream.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "sislauncherclient.h"
#include "swtypereginfo.h"
#endif

#include "scrhelperutil.h"
#include "scrdbconstants.h"
#include "log.h"
#include "swi/siscontroller.h"
#include "sisregistryserversession.h"
#include "sisregistryserver.h"
#include "sisregistryserverconst.h"
#include "sisregistryobject.h"
#include "sisregistryfiledescription.h"
#include "sisregistrydependency.h"
#include "controllerinfo.h"
#include "arrayutils.h"
#include "hashcontainer.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"
#include "sisrevocationmanager.h"
#include "siscontroller.h"
#include "siscertificatechain.h"
#include "sisregistryproperty.h"

#include "cleanuputils.h"
#include "sisregistryutil.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "securitymanager.h"
#include "siscontrollerverifier.h"
#include "sisinstallblock.h"
#include "sisregistryfiledescription.h"
#include "sisregistrywritablesession.h"  

using namespace Swi;
using namespace Usif;

const TInt KUidHexLength = 8;
_LIT(KSWIRegFirstInitFile, "\\sys\\install\\sisregistry\\swiregfirstinit.log");

// Function to panic an offending client
void CSisRegistrySession::PanicClient(const RMessagePtr2& aMessage, Swi::TSisRegistryPanic aPanic)
	{
	aMessage.Panic(KSisRegistryShortName, aPanic);
	}

//
// 2nd phase construct for sessions - called by the CServer framework
//
void CSisRegistrySession::CreateL()
	{
	Server().AddSession();

	User::LeaveIfError(iFs.Connect());	
	iSystemDrive = iFs.GetSystemDrive();

	// Create a session with the Software Component Registry
	User::LeaveIfError(iScrSession.Connect());

	TInt res = KErrNone;
	TRAP(res , res = IsFirmwareUpdatedL());
	
	if (IsFirstInvocationL() || res )
		{
		isFwUpdated = ETrue;
		FirstInvocationInitL();
		}
		
    TRAP(res, UpdateRecentFWVersionL(););
    if (res != KErrNone)
        {
        // log that
        DEBUG_PRINTF2(_L8("Updating recent Firmware Version failed with error code = %d."), res);
        }
	}

CSisRegistrySession::~CSisRegistrySession()
	{
	Server().DropSession();
	iOpenedPackages.ResetAndDestroy();	
	iScrSession.Close();
	iFs.Close();	
	}

// Returns ETrue if this is the first run of SWIRegistry server ever (or after firmware upgrade)
TBool CSisRegistrySession::IsFirstInvocationL()
	{
	TBool firstInvocation = ETrue;

	// Check if the flagging file KSWIRegFirstInitFile exists in on the <systemdrive>
	RBuf fileName;
	CleanupClosePushL(fileName);
	fileName.CreateL(KMaxPath);
	TDriveUnit drive(iSystemDrive);
	fileName = drive.Name();
	fileName.Append(KSWIRegFirstInitFile);
	TEntry entry;
	if (iFs.Entry(fileName, entry) == KErrNone)
		{
		firstInvocation = EFalse;
		}
	CleanupStack::PopAndDestroy(&fileName);
	return firstInvocation;
	}

// Does initialization required when run after very first boot of phone (or after firmware upgrade)
// Method is only invoked when such a state is detected
// Leaves behind a file in <systemdrive>:\sys to mark a successful initialization 
void CSisRegistrySession::FirstInvocationInitL()
	{
	// Add the ROM installed stub details to SCR
	// Create an SCR transaction, so that entries won't be added to the SCR if the function leaves
	iScrSession.CreateTransactionL();
	ProcessRomDriveL();
	iScrSession.CommitTransactionL();
	// Create a file in <systemdrive>:\sys to mark a successful initialization
	// so that we don't come here again (unless firmware is upgraded in which case <systemdrive>:\sys 
	// should be cleaned
	RBuf fileName;
	CleanupClosePushL(fileName);
	fileName.CreateL(KMaxPath);
	TDriveUnit drive(iSystemDrive);
	fileName = drive.Name();
	fileName.Append(KSWIRegFirstInitFile);
	RFile file;
	CleanupClosePushL(file);
	file.Replace(iFs, fileName, EFileWrite | EFileShareAny);
	CleanupStack::PopAndDestroy(2, &fileName);	// file
	}

//
// Handle a client request.
// Leaving is handled by CSisRegistry::ServiceError() which reports
// the error code to the client
//
void CSisRegistrySession::ServiceL(const RMessage2& aMessage)
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
		SubsessionL(aMessage.Int3()).RequestVersionRegistryEntryL(aMessage);
		break;
	case EPackageName:
		SubsessionL(aMessage.Int3()).RequestPackageNameRegistryEntryL(aMessage);
		break;
	case EUniqueVendorName:
		SubsessionL(aMessage.Int3()).RequestUniqueVendorNameRegistryEntryL(aMessage);
		break;	
	case ELocalizedVendorName:
		SubsessionL(aMessage.Int3()).RequestLocalizedVendorNameRegistryEntryL(aMessage);
		break;
	case EUid:
		SubsessionL(aMessage.Int3()).RequestUidRegistryEntryL(aMessage);
		break;
	case ESids:
		SubsessionL(aMessage.Int3()).RequestSidsRegistryEntryL(aMessage);
		break;
	case ELanguage:
		SubsessionL(aMessage.Int3()).RequestLanguageRegistryEntryL(aMessage);
		break;
	case EUidPresent:
		SubsessionL(aMessage.Int3()).UidPresentRegistryEntryL(aMessage);
		break;
	case ESigned:
		SubsessionL(aMessage.Int3()).SignedRegistryEntryL(aMessage);
		break;
	case EGetTrust:
		SubsessionL(aMessage.Int3()).RegistryEntryTrustL(aMessage);
		break;	
	case ETrustTimeStamp:
		SubsessionL(aMessage.Int3()).TrustTimeStampL(aMessage);
		break;	
	case ETrustStatus:
		SubsessionL(aMessage.Int3()).TrustStatusEntryL(aMessage);
		break; 
	case EIsSidPresent:
		IsSidPresentL(aMessage);
		break;
	case EShutdownAllApps:
		SubsessionL(aMessage.Int3()).ShutdownAllAppsL(aMessage);
		break;
	case ERevocationStatus:
		SubsessionL(aMessage.Int3()).RevocationCheckEntryL(aMessage);
		break;	
	case ECancelRevocationStatus:
		SubsessionL(aMessage.Int3()).CancelRevocationCheckEntryL(aMessage);
		break;			
	case EInRom:
		SubsessionL(aMessage.Int3()).InRomRegistryEntryL(aMessage);
		break;
	case EAugmentation:
		SubsessionL(aMessage.Int3()).AugmentationRegistryEntryL(aMessage);
		break;
	case EControllers:
		SubsessionL(aMessage.Int3()).RequestControllersL(aMessage);
		break;
	case EProperty:
		SubsessionL(aMessage.Int3()).RequestPropertyRegistryEntryL(aMessage);
		break;
	case EFiles:
		SubsessionL(aMessage.Int3()).RequestFileNamesRegistryEntryL(aMessage);
		break;
	case EHashEntry:
		SubsessionL(aMessage.Int3()).RequestHashRegistryEntryL(aMessage);
		break;	
	case EFileDescriptions:
		SubsessionL(aMessage.Int3()).RequestFileDescriptionsRegistryEntryL(aMessage);
		break;	
	case ESelectedDrive:
		SubsessionL(aMessage.Int3()).RequestSelectedDriveRegistryEntryL(aMessage);
		break;
	case EInstalledDrives:
		SubsessionL(aMessage.Int3()).RequestInstalledDrivesRegistryEntryL(aMessage);
		break;			
	case EPreInstalled:
		SubsessionL(aMessage.Int3()).PreInstalledRegistryEntryL(aMessage);
		break;
	case EInstallType:
		SubsessionL(aMessage.Int3()).RequestInstallTypeRegistryEntryL(aMessage);
		break;
	case ECertificateChains:
		SubsessionL(aMessage.Int3()).RequestCertificateChainsRegistryEntryL(aMessage);
		break;
	case EEmbeddedPackages:
		SubsessionL(aMessage.Int3()).RequestEmbeddedPackageRegistryEntryL(aMessage);
	    break; 
	case EEmbeddingPackages:
		SubsessionL(aMessage.Int3()).RequestEmbeddingPackagesRegistryEntryL(aMessage);
	    break; 
	case EPackageAugmentations:
		SubsessionL(aMessage.Int3()).RequestPackageAugmentationsRegistryEntryL(aMessage);
	    break;
	case EPackageAugmentationsNumber:
		SubsessionL(aMessage.Int3()).RequestPackageAugmentationsNumberL(aMessage);
		break;
	case ESize:
		SubsessionL(aMessage.Int3()).RequestSizeRegistryEntryL(aMessage);
	    break;
	case EPackage:
		SubsessionL(aMessage.Int3()).RequestPackageRegistryEntryL(aMessage);
	    break;    
	case EGetMatchingSupportedLanguages:
		SubsessionL(aMessage.Int3()).RequestMatchingSupportedLanguagesL(aMessage);
	    break;
	case ECreateTransaction:
		CreateTransactionL(aMessage);
		break;
	case ECommitTransaction:
		CommitTransactionL(aMessage);
		break;
	case ERollbackTransaction:
		RollbackTransactionL(aMessage);
		break;
	case EAddEntry:
		RegisterEntryL(aMessage, ETrue, EFalse);
		break;
	case EAddSoftwareTypeEntry:
		RegisterEntryL(aMessage, ETrue, ETrue);
		break;
	case EUpdateEntry:
		RegisterEntryL(aMessage, EFalse, EFalse);
		break;
	case EUpdateSoftwareTypeEntry:
		RegisterEntryL(aMessage, EFalse, ETrue);
		break;
	case EDeleteEntry:
		DeleteEntryL(aMessage);
		break;
	case EDependencies:
		SubsessionL(aMessage.Int3()).RequestDependenciesRegistryEntryL(aMessage);
		break; 
	case EDependentPackages:
		SubsessionL(aMessage.Int3()).RequestDependentPackagesRegistryEntryL(aMessage);
		break;
	case EGetEntry:
		RequestRegistryEntryL(aMessage);
		break;
	case EAddDrive:
		AddDriveL(aMessage);
		break;
	case EDeletablePreInstalled:
		SubsessionL(aMessage.Int3()).DeletablePreInstalledRegistryEntryL(aMessage);
		break;
	case EVerifyController:
		{
		SubsessionL(aMessage.Int3()).VerifyControllerL(aMessage);
		break;
		}
	case ERemoveWithLastDependent:
		{
		SubsessionL(aMessage.Int3()).RemoveWithLastDependentL(aMessage);
		break;
		} 
	case ESetRemoveWithLastDependent:
		{
		SubsessionL(aMessage.Int3()).SetRemoveWithLastDependentL(aMessage);
		break;
		} 
	case EloggingFile:
 		LoggingFileInfoL(aMessage);
 		break;
	case ENonRemovable:
		SubsessionL(aMessage.Int3()).IsRemovableL(aMessage);
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
		SubsessionL(aMessage.Int3()).SignedBySuCertRegistryEntryL(aMessage);
 		break;	
	case ESetComponentState:
 		SetComponentStateL(aMessage);
 		break;	
	case EIsFileRegistered:
 		IsFileRegisteredL(aMessage);
 		break;			
	case EComponentIdForUid:
		GetComponentIdForUidL(aMessage);
		break;
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CSisRegistrySession::RequestInstalledUidsL(const RMessage2& aMessage)
	{
	RArray<TUid> uids;
	CleanupClosePushL(uids);

	// Retrieve all the components from SCR
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	
	ScrHelperUtil::GetComponentIdListL(iScrSession, componentIdList);

	// Retrieve the UIDs for all the components
	TInt compCount = componentIdList.Count();
	for (TInt index = 0; index < compCount; ++index)
		{
		TComponentId compId = componentIdList[index];
		TUid uid = TUid::Uid(GetIntPropertyValueL(iScrSession, compId, KCompUid, ETrue));
		TInt err = uids.InsertInUnsignedKeyOrder(uid);
		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			CleanupStack::PopAndDestroy(&componentIdList);
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&componentIdList);	
	SisRegistryUtil::SendDataArrayL(aMessage, uids, EIpcArgument0);
	CleanupStack::PopAndDestroy(&uids);
	}
	
void CSisRegistrySession::RequestInstalledPackagesL(const RMessage2& aMessage)
	{
    RPointerArray<CSisRegistryPackage> packages; 
    CleanupResetAndDestroyPushL(packages);

	// Retrieve all the installed components from SCR
		
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	ScrHelperUtil::GetComponentIdListL(iScrSession, componentIdList);
				
	// Get the components count and iterate through all of them.
	TInt compCount = componentIdList.Count();
	for (TInt index = 0; index < compCount; ++index)
		{
		// Take the component Id.
		TComponentId compId = componentIdList[index];
		// Get the relevant package from SCR.
		CSisRegistryPackage *package = NULL;
		ScrHelperUtil::ReadFromScrL(iScrSession, compId, package);
		CleanupStack::PushL(package);
		packages.AppendL(package);
		CleanupStack::Pop(package);
		}
	CleanupStack::PopAndDestroy(&componentIdList); 
	SisRegistryUtil::SendDataPointerArrayL(aMessage, packages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&packages);
	}

void CSisRegistrySession::UidInstalledL(const RMessage2& aMessage)
	{
	TUid uid; 
	TPckg<TUid> packageUid(uid);
	aMessage.ReadL(EIpcArgument0, packageUid);
	
	// check whether is registered
	TBool isRegistered = IsRegisteredL(uid);
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
	installed = IsRegisteredL(*hash);
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

void CSisRegistrySession::CreateTransactionL(const RMessage2& aMessage)
	{
	TRAPD(err, iScrSession.CreateTransactionL());
	aMessage.Complete(err);
	}

void CSisRegistrySession::CommitTransactionL(const RMessage2& aMessage)
	{
	TRAPD(err, iScrSession.CommitTransactionL());
	aMessage.Complete(err);
	}

void CSisRegistrySession::RollbackTransactionL(const RMessage2& aMessage)
	{
	TRAPD(err, iScrSession.RollbackTransactionL());
	aMessage.Complete(err);
	}

void CSisRegistrySession::RegisterSoftwareTypesL(TComponentId aComponentId, const RMessage2& aMessage)
	{
	RIpcReadStream readStream;
	readStream.Open(aMessage, 3);
	CleanupClosePushL(readStream);
	
	RCPointerArray<CSoftwareTypeRegInfo> swTypeRegInfoArray;
	CleanupClosePushL(swTypeRegInfoArray);
	SoftwareTypeRegInfoUtils::UnserializeArrayL(readStream, swTypeRegInfoArray);
	
	for (TInt i=0; i<swTypeRegInfoArray.Count(); ++i)
		{
		const CSoftwareTypeRegInfo& info = *swTypeRegInfoArray[i];
		const RPointerArray<CLocalizedSoftwareTypeName>& locSwTypeNames = info.LocalizedSoftwareTypeNames();
		RCPointerArray<Usif::CLocalizedSoftwareTypeName> scrSwTypeNames;
		CleanupClosePushL(scrSwTypeNames);
		for (TInt i=0; i<locSwTypeNames.Count(); ++i)
			{
			scrSwTypeNames.AppendL(Usif::CLocalizedSoftwareTypeName::NewLC(locSwTypeNames[i]->Name(), locSwTypeNames[i]->Locale()));
			CleanupStack::Pop();
			}

		DEBUG_PRINTF2(_L("Sis Registry Server - Adding software type: %S"), &info.UniqueSoftwareTypeName());

		iScrSession.AddSoftwareTypeL(info.UniqueSoftwareTypeName(),
									 info.SifPluginUid(),
									 info.InstallerSecureId(),
									 info.ExecutionLayerSecureId(),
									 info.MimeTypes(),
									 &scrSwTypeNames);
		CleanupStack::PopAndDestroy(&scrSwTypeNames);
		}
	
	RBuf uniqueNames;
	uniqueNames.CleanupClosePushL();
	SoftwareTypeRegInfoUtils::SerializeUniqueSwTypeNamesL(swTypeRegInfoArray, uniqueNames);
	iScrSession.SetComponentPropertyL(aComponentId, KTxtSoftwareTypeComponentProperty, uniqueNames);

	CleanupStack::PopAndDestroy(3, &readStream); // swTypeRegInfoArray, uniqueNames
	}

void CSisRegistrySession::RegisterEntryL(const RMessage2& aMessage, TBool aNewEntry, TBool aRegisterSoftwareTypes)
	{
	// register/update new/existing registry entry 
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
	

	TBool isRegistered(EFalse);
	if(aNewEntry)
		{
		isRegistered = IsRegisteredL(object->Uid(), object->Name());
		}
	else
		{
		// We're upgrading an existing entry, if it's not an augmentation upgrade,
		// check for the existance of the base package. Otherwise, check by augmentation index
		if (object->InstallType() != Sis::EInstAugmentation)
			{
			isRegistered = IsRegisteredL(object->Uid());
			}
		else
			{
			isRegistered = IsRegisteredL(object->Uid(), object->Name());
			}
		}

  	// the entry should not be existing - new or
  	// the entry should be existing - upgrade
	if (isRegistered != aNewEntry)
		{
		// get this transaction ID from client space, using packaging
		TInt64 transactionID;
		TPckg<TInt64> pkgTransactionID(transactionID);
		aMessage.ReadL(EIpcArgument1, pkgTransactionID);
		
		RStsSession stsSession;
		stsSession.OpenTransactionL(transactionID);
		CleanupClosePushL(stsSession);
		
		if (!aNewEntry)
			{
			// Retrieve all the component details from SCR
			TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, object->Uid(), object->Index());

			CSisRegistryObject* existingObject = CSisRegistryObject::NewLC();
			ScrHelperUtil::GetComponentL(iScrSession, compId, *existingObject);
	
			// DEF085506 fix. remove control files (Previous SA and any of PUs) also while SA upgrading.
			if (object->InstallType() == Sis::EInstInstallation)
				{
				RemoveEntryL(*existingObject);
				RemoveCleanupInfrastructureL(*existingObject, stsSession);	
				}
			else // PartialUpgarde case remove only registry file.
				{
				// Essentially, this is an uninstall except we leave the controller file intact.
				RemoveEntryL(*existingObject);
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
		Usif::TScrComponentOperationType opType = aNewEntry ? Usif::EScrCompInstall : Usif::EScrCompUpgrade;
		const TComponentId compId = AddRegistryEntryL(*object, stsSession, *bufferCtrl, opType);
		
		// Register software types
		if (aRegisterSoftwareTypes)
			{
			RegisterSoftwareTypesL(compId, aMessage);
			}
		
 		CleanupStack::PopAndDestroy(2, &stsSession); // bufferCtrl
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

void CSisRegistrySession::UnregisterSoftwareTypesL(TComponentId aComponentId)
	{
	CLocalizablePropertyEntry* property = static_cast<CLocalizablePropertyEntry*>(iScrSession.GetComponentPropertyL(aComponentId, KTxtSoftwareTypeComponentProperty));
	if (property == NULL)
		{
		return;
		}
	CleanupStack::PushL(property);
	
	RArray<TPtrC> swTypeNames;
	CleanupClosePushL(swTypeNames);
	SoftwareTypeRegInfoUtils::UnserializeUniqueSwTypeNamesL(property->StrValue(), swTypeNames);
	
	RCPointerArray<HBufC8> deletedMimeTypes;
	CleanupClosePushL(deletedMimeTypes);

	for (TInt i=0; i<swTypeNames.Count(); ++i)
		{
		RCPointerArray<HBufC> deletedMimeTypes16;
		CleanupClosePushL(deletedMimeTypes16);
		iScrSession.DeleteSoftwareTypeL(swTypeNames[i], deletedMimeTypes16);
		
		for (TInt m=0; m<deletedMimeTypes16.Count(); ++m)
			{
			const TDesC& mimeType16 = *deletedMimeTypes16[m];
			HBufC8* mimeType = HBufC8::NewLC(mimeType16.Length());
			mimeType->Des().Copy(mimeType16);
			deletedMimeTypes.AppendL(mimeType);
			CleanupStack::Pop(mimeType);
			}
		
		CleanupStack::PopAndDestroy(&deletedMimeTypes16);
		}
	
	// Use SISLauncher to unregister MIME types for SIFLauncher
	RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
	TInt err = launcher.Connect();
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Failed to connect to the SISLauncher, err = %d\n"), err);
		User::Leave(err);
		}
	launcher.UnregisterSifLauncherMimeTypesL(deletedMimeTypes);
	
	CleanupStack::PopAndDestroy(4, property); // swTypeNames, deletedMimeTypes, launcher
	}

// Delete a registry entry keyed by package and vendor names
// uses integrity services and the associated transactionId
void CSisRegistrySession::DeleteEntryL(const RMessage2& aMessage)
	{

	TUid uid;
	TPckg<TUid> packageUid(uid);
	aMessage.ReadL(EIpcArgument0, packageUid, 0);

	TInt pkgIndex;
	TPckg<TInt> packageIndex(pkgIndex);
	aMessage.ReadL(EIpcArgument1, packageIndex);
	
	Usif::TComponentId componentId = ScrHelperUtil::GetComponentIdL(iScrSession, uid, pkgIndex);
	
	// If the component being removed has registered software types, unregister them now.
	// This operation deletes MIME types mapping from AppArc and therefore is not transactional.
	UnregisterSoftwareTypesL(componentId);

	CSisRegistryObject* object = CSisRegistryObject::NewLC();
	ScrHelperUtil::GetComponentL(iScrSession, componentId, *object);

	DEBUG_PRINTF4(_L("Sis Registry Server - Removing package registry entry for UID: %08x, Name: %S, Vendor %S."),
		object->Uid().iUid, &(object->Name()), &(object->Vendor()));
	
	// transaction ID - ipc(2)
	TInt64 transactionID;
	TPckg<TInt64> pkgTransactionID(transactionID);
	aMessage.ReadL(EIpcArgument2, pkgTransactionID);
	
	// create a integrity service object
	Usif::RStsSession stssession;
	stssession.OpenTransactionL(transactionID);
	CleanupClosePushL(stssession);
	
	RemoveEntryL(*object);
	RemoveCleanupInfrastructureL(*object, stssession);

	// If removal is for ROM upgrade type, after removing the existing registry entry set,
	// regenerate the Registry Entry Cache. 
	// If any of the ROM based stub doesn't have its registry set 
	// in appropriate path, it will create them (SCR entry & . ctl) 
	// from the ROM based stub sis file.	
	if ((object->InstallType() == Sis::EInstInstallation || 
		 object->InstallType() == Sis::EInstPartialUpgrade) &&	
		SisRegistryUtil::RomBasedPackageL(object->Uid()))
		{	
		// Re-add the ROM installed stub details to SCR (only those missing will be added)
		ProcessRomDriveL();
		}
 	CleanupStack::PopAndDestroy(2, object);// STS 
	
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

	TUint subsessionId = 0;
	TRAPD(err, subsessionId = CreateSubsessionHandleL(uid));
	if (err != KErrNone)
		{
		DEBUG_PRINTF3(_L8("Sis Registry Server - Error (%d) creating subsession handle for package uid: %08x"), err, uid.iUid);
		aMessage.Complete(err);
		return;
		}

	TPckgC<TInt> refId(subsessionId);
	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d."), subsessionId);
	aMessage.WriteL(EIpcArgument3, refId);
	aMessage.Complete(KErrNone);
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

	DEBUG_PRINTF5(_L("Sis Registry Server - Opening entry subsession for UID: %08x, Name: %S, Vendor: %S, Index:%d"),
		package->Uid().iUid, &(package->Name()), &(package->Vendor()), package->Index());

	TUint subsessionId = 0;
	TRAPD(err, subsessionId = CreateSubsessionHandleL(*package));
	if (err != KErrNone)
		{
		DEBUG_PRINTF3(_L8("Sis Registry Server - Error (%d) creating subsession handle for package uid: %08x"), err, package->Uid().iUid);
		aMessage.Complete(err);
		CleanupStack::PopAndDestroy(3, buffer); // buffer, stream, package
		return;
		}

	TPckgC<TInt> refId(subsessionId);
	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d."), subsessionId);
	aMessage.WriteL(EIpcArgument3, refId);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(3, buffer); // buffer, stream, package
	}
	
void CSisRegistrySession::OpenRegistryNamesEntryL(const RMessage2& aMessage)
	{
	// expects a package name and a vendor name as an arg 0 and 1
	// Package Name - ipc(0)
	TInt srcLen = aMessage.GetDesLengthL(EIpcArgument0);
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

	TUint subsessionId = 0;
	TRAPD(err, subsessionId = CreateSubsessionHandleL(*packageName, *vendorName));
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Error (%d) creating subsession handle for package"), err);
		aMessage.Complete(err);
		CleanupStack::PopAndDestroy(2, packageName);// packageName, vendorName	
		return;
		}
	
	TPckgC<TInt> refId(subsessionId);
	DEBUG_PRINTF2(_L8("Sis Registry Server - Sucessfully opened, subsession ID %d."), subsessionId);
	aMessage.WriteL(EIpcArgument3, refId);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(2, packageName);// packageName, vendorName	
	}

void CSisRegistrySession::CloseRegistryEntryL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - Closing open subsession, ID %d."),
		aMessage.Int3());

	CloseSubsessionHandleL(aMessage.Int3());
	aMessage.Complete(KErrNone);
	}

// Gets the total number of files in a stub file and all the entries as well.
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

void CSisRegistrySession::RequestRegistryEntryL(const RMessage2& aMessage)
	{
    TUid uid; 
	TPckg<TUid> packageUid(uid);
	aMessage.ReadL(EIpcArgument0, packageUid);
    
   	// Get the base component Id matching with the given Uid.
    TComponentId componentId = ScrHelperUtil::GetComponentIdL(iScrSession, uid, 0);

    // Create an empty registry object and pass on to helper util to populate it from SCR.
    CSisRegistryObject*	object = CSisRegistryObject::NewLC();
    ScrHelperUtil::GetComponentL(iScrSession, componentId, *object);

    // Serialize the object and send it to client.
    SisRegistryUtil::SendDataL(aMessage, *object, EIpcArgument1);  	
	CleanupStack::PopAndDestroy(object);
	}

void CSisRegistrySession::AddDriveL(const RMessage2& aMessage)
	{
	TInt drive;
	TPckg<TInt> pkgDrive(drive);
	aMessage.ReadL(EIpcArgument0, pkgDrive);

	DEBUG_PRINTF2(_L8("Sis Registry Server - Removable drive %d added."), drive);

	// Find flagging controllers for non-preinstalled packages on
	// this drive and do cleanup if necessary
	ProcessRemovableDriveL(drive);

	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestSidToPackageL(const RMessage2& aMessage)
	{
	TUid sid; 
	TPckg<TUid> executableSid(sid);
	
	aMessage.ReadL(EIpcArgument0, executableSid);

	// componentId and index of the first matching Sid in CompSID<index> array gets populated 
	// by call to GetCompIdAndCompSidIndexL().The value of index is redundant here.
	TComponentId componentId = 0;
	TInt index = 0;
	GetCompIdAndCompSidIndexL(sid, componentId, index);
	
	if(componentId == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}
		
	CSisRegistryPackage *package = NULL;
	ScrHelperUtil::ReadFromScrL(iScrSession, componentId, package);
	CleanupStack::PushL(package);
	
	DEBUG_PRINTF5(_L("Sis Registry Server - SID 0x%08x is owned by package UID: 0x%08x, Name: %S, Vendor: %S."),
		sid.iUid, package->Uid().iUid, &(package->Name()), &(package->Vendor()));
	
	SisRegistryUtil::SendDataL(aMessage, *package, EIpcArgument1);
	CleanupStack::PopAndDestroy(package);
	
	}

void CSisRegistrySession::RequestSidToFileNameL(const RMessage2& aMessage)
	{
	TInt thirdParameter; 
	TDriveUnit expectedDrive;
	TUid sid;
	TPckg<TUid> fileSid(sid);

	// Obtain the sid from argument slot 0 from the client
	aMessage.ReadL(EIpcArgument0, fileSid);

	// Obtain drive from argument slot 2 from the client.
	// This parameter will be set to -1 if user did not specified a drive with this request.
	thirdParameter = aMessage.Int2(); 

	// Obtain the componentId and array index of CompSID<index> custom property matching a given aSid.
	TComponentId componentId = 0;
	TInt index = 0;
	GetCompIdAndCompSidIndexL(sid, componentId, index, thirdParameter);
	
	if(componentId == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}

  	// retrieve the CompSidFileName property value , based on the index obtained.
  	TBuf<KSmlBufferSize> compSidFileName(KEmptyString);
  	compSidFileName.Format(KCompSidFileNameFormat, index);
		
	HBufC* fileName = GetStrPropertyValueL(iScrSession, componentId, compSidFileName);
	CleanupStack::PushL(fileName);
	DEBUG_PRINTF3(_L("Sis Registry Server - SID 0x%08x maps to file '%S'."),
		sid.iUid, fileName);
	
	// Send the filename back to the client in argument slot 1
	aMessage.WriteL(EIpcArgument1, *fileName);

	CleanupStack::PopAndDestroy(fileName);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestModifiableL(const RMessage2& aMessage)
	{
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);
	
	TBool modifiableFile = ModifiableL(*fileName);

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

void CSisRegistrySession::IsFileRegisteredL(const RMessage2& aMessage)
	{
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);

	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);	
	componentFilter->SetFileL(*fileName);

	RArray<TComponentId> componentsIdList;
	CleanupClosePushL(componentsIdList);
	iScrSession.GetComponentIdsL(componentsIdList, componentFilter);

	TBool result = (componentsIdList.Count() > 0);
	
	TPckgC<TBool> isRegistered(result);
	aMessage.WriteL(EIpcArgument1, isRegistered);
	aMessage.Complete(KErrNone);	
	CleanupStack::PopAndDestroy(3, fileName); // componentsIdList, componentFilter
	}
	
void CSisRegistrySession::GetComponentIdForUidL(const RMessage2& aMessage)
	{
	TInt uid = aMessage.Int0();
	TComponentId componentId(0);
	if (!ScrHelperUtil::GetComponentIdL(iScrSession, TUid::Uid(uid), CSisRegistryPackage::PrimaryIndex, componentId))
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Component id for UID %d was not found '%S'."), uid);
		User::Leave(KErrNotFound);
		} 
		
	TPckgC<Usif::TComponentId> componentIdPckg(componentId);
	aMessage.WriteL(EIpcArgument1, componentIdPckg);
	aMessage.Complete(KErrNone);
	}
	
void CSisRegistrySession::RequestHashL(const RMessage2& aMessage)
	{
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);

	DEBUG_PRINTF2(_L("Sis Registry Server - Client requested the hash of file '%S'."), fileName);

	CHashContainer* hashContainer = HashL(*fileName);
	CleanupStack::PushL(hashContainer);

	SisRegistryUtil::SendDataL(aMessage, *hashContainer, EIpcArgument1);

	CleanupStack::PopAndDestroy(hashContainer);
	CleanupStack::PopAndDestroy(fileName);
	}
	
// Handle an error from CSisRegistrySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
void CSisRegistrySession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	DEBUG_PRINTF2(_L8("Sis Registry Server - ServiceL left with code %d."), aError);
	
	if (aError == KErrBadDescriptor)
		{
		PanicClient(aMessage, EPanicBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}

void CSisRegistrySession::UpdateTrustStatusL(const TUid& aUid, const TSisTrustStatus& aTrustStatus)
	{

	// Get the component (base) for the specified uid
	TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, aUid, CSisRegistryPackage::PrimaryIndex);

	// Update the trust status
	ScrHelperUtil::WriteToScrL(iScrSession, compId, aTrustStatus);
	}

/**
	Handle the EIsSidPresent message by extracting a SID from the
	message and checking whether is in any registered package.
	
	@param	aMessage		Message refers to client-side descriptors
							which contains SID and boolean.
 */
void CSisRegistrySession::IsSidPresentL(const RMessage2& aMessage)
	{
	TPckgBuf<TUid> uid;
	aMessage.ReadL(EIpcArgument0, uid);
	
	TBool isPresent = EFalse;
	TComponentId componentId = 0;
	TInt index = 0;
	
	// If the supplied SID is present in SCR, then a call to GetCompIdAndCompSidIndexL succeeds.
	// Else the componenetId will result in KErrNotFound.	
	GetCompIdAndCompSidIndexL(uid(), componentId, index);
	if(componentId != KErrNotFound)
		{
		isPresent = ETrue;
		}

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

/**
	Handle the EPackageExistsInRom message by extracting a pkg uid in the message 
		
	@param	aMessage		Message refers to client-side descriptors
							which contains a pkg uid to be searched on ROM stub files 
							and a boolean which holds the return value (string found or not).
 */
void CSisRegistrySession::PackageExistsInRomL(const RMessage2& aMessage)
	{
    TPckgBuf<TUid> tuid;
	aMessage.ReadL(EIpcArgument0, tuid);
	TBool isInRom = SisRegistryUtil::RomBasedPackageL(tuid());
	
	TPckgBuf<TBool> present(isInRom);
	aMessage.WriteL(EIpcArgument1, present);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestRemovablePackagesL(const RMessage2& aMessage)
	{
    RPointerArray<CSisRegistryPackage> packages; 
    CleanupResetAndDestroyPushL(packages);
    RemovablePackageListL(packages);
    SisRegistryUtil::SendDataPointerArrayL(aMessage, packages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&packages);
	}

void CSisRegistrySession::RecoverL(const RMessage2& aMessage)
	{
	// Check to see if a backup is in progress...
	TInt backup = 0;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,conn::KUidBackupRestoreKey,backup));
	
	if(backup == conn::EBURUnset || (backup == (conn::EBURNormal | conn::ENoBackup))) //no backup or restore is in progress
		{
		//no recovery attempt is made during backup/restore as recovery may seriously affect the file system, which might impact backup/restore
		RStsRecoverySession stsRecoverySession;
		CleanupClosePushL(stsRecoverySession);
		TRAP_IGNORE(stsRecoverySession.RollbackAllPendingL());
		CleanupStack::PopAndDestroy(&stsRecoverySession);

		//Start SWI Observer so that it can process the log files left from the previous session.
		RSwiObserverSession swiObserver;
		swiObserver.ProcessLogsL(iFs);
		swiObserver.Close();	
		}
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySession::LoggingFileInfoL(const RMessage2& aMessage)
 	{	 
	CSecurityPolicy* securityPolicy = CSecurityPolicy::GetSecurityPolicyL();
	TInt maxNumOfEntries = securityPolicy->MaxNumOfLogEntries();
	HBufC8* buf = ScrHelperUtil::GetLogInfoLC(iScrSession, maxNumOfEntries);
 	if(buf)
 		{
 		aMessage.WriteL(EIpcArgument0, *buf);
 		CleanupStack::PopAndDestroy(buf);
 		}
 	aMessage.Complete(KErrNone);
 	}

void CSisRegistrySession::SetComponentStateL(const RMessage2& aMessage)
	{
	TPckgBuf<Usif::TComponentId> pkgComponentId;
	TPckgBuf<Usif::TScomoState> pkgState;
	aMessage.ReadL(EIpcArgument0, pkgComponentId, 0);
	aMessage.ReadL(EIpcArgument1, pkgState, 0);
	iScrSession.SetScomoStateL(pkgComponentId(), pkgState());
	aMessage.Complete(KErrNone);
	}

// Helper methods

TBool CSisRegistrySession::IsRegisteredL(const TUid& aUid)
	{
	return ScrHelperUtil::IsUidPresentL(iScrSession, aUid);
	}

TBool CSisRegistrySession::IsRegisteredL(const CHashContainer& aHashContainer)
	{
	// Iterates through all the components registered in SCR and tries to find any component's 
	// controller hash matches with the given hash vlue.
	CHashContainer* hashContainer = NULL;
	RPointerArray<CControllerInfo> controllerInfoArray;		
	
	// Create an empty filter.
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);
	
	// Get the list of all native components installed on the device.
	RArray<TComponentId> componentsIdList;
	CleanupClosePushL(componentsIdList);
	iScrSession.GetComponentIdsL(componentsIdList, componentFilter);
		
	// Iterate through all the components in the view.
	for (TInt idIndex = 0; idIndex < componentsIdList.Count(); idIndex++)
		{
		// Get the Controller Info array from SCR.
		CleanupResetAndDestroyPushL(controllerInfoArray);
		ScrHelperUtil::InternalizeControllerArrayL(iScrSession, componentsIdList[idIndex], controllerInfoArray);
		// Iterate through all the controllers, get the hash of each of them and compare with the given value.
		// If any hash value matches, say the package registered - TRUE.
		for (TInt index = 0; index < controllerInfoArray.Count(); index++)
			{
			ScrHelperUtil::ReadFromScrL(iScrSession, componentsIdList[idIndex], hashContainer, index);
			CleanupStack::PushL(hashContainer);
			if (aHashContainer == *hashContainer)
				{
				CleanupStack::PopAndDestroy(4, componentFilter); // componentFilter, componentsIdList, controllerInfoArray & hashContainer
				// There found a matching component. So, return TRUE.
				return ETrue;
				}
			CleanupStack::PopAndDestroy(hashContainer);
			}
	
		CleanupStack::PopAndDestroy(&controllerInfoArray);
		}
	
	CleanupStack::PopAndDestroy(2, componentFilter); //componentFilter & componentsIdList
		
	// None of the registered component's controller hash matches with the given hash.
	// So, return FALSE. Mean to say, the component on invocation is not registered yet.
	return EFalse;
	}

TBool CSisRegistrySession::IsRegisteredL(const TUid& aUid, const TDesC& aPackageName)
	{
	return ScrHelperUtil::IsUidAndNamePresentL(iScrSession, aUid, aPackageName);
	}

void CSisRegistrySession::RemoveEntryL(const CSisRegistryPackage& aPackage)
	{
	DEBUG_PRINTF4(_L("Sis Registry Server - Removing the entry from Software Component Registry of package : UID: 0x%08x, Name: %S, Vendor: %S ."),
			aPackage.Uid().iUid, &aPackage.Name(), &aPackage.Vendor());

	// Get the corresponding component from SCR
	TComponentId compId = 0;
	if (!ScrHelperUtil::GetComponentIdL(iScrSession, aPackage.Uid(), aPackage.Index(), compId))
		{
		//if aPackage is not present in the database , then display the message and leave
		DEBUG_PRINTF4(_L("Component Entry : UID: 0x%08x, Name: %S, Vendor: %S , not found in Software Component Registry"),
			aPackage.Uid().iUid, &aPackage.Name(), &aPackage.Vendor());
		User::Leave(KErrNotFound);
		}

	iScrSession.DeleteComponentL(compId);
	}

void CSisRegistrySession::RemoveCleanupInfrastructureL(const CSisRegistryObject& aObject, RStsSession& aStsSession)
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Removing cleanup infrastructure."));
	
	RArray<TInt> driveArray;
	CleanupClosePushL(driveArray);
	ControllerDriveListL(aObject, driveArray);
	TInt count(driveArray.Count());
	for (TInt i = 0; i < count; ++i)
		{
		TInt driveNum = driveArray[i];
		TChar driveChar;
		User::LeaveIfError(RFs::DriveToChar(driveNum, driveChar)); 

		// Check if the drive exists before trying to do anything with it,
		// otherwise ignore it and continue to the next drive.
		// Also skip write-protected media for pre-installed packages.
		// An entry registered as a preinstalled package should only have a
		// drive in this array when it is a package installed from a removable
		// media stub (also known as a propagation) and the swipolicy indicates
		// that such packages are deletable.
		TVolumeInfo volInfo;
		if (KErrNone == iFs.Volume(volInfo, driveNum) &&
			(!aObject.PreInstalled() ||
			!(volInfo.iDrive.iMediaAtt & KMediaAttWriteProtected)))
			{
			// remove the controller(s) for this registry entry and drive
			if (RemoveControllerL(aObject, aStsSession, driveNum))
				{
				// If the controller was sucessfully removed then remove the uninstall logs
				// Uninstall logs aren't created for files installed on the system drive
				if (driveNum == iSystemDrive || aObject.InRom())
					{
					continue;
					}

				HBufC* name = SisRegistryUtil::BuildUninstallLogFileNameLC(aObject.Uid(), aObject.Index(), driveChar);
				DEBUG_PRINTF2(_L("Sis Registry Server - Attempting to remove cleanup log '%S'"), name);
				TRAPD(err, aStsSession.RemoveL(*name));
				if (err != KErrNone)
					{
					DEBUG_PRINTF2(_L("Sis Registry Server - Failed to remove cleanup log (unexpected error: %d.)"), err);
					User::Leave(err);
					}
				CleanupStack::PopAndDestroy(name);
				}
			}
		}

	CleanupStack::PopAndDestroy(&driveArray);
	}

TBool CSisRegistrySession::RemoveControllerL(const CSisRegistryObject& aObject, RStsSession& aStsSession, TInt aDrive)
	{
	TInt count(aObject.ControllerInfo().Count());
	TBool removed = ETrue;
	for (TInt i = 0; i < count; i++)
		{
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo()[i]->Offset(), aDrive);
		DEBUG_PRINTF2(_L("Sis Registry Server - Removing flagging controller '%S'."), name);

		TRAPD(err, aStsSession.RemoveL(*name));
		if (err != KErrNone)
			{
			DEBUG_PRINTF2(_L8("Sis Registry Server - Removal of flagging controller failed (Unexpected error %d.)"), err);
			removed = EFalse;
			}
	
		CleanupStack::PopAndDestroy(name);
		}
	return removed;
	}
	
// creates a compacted array using a token
void CSisRegistrySession::ControllerDriveListL(const CSisRegistryObject& aObject, RArray<TInt>& aDriveList)
	{
	aDriveList.Reset();
	// a copy of the controller is always kept on drive C
	aDriveList.Append(iSystemDrive);
	
	// only controllers will be written to removable media and 
	// we have now to check for those 
	TUint installationDrives  = aObject.Drives();
	TUint fixedDrives = FixedDrivesL();
	TUint remainingDrives = installationDrives & ~fixedDrives;

	if (remainingDrives)
		{
		TInt index = 0;
		// reuse the path but change drive letter
		while (remainingDrives)
			{
			// compare a single drive digit
			if (remainingDrives & 0x00000001)
				{
				User::LeaveIfError(aDriveList.Append(index)); 
				}
			remainingDrives>>=1;
			index++;
			}
		}
	}

// Returns a bitmap of internal drives present on the system
TUint CSisRegistrySession::FixedDrivesL() const
	{
	TUint intDrives = 0;
	TDriveList driveList; 

	// Get all the internal drives and convert the list to a bit array
	User::LeaveIfError(iFs.DriveList(driveList, KDriveAttInternal));
	for (TInt drive = 0; drive < KMaxDrives; ++drive)
		{
		if (driveList[drive] != 0)
			{
			intDrives |= (1 << drive);	
			}
		}
	return intDrives;
	}

TComponentId CSisRegistrySession::AddRegistryEntryL(CSisRegistryObject& aObject, RStsSession& aStsSession, const TDesC8& aController, Usif::TScrComponentOperationType aOpType)
	{
	const TComponentId compId = AddEntryL(aObject, aOpType);

	// If swipolicy.ini sets false to DeletePreinstalledFilesOnUninstall  
	// we don't need to create an uninstall log because we don't want cleanup activity to happen.
	// Just add the controller for preinstalled app in system drive.
	if(aObject.PreInstalled() && !aObject.DeletablePreInstalled())
		{
		// Create a controller on system drive for preinstalled application
		AddControllerL(aObject, aStsSession, aController, iSystemDrive);
		}
	else
		{
		// Add cleanup infrastructure if required
		AddCleanupInfrastructureL(aObject, aStsSession, aController);
		}

	return compId;
	}

TComponentId CSisRegistrySession::AddEntryL(CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType)
	{
	// set the index
	if (aObject.InstallType() == Sis::EInstAugmentation ||
		aObject.InstallType() == Sis::EInstPreInstalledPatch)
		{
		// Just get the number of installed augmentations - this is nothing but the index of the last augmentation 
		aObject.SetIndex(GetAugmentationCountL(aObject.Uid()) + 1);
		}
	
	// Resolve the embedded packages index values
	TInt embeddedIndex = 0;
	CSisRegistryPackage* embeddedPackage = NULL;
	while ((embeddedPackage = aObject.EmbeddedPackage(embeddedIndex++)) != NULL)
		{
		// Retrieve the component Id from SCR for a given Package UID, name and vendor.
		// If the package is not present in the registry, do not consider it as an error - embedded packages may upgrade each other causing a single registry entry
		TComponentId componentId(0); 
		if (!ScrHelperUtil::GetComponentIdL(iScrSession, embeddedPackage->Name(), embeddedPackage->Vendor(), componentId))
			continue;

		TInt packageIndex = GetIntPropertyValueL(iScrSession, componentId, KCompIndex, EFalse, KDefaultComponentIndex);
		embeddedPackage->SetIndex(packageIndex);
		}
	
	// Store the SIS Registry Object in to the Software Component Registry.
	TComponentId compId = 0;
	ScrHelperUtil::AddComponentL(iScrSession, compId, aObject, aOpType);
	// Set SCOMO state to activated - when SWI Registry supports de-activation, should be changed accordingly
	iScrSession.SetScomoStateL(compId, EActivated);
	
	return compId;
	}
	
void CSisRegistrySession::AddControllerL(const CSisRegistryObject& aObject, 
							RStsSession& aStsSession, const TDesC8& aBuffer, const TInt aDrive)		
	{
	HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), aObject.Index(), aObject.ControllerInfo().Count()-1, aDrive);
	DEBUG_PRINTF2(_L("Sis Registry Server - Installing controller file to '%S'."), name);

	TEntry entry;
	if (KErrNone == iFs.Entry(*name, entry))
		{
		// Remove any existing file with the same name
		aStsSession.RemoveL(*name);
		}

	// add controller to integrity services log
	aStsSession.RegisterNewL(*name);

	// store
	SisRegistryUtil::EnsureDirExistsL(iFs, *name);
		
	RFileWriteStream fileStream;
	User::LeaveIfError(fileStream.Create(iFs, *name, EFileWrite|EFileShareExclusive));
	CleanupStack::PopAndDestroy(name);
	CleanupClosePushL(fileStream);
	fileStream.WriteL(aBuffer);
 
	// commit and close file
	CleanupStack::PopAndDestroy(&fileStream);
	}

void CSisRegistrySession::AddCleanupInfrastructureL(CSisRegistryObject& aObject, RStsSession& aStsSession, const TDesC8& aControllerBuffer)
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Adding Cleanup Infrastructure."));

	RArray<TInt> driveArray;
	CleanupClosePushL(driveArray);
	ControllerDriveListL(aObject, driveArray);
	TInt driveCount(driveArray.Count());
	// Loop through all the removable drives (and the system drive)
	for (TInt i = 0; i < driveCount; ++i)
		{
		TInt driveNum = driveArray[i];
		// Skip drives which are not present.  Note that this will only be
		// valid for filenull operations, but we will otherwise have failed
		// earlier in installation planning so we don't need to check
		// the operation again.
		TVolumeInfo volInfo;
		if (KErrNone != iFs.Volume(volInfo, driveNum))
			{
			continue;
			}

		// Create a flagging controller on the drive in question
		AddControllerL(aObject, aStsSession, aControllerBuffer, driveNum);

		// There is no need to create uninstall logs for the files put on the 
		// system drive; all other drives in the list should be removable.
		// We can also skip writing uninstall logs if the object is in ROM,
		// since uninstallation should never be possible.
		if (driveNum == iSystemDrive || aObject.InRom())
			{
			continue;
			}

		// Create uninstall log on the system drive and add all installed files to it
		// Note that there is a separate uninstall log created for each removable drive (these
		// go in separate contained directories named the same as the drive letter)
		TChar driveLetter;
		User::LeaveIfError(iFs.DriveToChar(driveNum, driveLetter));
		driveLetter.Fold();
		HBufC* ulogName = SisRegistryUtil::BuildUninstallLogFileNameLC(aObject.Uid(), aObject.Index(), driveLetter);

		// If dir exists register it for removal since this is a data-caged area and we 
		// don't want presence of any other files here.
		// NOTE: Here we are relying on the ordering of operations executed by STS
		// We are expecting the removal first and then addition of a new file!
		TParsePtrC parseName(*ulogName);
		TPtrC parsePath = parseName.DriveAndPath();
		TEntry entry;
		if (iFs.Entry(parsePath, entry) == KErrNone)
			{
			// Remove any uninstall logs with this path
			aStsSession.RemoveL(parsePath);
			}

		SisRegistryUtil::EnsureDirExistsL(iFs, *ulogName);
		DEBUG_PRINTF2(_L("Sis Registry Server - Creating uninstall log '%S'"), ulogName);

		RFile ulogFile;
		CleanupClosePushL(ulogFile);
		aStsSession.CreateNewL(*ulogName, ulogFile, TFileMode(EFileShareExclusive |EFileStream));
		RFileWriteStream ulogStream(ulogFile);
		CleanupClosePushL(ulogStream);

		TInt filesCount = aObject.FileDescriptions().Count();
		for (TInt index = 0; index < filesCount; ++index)
			{
			const TDesC& target = aObject.FileDescriptions()[index]->Target();
			TChar targetDrive = target[0];
			targetDrive.Fold();
			// Only add files that are installed to this removable drive
			if (driveLetter == targetDrive)
				{
				DEBUG_PRINTF2(_L("Sis Registry Server - Adding file '%S' cleanup uninstall log"), &target);
				ulogStream << target;
				}
			}

		// Add any potential private paths as targets for cleanup operations
		RBuf privatePath;
		privatePath.CreateL(KMaxFileName);
		CleanupClosePushL(privatePath);
		TInt sidCount = aObject.Sids().Count();
		for (TInt index = 0; index < sidCount; ++index)
			{
			privatePath = KPrivatePathFormat;
			privatePath[0] = driveLetter;
			privatePath.AppendNumFixedWidthUC(aObject.Sids()[index].iUid, EHex, KUidHexLength);
			privatePath.Append(KPathDelimiter);

			DEBUG_PRINTF2(_L("Sis Registry Server - Adding (potential) private path '%S' to cleanup uninstall log"), &privatePath);
			ulogStream << privatePath;
			}

		// Add the flagging controller
		HBufC* controllerName = SisRegistryUtil::BuildControllerFileNameLC(aObject.Uid(), 
				aObject.Index(), aObject.ControllerInfo().Count() - 1, driveNum);
		ulogStream << *controllerName;

		// We need to ensure that an automatic cleanup does not happen if the media card is inserted
		// in to a different phone since this would leave behind a broken installation on the original
		// phone even when the card is put back into it.
		CleanupStack::PopAndDestroy(5, ulogName); // ulogFile, ulogStream, privatePath, controllerName
		}

	CleanupStack::PopAndDestroy(&driveArray); 
	}									

/*
 * Subsession creation helper method. For the supplied package UID opens the corresponding 
 * component (base), stores it in an array and returns the entry's index as the subsession ID
 */
TUint CSisRegistrySession::CreateSubsessionHandleL(const TUid& aPackageUid)
	{
	// Get the component matching the Uid (get the base component)
	TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, aPackageUid, CSisRegistryPackage::PrimaryIndex);
	
	// Store the component ID and return the array index as subsession ID
	// NOTE: This code assumes the items are appended sequentially thereby the indexing increases monotonically!
	CSisRegistrySubSession *subsession = new (ELeave) CSisRegistrySubSession(*this, compId, aPackageUid);
	CleanupStack::PushL(subsession);
	iOpenedPackages.AppendL(subsession);
	CleanupStack::Pop(subsession);
	return iOpenedPackages.Count();	// Will be index + 1
	}

/*
 * Subsession creation helper method. For the supplied package opens the corresponding 
 * component, stores it in an array and returns the entry's index as the subsession ID
 */
TUint CSisRegistrySession::CreateSubsessionHandleL(const CSisRegistryPackage& aPackage)
	{
	// Get the component matching the Uid (get the base component)
	TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, aPackage.Uid(), aPackage.Index());

	// Store the component ID and return the array index as subsession ID
	// NOTE: This code assumes the items are appended sequentially thereby the indexing increases monotonically!
	CSisRegistrySubSession *subsession = new (ELeave) CSisRegistrySubSession(*this, compId, aPackage.Uid());
	CleanupStack::PushL(subsession);
	iOpenedPackages.AppendL(subsession);
	CleanupStack::Pop(subsession);
	return iOpenedPackages.Count();	// Will be index + 1
	}

/*
 * Subsession creation helper method. For the supplied package name and vendor name opens the 
 * corresponding component, stores it in an array and returns the entry's index as the subsession ID
 * NOTE: There can be multiple such components and this API opnes the first one found! 
 */
TUint CSisRegistrySession::CreateSubsessionHandleL(const TDesC& aPackageName, const TDesC& aVendorName)
	{
	// Get the component matching the Uid (get the base component)
	TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, aPackageName, aVendorName);
	// Get the uid
	TUid uid = TUid::Uid(GetIntPropertyValueL(iScrSession, compId, KCompUid, ETrue));
	
	// Store the component ID and return the array index as subsession ID
	// NOTE: This code assumes the items are appended sequentially thereby the indexing increases monotonically!
	CSisRegistrySubSession *subsession = new (ELeave) CSisRegistrySubSession(*this, compId, uid);
	CleanupStack::PushL(subsession);
	iOpenedPackages.AppendL(subsession);
	CleanupStack::Pop(subsession);
	return iOpenedPackages.Count();	// Will be index + 1
	}

void CSisRegistrySession::CloseSubsessionHandleL(TUint aId)
	{
	TInt index = OpenedPackageIndexL(aId);
	CSisRegistrySubSession* subsession = iOpenedPackages[index];
	iOpenedPackages.Remove(index);
	delete subsession;
	}

CSisRegistrySubSession& CSisRegistrySession::SubsessionL(TUint aSubsessionId)
	{
	// Lookup the component ID for this subsession ID
	TInt index = OpenedPackageIndexL(aSubsessionId); 
	return *iOpenedPackages[index];
	}

TUint CSisRegistrySession::OpenedPackageIndexL(TUint aSubsessionId)
	{
	TInt index = aSubsessionId - 1;	// Subtract 1 since we return subsession ID as index + 1
	if (index < 0 || index >= iOpenedPackages.Count())
		{
		User::Leave(KErrNotFound);
		}
	return index;
	}

TBool CSisRegistrySession::IdentifyControllerL(TComponentId aComponentId, const TDesC& aFileName) 
	{
	TBool found = EFalse;

	// Get the controller's hashes to verify if the controller file belongs to this component
	RPointerArray<CControllerInfo> controllerInfo;
	CleanupResetAndDestroyPushL(controllerInfo);
	ScrHelperUtil::InternalizeControllerArrayL(iScrSession, aComponentId, controllerInfo);

	// Calculate the hash of the controller passed in as a filename
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	HBufC8* buffer = HBufC8::NewLC(fileSize);
	TPtr8 ptr(buffer->Des());
	User::LeaveIfError(file.Read(0, ptr, fileSize));

	// Read the controller
	CDesDataProvider* desProvider = CDesDataProvider::NewLC(*buffer);
	Sis::CController* controller = Sis::CController::NewLC(*desProvider, Sis::EAssumeType);
 	CMessageDigest* msgDigest = controller->GenerateControllerHashLC(*buffer);
	TPtrC8 hash = msgDigest->Final();

	TInt controllerCount = controllerInfo.Count();
	for (TInt index = 0; index < controllerCount; index++)
		{
		if (controllerInfo[index]->HashContainer().Data() == hash)
			{
			found = ETrue;
			break;
			}
		}

	CleanupStack::PopAndDestroy(6, &controllerInfo);	// file, buffer, desProvider, controller, msgDigest
	return found;
	}

// Processes the removable looking for any installations for cleanup 
void CSisRegistrySession::ProcessRemovableDriveL(TInt aDrive)
	{
	TDriveUnit drive(aDrive);
	// create directory path
	RBuf regPath;
	regPath.CreateL(drive.Name(), KMaxFileName);
	CleanupClosePushL(regPath);
	regPath.Append(KRegistryPath);
	
	CDir* dir;
	TInt err = iFs.GetDir(regPath, KEntryAttDir, ESortNone, dir);
	CleanupStack::PushL(dir);
	if(err == KErrNone)
		{
		TInt count(dir->Count());
		for (TInt i = 0; i < count; ++i)
			{
			// want to be able to trap the failure, log it and process the next controller
			// in this way a corrupt controler would not hog the processing
			TRAPD(res, DiscoverControllersL(regPath, (*dir)[i].iName));
			if (res != KErrNone)
				{
				// log that
				DEBUG_PRINTF2(_L8("Sis Registry Server - DiscoverController failed with error %d."), res);
				}
			}
		}

	CleanupStack::PopAndDestroy(2, &regPath); // dir
	}

void CSisRegistrySession::DiscoverControllersL(const TDesC& aRegistryPath, const TDesC& aDirectoryName)
	{
	DEBUG_PRINTF2(_L("Sis Registry Server - Attempting to discover controllers we can clean up (Directory '%S')"),
		&aDirectoryName);
	
	// we expect uid directories to be 8 characters long, ignore others
	if (aDirectoryName.Length() != KUidHexLength)
		return;

	// Determine the uid from the directory name
	TLex lex(aDirectoryName);
	TUint uidValue;
	User::LeaveIfError(lex.Val(uidValue, EHex));	
	
	TUid uid = TUid::Uid(uidValue);
	RBuf controllerPath;
	controllerPath.CreateL(aRegistryPath, KMaxFileName);
	CleanupClosePushL(controllerPath);
	controllerPath.Append(aDirectoryName);
	controllerPath.Append(KPathDelimiter);
	controllerPath.Append(KControllerFileFilter);
	
	CDir* dir;
	// List the directory contents, excluding directories, system and hidden files.
	// i.e. just normal files
	TInt err = iFs.GetDir(controllerPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
	DEBUG_PRINTF2(_L8("Sis Registry Server - Flagging controller directory, read returned code %d."), err);	

	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&controllerPath);
		return;
		}

	CleanupStack::PushL(dir);	
	TParsePtrC parse(controllerPath);
	RBuf controllerName;
	controllerName.CreateL(KMaxFileName);
	CleanupClosePushL(controllerName);

	TInt count(dir->Count());
	for (TInt i = 0; i < count; ++i)
		{
		const TEntry& entry = (*dir)[i];

		// Determine the augmentation index
		TLex parser(entry.iName);
		TUint augmentationIndex = 0;
		if (parser.Val(augmentationIndex, EHex) != KErrNone)
			{
			continue;
			}

		controllerName = parse.DriveAndPath();
		controllerName.Append(entry.iName);

		// Controller present, see if is still installed on this device
		TComponentId componentId = 0;
		if (ScrHelperUtil::GetComponentIdL(iScrSession, uid, augmentationIndex, componentId) &&
			IdentifyControllerL(componentId, controllerName))
			{
			// It's installed so skip over it
			continue;
			}
		// Also make sure that it *was* installed on this phone.
		// The presence of the uninstall log ensures this.
		// It's not installed so use the uninstall log created during install to cleanup the installation
		HBufC* ulogName = SisRegistryUtil::BuildUninstallLogFileNameLC(uid, augmentationIndex, aRegistryPath[0]);
		TEntry entryUninstallLogFile;
		TInt err = iFs.Entry(*ulogName, entryUninstallLogFile);
		if (err == KErrNone)
			{
			DEBUG_PRINTF3(_L("Sis Registry Server - App no longer on device, cleaning up files for '%S' using log '%S'."), 
					&controllerName, ulogName);
			ExecuteUninstallLogL(*ulogName, controllerName);
			}
		else
			{
			// Uninstall log is not present so don't cleanup
			DEBUG_PRINTF(_L("Sis Registry Server - Skipping cleanup since app wasn't installed on this phone.")); 
			}
		CleanupStack::PopAndDestroy(ulogName);
		}
	CleanupStack::PopAndDestroy(3, &controllerPath); // dir, controllerName
	}

void CSisRegistrySession::ExecuteUninstallLogL(const TDesC& aUninstallLogFile, const TDesC& aControllerFile)
	{
	// Process uninstall log file for cleanup
	TParsePtrC ctlFile(aControllerFile);
	TChar driveChar = ctlFile.Drive()[0];
	driveChar.Fold();

	RPointerArray<HBufC> entryList;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(entryList);

	RFileReadStream ulogStream;
	CleanupClosePushL(ulogStream);

	TEntry entry;
	if ((iFs.Entry(aUninstallLogFile, entry) == KErrNone) &&
			(ulogStream.Open(iFs, aUninstallLogFile, EFileShareAny | EFileStream) == KErrNone))
		{
		// Process the log and collect the files and private path entries
		for (;;)
			{
			HBufC* logEntry = NULL;
			TRAPD(err, logEntry = HBufC::NewL(ulogStream, KMaxFileName));
			if (err == KErrEof)
				{
				break;
				}
			User::LeaveIfError(err);
			CleanupStack::PushL(logEntry);
			if (logEntry->Length() == 0)
				{
				CleanupStack::PopAndDestroy(logEntry);
				break;
				}
			// Make sure the entry's location is on the same drive as the removable media
			// Also ignore any wildcard specification (this should never be legally present in the log)
			TParsePtrC parseEntry(*logEntry);
			TChar logEntryDrive = parseEntry.Drive()[0];
			logEntryDrive.Fold();
			if ((logEntryDrive != driveChar) || (parseEntry.IsWild()))
				{
				// Ignore this entry
				CleanupStack::PopAndDestroy(logEntry);
				continue;
				}
			entryList.AppendL(logEntry);		// Ownership transferred to array
			CleanupStack::Pop(logEntry);
			}
		}

	// Close the log file early since we need to delete it now  
	CleanupStack::PopAndDestroy(&ulogStream);

	// Add the log file to the entry list for cleanup. 
	// Controller file would already be part of cleanup log
	// Removing log file and ctl file at the end of cleanup is better since if the 
	// removable drive is ejected while we are cleaning up, we can try the cleanup next
	// time the card is inserted again
	HBufC* tempEntry = aUninstallLogFile.AllocLC();
	entryList.AppendL(tempEntry);	// takes ownership
	CleanupStack::Pop(tempEntry);

	// Delete all collected entries
	TInt entryCount = entryList.Count();
	for (TInt index = 0; index < entryCount; ++index)
		{
		HBufC* entry = entryList[index];
		TParsePtrC parseEntry(*entry);
		DEBUG_PRINTF2(_L("Sis Registry Server - Deleting file and path '%S'"), entry);
		// Check if it's a file
		if (!parseEntry.NamePresent())
			{
			MiscUtil::DeletePathIfEmpty(iFs, *entry);
			continue;
			}
		TInt err = iFs.Delete(*entry);
		if (err == KErrAccessDenied)
			{
			// File may be read-only. Clear the attribute and try again
			iFs.SetAtt(*entry, 0, KEntryAttReadOnly);
			iFs.Delete(*entry);
			}
		MiscUtil::DeletePathIfEmpty(iFs, *entry);
		}

	CleanupStack::PopAndDestroy(&entryList);
	}

void CSisRegistrySession::GetCompIdAndCompSidIndexL(const TUid& aSid, TComponentId& aComponentId, TInt& aIndex, TInt aExpectedDrive)
	{
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);	
	componentFilter->AddPropertyL(KCompSidsPresent, 1);
		
	// Retrieve the componentId of all the components with SIDs from the SCR.
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	iScrSession.GetComponentIdsL(componentIdList, componentFilter);
		
	TInt componentCount = componentIdList.Count();
	RArray<TUid> sidArray;
	CleanupClosePushL(sidArray);
	TBool sidMatchFound = EFalse;

	for(TInt i = 0; i < componentCount && !sidMatchFound; ++i)
		{
		TComponentId componentId = componentIdList[i];
		sidArray.Reset();
		ScrHelperUtil::InternalizeSidArrayL(iScrSession, componentId, sidArray);
		
		// index of the first matching SID in the array.
		aIndex = sidArray.Find(aSid);	
		if(aIndex == KErrNotFound)
			continue;

		// If the search is narrowed to a particular drive, get the file name and check whether it matches the drive
		if (aExpectedDrive != -1)
			{
			TBuf<KSmlBufferSize> compSidFileName(KEmptyString);
			compSidFileName.Format(KCompSidFileNameFormat, aIndex);
			
			HBufC* fileName = GetStrPropertyValueL(iScrSession, componentId, compSidFileName);
			CleanupStack::PushL(fileName);			
			if (fileName->Length() == 0)
				{
				CleanupStack::PopAndDestroy(fileName);
				continue;
				}
			TInt drive; 
 			User::LeaveIfError(RFs::CharToDrive((*fileName)[0], drive));
 			if(drive != aExpectedDrive)
 				{
				CleanupStack::PopAndDestroy(fileName);
 				continue;
 				}			
			CleanupStack::PopAndDestroy(fileName);
			}
			
		aComponentId = componentId;
		sidMatchFound = ETrue;
		}
	
	CleanupStack::PopAndDestroy(3, componentFilter); // componentIdList and sidArray.
	if(!sidMatchFound)
		{
		// No component contains aSid.
		aComponentId = KErrNotFound;
		}
	}

TBool CSisRegistrySession::ModifiableL(const TDesC& aFileName)
	{
	// Retrieve the list of components which contain aFileName.
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	iScrSession.GetComponentsL(aFileName, componentIdList);
	
	// No component in Software Component Registry contain aFileName.
	if(componentIdList.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
		
	// having obtained the list of components containing aFilename in componentIdList , use any
	// one of the component from the list to find out whether aFileName is modifiable or not.
	
	const TComponentId componentId = componentIdList[0];
	CleanupStack::PopAndDestroy(&componentIdList);
	

	Sis::TSISFileOperationOptions operationOptions = static_cast<Sis::TSISFileOperationOptions>(0);
	Usif::CPropertyEntry* propertyEntry = NULL;
	propertyEntry = iScrSession.GetFilePropertyL(componentId, aFileName, KFileFileOperationOptions);

	if (propertyEntry)
		{
		operationOptions = static_cast<Sis::TSISFileOperationOptions>(static_cast<Usif::CIntPropertyEntry*>(propertyEntry)->IntValue());
		}

	// find out if the file is modifiable, and return an appropriate
	// Boolean value. If the expression below is True, then the file is modifiable.
	TBool isModifiable = (operationOptions & Sis::EInstVerifyOnRestore) ? EFalse : ETrue;

	// Files under \sys or \resource are never modifiable
	_LIT(KSys, "\\sys\\"); 
	_LIT(KResource, "\\resource\\");			
	if (aFileName.FindF(KSys) == 2 || aFileName.FindF(KResource) == 2)
		{
		isModifiable = EFalse;
		}	
	
	return isModifiable;
	}

CHashContainer* CSisRegistrySession::HashL(const TDesC& aFileName)
	{
	// Retrieve the list of components which contain aFileName.
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	iScrSession.GetComponentsL(aFileName, componentIdList);
	
	// No component in Software Component Registry contain aFileName.
	if(componentIdList.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	// having obtained the list of components containing aFilename in componentIdList , use any
	// one of the component from the list to retrieve the hash of aFileName.
	TInt index = 0;
	const TComponentId componentId = componentIdList[index];
	CleanupStack::PopAndDestroy(&componentIdList);

	CHashContainer* hashContainer = NULL;
	ScrHelperUtil::ReadFromScrL(iScrSession, componentId, hashContainer, aFileName);
	return hashContainer;	// ownership is transferred to the caller(ie, Swi::CSisRegistrySession::RequestHashL())
	}

void CSisRegistrySession::RemovablePackageListL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	aPackages.ResetAndDestroy();
	
	// Filter the components from SCR whose Removable flag is set.
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);
	componentFilter->SetRemovable(ETrue);
	
	RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession); 	
	subSession.OpenViewL(iScrSession, componentFilter);	
	
	// Create CSisRegistryPackage object for all the removable components and append them in aPackages array..
	CComponentEntry* componentEntry = NULL;
	while((componentEntry = subSession.NextComponentL()) != NULL)
		{
		CleanupStack::PushL(componentEntry);
		TComponentId componentId = componentEntry->ComponentId();
		
		CSisRegistryPackage *package = NULL;
		ScrHelperUtil::ReadFromScrL(iScrSession, componentId, package);
		CleanupStack::PushL(package);
		aPackages.AppendL(package);
		CleanupStack::Pop(package);
		CleanupStack::PopAndDestroy(componentEntry);
		
		}
	
	CleanupStack::PopAndDestroy(2, componentFilter); //componentFilter and subSession.
	}

void CSisRegistrySession::ProcessRomDriveL()
	{
	DEBUG_PRINTF(_L8("Sis Registry Server - Registering In-Rom controllers."));
	
	TDriveUnit romDrive(SisRegistryUtil::SystemRomDrive());
	RBuf romRegistryPath;
	romRegistryPath.CreateL(romDrive.Name(), KMaxPath);
	CleanupClosePushL(romRegistryPath);
	romRegistryPath.Append(KPreInstalledPath);
	// open the directory
	CDir* dir;
	TInt err = iFs.GetDir(romRegistryPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
	// check that all is correct but make sure to handle missing directory	
	
	DEBUG_PRINTF2(_L8("Sis Registry Server - Reading ROM stub directory returned %d."), err);
	
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
			TRAPD(res, RegisterInRomControllerL(controllerFileName));
			if (res != KErrNone)
				{
				// log it only, we cannot stop as the next might be ok
				DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to register in ROM controller. Error code %d."), res);
				}
			}

		CleanupStack::PopAndDestroy(2, dir); // controllerFileName
		}
	else if(err != KErrPathNotFound)
		{
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(&romRegistryPath);
	}
	
void CSisRegistrySession::RegisterInRomControllerL(const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("Sis Registry Server - Registering In-ROM controller file '%S'."), &aFileName);
	
	RFile romController;
	CleanupClosePushL(romController);
	// open file and read the data
	User::LeaveIfError(romController.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	// read the data from the file
	TInt fileSize;
	romController.Size(fileSize);
	HBufC8* controllerData = HBufC8::NewLC(fileSize);
	TPtr8 ptr(controllerData->Des());
	romController.Read(0, ptr, fileSize);
	
	CDesDataProvider* desProvider = CDesDataProvider::NewLC(*controllerData);
	// read the controller
	Swi::Sis::CController* controller = Swi::Sis::CController::NewLC(*desProvider);

	// create an object from the controller
	CSisRegistryObject* object = CSisRegistryObject::NewLC();

	// Discard the 4 byte type field for consistency with
	// all other registry entries
	TPtrC8 assumedTypeController(controllerData->Mid(4));
	// we cannot guess whether it is registered without opening it	
	object->ProcessInRomControllerL(*controller, assumedTypeController);
	

	TBool overwriteRegEntry = EFalse;
	
    TBool isStubRegistered = IsRegisteredL(object->Uid(), object->Name());
    if ( isFwUpdated && isStubRegistered )
	    {
		TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, object->Uid(), object->Index());
		TSisPackageTrust trustStatus;
		trustStatus = static_cast<TSisPackageTrust>(GetIntPropertyValueL(iScrSession, compId, KCompTrust,  EFalse, KDefaultTrustValue));
		//ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, trustStatus);
	    if ( ESisPackageBuiltIntoRom == trustStatus )
	        {
            DEBUG_PRINTF2(_L8("Sis Registry Server - Attempting to delete registry entry 0x%08x as a firmware update detected"), object->Uid().iUid);
            
            // If the component being removed has registered software types, unregister them now.
            // This operation deletes MIME types mapping from AppArc and therefore is not transactional.
            UnregisterSoftwareTypesL(compId);

            CSisRegistryObject* object = CSisRegistryObject::NewLC();
            ScrHelperUtil::GetComponentL(iScrSession, compId, *object);

            DEBUG_PRINTF4(_L("Sis Registry Server - Removing package registry entry for UID: %08x, Name: %S, Vendor %S."),
                object->Uid().iUid, &(object->Name()), &(object->Vendor()));
                
            RemoveEntryL(*object);           

            CleanupStack::PopAndDestroy();
            overwriteRegEntry = ETrue;
	        }
	    }


	if ( !isStubRegistered || overwriteRegEntry )
		{
		// update cache or just call refresh
		AddEntryL(*object, Usif::EScrCompHidden); // EScrCompHidden is supplied not to create any log for the ROM controller on the SCR side.
		
		// store a copy of the controller
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(object->Uid(), object->Index(),
				0);
		
		SisRegistryUtil::EnsureDirExistsL(iFs, *name);
		RFile newController;
		
		// If stray .ctl files exist (for example, after phone was rebooted in the middle of registration), calling RFile::Create() will result a Leave.
		// Therefore, RFile::Replace() is rather ideal.
		User::LeaveIfError(newController.Replace(iFs, *name, EFileWrite | EFileShareExclusive));
		CleanupStack::PopAndDestroy(name);
		CleanupClosePushL(newController);
		
		// Write the controller, minus the 4 byte type field for consistency with
		// all other registry entries		
		newController.Write(assumedTypeController);
		CleanupStack::PopAndDestroy(&newController);
		}
	
	CleanupStack::PopAndDestroy(5, &romController); //controller, object, desProvider, controllerData
	}

TInt CSisRegistrySession::GetAugmentationCountL(const TUid& aUid)	
	{
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->AddPropertyL(KCompUid, aUid.iUid);
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);

	RArray<Usif::TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	iScrSession.GetComponentIdsL(componentIdList, componentFilter);
	
	TInt componentsCount = componentIdList.Count();
	TInt result =  componentsCount ? componentsCount - 1 : 0;
	CleanupStack::PopAndDestroy(2, componentFilter); // componentIdList
	return result;
	}
	
void CSisRegistrySession::GetAugmentationDetailsL(const TUid& aUid, RPointerArray<CSisRegistryPackage>& aPackagesArray, TComponentId aBaseComponentId)
	{
	TInt componentId = 0;
	TInt componentIndex = 0;
	CSisRegistryPackage* augmentedPackage = NULL;
	// Create the component filter with Component Uid.
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->AddPropertyL(KCompUid, aUid.iUid);
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);
	RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession); 	
	// Populate the view with components matching the filter specification ( Comp' Uid ).
	subSession.OpenViewL(iScrSession, componentFilter);	
	
	// Iterate through all the components with this Uid
	CComponentEntry* componentEntry = NULL;
	while((componentEntry = subSession.NextComponentL()) != NULL)
		{
		CleanupStack::PushL(componentEntry);
		componentId = componentEntry->ComponentId();
		if (aBaseComponentId == componentId)
			{
			CleanupStack::PopAndDestroy(componentEntry);
			continue; // An optimisation - skip the base component
			}
		componentIndex = GetIntPropertyValueL(iScrSession, componentId, KCompIndex, EFalse, KDefaultComponentIndex);
		// Check if the package is not base package (SA). If so, add it to the list of augmentated packages.
		if (componentIndex != CSisRegistryPackage::PrimaryIndex)
			{
			// Get the unique vendor name that is stored as a property
			HBufC* uniqueVendor = GetStrPropertyValueL(iScrSession, componentId, KCompUniqueVendorName);
			CleanupStack::PushL(uniqueVendor);
			augmentedPackage = CSisRegistryPackage::NewLC(aUid, componentEntry->Name(), *uniqueVendor);
			augmentedPackage->SetIndex(componentIndex);
			aPackagesArray.AppendL(augmentedPackage);
			CleanupStack::Pop(augmentedPackage);
			CleanupStack::PopAndDestroy(uniqueVendor);
			}
			
		CleanupStack::PopAndDestroy(componentEntry);
		}	
				
	CleanupStack::PopAndDestroy(2, componentFilter); // componentFilter, subSession
	}


_LIT( KSWVersionFileName, "Z:\\resource\\versions\\sw.txt" );
const TInt KSysUtilVersionTextLength = 64;
_LIT( KNewLinePattern, "\\n" );
_LIT( KNewline, "\n" );
/**
Fetch text from specified files.
Copied from /common/generic/syslibs/bafl/sysutil/src/sysutil.cpp 
*/
static TInt GetTextFromFile(
    const TDesC& aFilename,
    TDes& aValue,
    TBool aRemoveNewLines )
    {
    RFs fs;
    TInt err;
    err = fs.Connect();
    if (err != KErrNone)
        return err;

    RFile file;
    err = file.Open( fs, aFilename,
                     EFileRead | EFileStreamText | EFileShareReadersOnly );
    if (err != KErrNone)
        {
        fs.Close();
        return err;
        }

    TBuf8<2> characters;    
    err =  file.Read(characters);
    
    if (err == KErrNone || err == KErrTooBig)
        {
        // This means that we have an ANSI file (without the header bytes)
        if( characters.Length() == 0 || characters.Length() == 1 )
            {
            file.Close();
            fs.Close();
            return KErrCorrupt;     
            }
        else 
            {
            TUint8 firstByte = characters[0];
            TUint8 secondByte = characters[1];
            
            // Heading byte values for unicode files
            const TInt KFFByte = 255;
            const TInt KFEByte = 254;
            
            // If file isn't unicode KErrCorrupt is returned
            if( (firstByte!=KFFByte && secondByte!=KFEByte) && (secondByte!=KFFByte && firstByte!=KFEByte) )
                {
                file.Close();
                fs.Close();
                return KErrCorrupt;
                } 
            } 
        }
    
    TFileText tf;
    tf.Set(file);
    err = tf.Read(aValue);
    // If the maximum length of the descriptor is insufficient to hold the record,
    // the Read() function returns KErrTooBig and the descriptor is filled to its maximum length.
    //
    // If Read() is called when the current position is the end of the file (that is, after
    // the last line delimiter in the file), KErrEof is returned, and the length of the buffer
    // is set to zero. In this case, this would mean an empty file, as this code always reads
    // from the beginning of the file.
    
    if (err == KErrNone || err == KErrTooBig)
        {
        if (aValue.Length() > KSysUtilVersionTextLength)
            {
            // File content is larger than 64 characters. Truncate to 64 characters.
            aValue.Delete(KSysUtilVersionTextLength,aValue.Length() - KSysUtilVersionTextLength);
            err = KErrTooBig;
            }
        
        if (aRemoveNewLines)
            {
            // Replace new-line patterns with real ones
            TInt error = aValue.Find(KNewLinePattern);
            while (error != KErrNotFound)
                {
                // error is a position
                aValue.Replace(error, KNewLinePattern().Length(), KNewline );
                error = aValue.Find(KNewLinePattern);
                }
            }
        }

    file.Close();
    fs.Close();
    
    return err;
    }
// GetSWVersion.  Read sw.txt and return concatenated to single line
// Copied from /common/generic/syslibs/bafl/sysutil/src/sysutil.cpp but without caching functionality
static TInt GetSWVersion(TDes& aValue)
    {
    TInt err = GetTextFromFile( KSWVersionFileName, aValue, ETrue );


    if ( err != KErrNone )
        {
        DEBUG_PRINTF3(_L8("Error: %d, while processing: %S"),err, &KSWVersionFileName);
        }

    return err;
    }

const TInt KInfoBufLength=KSysUtilVersionTextLength;
_LIT(KROMVersionStringCacheDir, ":\\sys\\install\\sisregistry\\");
_LIT(KROMVersionStringCacheFileName, "ROMVersionStringCache_localSysUtil.bin");


TBool CSisRegistrySession::IsFirmwareUpdatedL()
    {
    TChar sysDrive = RFs::GetSystemDriveChar();
    TInt maxSizeofFileName = KROMVersionStringCacheDir().Length() + KROMVersionStringCacheFileName().Length() + 1;
    RBuf romVersionCacheFileName;
    romVersionCacheFileName.CreateL(maxSizeofFileName);
    romVersionCacheFileName.CleanupClosePushL();
    romVersionCacheFileName.Append(sysDrive);
    romVersionCacheFileName.Append(KROMVersionStringCacheDir());
    romVersionCacheFileName.Append(KROMVersionStringCacheFileName());
    
    //Read the length & value from it, if any.
    RFileReadStream romVerStream;
    TInt err = romVerStream.Open(iFs,romVersionCacheFileName,EFileRead);
    if ( err != KErrNone )
        {
        CleanupStack::PopAndDestroy(1);
        return ETrue;
        }
    CleanupClosePushL(romVerStream);
    TBuf<KInfoBufLength> version;
    TUint32 length = romVerStream.ReadUint32L();
    if (length>KInfoBufLength)
        {
        //File must be corrupt, an attempt to read will panic
        //User::Leave(KErrCorrupt);
        CleanupStack::PopAndDestroy(2);
        return ETrue;
        }   
    romVerStream.ReadL(version, length);

    //the persisted version has been successfully read
    //read the actual current version string
    TBuf<KInfoBufLength> actualVersion;

    err = GetSWVersion(actualVersion); //use the local implementation of the SysUtil::GetSWVersion function

    if(err == KErrNone)
        {
        if (version.Compare(actualVersion) != 0)
            {
            //Leave if the current version is different from the previous stored version and recreate applist.
            DEBUG_PRINTF(_L8("!!Firmware update detected!! Rebuilding AppList"));
            CleanupStack::PopAndDestroy(2);
            return ETrue;
            }
        }
    else
        {
        //Leave if any error reading the version information, except if file is not present
        if (err != KErrPathNotFound && err != KErrNotFound)
            {
            DEBUG_PRINTF2(_L8("!!Error %d reading Firmware version.  Rebuilding AppList"),err);
            CleanupStack::PopAndDestroy(2);
            return ETrue;
            }
        }
    CleanupStack::PopAndDestroy(2); //romVerStream, romVersionCacheFileName
    return EFalse;
    }

void CSisRegistrySession::UpdateRecentFWVersionL()
    {
        //Write a cache of the ROM version to a separate stream
        //Build the filename for the cache file
        TChar sysDrive = RFs::GetSystemDriveChar();
        TInt maxSizeofFileName = KROMVersionStringCacheDir().Length() + KROMVersionStringCacheFileName().Length() + 1;
        RBuf romVersionCacheFileName;
        romVersionCacheFileName.CreateL(maxSizeofFileName);
        romVersionCacheFileName.CleanupClosePushL();
        romVersionCacheFileName.Append(sysDrive);
        romVersionCacheFileName.Append(KROMVersionStringCacheDir());
        romVersionCacheFileName.Append(KROMVersionStringCacheFileName());
        
        //Read the length & value from it, if any.
        RFileWriteStream romVerStream;
        User::LeaveIfError(romVerStream.Replace(iFs,romVersionCacheFileName,EFileWrite));
        CleanupClosePushL(romVerStream);
        TBuf<KInfoBufLength> version;
        GetSWVersion(version);

        // Write even if SysUtil returns err since all conditions are taken care during restore.
        romVerStream.WriteUint32L(version.Length());
        romVerStream.WriteL(version);
        CleanupStack::PopAndDestroy(2); //romVerStream, romVersionCacheFileName
    }
