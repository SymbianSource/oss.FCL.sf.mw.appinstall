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
#include <startupdomainpskeys.h>
#include <e32const.h>

#include "sislauncherclient.h"
#include "swtypereginfo.h"
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
#include "securitycheckutil.h"

using namespace Swi;
using namespace Usif;

const TInt KUidHexLength = 8;
_LIT(KSWIRegFirstInitFile, "\\sys\\install\\sisregistry\\swiregfirstinit.log");
_LIT(KApparcRegFileNameFormat, "*10003a3f*");

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

	iIsFirstInit = IsFirstInvocationL();
	
#ifdef __WINSCW__
	_LIT_SECURE_ID(KAppArcSID, 0x10003A3F);
	if (KAppArcSID.iId == iClientSid.iId)
	    {
        DEBUG_PRINTF(_L8("SIS Registry Server: Init for a connection from AppArc"));
        ProcessApplicationsL();
	    }
	else
	    {
        if (iIsFirstInit)
            {
            DEBUG_PRINTF2(_L8("SIS Registry Server: Init for a connection from SID 0x%X"), iClientSid.iId);
            ProcessRomStubsL();
            }
	    }
#else
	TInt res = KErrNone;
	TRAP(res , res = IsFirmwareUpdatedL());
		
	if (iIsFirstInit || res)
		{
		if (res)
		    {
		    iIsFwUpdated = ETrue;
		    }

		FirstInvocationInitL();
		if (iIsFwUpdated)
		    {
			TRAP(res, UpdateRecentFWVersionL());
			if (res != KErrNone)
                {
                DEBUG_PRINTF2(_L8("Updating recent Firmware Version failed with error code = %d."), res);
                }
		    }		
		}
#endif
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

void CSisRegistrySession::ProcessRomStubsL()
    {
    DEBUG_PRINTF(_L8("SIS Registry Server - Starting processing of ROM Stub sis files"));
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
    CleanupStack::PopAndDestroy(2, &fileName);  // file
    
    DEBUG_PRINTF(_L8("SIS Registry Server - Completed processing of ROM Stub sis files"));
    }

void CSisRegistrySession::ProcessApplicationsL()
    {
    TComponentId compId = 0;
    DEBUG_PRINTF(_L8("SIS Registry Server - Deleteing all existing ROM applications."));
    ScrHelperUtil::DeleteApplicationEntriesL(iScrSession, compId);
    
    //Register all apps found in \private\10003a3f\apps
    TDriveUnit romDrive(SisRegistryUtil::SystemRomDrive());
    RBuf apparcRegFilePath;
    apparcRegFilePath.CreateL(romDrive.Name(), KMaxPath);
    CleanupClosePushL(apparcRegFilePath);
    apparcRegFilePath.Append(KApparcRegFilePath);
    
	//Create a single transaction to register all in rom apps
	iScrSession.CreateTransactionL();
	RegisterAllAppL(apparcRegFilePath);

    //Register all apps found in \private\10003a3f\import\apps
    apparcRegFilePath = romDrive.Name();
    apparcRegFilePath.Append(KApparcRegFileImportPath);
    RegisterAllAppL(apparcRegFilePath);
    
	//Register all apps found in UDA
	TDriveUnit systemDrive(iSystemDrive);

	//Register all apps found in <systemdrive>\private\10003a3f\apps
	apparcRegFilePath = systemDrive.Name();	
	apparcRegFilePath.Append(KApparcRegFilePath);
	RegisterAllAppL(apparcRegFilePath);

	//Register all apps found in <systemdrive>\private\10003a3f\import\apps
    apparcRegFilePath = systemDrive.Name();
    apparcRegFilePath.Append(KApparcRegFileImportPath);
    RegisterAllAppL(apparcRegFilePath);
	
	iScrSession.CommitTransactionL();    
	
	CleanupStack::PopAndDestroy(&apparcRegFilePath);
    
    DEBUG_PRINTF(_L8("SIS Registry Server - Completed processing of all existing ROM applications."));
    }

