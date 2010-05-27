/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisRegistrySubSession class implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include "sisregistryserversubsession.h"
#include <swi/sistruststatus.h>
#include <scs/ipcstream.h>
#include "swi/siscontroller.h"
#include "sisregistryserversession.h"
#include "sisregistryfiledescription.h"
#include "scrhelperutil.h"
#include "scrdbconstants.h"
#include "log.h"
#include "cleanuputils.h"
#include "sisregistryutil.h"
#include "dessisdataprovider.h"
#include "sisrevocationmanager.h"
#include "sisregistryproperty.h"
#include "controllerinfo.h"
#include "sisinfo.h"
#include "filesisdataprovider.h"
#include "sisregistryserverconst.h"
#include "siscontrollerverifier.h"
#include "sisregistryobject.h"
#include "arrayutils.h"
#include "sisregistrydependency.h"
#include "siscertificatechain.h"

using namespace Swi;
using namespace Usif;

CSisRegistrySubSession::~CSisRegistrySubSession()
	{
	ResetFileAPIData();
	delete iRevocationManager;
	delete iSisControllerVerifier;
	delete iComponentEntry;
	iControllerArray.ResetAndDestroy();
	}

CSisRegistrySubSession::CSisRegistrySubSession(CSisRegistrySession& aSession,
		Usif::TComponentId& aCompId, TUid aCompUid):
		iSession(aSession),
		iComponentId(aCompId),
		iCompUid(aCompUid)
	{
	}

void CSisRegistrySubSession::ResetFileAPIData()
	{
	iNumFilesSent = 0;
	iWildcardFileCount = 0;
	iRegularFileCount = 0;

	delete iSavedFileDesc;
	iSavedFileDesc = NULL;
	if (iScrFileList != NULL)
		{
		iScrFileList->Close();
		delete iScrFileList;
		iScrFileList = NULL;
		}
	}

void CSisRegistrySubSession::RequestVersionRegistryEntryL(const RMessage2& aMessage)
	{
	// the identity of the current entry/subsession is passed in IPC 3!
	// Lookup the component ID for the subsession handle
	CComponentEntry* component = ComponentL();
	const TDesC& verString = component->Version();
	TVersion version;
	StringToVersionL(verString, version);

	DEBUG_PRINTF5(_L8("Sis Registry Server - Version for subsession ID %d is %d.%d.%d"),
		aMessage.Int3(), version.iMajor, version.iMinor, version.iBuild);

	TPckg<TVersion> packageVersion(version);
	aMessage.WriteL(EIpcArgument0, packageVersion);
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySubSession::RequestPackageNameRegistryEntryL(const RMessage2& aMessage)
	{
	CComponentEntry* component = ComponentL();
	aMessage.WriteL(EIpcArgument0, component->Name());
	aMessage.Complete(KErrNone); 
	}

void CSisRegistrySubSession::RequestLocalizedVendorNameRegistryEntryL(const RMessage2& aMessage)
	{
	// Get the relevant component entry usnig the given Uid - IPC arg 3.
	CComponentEntry* component = ComponentL();
	HBufC* localizedVendorName = component->Vendor().AllocLC();
	DEBUG_PRINTF3(_L("Sis Registry Server - Vendor for subsession ID %d is '%S'"), 
		aMessage.Int3(), localizedVendorName);
	// Write the component name back in to IPC arg 0.
	aMessage.WriteL(EIpcArgument0, *localizedVendorName);
	CleanupStack::PopAndDestroy(localizedVendorName);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySubSession::RequestUniqueVendorNameRegistryEntryL(const RMessage2& aMessage)
	{
	// Get the Unique vendor name
	HBufC* uniqueVendorName = GetStrPropertyValueL(iSession.ScrSession(), iComponentId, KCompUniqueVendorName);
	DEBUG_PRINTF3(_L("Sis Registry Server - Unique Vendor for subsession ID %d is '%S'"),
		aMessage.Int3(), uniqueVendorName);
	CleanupStack::PushL(uniqueVendorName);

	aMessage.WriteL(EIpcArgument0, *uniqueVendorName);
	CleanupStack::PopAndDestroy(uniqueVendorName);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySubSession::RequestEmbeddedPackageRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<CSisRegistryPackage> embeddedPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddedPackages);
	ScrHelperUtil::InternalizeEmbeddedPackageArrayL(iSession.ScrSession(), iComponentId, embeddedPackages);

	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession %d has %d embedded packages."),
		aMessage.Int3(), embeddedPackages.Count());
	
	SisRegistryUtil::SendDataPointerArrayL(aMessage, embeddedPackages, EIpcArgument0);
	CleanupStack::PopAndDestroy(&embeddedPackages);
	}

void CSisRegistrySubSession::RequestHashRegistryEntryL(const RMessage2& aMessage)
	{
	// get file name
	TUint srcLen = aMessage.GetDesLengthL(EIpcArgument0);
	HBufC* fileName = HBufC::NewLC(srcLen);
	TPtr namePtr1 = fileName->Des();
	aMessage.ReadL(EIpcArgument0, namePtr1, 0);
	
	DEBUG_PRINTF3(_L("Sis Registry Server - Subsession ID %d requested hash for file '%S'."),
		aMessage.Int3(), fileName);

	CHashContainer* hashContainer = NULL;
	ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, hashContainer, *fileName);
	
	CleanupStack::PushL(hashContainer);
	SisRegistryUtil::SendDataL(aMessage, *hashContainer, EIpcArgument1);
    CleanupStack::PopAndDestroy(2, fileName); // hashContainer and fileName.
	}	
	
void CSisRegistrySubSession::RequestUidRegistryEntryL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF3(_L8("Sis Registry Server - Subsession ID %d has Package UID 0x%08x."),
		aMessage.Int3(), iCompUid.iUid);
	TPckg<TUid> packageUid(iCompUid);
	aMessage.WriteL(EIpcArgument0, packageUid);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySubSession::RequestLanguageRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the language component property 
	TLanguage language = static_cast<TLanguage> (GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompLanguage, EFalse, ELangEnglish));

	DEBUG_PRINTF3(_L8("Sis Registry Server - Subsession ID %d is localised in language %d."),
		aMessage.Int3(), language);
	
	TPckgC<TLanguage> packageLanguage(language);
	aMessage.WriteL(EIpcArgument0, packageLanguage);
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySubSession::UidPresentRegistryEntryL(const RMessage2& aMessage)
	{
	TBool isPresent = IsCompletelyPresentL();
	// Update the CompPresence property based on the component presence
	iSession.ScrSession().SetIsComponentPresentL(iComponentId, isPresent);
	
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
	
void CSisRegistrySubSession::SignedRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property 
	TBool isSigned = static_cast<TBool>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompIsSigned, EFalse, KDefaultIsSigned));
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

void CSisRegistrySubSession::SignedBySuCertRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property
	TBool isSignedBySuCert = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompSignedBySuCert, EFalse, KDefaultIsSignedBySuCert);
	TPckgC<TBool> package(isSignedBySuCert);
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySubSession::RegistryEntryTrustL(const RMessage2& aMessage)
	{
	TSisPackageTrust trust = static_cast<TSisPackageTrust>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompTrust, EFalse, KDefaultTrustValue));
	TPckgC<TSisPackageTrust> package(trust);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package trust status for subsession ID %d is %d."),
		aMessage.Int3(), trust);
	
	aMessage.WriteL(0, package);	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySubSession::TrustTimeStampL(const RMessage2& aMessage)
	{
	// Retrieve the component property 
	TTime time = TTime(GetInt64PropertyValueL(iSession.ScrSession(), iComponentId, KCompTrustTimeStamp, ETrue));
	TPckgC<TTime> package(time);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Last package trust update time for subsession %d was %Ld"),
		aMessage.Int3(), time.Int64());
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);			
	}

void CSisRegistrySubSession::TrustStatusEntryL(const RMessage2& aMessage)
    {
	// Retrieve the trust status from SCR
	TSisTrustStatus status;
	ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, status);
	
	DEBUG_PRINTF4(_L8("Sis Registry Server - Package for subsession ID %d has Validation Status: %d, Revocation Status: %d."),
		aMessage.Int3(), status.ValidationStatus(), status.RevocationStatus());
	
	SisRegistryUtil::SendDataL(aMessage,status,EIpcArgument0);
    }

void CSisRegistrySubSession::RevocationCheckEntryL(const RMessage2& aMessage)
    {
	// Get the OCSP Uri
    TUint uriLen = aMessage.GetDesLengthL(EIpcArgument0);
    HBufC8* ocspUri = HBufC8::NewLC(uriLen);
    TPtr8 ocspPtr(ocspUri->Des());
    aMessage.ReadL(EIpcArgument0, ocspPtr);

    DEBUG_PRINTF3(_L8("Sis Registry Server - Client has requested a revocation check for component id %d with URI '%S'."),
    		iComponentId, ocspUri);

    // Get the controllers for the component 
    // Note that just the first controller is used here
    RPointerArray<HBufC8> controllers;
	CleanupResetAndDestroyPushL(controllers);
	GenerateControllersArrayL(iComponentId, controllers);
	
	if (controllers.Count() > 0)
		{
		// Get the trust status
		TSisTrustStatus trustStatus;
		ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, trustStatus);

		// Get the install chain indicies
		RArray<TInt> chainIndexArray;
		CleanupClosePushL(chainIndexArray);
		ScrHelperUtil::InternalizeChainIndexArrayL(iSession.ScrSession(), iComponentId, chainIndexArray);

		CDesDataProvider* dataProvider = CDesDataProvider::NewLC(*controllers[0]);
	    Sis::CController* firstController = Sis::CController::NewLC(*dataProvider, Sis::EAssumeType);
	    if (iRevocationManager == NULL)
	    	{
	    	iRevocationManager = CSisRevocationManager::NewL(iSession);
	    	}
	    iRevocationManager->RevocationStatusRequestL(controllers[0], firstController,
	    		trustStatus, chainIndexArray, ocspPtr, aMessage);

	    controllers.Remove(0); // remove the buffer passed to the revocation manager
	    CleanupStack::Pop(firstController); // belongs to revocation manager now
	    CleanupStack::PopAndDestroy(2, &chainIndexArray);	//chainIndexArray and dataProvider.
		}
    CleanupStack::PopAndDestroy(2, ocspUri);	//controllers and ocspUri.
	}
	
void CSisRegistrySubSession::CancelRevocationCheckEntryL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF(_L8("Sis Registry Server - Cancelling revocation check."));
    
	if (iRevocationManager != NULL)   	
		{
		iRevocationManager->Cancel();
		}
	aMessage.Complete(KErrNone);	
	}

void CSisRegistrySubSession::InRomRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property (InRom)
	// Determine if the type is preinstalled
	TBool inRom = static_cast<TBool>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInRom, EFalse, KDefaultIsInRom));
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
	
void CSisRegistrySubSession::PreInstalledRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property (install type)
	// Determine if the type is preinstalled
	Sis::TInstallType instType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	TBool preInstalled = CSisRegistryObject::IsPreinstalled(instType);
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