// Does initialization required when run after very first boot of phone (or after firmware upgrade)
// Method is only invoked when such a state is detected (only in device, NOT in emulator)
// Leaves behind a file in <systemdrive>:\sys to mark a successful initialization 
void CSisRegistrySession::FirstInvocationInitL()
	{
	// Add the ROM installed stub details to SCR
	// Create an SCR transaction, so that entries won't be added to the SCR if the function leaves
    TInt value(EIdlePhase1NOK);
    TInt ret = RProperty::Get(KPSUidStartup, KPSIdlePhase1Ok, value);
	
    TBool isFromSWIDaemon = EFalse;
    _LIT_SECURE_ID(KSWIDaemonSID, 0x10202DCE);
    if (KSWIDaemonSID.iId == iClientSid.iId)
        { 
        isFromSWIDaemon = ETrue;
        }
	
	//If the image is NCP 'KPSUidStartup' property is not set, hence settig it explicitly for processing stubs
	if(ret == KErrNotFound)
		{
		DEBUG_PRINTF(_L8("SIS Registry Server - 'KPSUidStartup' property is not set"));
		value = EIdlePhase1Ok;
		}
	else
		{
		DEBUG_PRINTF(_L8("SIS Registry Server - 'KPSUidStartup' property is set"));
		}
	if ((value == EIdlePhase1Ok || isFromSWIDaemon) && iIsFirstInit)
	    {
        ProcessRomStubsL();
	    }

	if (iIsFwUpdated)
	    {
		// Delete the SisRegistry marker file
        RBuf fileName;
        CleanupClosePushL(fileName);
        fileName.CreateL(KMaxPath);
        TDriveUnit drive(iSystemDrive);
        fileName = drive.Name();
        fileName.Append(KSWIRegFirstInitFile);
        iFs.Delete(fileName);
        CleanupStack::PopAndDestroy(&fileName);

        ProcessApplicationsL();
	    }
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
    case EAddAppRegEntry:
        AppRegInfoEntryL(aMessage);
        break;
    case EUpdateAppRegEntry:
        AppRegInfoEntryL(aMessage);
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
	case ESetComponentPresence:
	    SetComponentPresenceL(aMessage);
	    break;
	case EIsFileRegistered:
 		IsFileRegisteredL(aMessage);
 		break;			
	case EComponentIdForUid:
		GetComponentIdForUidL(aMessage);
		break;
	case EComponentIdForPackage:
        GetComponentIdForPackageL(aMessage);
        break;
	case EAppUidsForComponent:
	    GetAppUidsForComponentL(aMessage);
        break;
	case EComponentIdsForPackageUid:	    
        GetComponentIdsForUidL(aMessage);        
        break;
	case EAddAppRegInfo:
	    AddAppRegInfoL(aMessage);
	    break;
	case ERemoveAppRegInfo:
	    RemoveAppRegInfoL(aMessage);
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
	
	RCPointerArray<Usif::CSoftwareTypeRegInfo> swTypeRegInfoArray;
	CleanupClosePushL(swTypeRegInfoArray);
	SoftwareTypeRegInfoUtils::UnserializeArrayL(readStream, swTypeRegInfoArray);
	
	for (TInt i=0; i<swTypeRegInfoArray.Count(); ++i)
		{
		iScrSession.AddSoftwareTypeL(*swTypeRegInfoArray[i]);
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
		
		HBufC* stubFileName = NULL;		    
		if (!aNewEntry)
			{
			// Retrieve all the component details from SCR
			TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, object->Uid(), object->Index());
			// Fetch the stub file name, if any (this will be present only in the case of a ROM upgrade)
			stubFileName = GetStrPropertyValueL(iScrSession, compId, KStubFileName, KUnspecifiedLocale, EFalse);
			if (NULL != stubFileName)
			    {
			    CleanupStack::PushL(stubFileName);
			    }

			CSisRegistryObject* existingObject = CSisRegistryObject::NewLC();
			ScrHelperUtil::GetComponentL(iScrSession, compId, *existingObject);
	
			// DEF085506 fix. remove control files (Previous SA and any of PUs) also while SA upgrading.
			if (object->InstallType() == Sis::EInstInstallation)
				{
				RemoveEntryL(compId);
				RemoveCleanupInfrastructureL(*existingObject, stsSession);	
				}
			else // PartialUpgarde case remove only registry file.
				{
				// Essentially, this is an uninstall except we leave the controller file intact.
				RemoveEntryL(compId);
				}
			
			CleanupStack::PopAndDestroy(existingObject);
			if (NULL != stubFileName)
			    {
			    CleanupStack::Pop(stubFileName);
			    }			
			}
		
		if (NULL != stubFileName)
		    {
		    CleanupStack::PushL(stubFileName);
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
		
		// the stub file name needs to be added to SCR in the case of a SA or PU rom upgrade
        // or in case of an indirect rom upgardes like (Stub -> SA+RU -> SA+RU) or (Stub -> PU -> PU).
        if (NULL != stubFileName)
            {
            iScrSession.SetComponentPropertyL(compId, KStubFileName, *stubFileName);
            CleanupStack::PopAndDestroy(3, &stsSession); // bufferCtrl, stubFileName
            }
        else
            {
            CleanupStack::PopAndDestroy(2, &stsSession); // bufferCtrl
            }
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
		
	DeleteEntryL(*object, transactionID, ETrue);
	
 	CleanupStack::PopAndDestroy(object);
	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::DeleteEntryL(const CSisRegistryObject& aObject, TInt64 aTransactionId, TBool aCleanupRequired/*=ETrue*/)
    {
    // create a integrity service object
    Usif::RStsSession stssession;
    stssession.OpenTransactionL(aTransactionId);
    CleanupClosePushL(stssession);
        
    HBufC* stubFileName = RemoveEntryLC(aObject);
    RemoveCleanupInfrastructureL(aObject, stssession);
    
    // If removal is for ROM upgrade type, after removing the existing registry entry set,
    // regenerate the Registry Entry from the stub sis file. 
    if ((aObject.InstallType() == Sis::EInstInstallation || aObject.InstallType() == Sis::EInstPartialUpgrade) &&
            (NULL != stubFileName) && aCleanupRequired)
        {   
        TRAPD(res, RegisterInRomControllerL(*stubFileName));
        if (res != KErrNone)
            {
            DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to register stub after removal of rom upgarde. Error code %d."), res);
            }
        }
    
    if (NULL != stubFileName)
        {
        CleanupStack::PopAndDestroy(stubFileName);
        }
    CleanupStack::PopAndDestroy(&stssession);
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
	TUid tUid;	
	TPckg<TUid> packageUid(tUid);
	aMessage.ReadL(EIpcArgument0, packageUid, 0);
	
	TStubExtractionMode tMode;
	TPckg<TStubExtractionMode> packageMode(tMode);
	aMessage.ReadL(EIpcArgument1, packageMode, 0);
	
	TInt startingFileNo = 0;
	if (tMode == EGetFiles)
	    {
        TPckg<TInt> start(startingFileNo);
        aMessage.ReadL(EIpcArgument2, start, 0);
	    }
	
	TInt fileCount = 0;
	RPointerArray<HBufC> fileNames;
	CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(fileNames);
	                    
	TRAPD(errCode, GetStubFileInfoL(tUid, tMode, startingFileNo, fileCount, fileNames));
	if (KErrNone == errCode)
	    {
	    if (tMode == EGetFiles)
	        {
	        // Stream via multiple IPC writes instead of
	        // copying to a buffer and streaming from there.
	        RIpcWriteStream ipcstream;
	        ipcstream.Open(aMessage, EIpcArgument3);
	        CleanupClosePushL(ipcstream);
	        ExternalizePointerArrayL(fileNames,ipcstream);
	        ipcstream.CommitL();
	        aMessage.Complete(KErrNone);
	        CleanupStack::PopAndDestroy(&ipcstream);
	        }
	    else if (tMode == EGetCount)
	        {
	        TPckgBuf<TInt> fileCnt(fileCount);
	        aMessage.WriteL(EIpcArgument2, fileCnt);
            aMessage.Complete(KErrNone);      
	        }
	    }
	else
	    {
	    aMessage.Complete(errCode);
	    }
	CleanupStack::PopAndDestroy();
	}

void CSisRegistrySession::GetStubFilesL(const TDesC& aFileName, RPointerArray<HBufC>& aFileNames)
    {
    // Read the ROM stub controller
    CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(iFs, aFileName);
    Swi::Sis::CController* stubController = NULL;
    TRAPD(errCode, stubController = Swi::Sis::CController::NewL(*fileProvider));
    if (errCode != KErrNone)
        {
        // Ignore the broken stub file under the ROM stub directory.
        DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to read the stub controller. Error code %d."), errCode);
        CleanupStack::PopAndDestroy(fileProvider);
        User::Leave(errCode);
        }
    CleanupStack::PushL(stubController);
    const RPointerArray<Sis::CFileDescription>& filesArray = stubController->InstallBlock().FileDescriptions();
    TInt totalFileCount = filesArray.Count();
    // Populate the files in to a temporary array.
    for(TInt fileCount = 0; fileCount < totalFileCount; ++fileCount )
        {
        HBufC* fileName = filesArray[fileCount]->Target().Data().AllocLC();
        // Adding drive letter of rom if not mentioned in stub sis file
        TPtr a=fileName->Des();
        if (a[0] == '!')
            {
            TChar driveChar;
            User::LeaveIfError(iFs.DriveToChar(EDriveZ, driveChar));
            a[0] = driveChar;
            }

		aFileNames.AppendL(fileName);
		CleanupStack::Pop(fileName);
		}
    CleanupStack::PopAndDestroy(2, fileProvider);
    }

void CSisRegistrySession::GetStubFileInfoL(TUid aUid, TStubExtractionMode aMode, TInt aStartingFileNo, TInt& aFileCount, RPointerArray<HBufC>& aFileNames)
    {
    TComponentId compId = 0;
        
    // Get the component Id for the pacakge
    if (!ScrHelperUtil::GetComponentIdL(iScrSession, aUid, 0, compId))
        {
        DEBUG_PRINTF2(_L("Sis Registry Server - GetStubFileInfoL - UID 0x%08x not found in Software Component Registry"), aUid.iUid);
        User::Leave(KErrNotFound);
        }
    
    // Get the stub file name for the given component Id.
    //
    // NOTE: Here in, if the 'compId' being checked does not contain a 'StubFileName' property the 
    // installation will abort. This is since, such a situation (a rom upgarde with no stub name)
    // would mean that there is an error in install logic.
    HBufC* stubFileName = GetStrPropertyValueL(iScrSession, compId, KStubFileName);
    CleanupStack::PushL(stubFileName);
        
    // Read the ROM stub controller
    CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(iFs, *stubFileName);
    Swi::Sis::CController* stubController = NULL;
    TRAPD(errCode, stubController = Swi::Sis::CController::NewL(*fileProvider));
    if (errCode != KErrNone)
        {
        // The stub file under the ROM stub directory is broken
        DEBUG_PRINTF2(_L8("Sis Registry Server - GetStubFileInfoL - Failed to read the stub controller. Error code %d."), errCode);
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(stubController);
        
    // If the UID in a stub file matches the current package's UID,
    // populate the list of eclipsable files from the same stub file.
    if (stubController->Info().Uid().Uid() == aUid)
        {
        const RPointerArray<Sis::CFileDescription>& filesArray = stubController->InstallBlock().FileDescriptions();
        // Get as many number of files as possible that can be accomodate in client allocated buffer.
        if (aMode == EGetFiles)
            {
            TInt sizeRed = 0;
            TInt totalFileCount = filesArray.Count();
            // Populate the files in to a temporary array.
            for(TInt fileCount = aStartingFileNo; fileCount < totalFileCount; ++fileCount )
                {
                sizeRed += filesArray[fileCount]->Target().Data().Size();
                // If amount of data red exceeds the client buffer size, break reading.
                if (sizeRed > KDefaultBufferSize)
                    {
                    break;
                    }
                        
                // Only create a TPtrC when we know we have space available
                HBufC* fileName = filesArray[fileCount]->Target().Data().AllocLC();
                // Adding drive letter of rom if not mentioned in stub sis file
                TPtr a=fileName->Des();
                if (a[0] == '!')
                    {
                    TChar driveChar;
                    User::LeaveIfError(iFs.DriveToChar(EDriveZ, driveChar));
                    a[0] = driveChar;
                    }                        
               
                aFileNames.AppendL(fileName);
                CleanupStack::Pop(fileName);                        
                } 
            }            
        else if (aMode == EGetCount) // If only the count needed, send the stub file's total entrie's count.
            {
            aFileCount = filesArray.Count();
            }               
        }
    else
        {
        DEBUG_PRINTF3(_L("Sis Registry Server - GetStubFileInfoL - Stub file %S does not have UID 0x%08x"), stubFileName, aUid.iUid);
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::PopAndDestroy(3, stubFileName); // fileProvider, stubController    
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
	TInt addedDrive;
	TPckg<TInt> pkgDrive(addedDrive);
	aMessage.ReadL(EIpcArgument0, pkgDrive);

	DEBUG_PRINTF2(_L8("Sis Registry Server - Removable drive %d added."), addedDrive);
	    
	// Get the drive character.
	TChar drive;
	User::LeaveIfError(iFs.DriveToChar(addedDrive, drive));
	TUint driveChar(drive);
	    
	// Retrieve drive info.
	TDriveInfo driveInfo;
	User::LeaveIfError(iFs.Drive(driveInfo, addedDrive));
#ifndef __WINSCW__	   
	if(driveInfo.iDriveAtt & KDriveAttLogicallyRemovable)
#endif	    
	    {
	    /*
	    In case a logically removable drive is added,
	    Look for the presence of the first boot marker file corresponding to it in the sisregistry private
	    folder in C drive. If absent, assume first boot and create the marker file. 
	    Also added a marker file in the <drive>\sys\install directory which would be used to detect a format. 
	         
	    Subsequent boots would look for the drive format marker to check if a format has occured and delete
	    the registry entries.
	    */
	    
	    // Create first boot marker path.
	    _LIT(KFirstBootMarkerFilePath, "%c:%SfirstBootMarkerFileFor%c");
	    RBuf privatePath;
		privatePath.CreateL(KMaxPath);
	    CleanupClosePushL(privatePath);
	    User::LeaveIfError(iFs.PrivatePath(privatePath));

	    RBuf firstBootMarkerFilePath;
		firstBootMarkerFilePath.CreateL(KMaxPath);
	    CleanupClosePushL(firstBootMarkerFilePath);

	    TChar systemDrive = RFs::GetSystemDriveChar();
	    firstBootMarkerFilePath.Format(KFirstBootMarkerFilePath, static_cast<TUint>(systemDrive), &privatePath, driveChar); 
	        
	    // Create drive format marker path.
	    _LIT(KFormatMarkerPath, "%c:\\sys\\install\\formatMarkerFile");
	    
		RBuf formatMarkerPath;
		formatMarkerPath.CreateL(KMaxPath);
	    CleanupClosePushL(formatMarkerPath);
		formatMarkerPath.Format(KFormatMarkerPath, driveChar); 
	       
		// The drive marker files are marked hidden and read-only.
        TUint fileAttributes = KEntryAttReadOnly | KEntryAttHidden;

	    if(!SisRegistryUtil::FileExistsL(iFs, firstBootMarkerFilePath))
	        {
	        // First boot detected. Add the first boot marker file as well as the format marker on the drive.
	        SisRegistryUtil::CreateFileWithAttributesL(iFs, firstBootMarkerFilePath);
	        TRAPD(err,SisRegistryUtil::CreateFileWithAttributesL(iFs, formatMarkerPath, fileAttributes);)
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}   
	        }
	    else
	        {
	        // Subsequent boot. Check for the presence of a marker file <drive>\sys\install directory.
	        // If absent, assume format.
	        if(!SisRegistryUtil::FileExistsL(iFs, formatMarkerPath))
	            {
	            DriveFormatDetectedL(TDriveUnit(addedDrive));
	            //Add missing format marker file
	            SisRegistryUtil::CreateFileWithAttributesL(iFs, formatMarkerPath, fileAttributes);
	            }           
	        }
		CleanupStack::PopAndDestroy(3, &privatePath);
	    }
	    
	// Find flagging controllers for non-preinstalled packages on
	// this drive and do cleanup if necessary
	ProcessRemovableDriveL(addedDrive);

	aMessage.Complete(KErrNone);
	}

void CSisRegistrySession::RequestSidToPackageL(const RMessage2& aMessage)
	{
	TUid sid; 
	TPckg<TUid> executableSid(sid);
	
	aMessage.ReadL(EIpcArgument0, executableSid);

	// Retrieve the component Id's list for the Sid.
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	
	GetComponentIdsForSidL(sid, componentIdList);
	
	if(componentIdList.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
		
	CSisRegistryPackage *package = NULL;
	ScrHelperUtil::ReadFromScrL(iScrSession, componentIdList[0], package);
	CleanupStack::PushL(package);
	
	DEBUG_PRINTF5(_L("Sis Registry Server - SID 0x%08x is owned by package UID: 0x%08x, Name: %S, Vendor: %S."),
		sid.iUid, package->Uid().iUid, &(package->Name()), &(package->Vendor()));
	
	SisRegistryUtil::SendDataL(aMessage, *package, EIpcArgument1);
	CleanupStack::PopAndDestroy(2, &componentIdList);
	
	}

void CSisRegistrySession::RequestSidToFileNameL(const RMessage2& aMessage)
	{
	TInt thirdParameter; 
	TUid sid;
	TPckg<TUid> fileSid(sid);

	// Obtain the sid from argument slot 0 from the client
	aMessage.ReadL(EIpcArgument0, fileSid);

	// Obtain drive from argument slot 2 from the client.
	// This parameter will be set to -1 if user did not specified a drive with this request.
	thirdParameter = aMessage.Int2(); 

	// Obtain the componentId and array index of CompSID<index> custom property matching a given aSid.
	TComponentId componentId = 0;
	HBufC* fileName = SidToFileNameL(sid, componentId, thirdParameter);
	
	if(componentId == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}
	
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
	RArray<TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	
	// If the supplied SID is present in SCR, then a call to GetComponentIdsForSidL succeeds.
	// Else componentIdList would be empty.	
	GetComponentIdsForSidL(uid(), componentIdList);
	
	if(componentIdList.Count() != 0)
		{
		isPresent = ETrue;
		}
	CleanupStack::PopAndDestroy(&componentIdList);
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
	TBool isInRom = RomBasedPackageL(tuid());
	
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

void CSisRegistrySession::SetComponentPresenceL(const RMessage2& aMessage)
    {
    TPckgBuf<Usif::TComponentId> componentId;
    TPckgBuf<TBool> componentPresence;
    aMessage.ReadL(EIpcArgument0, componentId, 0);
    aMessage.ReadL(EIpcArgument1, componentPresence, 0);
    iScrSession.SetIsComponentPresentL(componentId(), componentPresence());
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


void CSisRegistrySession::RemoveEntryL(const TComponentId aCompId)
    {
    iScrSession.DeleteApplicationEntriesL(aCompId);
    iScrSession.DeleteComponentL(aCompId);
    }

HBufC* CSisRegistrySession::RemoveEntryLC(const CSisRegistryPackage& aPackage)
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
	
	// Get the name of the stub, if any
	HBufC* stubFileName = GetStrPropertyValueL(iScrSession, compId, KStubFileName, KUnspecifiedLocale, EFalse);
	if (NULL != stubFileName)
	    {
        DEBUG_PRINTF3(_L("Package with UID 0x%08x has a stub (%S)."), aPackage.Uid().iUid, stubFileName);
        CleanupStack::PushL(stubFileName);
	    }
	
	RemoveEntryL(compId);
	
	return stubFileName;
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

TComponentId CSisRegistrySession::AddInRomControllerEntryL(CSisRegistryObject& aObject, const TDesC& aStubFileName)
    {
    // Check if we are dealing with a stub
    ASSERT(KDefaultIsInRom == aObject.InRom());
    
    TComponentId compId = AddEntryL(aObject, Usif::EScrCompHidden); // EScrCompHidden is supplied not to create any log for the ROM controller on the SCR side.
    iScrSession.SetComponentPropertyL(compId, KStubFileName, aStubFileName);
    return compId;
    }
	
void CSisRegistrySession::AddAppsFromStubL(TComponentId aCompId, const TDesC& aFileName)
    {
    RPointerArray<HBufC> romFiles;
    CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(romFiles);
    GetStubFilesL(aFileName, romFiles);
    RPointerArray<HBufC> apparcRegFiles;
    CleanupResetAndDestroy<RPointerArray<HBufC> >::PushL(apparcRegFiles);
       
    for(TInt i=0; i< romFiles.Count(); i++)
        {
        TInt ret = romFiles[i]->Match(KApparcRegFileNameFormat);
        
        TBuf<100> fileName(*romFiles[i]);
        if (KErrNotFound != ret)
            {
            HBufC* file = romFiles[i]->AllocL();
            apparcRegFiles.Append(file);
            }
        }
             
	const TInt appCount = apparcRegFiles.Count();
	if (appCount)
		{
		RSisLauncherSession launcher;
		CleanupClosePushL(launcher);
		User::LeaveIfError(launcher.Connect());
		RFs fs;
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(fs.ShareProtected());
    
		for (TInt i=0; i<appCount; ++i)
			{
			TInt bufLen = apparcRegFiles[i]->Size();
			HBufC* buf = HBufC::NewLC(bufLen);
			TPtr appFile = buf->Des();
			appFile.Format(_L("%S"), apparcRegFiles[i]);
			appFile.Replace(0, 1, _L("z"));
			
			RFile file;
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(fs, appFile, EFileRead));
			Usif::CApplicationRegistrationData* appRegData  = launcher.SyncParseResourceFileL(file);
			CleanupStack::PushL(appRegData);
			
			TRAPD(err, ScrHelperUtil::AddApplicationEntryL(iScrSession, aCompId, *appRegData));
			if (err == KErrAlreadyExists)
                {
                // Delete the existing application entry, which is not associated with any package 
                ScrHelperUtil::DeleteApplicationEntryL(iScrSession, appRegData->AppUid());
                ScrHelperUtil::AddApplicationEntryL(iScrSession, aCompId, *appRegData);
                }
			
			CleanupStack::PopAndDestroy(appRegData);
			CleanupStack::PopAndDestroy(2, buf);
			}
		CleanupStack::PopAndDestroy(2, &launcher);    //fs 
		}
	CleanupStack::PopAndDestroy(2, &romFiles);        //apparcRegFiles        
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

void CSisRegistrySession::GetComponentIdsForSidL(TUid aSid, RArray<TComponentId>& aComponentIds)
	{   
    _LIT(KComponentSidPropertyRegex, "CompSid%");
	CComponentFilter* componentFilter = CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(KSoftwareTypeNative);	
	componentFilter->AddPropertyL(KComponentSidPropertyRegex, static_cast<TUint>(aSid.iUid), CComponentFilter::ELike, CComponentFilter::EEqual);		
	
	// Retrieve all component Id's 
	iScrSession.GetComponentIdsL(aComponentIds, componentFilter);
	
	CleanupStack::PopAndDestroy(componentFilter);
	}
	

HBufC* CSisRegistrySession::SidToFileNameL(TUid aSid, TComponentId& aComponentId, TInt aExpectedDrive)
    {    
    aComponentId = KErrNotFound;
    RArray<TComponentId> componentIdList;
    CleanupClosePushL(componentIdList);
    
    // Retrieve the component Id's.
    GetComponentIdsForSidL(aSid, componentIdList);
    
    if(componentIdList.Count() == 0)
        {
        CleanupStack::PopAndDestroy(&componentIdList);
        return NULL;
        }
    
    RArray<TUid> sidArray;
    CleanupClosePushL(sidArray);
    TInt index(0);
    TBuf<KSmlBufferSize> compSidFileName(KEmptyString);
    HBufC* fileName = NULL;
    
    
    if(aExpectedDrive == -1)
        {
        // No drive specified, just return the first component Id and the corresponding Sid index.  
        aComponentId = componentIdList[0];
        sidArray.Reset();
        ScrHelperUtil::InternalizeSidArrayL(iScrSession, aComponentId, sidArray);
        index = sidArray.Find(aSid);   
        compSidFileName.Format(KCompSidFileNameFormat, index);   
        fileName = GetStrPropertyValueL(iScrSession, aComponentId, compSidFileName); 
        }
    else
        {
        // If the search is narrowed to a particular drive, get the file name and check whether it matches the drive
    
        for(TInt i=0; i<componentIdList.Count(); ++i)
            {
            sidArray.Reset();
            ScrHelperUtil::InternalizeSidArrayL(iScrSession, componentIdList[i], sidArray);
            index = sidArray.Find(aSid);
            
            compSidFileName.Format(KCompSidFileNameFormat, index); 
            fileName = GetStrPropertyValueL(iScrSession, componentIdList[i], compSidFileName);   
			CleanupStack::PushL(fileName);
          
            if (fileName->Length() == 0)
                {
				 CleanupStack::PopAndDestroy(fileName);
                 fileName = NULL;
                 continue;
                }
            
            TInt drive; 
            User::LeaveIfError(RFs::CharToDrive((*fileName)[0], drive));
            if(drive != aExpectedDrive)
                {
                CleanupStack::PopAndDestroy(fileName);
                fileName = NULL;
                continue;
                }        
            
            //Expected drive found !
            aComponentId = componentIdList[i];
			CleanupStack::Pop(fileName);
            break;
            }     
        } 	
	CleanupStack::PopAndDestroy(2, &componentIdList);	
	return fileName; // Ownership with caller.
			
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
    if ( iIsFirstInit && isStubRegistered )
	    {
		TComponentId compId = ScrHelperUtil::GetComponentIdL(iScrSession, object->Uid(), object->Index());
		TSisPackageTrust trustStatus;
		trustStatus = static_cast<TSisPackageTrust>(GetIntPropertyValueL(iScrSession, compId, KCompTrust,  EFalse, KDefaultTrustValue));
	    if ( ESisPackageBuiltIntoRom == trustStatus )
	        {
            DEBUG_PRINTF2(_L8("Sis Registry Server - Attempting to delete registry entry 0x%08x as a firmware update detected"), object->Uid().iUid);
            
            // If the component being removed has registered software types, unregister them now.
            // This operation deletes MIME types mapping from AppArc and therefore is not transactional.
            UnregisterSoftwareTypesL(compId);
                
            RemoveEntryL(compId);           

            overwriteRegEntry = ETrue;
	        }
	    }

	if ( !isStubRegistered || overwriteRegEntry )
		{
		TComponentId compId = AddInRomControllerEntryL(*object, aFileName);
		AddAppsFromStubL(compId, aFileName);
		
		// store a copy of the controller
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(object->Uid(), object->Index(), 0);
		
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
    
    iFs.MkDirAll(romVersionCacheFileName);
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

void CSisRegistrySession::AppRegInfoEntryL(const RMessage2& aMessage)
    {
    Usif::TComponentId componentId = aMessage.Int0();

	RIpcReadStream stream;
	CleanupClosePushL(stream);
	stream.Open(aMessage, 1);
	CApplicationRegistrationData *applicationData = CApplicationRegistrationData::NewL(stream);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PushL(applicationData);

    TRAPD(err, ScrHelperUtil::AddApplicationEntryL(iScrSession, componentId, *applicationData));
    if (err == KErrAlreadyExists)
        {
        //Delete the existing application
        ScrHelperUtil::DeleteApplicationEntryL(iScrSession, applicationData->AppUid());
        err = KErrNone;
        TRAP(err, ScrHelperUtil::AddApplicationEntryL(iScrSession, componentId, *applicationData));
        }
    aMessage.Complete(err);
    CleanupStack::PopAndDestroy(applicationData);
    }

void CSisRegistrySession::GetComponentIdForPackageL(const RMessage2& aMessage)
    {
    Usif::TComponentId componentId(0);
    TInt strLen = aMessage.GetDesLengthL(EIpcArgument0);
    HBufC* packageName = HBufC::NewLC(strLen);
    TPtr pkgname = packageName->Des();
    aMessage.ReadL(EIpcArgument0, pkgname, 0);
    
    strLen = aMessage.GetDesLengthL(EIpcArgument1);
    HBufC* vendorName = HBufC::NewLC(strLen);
    TPtr vndrname = vendorName->Des();
    aMessage.ReadL(EIpcArgument1, vndrname, 0);
    componentId = ScrHelperUtil::GetComponentIdL(iScrSession, *packageName, *vendorName);
    if (!componentId)
        {
        DEBUG_PRINTF2(_L8("Sis Registry Server - Component id for package Name %S was not found."), packageName);
        User::Leave(KErrNotFound);
        } 
    CleanupStack::PopAndDestroy(2, packageName); //vendorName
    TPckgC<Usif::TComponentId> componentIdPckg(componentId);
    aMessage.WriteL(EIpcArgument2, componentIdPckg);
    aMessage.Complete(KErrNone);
    }

void CSisRegistrySession::GetAppUidsForComponentL(const RMessage2& aMessage)
    {
    Usif::TComponentId compId;
    TPckg<Usif::TComponentId> componentId(compId);
    RArray<TUid> appUids;
    CleanupClosePushL(appUids);
    aMessage.ReadL(EIpcArgument0, componentId);
    iScrSession.GetAppUidsForComponentL(compId, appUids);        
    SisRegistryUtil::SendDataArrayL(aMessage, appUids, EIpcArgument1);        
    CleanupStack::PopAndDestroy(&appUids);
    }

void CSisRegistrySession::GetComponentIdsForUidL(const RMessage2& aMessage)
    {
    TUid Uid;
    TPckg<TUid> pkgUid(Uid);
    aMessage.ReadL(EIpcArgument0, pkgUid);
    
    // Setup a filter based on the input parameters
    Usif::CComponentFilter* compFilter = Usif::CComponentFilter::NewLC();
    compFilter->AddPropertyL(KCompUid, Uid.iUid);   
    compFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);    
    RArray<Usif::TComponentId> componentIdList;
    CleanupClosePushL(componentIdList);
    iScrSession.GetComponentIdsL(componentIdList, compFilter);    
    if(componentIdList.Count() == 0)
        {
        // No components are found in the SCR. So, return false
        CleanupStack::PopAndDestroy(2, compFilter); // componentIdList
        DEBUG_PRINTF2(_L8("Sis Registry Server - Unable to find component for UID: %x"), Uid.iUid);  
        SisRegistryUtil::SendDataArrayL(aMessage, componentIdList, EIpcArgument1);
        return;
        }
    SisRegistryUtil::SendDataArrayL(aMessage, componentIdList, EIpcArgument1);        
    CleanupStack::PopAndDestroy(2, compFilter);    
    }

void CSisRegistrySession::RegisterAllAppL(RBuf& aApparcRegFilePath)
    {
	CDir* dir;
	TInt err = iFs.GetDir(aApparcRegFilePath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);	
	DEBUG_PRINTF3(_L("Sis Registry Server - Reading apparc registration file directory(%S) returned %d."), &aApparcRegFilePath, err);
	
	if (err == KErrNone)
		{
		CleanupStack::PushL(dir);

		TInt count(dir->Count());
		RBuf appRegFileName;
		appRegFileName.CreateL(KMaxFileName);
		CleanupClosePushL(appRegFileName);
		RSisLauncherSession launcher;
		CleanupClosePushL(launcher);
		User::LeaveIfError(launcher.Connect());
		RFs fs;
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(fs.ShareProtected());
		for (TInt index = 0; index < count; ++index)
			{
			appRegFileName = TParsePtrC(aApparcRegFilePath).DriveAndPath();
			appRegFileName.Append((*dir)[index].iName);
			RFile file;
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(fs, appRegFileName, EFileRead));
			
			Usif::CApplicationRegistrationData* appRegData = NULL;	
			TRAPD(result, appRegData = launcher.SyncParseResourceFileL(file));
			if (result == KErrNone)
			    {
                CleanupStack::PushL(appRegData);
                TComponentId compId = 0; // ROM application
                TRAPD(res, ScrHelperUtil::AddApplicationEntryL(iScrSession, compId, *appRegData););
                if (res != KErrNone)
                    {
                    // log it only, we cannot stop as the next might be ok
                    DEBUG_PRINTF3(_L("Sis Registry Server - Failed to register in ROM application %S. Error code %d."), &appRegFileName, res);
                    }
                CleanupStack::PopAndDestroy(appRegData);
			    }
			else
			    DEBUG_PRINTF2(_L8("Sis Registry Server - Parsing in ROM application . Error code %d."), result);
			                
			CleanupStack::PopAndDestroy(&file);
			}

		CleanupStack::PopAndDestroy(4, dir); // appRegFileName, launcher, fs
		}
	else if(err != KErrPathNotFound)
		{
		User::Leave(err);
		}
    }

void CSisRegistrySession::DriveFormatDetectedL(TDriveUnit aDrive)
    {
    DEBUG_PRINTF2(_L("Sis Registry Server - Drive format detected for drive %d"), static_cast<TInt>(aDrive));
    
    Usif::RStsSession stsSession;
    TInt64 transactionId = stsSession.CreateTransactionL();
    CleanupClosePushL(stsSession);
    
	//Create SCR Transaction
    iScrSession.CreateTransactionL();
    
    RArray<TComponentId> foundComponentIds;
    CleanupClosePushL(foundComponentIds);
        
    TDriveList filterFormatDrive;
    filterFormatDrive.FillZ(KMaxDrives);
    filterFormatDrive[static_cast<TInt>(aDrive)] = 1;
    
    CComponentFilter* componentFilter = CComponentFilter::NewLC();
    componentFilter->SetInstalledDrivesL(filterFormatDrive);
        
    iScrSession.GetComponentIdsL(foundComponentIds, componentFilter);
    
    TBool processRomDrive = EFalse;
    for(TInt i=0; i < foundComponentIds.Count(); ++i)
        {
        CSisRegistryObject* object = CSisRegistryObject::NewLC();
        ScrHelperUtil::GetComponentL(iScrSession, foundComponentIds[i], *object);
        TUint installedDrives = object->Drives();
            
        if(installedDrives & (1 << aDrive))
            {
            if (!processRomDrive && 
                    (object->InstallType() == Sis::EInstInstallation || object->InstallType() == Sis::EInstPartialUpgrade) &&
                        RomBasedPackageL(object->Uid()))
                {   
                processRomDrive = ETrue;                   
                }  
                    
            // Retrieve all the associated files.
            RPointerArray<CSisRegistryFileDescription>& fileDescriptions = object->FileDescriptions();
            _LIT(KHashPathFormat, "%c:\\sys\\hash\\%S");
            for(TInt j=0; j<fileDescriptions.Count(); ++j)
                {
                const TDesC& targetPath = fileDescriptions[j]->Target();
                        
                // Get the drive on which the file is present.
                TInt drive; 
                User::LeaveIfError(RFs::CharToDrive(targetPath[0], drive));
                        
                // If the file is a binary ( present in \sys\bin), delete the corresponding hash present in 
                // C:\sys\hash
                        
                if(KErrNotFound != targetPath.FindF(KBinPath))
                    {
                    // Retrieve the filename from the target path.
                    TParsePtrC parser(targetPath);
					HBufC* fileName = parser.NameAndExt().AllocLC();
                    
                    TChar systemDrive = RFs::GetSystemDriveChar();
                    
					// Create the hash file path.
                    RBuf hashFilePath;
                    hashFilePath.CreateL(KMaxPath);
                    CleanupClosePushL(hashFilePath);
                    hashFilePath.Format(KHashPathFormat, static_cast<TUint>(systemDrive), fileName);
                            
                    // Delete hash file.
                    SisRegistryUtil::DeleteFile(iFs, hashFilePath); //Ignore return code.
                    
					CleanupStack::PopAndDestroy(2, fileName);
					}
                        
                if(aDrive == TDriveUnit(drive))
                    {
                    // File does not exist on disk as the drive is assumed to have been formatted.
                    continue;
                    }
                SisRegistryUtil::DeleteFile(iFs, targetPath); //Ignore return code.           
                }
                    
            DeleteEntryL(*object, transactionId, EFalse);
            CleanupStack::PopAndDestroy(object);
            }
        }

    if(processRomDrive)
        {
        // Re-add the ROM installed stub details to SCR (only those missing will be added)
        ProcessRomDriveL();
        }
    
    //Commit the changes.
    stsSession.CommitL();
    iScrSession.CommitTransactionL();

    CleanupStack::PopAndDestroy(3, &stsSession);
    }

void CSisRegistrySession::AddAppRegInfoL(const RMessage2& aMessage)
    {   
    //This API is for use during development/debug use only; not used in production software
    
    TUint regFileNameLen = aMessage.GetDesLengthL(EIpcArgument0);
    HBufC* regFileName = HBufC::NewLC(regFileNameLen);
    TPtr namePtr = regFileName->Des();
    aMessage.ReadL(EIpcArgument0, namePtr);
    
    Usif::CApplicationRegistrationData* appRegData = NULL;
    //Check if the file name passed is valid reg file or not , if valid then parse
    TRAPD(result, ValidateAndParseAppRegFileL(*regFileName, appRegData, EFalse));
    
    if (result == KErrNone)
        {
        CleanupStack::PushL(appRegData);
        TComponentId compId = 0; 
        TUid appUid = appRegData->AppUid();
        //Check if component exists for the appUid, if then append the app reg info to the same compId(base)        
        TRAP_IGNORE(compId = iScrSession.GetComponentIdForAppL(appUid));
        TRAPD(res, ScrHelperUtil::AddApplicationEntryL(iScrSession, compId, *appRegData));
        if (res != KErrNone && res != KErrAlreadyExists )
            {
            DEBUG_PRINTF2(_L("Sis Registry Server - Failed to add app registration data of in the SCR . Error code %d."), res);
            CleanupStack::PopAndDestroy(2, regFileName); //appRegData
            aMessage.Complete(res);
            return;
            }        
        else if (res == KErrAlreadyExists)
            {   
            // Delete the existing application entry, which is not associated with any package 
            ScrHelperUtil::DeleteApplicationEntryL(iScrSession, appUid);            
            ScrHelperUtil::AddApplicationEntryL(iScrSession, compId, *appRegData);
            }
        
        //Notify Apparc of the new app
        RSisLauncherSession launcher;
        CleanupClosePushL(launcher);
        User::LeaveIfError(launcher.Connect());
        TAppUpdateInfo newAppInfo;
        RArray<TAppUpdateInfo> affectedApps;    
        CleanupClosePushL(affectedApps);
        newAppInfo = TAppUpdateInfo(appUid, EAppInstalled);
        affectedApps.AppendL(newAppInfo);
        launcher.NotifyNewAppsL(affectedApps);
        CleanupStack::PopAndDestroy(3, appRegData); // affectedApps, launcher
        }
    else
        {
        DEBUG_PRINTF2(_L8("Sis Registry Server - Parsing application registration info. Error code %d."), result);
        }
    
    CleanupStack::PopAndDestroy(regFileName);
    aMessage.Complete(result);
    }

void CSisRegistrySession::RemoveAppRegInfoL(const RMessage2& aMessage)
    {
    //This API is for use during development/debug use only; not used in production software
    
    TUint regFileNameLen = aMessage.GetDesLengthL(EIpcArgument0);
    HBufC* regFileName = HBufC::NewLC(regFileNameLen);
    TPtr namePtr = regFileName->Des();
    aMessage.ReadL(EIpcArgument0, namePtr);
    
    Usif::CApplicationRegistrationData* appRegData = NULL;
    // Check if the file name passed is valid reg file or not , if valid then parse it
    TRAPD(result, ValidateAndParseAppRegFileL(*regFileName, appRegData, EFalse));
    
    if(result == KErrNone)
        {
        TUid appUid = appRegData->AppUid();
        delete appRegData;
        appRegData = NULL;
    
        // Check if component exists for the appUid
        TComponentId compId(0);
        TRAPD(err,compId = iScrSession.GetComponentIdForAppL(appUid));
        
        // Check if appUid(reg file data to be removed) is registered with scr 
        if(err != KErrNone)
            {
            DEBUG_PRINTF2(_L8("Sis Registry Server -Reg file (%S) not registered with scr"), &regFileName);
            User::Leave(KErrNotFound);
            }
        
        TBool isAppReRegistered(ETrue);
        if(compId == 0)                                   // If component Id is 0 i.e. for in-rom *_reg.rsc files(without stub association) or inserted using AddAppRegInfoL().
            {                        
            TDriveUnit romDrive(SisRegistryUtil::SystemRomDrive());
            RBuf romRegFilesPath;
            CleanupClosePushL(romRegFilesPath);
            romRegFilesPath.CreateL(romDrive.Name(), KMaxPath);            
            romRegFilesPath.Append(KApparcRegFilePath);
           
            /* Scan the reg files in apparc's pvt folder's in rom, and compare their app uid with the uid of the reg file to be removed, 
             * if found register the reg file present in rom replacing the existing app
             */
            TRAPD(err, ReRegisterAppRegFileL(romRegFilesPath, appUid));
            CleanupStack::PopAndDestroy(&romRegFilesPath);
                        
            if(err == KErrNotFound)                       //If reg file is not found, search it in under "\\private\\10003a3f\\import\\apps\\*.rsc" path
                {
                RBuf romRegImportFilesPath;
                CleanupClosePushL(romRegImportFilesPath);
                romRegImportFilesPath.CreateL(romDrive.Name(), KMaxPath);                
                romRegImportFilesPath.Append(KApparcRegFileImportPath);
                err = 0; 
                TRAP(err, ReRegisterAppRegFileL(romRegImportFilesPath,appUid));
                CleanupStack::PopAndDestroy(&romRegImportFilesPath);
                }
            
            if(err == KErrNotFound)                       // Since _reg.rsc file doesn't exists in rom and it is registered in scr, remove it                        
                {
                // Delete the existing application entry, which is not associated with any package 
                ScrHelperUtil::DeleteApplicationEntryL(iScrSession, appUid); 
                isAppReRegistered = EFalse;
                }   
            
            else if(err != KErrNone && err != KErrNotFound)     
                {
                DEBUG_PRINTF2(_L8("Sis Registry Server - Error in removing reg file (%S)"), &regFileName);
                User::Leave(err);
                }                                            
            }
        else           // else if compId is not 0 i.e. either appUid is associated with stub in rom or any other package in other drive
            {            
            CSisRegistryObject* object = CSisRegistryObject::NewLC();
            ScrHelperUtil::GetComponentL(iScrSession, compId, *object);            
            if (object->InRom())            // If its a rom based pkg which has not been eclipsed then we can go back to the stubs
                {   
                RemoveEntryL(compId);
                // Re-add the ROM installed stub details to SCR (only those missing will be added)
                ProcessRomDriveL();
                }    
            else
                {
                // Delete the existing application entry, which is not associated with any package 
                ScrHelperUtil::DeleteApplicationEntryL(iScrSession, appUid);
                // Register the reg files of the applications, only the unregistered reg file data will be added(the association of reg file with stub will be lost) 
                ProcessApplicationsL();
                }
            CleanupStack::PopAndDestroy(object);
            }
        
        //Notify Apparc of the app being removed      
        RSisLauncherSession launcher;
        CleanupClosePushL(launcher);
        User::LeaveIfError(launcher.Connect());
        RArray<TAppUpdateInfo> affectedApps; 
        CleanupClosePushL(affectedApps);
        TAppUpdateInfo newAppInfo;
        if(isAppReRegistered)       // If App reg data in rom is registerted again(i.e overwrites the existing one) we notify apparc of Installed app else Uninstalled app
            {
            newAppInfo = TAppUpdateInfo(appUid, EAppInstalled);
            }
        else
            {
            newAppInfo = TAppUpdateInfo(appUid, EAppUninstalled);
            }
        affectedApps.AppendL(newAppInfo);        
        launcher.NotifyNewAppsL(affectedApps);
        CleanupStack::PopAndDestroy(2, &launcher); // affectedApps        
        }
    else
        {
        DEBUG_PRINTF2(_L8("Sis Registry Server - Parsing application registration info. Error code %d."), result);
        }
    CleanupStack::PopAndDestroy(regFileName);
    aMessage.Complete(result);
    }

void CSisRegistrySession::ValidateAndParseAppRegFileL(const TDesC& aRegFileName, Usif::CApplicationRegistrationData*& aAppRegData,TBool aInternal)
    {
    
    TParsePtrC filename(aRegFileName);
    if(!aInternal)                               //check if called for API and regFile path is "\\private\\10003a3f\\import\\apps\\"
        {
        if (filename.Path().Left(KApparcRegImportDir().Length()).CompareF(KApparcRegImportDir) != 0)
            {
            User::Leave(KErrNotSupported);
            }
        }
    
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());
    User::LeaveIfError(fs.ShareProtected());
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(fs, aRegFileName, EFileRead));
    
    // Reading the TUidType information fron the reg rsc file header
    TBuf8<sizeof(TCheckedUid)> uidBuf;
    TInt err = file.Read(0, uidBuf, sizeof(TCheckedUid));
    if (err != KErrNone)
        {
        CleanupStack::PopAndDestroy(2, &fs);  //file
        User::Leave(KErrNotSupported);
        }
    
    if(uidBuf.Size() != sizeof(TCheckedUid))
        {
        DEBUG_PRINTF(_L("The file is not a valid registration resource file"));
        CleanupStack::PopAndDestroy(2, &fs);  // file
        User::Leave(KErrNotSupported);
        }
    TCheckedUid uid(uidBuf);
    
    //check the uid of the reg file to be parsed 
    if(!(uid.UidType()[1] == KUidAppRegistrationFile))
        {
        CleanupStack::PopAndDestroy(2, &fs);  //file
        User::Leave(KErrNotSupported);
        }
    
    RSisLauncherSession launcher;
    CleanupClosePushL(launcher);
    User::LeaveIfError(launcher.Connect()); 
    aAppRegData = launcher.SyncParseResourceFileL(file);
    CleanupStack::PopAndDestroy(3, &fs);  //launcher, file
    }

void CSisRegistrySession::ReRegisterAppRegFileL(const TDesC& aRegFileDrivePath, const TUid& aAppUid)
    {
    /* 
     * Reads and parses the *_reg.rsc files in the aRegFileDrivePath path and compare uid of each with the aAppUid, if matched remove the existing i.e. aAppUid
     * and register the one found in the path.
     */
    
    // open the directory
    CDir* dir;
    TInt err = iFs.GetDir(aRegFileDrivePath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
    
    if (err == KErrNone)
        {
        CleanupStack::PushL(dir);
        TInt count(dir->Count());
        TBool foundAndReplaced(EFalse);
        // scan through all the *_reg.rsc files
        for (TInt index = 0; index < count; ++index)
            {
            RBuf regResourceFileName;
            CleanupClosePushL(regResourceFileName);
            regResourceFileName.CreateL(KMaxFileName);            
            //construct the app reg file present in rom
            regResourceFileName = TParsePtrC(aRegFileDrivePath).DriveAndPath();
            regResourceFileName.Append((*dir)[index].iName);
           //validate and parse the app reg file
            Usif::CApplicationRegistrationData* appRegData = NULL;
            TRAPD(res,ValidateAndParseAppRegFileL(regResourceFileName, appRegData, ETrue));
            CleanupStack::PopAndDestroy(&regResourceFileName); 
            CleanupStack::PushL(appRegData);
            
            if(res != KErrNone)
                {
                // log it only, we cannot stop as the next might be ok
                DEBUG_PRINTF2(_L8("Sis Registry Server - Failed to register in ROM controller. Error code %d."), res); 
                CleanupStack::Pop(appRegData);
                continue;
                }
            
            /* If appuid of the current processing reg file is same as of the reg file to be removed, delete existing app reg data and 
             * add the reg entry of _reg.rsc file currently processed.
             */
            if(aAppUid == appRegData->AppUid())
                {                        
                TComponentId compId(0);
                // Delete the existing application entry, which is not associated with any package 
                ScrHelperUtil::DeleteApplicationEntryL(iScrSession, aAppUid); 
                ScrHelperUtil::AddApplicationEntryL(iScrSession, compId, *appRegData);    
                CleanupStack::PopAndDestroy(appRegData);
                foundAndReplaced = ETrue;
                break;
                }    
            CleanupStack::PopAndDestroy(appRegData); 
            }
        if(!foundAndReplaced)
            {
            User::Leave(KErrNotFound);
            }
        CleanupStack::PopAndDestroy(dir); 
        }
    else
    	{
    	User::Leave(KErrNotFound);
    	}
    }

// Retuns ETrue if the aUid is found in the ROM stub for that package, else returns EFalse.
TBool CSisRegistrySession::RomBasedPackageL(const TUid& aUid)
    {
    TBool isInRom = EFalse;    
    TComponentId compId = 0;
    
    // Get the component Id for the pacakge
    if (!ScrHelperUtil::GetComponentIdL(iScrSession, aUid, 0, compId))
        {
        DEBUG_PRINTF2(_L("Sis Registry Server - RomBasedPackageL - UID 0x%08x not found in Software Component Registry"), aUid.iUid);
        return EFalse;
        }
    
    HBufC* stubFileName = GetStrPropertyValueL(iScrSession, compId, KStubFileName, KUnspecifiedLocale, EFalse);
    if (NULL != stubFileName)
        {
        CleanupStack::PushL(stubFileName);
        
        TRAPD(err, isInRom = SisRegistryUtil::CheckIfMatchingStubPUidL(aUid, iFs, *stubFileName));                
        if (KErrNone != err)
            {
            DEBUG_PRINTF3(_L("Sis Registry Server - RomBasedPackageL - Stub file in ROM failed to parse. Filename: %S. Error code: %d."), stubFileName, err);
            }
        
        CleanupStack::PopAndDestroy(stubFileName);
        }
    
    DEBUG_PRINTF3(_L("Sis Registry Server - RomBasedPackageL - The outcome for UID 0x%08x is %d"), aUid.iUid, isInRom);
    return isInRom;
    }