void CSisRegistrySubSession::DeletablePreInstalledRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property
	TBool deletablePreInstalled = static_cast<TBool>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompDeletablePreinstalled, EFalse, KDefaultIsDeletablePreInstalled));
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

void CSisRegistrySubSession::AugmentationRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the component property (install type)
	Sis::TInstallType instType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	// Determine if the type is an augmentation
	TBool aug = (instType == Sis::EInstAugmentation);
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

// Request the user selected drive
void CSisRegistrySubSession::RequestSelectedDriveRegistryEntryL(const RMessage2& aMessage)	
	{
	// Retrieve the selected drive
	TChar selectedDrive = static_cast<TChar>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompSelectedDrive, EFalse));
	TInt drive = selectedDrive;
	TPckg<TInt> packageDrive(drive);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has selected drive %d"),	
		aMessage.Int3(), drive);
	
	aMessage.WriteL(EIpcArgument0, packageDrive);
	aMessage.Complete(KErrNone);
	}

// Request the drive bitmask where the files are installed
void CSisRegistrySubSession::RequestInstalledDrivesRegistryEntryL(const RMessage2& aMessage)	
	{
	// Get the list of installed files
	TUint instDrives = InstalledDrivesL();
	TPckg<TUint> installedfilesDrive(instDrives);
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has Installed drives bitmask %d"),	
		aMessage.Int3(), instDrives);

	aMessage.WriteL(EIpcArgument0, installedfilesDrive);
	aMessage.Complete(KErrNone);
	}

// Request the install type
void CSisRegistrySubSession::RequestInstallTypeRegistryEntryL(const RMessage2& aMessage)	
	{
	// Retrieve the component property (install type)
	Sis::TInstallType installType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	TPckgC<Sis::TInstallType> packageInstallType(installType);

	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d is of type %d."),
		aMessage.Int3(), installType);
	
	aMessage.WriteL(EIpcArgument0, packageInstallType);
	aMessage.Complete(KErrNone);	
	}	
	
// Request the value of property, based on specific property key.
// if not found returns to client KErrNotFound.
void CSisRegistrySubSession::RequestPropertyRegistryEntryL(const RMessage2& aMessage)
	{
	TInt value = 0;	
	TBool propertyFound = EFalse;
	RPointerArray<CSisRegistryProperty> propertiesArray;
	TInt givenPropertyKey = aMessage.Int0();
	
	// Get the component properties from SCR.
	ScrHelperUtil::InternalizePropertiesArrayL(iSession.ScrSession(), iComponentId, propertiesArray);
	CleanupResetAndDestroyPushL(propertiesArray);
	
	// Iterate through the properties array and get the value for given property key.
	for (TInt property = 0; property < propertiesArray.Count(); property++)
		{
		if(propertiesArray[property]->Key() == givenPropertyKey)
			{				
			value = propertiesArray[property]->Value();
			propertyFound = ETrue;
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(&propertiesArray);
		
	if(!propertyFound)
		{
		value = KErrNotFound;
		}

	DEBUG_PRINTF4(_L8("Sis Registry Server - Value of property %d for subsession ID %d is %d"),
		aMessage.Int0(), aMessage.Int3(), value);

	TPckgC<TInt> packageValue(value);
	aMessage.WriteL(EIpcArgument1, packageValue);
	aMessage.Complete(KErrNone);	
	}
	
void CSisRegistrySubSession::RequestSizeRegistryEntryL(const RMessage2& aMessage)
	{
	TInt64 size = 0;

	// Lookup the component entry for the subsession handle
	CComponentEntry* componentEntry = ComponentL();
	size = componentEntry->ComponentSize();
	TPckg<TInt64> packageSize(size);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d takes up %Ld bytes."),	
		aMessage.Int3(), size);
	
	aMessage.WriteL(EIpcArgument0, packageSize);
	aMessage.Complete(KErrNone);	
	
	}

void CSisRegistrySubSession::RequestPackageRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the package from SCR
	CSisRegistryPackage *package = NULL;
	ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, package);
	CleanupStack::PushL(package);
	SisRegistryUtil::SendDataL(aMessage, *package, EIpcArgument0);
	CleanupStack::PopAndDestroy(package);
	}

void CSisRegistrySubSession::RequestFileNamesRegistryEntryL(const RMessage2& aMessage)
	{
	TStubExtractionMode tMode;
	TPckg<TStubExtractionMode> packageMode(tMode);
	aMessage.ReadL(EIpcArgument0, packageMode, 0);

	switch (tMode)
		{
		// If only the count needed, send the total entries count
		case EGetCount:
			{
		 	// Get the number of regular files and wildcarded files
			iRegularFileCount = iSession.ScrSession().GetComponentFilesCountL(iComponentId);
			iWildcardFileCount = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompWildCardFileCount, EFalse, KDefaultWildCardFileCount);
			TInt totalFileCount = iRegularFileCount + iWildcardFileCount;
			TPckgBuf<TInt> pkgFileCount(totalFileCount);

			DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d contains %d file names."),
					aMessage.Int3(), totalFileCount);		

			aMessage.WriteL(EIpcArgument1, pkgFileCount);
			aMessage.Complete(KErrNone);
			}
			break;
		// Need to send file names. Ensure not to send larger than KDefaultBufferSize in one message
		case EGetFiles:
			{
			// Retrieve the list of files
			RPointerArray<HBufC> files;
			CleanupResetAndDestroyPushL(files);
			// Make sure we don't send more than a predefined buffer size
			FileListL(files);

			// Stream the array back to client
			RIpcWriteStream ipcstream;
			ipcstream.Open(aMessage, EIpcArgument1);
			CleanupClosePushL(ipcstream);
			ExternalizePointerArrayL(files, ipcstream);
			ipcstream.CommitL();
			aMessage.Complete(KErrNone);
			CleanupStack::PopAndDestroy(2, &files);	// ipcstream 
			}
			break;
		default:
			// No operation mode specified
			iSession.PanicClient(aMessage, EPanicBadOperationMode);
		}
	}

void CSisRegistrySubSession::RequestFileDescriptionsRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the list of file descriptions
	RPointerArray<CSisRegistryFileDescription> fileDesc;
	CleanupResetAndDestroyPushL(fileDesc);
	// Make sure we don't send more than a predefined buffer size
	FileDescriptionListL(fileDesc);

	SisRegistryUtil::SendDataPointerArrayL(aMessage, fileDesc, EIpcArgument0);	// Also completes the message
	CleanupStack::PopAndDestroy(&fileDesc); 
	}

void CSisRegistrySubSession::RequestSidsRegistryEntryL(const RMessage2& aMessage)
	{
	// Retrieve the list of SIDs
	RArray<TUid> sids;
	CleanupClosePushL(sids);
	ScrHelperUtil::InternalizeSidArrayL(iSession.ScrSession(), iComponentId, sids);

	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d contains %d SIDs."),
		aMessage.Int3(), sids.Count()); 

	SisRegistryUtil::SendDataArrayL(aMessage, sids, EIpcArgument0);

    CleanupStack::PopAndDestroy(&sids);
	}

void CSisRegistrySubSession::RequestCertificateChainsRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<HBufC8> chainList;
	CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(chainList);

	// Check if the package was signed since only signed entries have certificate chains
	TBool isSigned = static_cast<TBool>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompIsSigned, EFalse, KDefaultIsSigned));
	if (isSigned)
		{
		GenerateChainListL(iComponentId, chainList);	 
		}
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d is signed with %d certificate chains"),
		aMessage.Int3(), chainList.Count());
		
	SisRegistryUtil::SendDataPointerArrayL(aMessage, chainList, EIpcArgument0);
	CleanupStack::PopAndDestroy(&chainList);
	}

// Finds and returns the array of packages this package depends on.
void CSisRegistrySubSession::RequestDependenciesRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<CSisRegistryDependency> dependencies;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryDependency> >::PushL(dependencies);
	
	CComponentEntry* component = ComponentL();
	Sis::TInstallType installType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	// Get the dependency package list for the given component.
	ScrHelperUtil::InternalizeSupplierPackageArrayL(iSession.ScrSession(), iCompUid, component->Name(), installType, dependencies);
						
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has %d dependencies."),
		aMessage.Int3(), dependencies.Count());
	
	SisRegistryUtil::SendDataPointerArrayL(aMessage, dependencies, EIpcArgument0);
	CleanupStack::PopAndDestroy(&dependencies);
	}
	
// Finds and returns the array of packages which are either dependent on this package or are
// augmentations of this package.
void CSisRegistrySubSession::RequestDependentPackagesRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<CSisRegistryPackage> dependentPackages;

	TVersion baseCompVersion;		
	CleanupResetAndDestroyPushL(dependentPackages);
	
	// Get the base packages version information.
	CComponentEntry* component = ComponentL();		
	const TDesC& versionString = component->Version();	
	StringToVersionL(versionString, baseCompVersion);	
	
	iSession.GetAugmentationDetailsL(iCompUid, dependentPackages, iComponentId);

	// Now search for other packages which are depending on this package. 
	// For that, get all the installed components and iterate through one by one.	
	TBuf<KCompUidSize> supplierGlobalIdString;	
	supplierGlobalIdString.AppendFormat(KGlobalIdFormat, iCompUid);
	
	Usif::CGlobalComponentId* supplierGlobalId = Usif::CGlobalComponentId::NewLC(supplierGlobalIdString, Usif::KSoftwareTypeNative);
	
	RPointerArray<CVersionedComponentId> dependentCompList;
	CleanupResetAndDestroyPushL(dependentCompList);
	
	iSession.ScrSession().GetDependantComponentsL(*supplierGlobalId, dependentCompList);
	
	for (TInt index = 0; index < dependentCompList.Count(); index++)
		{
		const Usif::CGlobalComponentId &dependentGlobalId = dependentCompList[index]->GlobalId();		
		TComponentId dependantCompId = iSession.ScrSession().GetComponentIdL(dependentGlobalId.GlobalIdName(), dependentGlobalId.SoftwareTypeName());

		CSisRegistryPackage* dependentPackage = NULL;
		ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), dependantCompId, dependentPackage);
		CleanupStack::PushL(dependentPackage);					
		dependentPackages.AppendL(dependentPackage);
		CleanupStack::Pop(dependentPackage);		
		}
	CleanupStack::PopAndDestroy(2, supplierGlobalId);
	
	SisRegistryUtil::SendDataPointerArrayL(aMessage, dependentPackages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&dependentPackages);
	}

void CSisRegistrySubSession::RequestEmbeddingPackagesRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<CSisRegistryPackage> embeddingPackages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(embeddingPackages);
	EmbeddingPackagesL(iComponentId, iCompUid, embeddingPackages);
	
	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d (and its partial upgrades) was embedded in %d other packages."),
		aMessage.Int3(), embeddingPackages.Count());
    
	SisRegistryUtil::SendDataPointerArrayL(aMessage, embeddingPackages, EIpcArgument0);
    CleanupStack::PopAndDestroy(&embeddingPackages);
	}

void CSisRegistrySubSession::RequestControllersL(const RMessage2& aMessage)
	{
	RPointerArray<HBufC8> controllers;
	CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(controllers);
	GenerateControllersArrayL(iComponentId, controllers);

   	DEBUG_PRINTF3(_L8("Sis Registry Server - Package entry for subsession ID %d has %d controllers."),
		aMessage.Int3(), controllers.Count());

	TInt len = 0;
   	TPckg<TInt> lenPckg(len);
   	// Calculate the total length for the buffer
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
		TInt pos = 0;
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

	CleanupStack::PopAndDestroy(&controllers);
	}

void CSisRegistrySubSession::ShutdownAllAppsL(const RMessage2& aMessage)
	{
	TInt componentIndex = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompIndex, EFalse, KDefaultComponentIndex);

	RPointerArray<CControllerInfo> controllerInfoArray;
	CleanupResetAndDestroyPushL(controllerInfoArray);
	ScrHelperUtil::InternalizeControllerArrayL(iSession.ScrSession(), iComponentId, controllerInfoArray);
	TInt controllerCount = controllerInfoArray.Count();
	
	TInt i = 0;
	TBool shutdownAllApps = EFalse;	
		
	while (!shutdownAllApps && i < controllerCount)
		{
		// construct the name using the package and offset
		HBufC* fileName = SisRegistryUtil::BuildControllerFileNameLC(iCompUid, componentIndex, 
			controllerInfoArray[i]->Offset());

		// load the controller directly from the file
		// DO NOT TRY TO LOAD THE CONTROLLER FROM A BUFFER BECAUSE 
		// THIS CAN CAUSE SISREGISTRY SERVER TO RUN OUT OF MEMORY.
		CFileSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(iSession.Fs(), *fileName);
		Sis::CController* controller = Sis::CController::NewLC(*dataProvider, Sis::EAssumeType);
		shutdownAllApps |= (controller->Info().InstallFlags() & Sis::EInstFlagShutdownApps);
			
		CleanupStack::PopAndDestroy(3, fileName); // controller, dataprovider, fileName
		++i;
		}

	CleanupStack::PopAndDestroy(&controllerInfoArray);

	TPckg<TBool> packageShutdownAllApps(shutdownAllApps);
	aMessage.WriteL(EIpcArgument0, packageShutdownAllApps);
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySubSession::RequestPackageAugmentationsRegistryEntryL(const RMessage2& aMessage)
	{
	RPointerArray<CSisRegistryPackage> augmentationPackages;
	
	// Get Install type using component Id from SCR.
	Sis::TInstallType installType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	
	// Populate the augmentation packages array.
	CleanupResetAndDestroyPushL(augmentationPackages);
	if ((installType != Sis::EInstAugmentation) && (installType != Sis::EInstPreInstalledPatch))
		{
		iSession.GetAugmentationDetailsL(iCompUid, augmentationPackages, iComponentId);
		}

	DEBUG_PRINTF3(_L8("Sis Registry Server - Package for subsession ID %d has %d augmentations."),
		aMessage.Int3(), augmentationPackages.Count());
	
	SisRegistryUtil::SendDataPointerArrayL(aMessage, augmentationPackages, EIpcArgument0);
	CleanupStack::PopAndDestroy(&augmentationPackages);
	}
	
void CSisRegistrySubSession::RequestPackageAugmentationsNumberL(const RMessage2& aMessage)
	{
	TInt numAugmentations = 0;
	
	// Get Install type using component Id from SCR.
	Sis::TInstallType installType = static_cast<Sis::TInstallType>(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompInstallType, EFalse, KDefaultInstallType));
	if (installType != Sis::EInstAugmentation)
		{
		numAugmentations = iSession.GetAugmentationCountL(iCompUid);
		}
	
	TPckgC<TInt> package(numAugmentations);
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySubSession::RemoveWithLastDependentL(const RMessage2& aMessage)
	{
	// Retrieve the component property 
	TInt isRemoveWithLastDependent = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompIsRemoveWithLastDependent, EFalse, KDefaultRemoveWithLastDependent);
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

void CSisRegistrySubSession::SetRemoveWithLastDependentL(const RMessage2& aMessage)
	{
	// Set the component property.
	const TInt KIsRemoveWithLastDependentValue = 1;
	iSession.ScrSession().SetComponentPropertyL(iComponentId, KCompIsRemoveWithLastDependent, KIsRemoveWithLastDependentValue); 
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySubSession::IsRemovableL(const RMessage2& aMessage)
	{
	// Retrieve the component entry passing in the subsession ID
	CComponentEntry* component = ComponentL();
	TBool isRemovable = component->IsRemovable();
	TPckgC<TBool> package(isRemovable);
	
	aMessage.WriteL(EIpcArgument0, package);	
	aMessage.Complete(KErrNone);
	}

void CSisRegistrySubSession::VerifyControllerL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L("CSisRegistry::VerifyControllerSignature"));

	iControllerArray.ResetAndDestroy();
	GenerateControllersArrayL(iComponentId, iControllerArray);

	delete iSisControllerVerifier;
	iSisControllerVerifier = CSisControllerVerifier::NewL(aMessage);

	iSisControllerVerifier->VerifyControllerL(iControllerArray);
	}
	    	
void CSisRegistrySubSession::RequestMatchingSupportedLanguagesL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF(_L("CSisRegistry::RequestMachingSupportedLanguagesL"));
	
	RArray<TLanguage> aMatchingSupportedLanguages;
	CleanupClosePushL(aMatchingSupportedLanguages);

	iSession.ScrSession().GetComponentSupportedLocalesListL(iComponentId, aMatchingSupportedLanguages);
		
	SisRegistryUtil::SendDataArrayL(aMessage, aMatchingSupportedLanguages, EIpcArgument0);
	CleanupStack::PopAndDestroy(&aMatchingSupportedLanguages);

	}

///////////////////////////////////// Helper methods

// Returns the component entry associated with this subsession
CComponentEntry* CSisRegistrySubSession::ComponentL()
	{
	// Retrieve the corresponding component from SCR if it was not fetched before
	if (!iComponentEntry)
		{
		iComponentEntry = Usif::CComponentEntry::NewL();
		iSession.ScrSession().GetComponentL(iComponentId, *iComponentEntry);
		}
	return iComponentEntry;
	}

void CSisRegistrySubSession::FileListL(RPointerArray<HBufC>& aFiles)
	{
	if (iNumFilesSent == 0)
		{
		// Don't need to get this information for subsequent calls
		iWildcardFileCount = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompWildCardFileCount, EFalse, KDefaultWildCardFileCount);
		iRegularFileCount = iSession.ScrSession().GetComponentFilesCountL(iComponentId);
		}

	// From the subsession info retrieve the number of files already sent
	TBuf<KBigBufferSize> propertyName;
	TInt bufSize = 0;
	TInt fileNum = iNumFilesSent;
	for (; fileNum < iWildcardFileCount + iRegularFileCount; ++fileNum)
		{
		// First pick up the files with wildcards
		HBufC* fileName = NULL;
		if (fileNum < iWildcardFileCount)
			{
			propertyName.Format(KCompWildCardFile, fileNum);
			fileName = GetStrPropertyValueL(iSession.ScrSession(), iComponentId, propertyName);
			CleanupStack::PushL(fileName);
			}
		else
			{
			// Retrieve the regular files
			// Open a file view if not opened earlier
			if (iScrFileList == NULL)
				{
				iScrFileList = new (ELeave) RSoftwareComponentRegistryFilesList;
				iScrFileList->OpenListL(iSession.ScrSession(), iComponentId);
				}
			fileName = iScrFileList->NextFileL();
			if (fileName == NULL)
				{
				break;
				}
			CleanupStack::PushL(fileName);
			}

		aFiles.AppendL(fileName);
		bufSize += fileName->Size();
		CleanupStack::Pop(fileName);

		// Make sure we don't go too much over the buffer size
		if (bufSize >= KDefaultBufferSize - KMaxFileName)
			{
			break;
			}
		}
	
	if (fileNum >= iWildcardFileCount + iRegularFileCount)
		{
		// We are done sending all files so reset the subsession file list API data
		ResetFileAPIData();
		}
	else
		{
		// Record the number of files sent as part of the subsession file list API data
		iNumFilesSent = fileNum + 1;
		}
	}

void CSisRegistrySubSession::FileDescriptionListL(RPointerArray<CSisRegistryFileDescription>& aFileDescriptions)
	{
	// From the subsession info retrieve the number of entries already sent
	TBuf<KBigBufferSize> propertyName;
	TInt bufSize = 0;
	TInt fileNum = iNumFilesSent;
	// If we did not manage to fit the last entry at the previous call, try doing it now
	if (iSavedFileDesc != NULL)
		{
		aFileDescriptions.AppendL(iSavedFileDesc);
		bufSize += iSavedFileDesc->SerializedSize();
		iSavedFileDesc = NULL;
		++iNumFilesSent;
		}

	for (; fileNum < iWildcardFileCount + iRegularFileCount; ++fileNum)
		{
		CSisRegistryFileDescription* fileDesc = NULL;
		// First pick up the files with wildcards
		if (fileNum < iWildcardFileCount)
			{
			propertyName.Format(KCompWildCardFile, fileNum);
			HBufC* fileName = GetStrPropertyValueL(iSession.ScrSession(), iComponentId, propertyName);
			CleanupStack::PushL(fileName);
			// Retrieve other details about this file
			// Pass in ETrue and index for wild carded files
			ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, fileDesc, *fileName, ETrue);
			CleanupStack::PopAndDestroy(fileName);
			CleanupStack::PushL(fileDesc);
			}
		else
			{
			// Retrieve the regular files
			// Open a file view if not opened earlier
			if (iScrFileList == NULL)
				{				
				iScrFileList = new (ELeave) RSoftwareComponentRegistryFilesList;
				iScrFileList->OpenListL(iSession.ScrSession(), iComponentId);
				}
			HBufC* fileName = iScrFileList->NextFileL();
			if (fileName == NULL)
				{
				break;
				}
			CleanupStack::PushL(fileName);
			// Retrieve other details about this file
			// Pass in EFalse for normal (non wild carded) files
			ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), iComponentId, fileDesc, *fileName, EFalse); 
			CleanupStack::PopAndDestroy(fileName);
			CleanupStack::PushL(fileDesc);
			}

		bufSize += fileDesc->SerializedSize();
		// Make sure we don't go too much over the buffer size
		if (bufSize >= KDefaultBufferSize)
			{
			CleanupStack::Pop(fileDesc);
			iSavedFileDesc = fileDesc;
			break;
			}

		aFileDescriptions.AppendL(fileDesc);
		CleanupStack::Pop(fileDesc);
		}
	
	if (fileNum >= iWildcardFileCount + iRegularFileCount)
		{
		// We are done sending all files so reset the subsession file list API data
		ResetFileAPIData();
		}
	else
		{
		// Record the number of files sent as part of the subsession file list API data
		iNumFilesSent = fileNum;
		}
	}

TBool CSisRegistrySubSession::UidPresentInStubL(const TUid& aUid, TInt aDrive) 
	{
	HBufC* stubPath = SisRegistryUtil::BuildStubFilesPathLC(aDrive);
	CDir* dir;
	TInt err = iSession.Fs().GetDir(*stubPath, KEntryAttMatchExclude | KEntryAttDir, ESortNone, dir);
	if(err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Reading stub directory failed (Error code: %d.)"), err);
		CleanupStack::PopAndDestroy(stubPath);
		return EFalse;
		}

	CleanupStack::PushL(dir);

	TInt count(dir->Count());
	RBuf filename;
	filename.CreateL(KMaxFileName);
	CleanupClosePushL(filename);
	TBool packageFound = EFalse;
	for (TInt i = 0; i < count; ++i)
		{
		filename.Zero();
		filename.Append(*stubPath);
		filename.Append((*dir)[i].iName);
		TUid foundUid;
		// Trap so that invalid sisx files don't cause a problem.
		TRAP(err, foundUid = SisRegistryUtil::IdentifyUidFromSisFileL(iSession.Fs(), filename));
		if (err != KErrNone)
			{
			continue;
			}
		DEBUG_PRINTF2(_L("Sis Registry Server - Identified stub '%S'."), &filename);
		if (foundUid == aUid)
			{
			packageFound = ETrue;
			break;
			}
		}
	CleanupStack::PopAndDestroy(3, stubPath); // dir, filename
	return packageFound;
	}

TBool CSisRegistrySubSession::UidPresentInFlaggingControllerL(TComponentId aComponentId, const TUid& aUid, TInt aDrive, TInt aAugmentationIndex, TInt aComponentControllerCount) 
	{
	HBufC* controllerFilePath = SisRegistryUtil::BuildControllerFileNameLC(aUid, aAugmentationIndex, aComponentControllerCount - 1, aDrive);
	TEntry entry;
	TBool fileFound = (iSession.Fs().Entry(*controllerFilePath, entry) == KErrNone);
	TBool res = fileFound && iSession.IdentifyControllerL(aComponentId, *controllerFilePath);
	CleanupStack::PopAndDestroy(controllerFilePath);
	return res;
	}

// Returns ETrue if all installed drives are present on the device
TBool CSisRegistrySubSession::IsCompletelyPresentL()
	{
	// Note: We cannot use the SCR API IsMediaPresentL() to do a preliminary check since
	// IsMediaPresentL() uses SCR's view of installed drives (which would include drives which
	// only had files with FILENULL attribute. 
	// Therefore we get SWI's view of installed drives and check for their presence.
	// However we need to ensure that the correct media is present (the one where the application was
	// installed to) - for each removable drive in the drive list we ensure that our package is present on it 

	TUint installedDrives = InstalledDrivesL();

	// Get the package UID of the component
	TUid packageUid = TUid::Uid(GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompUid, ETrue));

	TBool isPresent = ETrue;
	for (TInt driveNum = EDriveA; driveNum <= EDriveZ; ++driveNum)
		{
		// Non-zero bit indicates files were installed to drive
		if ((installedDrives & (1 << driveNum)) == 0)
			{
			continue;
			}
		
		TVolumeInfo volInfo;
		if (iSession.Fs().Volume(volInfo, driveNum) != KErrNone)
			{
			isPresent = EFalse;
			break;
			}
		if (!(volInfo.iDrive.iDriveAtt & KDriveAttRemovable))
			{
			continue;	// Skip fixed drives
			}

		// Check whether the removable drive contains the corresponding SIS stub - in this case, the media matches
		if (UidPresentInStubL(packageUid, driveNum)) 
			{
			continue; 
			}
		TInt augmentationIndex = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompIndex, EFalse, KDefaultComponentIndex); 
		TInt componentControllerCount = GetIntPropertyValueL(iSession.ScrSession(), iComponentId, KCompControllerCount, EFalse, KDefaultControllerCount); 

		// Check whether a flagging controller contains the component
		if (!UidPresentInFlaggingControllerL(iComponentId, packageUid, driveNum, augmentationIndex, componentControllerCount))
			{
			isPresent = EFalse;
			DEBUG_PRINTF3(_L("Drive %c for component %d was not found!"), 'A' + driveNum, iComponentId);
			break;
			}
		}

	return isPresent;
	}

void CSisRegistrySubSession::GenerateControllersArrayL(const TComponentId aCompId, 
												RPointerArray<HBufC8>& aControllers)
	{
	aControllers.ResetAndDestroy();
	// Get the package details from SCR
	CSisRegistryPackage *package = NULL;
	ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), aCompId, package);
	CleanupStack::PushL(package);

	// Get the number of controllers
	TInt controllerCount = GetIntPropertyValueL(iSession.ScrSession(), aCompId, KCompControllerCount, EFalse, KDefaultControllerCount);

	// Read in the controllers from disk	
	TBuf<KBigBufferSize> controllerOffsetName;	
	for (TInt i = 0; i < controllerCount; ++i)
		{
		// Get the controller offset
		controllerOffsetName.Format(KOffsetNameFormat, i);
		TInt ctlOffset = GetIntPropertyValueL(iSession.ScrSession(), aCompId, controllerOffsetName, EFalse, 0);	

		// Construct the controller filename using the package and offset
		HBufC* fileName = SisRegistryUtil::BuildControllerFileNameLC(package->Uid(), package->Index(), ctlOffset);
		// Open file and read the data
		RFile file;
		User::LeaveIfError(file.Open(iSession.Fs(), *fileName, EFileRead | EFileShareReadersOnly));
		CleanupStack::PopAndDestroy(fileName);
		CleanupClosePushL(file);
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		// Create a buffer according to file size and load it with data
		HBufC8* buffer = HBufC8::NewLC(fileSize);
		TPtr8 ptr(buffer->Des());
		User::LeaveIfError(file.Read(0, ptr, fileSize));
		// Add it to the list
		aControllers.AppendL(buffer);	// Ownership transferred
		CleanupStack::Pop(buffer);
		CleanupStack::PopAndDestroy(&file);
		}

	CleanupStack::PopAndDestroy(package);
	}

TUint CSisRegistrySubSession::InstalledDrivesL()
	{
	CComponentEntry* component = ComponentL();
	const TDriveList& componentDriveList = component->InstalledDrives();
	
	// Convert TDriveList to bitmask
	TUint installedDrives(0);	
	TInt driveListLength = componentDriveList.Length();
	
   	for(TInt driveNum = EDriveA; driveNum <= EDriveZ; ++driveNum)
   		if (componentDriveList[driveNum])
   			installedDrives |= 1 << driveNum;
			
	return installedDrives;
	}

void CSisRegistrySubSession::EmbeddingPackagesL(const TComponentId aCompId, TUid aCompUid,
									   RPointerArray<CSisRegistryPackage>& aEmbeddingPackages)
	{
	DEBUG_PRINTF2(_L("Sis Registry Server - Retrieving an array of packages embedding the component Id: %d."), aCompId);
	aEmbeddingPackages.ResetAndDestroy();

	// Get the package index
	TInt pkgIndex = GetIntPropertyValueL(iSession.ScrSession(), aCompId, KCompIndex, EFalse, KDefaultComponentIndex); 

	// Iterate through all the components
	CComponentFilter* filter = CComponentFilter::NewLC();
	filter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	filter->AddPropertyL(KCompEmbeddedPackagePresent, 1);
	
	RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession);	
	subSession.OpenViewL(iSession.ScrSession(), filter);

	for(;;)
		{
		CComponentEntry* component = subSession.NextComponentL();
		if(!component)
			{ 
			break;		// No more entries in the database.
			}
		CleanupStack::PushL(component);
		
		// Get the componentId of the component
		TComponentId embeddingCompId = component->ComponentId();
		
		// Get the number of embedded components
		TInt embeddedCount = GetIntPropertyValueL(iSession.ScrSession(), embeddingCompId, KCompEmbeddedPackageCount, EFalse, KDefaultEmbeddedPackageCount);
		
		for(TInt i = 0; i < embeddedCount; ++i)
			{
			// Retrieve the embedded component uid and index
			TBuf<KBigBufferSize> propertyName;
			propertyName.Format(KCompEmbeddedPackageUid, i);
			TUid embeddedPkgUid = TUid::Uid(GetIntPropertyValueL(iSession.ScrSession(), embeddingCompId, propertyName, ETrue));
			propertyName.Format(KCompEmbeddedPackageIndex, i);
			TInt embeddedPkgIndex = GetIntPropertyValueL(iSession.ScrSession(), embeddingCompId, propertyName, ETrue);
			// Comparing the componentId of aPackage with the value of CompEmbeddedComponentId property for each component entry in the database.
			if(embeddedPkgUid == aCompUid && embeddedPkgIndex == pkgIndex)
				{
				// Found a component that embedds the passed-in component
				CSisRegistryPackage* package = NULL;
				ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), embeddingCompId, package);
				CleanupStack::PushL(package);
				aEmbeddingPackages.AppendL(package);		// Ownership transferred
				CleanupStack::Pop(package);
				break;
				}
			}
		CleanupStack::PopAndDestroy(component);
		} // while
	CleanupStack::PopAndDestroy(2, filter);	// subSession
	}

void CSisRegistrySubSession::GenerateChainListL(const TComponentId aCompId, 
											 RPointerArray<HBufC8>& aChainList)	
	{
	aChainList.ResetAndDestroy();

	// Get the package details from SCR
	CSisRegistryPackage *package = NULL;
	ScrHelperUtil::ReadFromScrL(iSession.ScrSession(), aCompId, package);
	CleanupStack::PushL(package);

	// Get all the controller information
	RPointerArray<CControllerInfo> controllerInfoArray;	
	CleanupResetAndDestroy<RPointerArray<CControllerInfo> >::PushL(controllerInfoArray);
	ScrHelperUtil::InternalizeControllerArrayL(iSession.ScrSession(), aCompId, controllerInfoArray);

	TInt count = controllerInfoArray.Count();
	for (TInt i = 0; i < count; ++i)
		{
		// Open every controller to extract the information
		HBufC* name = SisRegistryUtil::BuildControllerFileNameLC(package->Uid(), package->Index(), controllerInfoArray[i]->Offset());
		CFileSisDataProvider* fileProvider = CFileSisDataProvider::NewLC(iSession.Fs(), *name);
		// Read the controller
		Swi::Sis::CController* controller = Swi::Sis::CController::NewLC(*fileProvider, Swi::Sis::EAssumeType);
		// Add the data to the list of certificate Chains
		TInt chainCount = controller->SignatureCertificateChains().Count();
		for (TInt j = 0; j < chainCount; ++j)
			{
			HBufC8* tmp = controller->SignatureCertificateChains()[j]->CertificateChain().Data().AllocLC();
			aChainList.AppendL(tmp);	// Ownership transferred
			CleanupStack::Pop(tmp);
			}
		CleanupStack::PopAndDestroy(3, name); // fileProvider, controller
		}	

	CleanupStack::PopAndDestroy(2, package);	// controllerInfoArray
	}
