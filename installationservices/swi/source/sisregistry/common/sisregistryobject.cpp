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
* SisRegistry - entry implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#include<s32strm.h>
#include <e32uid.h>

#include "siscontroller.h"
#include "swi/siscontents.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "sisversion.h"
#include "sisregistrypackage.h"
#include "sisregistryobject.h"
#include "sisregistryfiledescription.h"
#include "sisregistryproperty.h"
#include "sisregistrydependency.h"
#include "arrayutils.h"
#include "application.h"
#include "userselections.h"
#include "sissupportedlanguages.h"
#include "sisproperties.h"
#include "sisprerequisites.h"
#include "sisdependency.h"
#include "installtypes.h"
#include "sisinstallblock.h"
#include "hashcontainer.h"
#include "controllerinfo.h"
#include "securitycheckutil.h"
#include "log.h"
#include "versionutil.h"
#include "securityinfo.h"
#include "dessisdataprovider.h"
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "sisregistryhelperclient.h"


namespace 
{
    /// Note that a change in file major version indicates 
    /// non-interopeabilty between old code and new formats
    /// new code MAY be written to handle older file versions. 
    const TInt KCurrentFileMajorVersion = 5;
    /// changing the minor version to 4 to accomodate the locale
    /// specific language tokens for package and vendor names
    /// and changes from 9.4, 9.5 and MCL releases
    const TInt KCurrentFileMinorVersion = 4;
}
#endif

using namespace Swi;

EXPORT_C CSisRegistryObject* CSisRegistryObject::NewL()
	{
	CSisRegistryObject* self = CSisRegistryObject::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryObject* CSisRegistryObject::NewLC()
	{
	CSisRegistryObject* self = new(ELeave) CSisRegistryObject();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSisRegistryObject* CSisRegistryObject::NewL(RReadStream& aStream)
	{
	CSisRegistryObject* self = CSisRegistryObject::NewLC(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSisRegistryObject* CSisRegistryObject::NewLC(RReadStream& aStream)
	{
	CSisRegistryObject* self = new(ELeave) CSisRegistryObject();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

CSisRegistryObject::~CSisRegistryObject()
	{
	delete iVendorLocalizedName;
    iInstallChainIndices.Close();
	
	// in an effort to reduce memory consumption,
	// the registry object may not own all its own file descriptions...
	// delete only those we own.
	for (TInt i = 0; i < iOwnedFileDescriptions; ++i)
		{
		delete iFileDescriptions[i];
		}
	iFileDescriptions.Reset();	
	
	iProperties.ResetAndDestroy();
	iDependencies.ResetAndDestroy();
	iEmbeddedPackages.ResetAndDestroy();
	
	iSupportedLanguageIds.Close();
	iLocalizedPackageNames.ResetAndDestroy();
	iLocalizedVendorNames.ResetAndDestroy();
	
	}

void CSisRegistryObject::ConstructL()
	{
	CSisRegistryToken::ConstructL();
	}

void CSisRegistryObject::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	UpdateLocalizedInfoL();
	#endif
	}

TInt CSisRegistryObject::UserSelectedLanguageIndexL(const CApplication& aApplication) const
// used to find out which is the index of the selected language, 
// based on the language selection. This will be used for the relevant package and vendor names
	{
	TLanguage language = aApplication.UserSelections().Language();
	
	TInt index = KErrNotFound;
	for (TInt i = 0; i < aApplication.ControllerL().SupportedLanguages().Count(); i++)
		{
		if (aApplication.ControllerL().SupportedLanguages()[i] == language)
			{
			index = i;
			break;	
			}
		}
	User::LeaveIfError(index);
	return index;
	}	
	
void CSisRegistryObject::ProcessApplicationL(const CApplication& aApplication, const TDesC8& aController)
	{
	__ASSERT_ALWAYS(!(aApplication.IsUninstall()), User::Leave(KErrArgument));
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	iFileMajorVersion = KCurrentFileMajorVersion;
	iFileMinorVersion = KCurrentFileMinorVersion;
	#endif
	
    // Uid
 	iUid = aApplication.ControllerL().Info().Uid().Uid();
    
 	CMessageDigest* msgDigest = const_cast<Sis::CController&>(aApplication.ControllerL()).GenerateControllerHashLC(aController);
 	
 	StoreControllerInfoL(*msgDigest, aApplication.ControllerL().Info().Version());
	
	CleanupStack::PopAndDestroy(msgDigest);
	
    if (aApplication.IsInstall())
    	{
    	iInstallType = EInstInstallation;
    	}
    else if (aApplication.IsAugmentation())
    	{
    	iInstallType = EInstAugmentation;
    	}
    else if (aApplication.IsPartialUpgrade())
    	{
    	iInstallType = EInstPartialUpgrade;
    	}
    else if (aApplication.IsPreInstalledApp())
    	{
    	iInstallType = EInstPreInstalledApp;
    	}
    else if (aApplication.IsPreInstalledPatch())
    	{
    	iInstallType = EInstPreInstalledPatch;
    	}
    	
	iSelectedDrive = aApplication.UserSelections().Drive();
	iInRom = aApplication.IsInROM();

	// Set whether this object represents a preinstalled application which
	// should have its files deleted on uninstall.  This will be true if
	// allowed by the swipolicy and the stub sis file was writable at install
	// time.
	iDeletablePreInstalled = aApplication.IsDeletablePreinstalled();

	iLanguage = aApplication.UserSelections().Language();

	PopulateLocalizedPackageAndVendorNamesL(aApplication);

	iVendorName = aApplication.ControllerL().Info().UniqueVendorName().Data().AllocL();

	if (aApplication.IsUpgrade())
		{
		iIndex = aApplication.PackageL().Index();
		}
	
	// signed 
	iSigned = (aApplication.ControllerL().SignatureCertificateChains().Count() == 0)?EFalse:ETrue;

	// signed with SU Cert and allow install based on SU Cert Rules
	iSignedBySuCert = aApplication.IsInstallSuCertBased();
	
	// removable
	iIsRemovable = !(aApplication.ControllerL().Info().InstallFlags() & EInstFlagNonRemovable);

	// Get trust level of the application
	iTrust = aApplication.Trust();
	
	// Set timestamp for the level of trust 
	iTrustTimeStamp.UniversalTime();

    iTrustStatus = aApplication.ControllerL().TrustStatus();
    
    iRemoveWithLastDependent = aApplication.ControllerL().RemoveWithLastDependent();
    
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    iIsDrmProtected = aApplication.IsDrmProtected();
    
    iIsHidden = !(!(aApplication.ControllerL().Info().InstallFlags() & EInstFlagHide));
    #endif
    
    const RArray<TInt>& indices = aApplication.ControllerL().CertChainIndices();
    
    CopyArrayL(iInstallChainIndices, indices);
        
	StorePropertiesL(aApplication.ControllerL());
	StoreDependenciesL(aApplication.ControllerL());
	UpdateEmbeddedAppsL(aApplication);
	AddFilesL(aApplication);
	UpdateDrivesL();
	}

EXPORT_C void CSisRegistryObject::ProcessInRomControllerL(const Swi::Sis::CController& aController, 
													 	const TDesC8& aControllerData)
	{
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	iFileMajorVersion = KCurrentFileMajorVersion;
	iFileMinorVersion = KCurrentFileMinorVersion;
	#endif
	
    // Uid
 	iUid = aController.Info().Uid().Uid();

 	CMessageDigest* msgDigest = const_cast<Sis::CController&>(aController).GenerateControllerHashLC(aControllerData);
 	
 	StoreControllerInfoL(*msgDigest, aController.Info().Version());

	CleanupStack::PopAndDestroy(msgDigest);

    // install type, drive, inrom, preinstalled, selected language
    iInstallType = aController.Info().InstallType();
	iInRom = ETrue;

	// If pre-installed in ROM, cannot be deletable.
	iDeletablePreInstalled = EFalse;
	
	// user selection index; set to default english
	TInt langIndx = 0;
	
	iLanguage = aController.SupportedLanguages()[langIndx];
	
	// get the names
	if (langIndx < aController.Info().Names().Count())
		{
		iPackageName = aController.Info().Names()[langIndx]->Data().AllocL();
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
		
	if (langIndx < aController.Info().VendorNames().Count())
		{
		iVendorLocalizedName = aController.Info().VendorNames()[langIndx]->Data().AllocL();
		}
	else
		{
		User::Leave(KErrCorrupt);
		}		
	iVendorName = aController.Info().UniqueVendorName().Data().AllocL(); 
    	
	iSigned = (aController.SignatureCertificateChains().Count() == 0)? EFalse:ETrue;

	// removable
	iIsRemovable = EFalse;

	// Set trust level 
	iTrust = ESisPackageBuiltIntoRom;
	
	// Set timestamp for trust
	iTrustTimeStamp.UniversalTime();
	
    iTrustStatus.SetValidationStatus(EPackageInRom);
    iTrustStatus.SetRevocationStatus(EOcspNotPerformed);
    CopyArrayL(iInstallChainIndices, aController.CertChainIndices());
    
    iRemoveWithLastDependent = aController.RemoveWithLastDependent();
    
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    // By default the DRM protection will be false for ROM based packages.
    iIsDrmProtected = EFalse;
    // By default the component's hidden property will be False
    iIsHidden = EFalse;
    #endif
    
	StorePropertiesL(aController);
	StoreDependenciesL(aController);
	const RPointerArray<Sis::CFileDescription> depArray = 
									aController.InstallBlock().FileDescriptions();
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
							
	// populate data with file description and SIDs
	for (TInt file = 0; file < depArray.Count(); file++)
		{
		CSisRegistryFileDescription* fileDescription = 
					CSisRegistryFileDescription::NewLC(*depArray[file], 'z', ETrue);
		const TDesC& target = fileDescription->Target();
 		
 		// if the target name contains wildchars, do not open to check SID, it is for eclipsing 						
 		if( (target.LocateF('?') == KErrNotFound && target.LocateF('*') == KErrNotFound)
 				&& target.FindF(KBinPath) != KErrNotFound ) 
 			{
 			TEntry entry;
			TInt err = fs.Entry(target, entry);
		
			if (err == KErrNone)
				{
				// Target exists
				// If it's an executable, grab its SID to register
				if (entry.IsTypeValid() && (entry[0].iUid == KExecutableImageUidValue))
					{
					TSecurityInfo info;
					SecurityInfo::RetrieveExecutableSecurityInfoL(fs, target, info);				
					TUid sid = {info.iSecureId.iId};					
					fileDescription->SetSid(sid);
					if(sid != KNullUid)
						{
						iSids.AppendL(sid);
						}			
					}			
				}
			else if (err != KErrNotFound && err != KErrPathNotFound)
				{
				// Reading the fs entry failed with an unexpected error
				User::Leave(err);
				}
	 		}			
		iFileDescriptions.AppendL(fileDescription);
		iOwnedFileDescriptions++;
		CleanupStack::Pop(fileDescription); 
		}
		
	CleanupStack::PopAndDestroy(&fs);
	iDrives = 1 << EDriveZ;
	}
		 
void CSisRegistryObject::UpgradeApplicationL(const CApplication& aApplication, const TDesC8& aController)
	{
	__ASSERT_ALWAYS(!(aApplication.IsUninstall()), User::Leave(KErrArgument));
	
 	CMessageDigest* msgDigest = const_cast<Sis::CController&>(aApplication.ControllerL()).GenerateControllerHashLC(aController);
 	
 	StoreControllerInfoL(*msgDigest, aApplication.ControllerL().Info().Version());
	
	CleanupStack::PopAndDestroy(msgDigest);
   
    iInstallType = aApplication.ControllerL().Info().InstallType();
		
    // install type, drive, inrom, preinstalled, selected language
	iSelectedDrive = aApplication.UserSelections().Drive();
	//if this app is an upgrade to a ROM pkg then the original value of iInRom is ETRUE and the resulting value will automatically remain TRUE
	//however if it isn't the case then aApplication.IsInROM()'s return value will decide the resulting value
	if(iInRom == EFalse) //that's package is NOT in ROM
		{
		iInRom = aApplication.IsInROM();
		}
	
	// signed 
	if (iSigned == EFalse)
		{
		iSigned = (aApplication.ControllerL().SignatureCertificateChains().Count() == 0)?EFalse:ETrue;
		}

	// signed with SU Cert and allow install based on SU Cert Rules
	iSignedBySuCert = aApplication.IsInstallSuCertBased();

	// Get trust level of application
	iTrust = aApplication.Trust();
	
	// Set timestamp for trust
	iTrustTimeStamp.UniversalTime();
    
    iTrustStatus = aApplication.ControllerL().TrustStatus();
    
    iRemoveWithLastDependent = aApplication.ControllerL().RemoveWithLastDependent();
     
    const RArray<TInt>& indices = aApplication.ControllerL().CertChainIndices();
	CopyArrayL(iInstallChainIndices, indices);
        
	StorePropertiesL(aApplication.ControllerL());
	StoreDependenciesL(aApplication.ControllerL());
	UpdateEmbeddedAppsL(aApplication);
	RemoveFilesL(aApplication);
	AddFilesL(aApplication);
	UpdateDrivesL();
	PopulateLocalizedPackageAndVendorNamesL(aApplication);
	}
	
EXPORT_C void CSisRegistryObject::ExternalizeL(RWriteStream& aStream) const
	{
	// the package section
	CSisRegistryToken::ExternalizeL(aStream);
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	aStream.WriteInt16L(iFileMajorVersion);
	aStream.WriteInt16L(iFileMinorVersion);
	#endif
	
	// general bit
	aStream << *iVendorLocalizedName;

	aStream.WriteUint32L(static_cast<TUint>(iInstallType));
	aStream.WriteUint32L(static_cast<TUint>(iInRom));
	
	aStream.WriteUint32L(static_cast<TUint>(iDeletablePreInstalled));
	aStream.WriteUint32L(static_cast<TUint>(iSigned));
	
	aStream.WriteInt32L(static_cast<TInt>(iTrust));

	// uninstallation option
    aStream.WriteInt32L(iRemoveWithLastDependent);

	// Write out the timestamp
	TPckg <TTime> timePckg(iTrustTimeStamp);
	aStream.WriteL(timePckg, timePckg.MaxLength());

	ExternalizePointerArrayL(iDependencies, aStream);
	ExternalizePointerArrayL(iEmbeddedPackages, aStream);
	ExternalizePointerArrayL(iProperties, aStream);
	ExternalizePointerArrayL(iFileDescriptions, aStream);
	
	iTrustStatus.ExternalizeL(aStream);
	
	ExternalizeArrayL(iInstallChainIndices, aStream);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	aStream.WriteUint32L(static_cast<TUint>(iIsRemovable));
	aStream.WriteUint32L(static_cast<TUint>(iSignedBySuCert));
	aStream.WriteUint32L(static_cast<TUint>(iIsDrmProtected));
	aStream.WriteUint32L(static_cast<TUint>(iIsHidden));
	ExternalizeArrayL(iSupportedLanguageIds, aStream);
	ExternalizePointerArrayL(iLocalizedPackageNames, aStream);
	ExternalizePointerArrayL(iLocalizedVendorNames, aStream);
	#else
    if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion > 0))
    	{	
		aStream.WriteUint32L(static_cast<TUint>(iIsRemovable));
    	}
    else
    	{	
    	aStream.WriteUint32L(static_cast<TUint>(ETrue));	
    	}
	if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 3))
	   	{	
		aStream.WriteUint32L(static_cast<TUint>(iSignedBySuCert));
	   	}
	   else
	   	{	
	   	aStream.WriteUint32L(static_cast<TUint>(EFalse));	
	   	}	
	// Changes introduced as part of 5.4 version of SIS Registry *.reg file format
	if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 4))
		{   
		ExternalizeArrayL(iSupportedLanguageIds, aStream);
		ExternalizePointerArrayL(iLocalizedPackageNames, aStream);
		ExternalizePointerArrayL(iLocalizedVendorNames, aStream);
		}

	#endif
	}

EXPORT_C void CSisRegistryObject::InternalizeL(RReadStream& aStream)
	{
	CSisRegistryToken::InternalizeL(aStream);
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iFileMajorVersion = aStream.ReadInt16L();
	iFileMinorVersion = aStream.ReadInt16L();
		
	if (iFileMajorVersion > KCurrentFileMajorVersion)
	    {
	    User::Leave(KErrNotSupported);
	    }
	#endif

	// no restriction on name; can't use KMaxFileName	
	iVendorLocalizedName = HBufC::NewL(aStream, KMaxTInt);
	iInstallType = static_cast<Sis::TInstallType>(aStream.ReadUint32L());
	iInRom = static_cast<TBool>(aStream.ReadUint32L());

	iDeletablePreInstalled = static_cast<TBool>(aStream.ReadUint32L());
	iSigned = static_cast<TBool>(aStream.ReadUint32L());
	
	iTrust = static_cast <TSisPackageTrust>(aStream.ReadInt32L());
	
	// unistallation option
	iRemoveWithLastDependent = aStream.ReadInt32L();
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	if(iFileMajorVersion < 5)
		{
		iRemoveWithLastDependent = 0;
		}
	#endif
		
	// read the timestamp
	TPckg <TTime> timePckg(iTrustTimeStamp);
	aStream.ReadL(timePckg, timePckg.MaxLength());

	InternalizePointerArrayL(iDependencies, aStream);
	InternalizePointerArrayL(iEmbeddedPackages, aStream);
	InternalizePointerArrayL(iProperties, aStream);
	
	InternalizePointerArrayL(iFileDescriptions, aStream);
	iOwnedFileDescriptions = iFileDescriptions.Count();
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iTrustStatus.InternalizeL(aStream);
    InternalizeArrayL(iInstallChainIndices, aStream);    
    
    iIsRemovable = static_cast<TBool>(aStream.ReadUint32L());
    iSignedBySuCert = static_cast<TBool>(aStream.ReadUint32L());
    iIsDrmProtected = static_cast<TBool>(aStream.ReadUint32L());
    iIsHidden = static_cast<TBool>(aStream.ReadUint32L());
    InternalizeArrayL(iSupportedLanguageIds,aStream);
    InternalizePointerArrayL(iLocalizedPackageNames,aStream);
    InternalizePointerArrayL(iLocalizedVendorNames,aStream);
	#else
    if(iFileMajorVersion > 2)
    	{
		iTrustStatus.InternalizeL(aStream);
        InternalizeArrayL(iInstallChainIndices, aStream);
       	}
    else if(iFileMajorVersion == 2)
        {
        // This caused problems because sizeof(TSisTrustStatus)
        // is different on hardware vs emulator
        TPckg<TSisTrustStatus> trustPkg(iTrustStatus);
        aStream.ReadL(trustPkg, trustPkg.MaxLength());
        InternalizeArrayL(iInstallChainIndices, aStream);
        iFileMajorVersion = KCurrentFileMajorVersion;
	    iFileMinorVersion = KCurrentFileMinorVersion;
		}
    else 
        {
        ConvertTrustToTrustStatus(iTrust, iTrustStatus);
        iFileMajorVersion = KCurrentFileMajorVersion;
	    iFileMinorVersion = KCurrentFileMinorVersion;
        }
        
    iIsRemovable = ETrue;
    if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion > 0))
    	{
		iIsRemovable = static_cast<TBool>(aStream.ReadUint32L());    	
    	}

	if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 3))
	   	{
		iSignedBySuCert = static_cast<TBool>(aStream.ReadUint32L());    	
	   	}
	// Changes introduced as part 5.4 version of SIS Registry *.reg file format    
    if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 4))
    	{
    	InternalizeArrayL(iSupportedLanguageIds,aStream);
    	InternalizePointerArrayL(iLocalizedPackageNames,aStream);
    	InternalizePointerArrayL(iLocalizedVendorNames,aStream);
    	}
	#endif
	}
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CSisRegistryObject::ConvertTrustToTrustStatus(
    const TSisPackageTrust& trust, 
    TSisTrustStatus& trustStatus) const
    {
    trustStatus.SetRevocationStatus(EOcspNotPerformed);

    switch (trust) 
        {
        case ESisPackageUnsignedOrSelfSigned:
            // Untrusted
            trustStatus.SetValidationStatus(EUnsigned);
            break;
        case ESisPackageValidationFailed:
            // Untrusted
            trustStatus.SetValidationStatus(EInvalid);
            break;
        case ESisPackageCertificateChainNoTrustAnchor:
            // Untrusted
            trustStatus.SetValidationStatus(EValidated);
            break;
        case ESisPackageCertificateChainValidatedToTrustAnchor:
            // Trusted
            trustStatus.SetValidationStatus(EValidatedToAnchor);
            trustStatus.SetRevocationStatus(EOcspNotPerformed);
            break;
        case ESisPackageChainValidatedToTrustAnchorOCSPTransientError:
          // Trusted
            trustStatus.SetValidationStatus(EValidatedToAnchor);
            trustStatus.SetRevocationStatus(EOcspTransient);
            break;
        case ESisPackageChainValidatedToTrustAnchorAndOCSPValid:
          // Trusted
            trustStatus.SetValidationStatus(EValidatedToAnchor);
            trustStatus.SetRevocationStatus(EOcspGood);
            break;
        case ESisPackageBuiltIntoRom:
          // Trusted
            trustStatus.SetValidationStatus(EPackageInRom);
            trustStatus.SetRevocationStatus(EOcspNotPerformed);
            break;
        }
    }

EXPORT_C void CSisRegistryObject::EmbeddedPackagesL(RPointerArray<CSisRegistryPackage>& aEmbedded)  const
	{
	CopyPointerArrayL(aEmbedded, iEmbeddedPackages);
	}

EXPORT_C void CSisRegistryObject::DependsOnL(RPointerArray<CSisRegistryDependency>& aDependents)  const
	{
	// populate the aDependents array and pass the responsibility for deallocation to client
	CopyPointerArrayL(aDependents, iDependencies);
	}
	
EXPORT_C TInt CSisRegistryObject::Property(TInt aKey) const
	{
	for (TInt property=0; property<iProperties.Count(); property++)
		{
		if(iProperties[property]->Key() == aKey)
			{
			return iProperties[property]->Value();
			}
		}
	return KErrNotFound;
	}

EXPORT_C const CSisRegistryFileDescription& CSisRegistryObject::FileDescriptionL(const TDesC& aFileName) const
	{
	for (TInt i = 0; i < iFileDescriptions.Count(); i++)
		{
		if (iFileDescriptions[i]->Target().CompareF(aFileName) == 0)
			{
			return *iFileDescriptions[i];
			}		
		}

	User::Leave(KErrNotFound);
	// cannot reach this section but to keep compiler happy 
	return *iFileDescriptions[0];	
	} 

EXPORT_C const CSisRegistryFileDescription& CSisRegistryObject::FileDescriptionL(TUid aSid) const
	{
	for (TInt fileIndex = 0; fileIndex<iFileDescriptions.Count(); fileIndex++)
		{
		if (iFileDescriptions[fileIndex]->Sid() == aSid)
			{
			return *iFileDescriptions[fileIndex];
			}
		}
	User::Leave(KErrNotFound);
	// cannot reach this section but to keep compiler happy 
	return *iFileDescriptions[0];	
	}

EXPORT_C TBool CSisRegistryObject::DependsOnObject(const CSisRegistryObject& aObject) const
	{
	TUid matchingUid = aObject.Uid();
	TVersion matchingVersion = aObject.Version();

	// check whether has matching UID in its dependency list
	// and check that the versions are compatible 
    for (TInt i = 0; i < iDependencies.Count(); i++)
    	{
    	if (matchingUid == iDependencies[i]->Uid() && 
    		iDependencies[i]->IsCompatible(matchingVersion))
    		{
    		return ETrue;
    		}
    	}
    return EFalse;
	}
	
EXPORT_C TBool CSisRegistryObject::EmbedsPackage(const CSisRegistryPackage& aPackage) const
	{	
	// check whether packages match
	for (TInt i = 0; i < iEmbeddedPackages.Count(); i++)
    	{
    	if (*iEmbeddedPackages[i] == aPackage)
    		{
    		return ETrue;
    		}
    	}
    return EFalse;
	}
	
EXPORT_C TInt64 CSisRegistryObject::SizeL() const 
	{
	// at this time the size is calculated based on the file description information
	// we'll need probably to address the size on the device if assume 
	// that that would be most accurate...
	TInt64 size = 0;
	for(TInt i = 0; i < iFileDescriptions.Count(); i++)
		{
		size += iFileDescriptions[i]->UncompressedLength();
		}
	return size;
	}		

EXPORT_C void CSisRegistryObject::SetTrustStatus(const TSisTrustStatus& status)
    {
        iTrustStatus = status;
    }
        
EXPORT_C void CSisRegistryObject::SetRemoveWithLastDependent()
	{
	iRemoveWithLastDependent = 1;
	}

void CSisRegistryObject::UpdateLocalizedInfoL()
	{
	TLanguage currentLanguage = User::Language();
	TInt count = iSupportedLanguageIds.Count();
	TInt leastIndex = 0;
	TInt i = 0;
	for ( i = 0; i<count; i++)
		{
			// If current language is one among the languages supported
			// by SIS file then update the locale specific data	
			if ( currentLanguage == iSupportedLanguageIds[i])
				{
				delete iVendorLocalizedName;
				delete iPackageName;
				iVendorLocalizedName = iLocalizedVendorNames[i]->AllocL();
				iPackageName = iLocalizedPackageNames[i]->AllocL();
				return;
				}
			if ( iSupportedLanguageIds[leastIndex] > iSupportedLanguageIds[i] )
				{
				leastIndex = i;
				}
		}

	if ( count > 0)
	    {
        RSisRegistryHelper sisRegistryHelper;
        User::LeaveIfError(sisRegistryHelper.Connect());
        CleanupClosePushL(sisRegistryHelper);
        
        RArray<TLanguage> equivalentLanguages;
        
        sisRegistryHelper.GetEquivalentLanguagesL(currentLanguage, equivalentLanguages);
        CleanupClosePushL(equivalentLanguages);
        TInt size = equivalentLanguages.Count();
        for ( TInt i = 0; i < size; i++ )	
            {
            for (TInt j=0; j< count; j++ )
                {
                if ( iSupportedLanguageIds[j] == equivalentLanguages[i])
                    {
                    delete iVendorLocalizedName;
                    delete iPackageName;
                    iVendorLocalizedName = iLocalizedVendorNames[j]->AllocL();
                    iPackageName = iLocalizedPackageNames[j]->AllocL();
                    CleanupStack::PopAndDestroy(2, &sisRegistryHelper);
                    return;
                    }
                }
            }
        // Update the localized package name and vendor name with the least language ID
        delete iVendorLocalizedName;
        delete iPackageName;
        iVendorLocalizedName = iLocalizedVendorNames[leastIndex]->AllocL();
        iPackageName = iLocalizedPackageNames[leastIndex]->AllocL();
        CleanupStack::PopAndDestroy(2, &sisRegistryHelper);
        }
	return;
	}

#endif
void CSisRegistryObject::PopulateLocalizedPackageAndVendorNamesL(const CApplication& aApplication)
	{
	// user selection index
	TInt langIndx = UserSelectedLanguageIndexL(aApplication);

	const RArray<TInt>& matchingDeviceLanguagesArray = (const_cast <CApplication&>(aApplication)).GetMatchingDeviceLanguages();
	TInt matchingLanguageCount = matchingDeviceLanguagesArray.Count();
	
	// If matchingLanguageCount is zero, then use the available localized names.
	if ( 0 == matchingLanguageCount)
		{
		if ( 0 == iPackageName )
		    {
            iPackageName = aApplication.ControllerL().Info().Names()[langIndx]->Data().AllocL();
            iVendorLocalizedName = aApplication.ControllerL().Info().VendorNames()[langIndx]->Data().AllocL();
            }
		}
	else
		{
		// Update the Array's containing localized PackageNames and VendorLocalizedNames
		for (TInt i=0; i < matchingLanguageCount; i++)
			{
			TInt languageId = matchingDeviceLanguagesArray[i];
			TInt supportedLanguageCount = aApplication.ControllerL().SupportedLanguages().Count();
			for ( TInt j=0; j < supportedLanguageCount; j++ )
				{
				if ( aApplication.ControllerL().SupportedLanguages()[j] == languageId)
					{
					TInt index = iSupportedLanguageIds.Find(languageId);
					if ( KErrNotFound == index )
					    {
					    iSupportedLanguageIds.AppendL(matchingDeviceLanguagesArray[i]);
					    iLocalizedVendorNames.AppendL(aApplication.ControllerL().Info().VendorNames()[j]->Data().AllocL());
					    iLocalizedPackageNames.AppendL(aApplication.ControllerL().Info().Names()[j]->Data().AllocL());
					    break;
					    }
					else
					    {
					    iSupportedLanguageIds.Remove(index);
					    delete iLocalizedVendorNames[index];
					    iLocalizedVendorNames.Remove(index);
					    delete iLocalizedPackageNames[index];
					    iLocalizedPackageNames.Remove(index);
                        iSupportedLanguageIds.AppendL(matchingDeviceLanguagesArray[i]);
                        iLocalizedVendorNames.AppendL(aApplication.ControllerL().Info().VendorNames()[j]->Data().AllocL());
                        iLocalizedPackageNames.AppendL(aApplication.ControllerL().Info().Names()[j]->Data().AllocL());
					    }
					break;
					}
				}
			}
		if ( 0 == iPackageName )
		    {
                iPackageName = aApplication.ControllerL().Info().Names()[langIndx]->Data().AllocL();
                iVendorLocalizedName = aApplication.ControllerL().Info().VendorNames()[langIndx]->Data().AllocL();
		    }
		}
	
	}


EXPORT_C const RArray<TInt>& CSisRegistryObject::InstallChainIndicies() const
    {
        return iInstallChainIndices;
    }

EXPORT_C TInt CSisRegistryObject::RemoveWithLastDependent() const
	{
	return iRemoveWithLastDependent;
	}

EXPORT_C const TSisTrustStatus& CSisRegistryObject::TrustStatus() const
    {
        return iTrustStatus;
    }
	
void CSisRegistryObject::AddFilesL(const CApplication& aApplication)
	{
	for (TInt file = 0; file < aApplication.FilesToAdd().Count(); file++)
		{
		// to save memory in these very transient sis registry objects, we don't
		// actually own every file description.
		
		CSisRegistryFileDescription* desc = 
			const_cast<CSisRegistryFileDescription*>(aApplication.FilesToAdd()[file]);
		iFileDescriptions.AppendL(desc);
		TUid sid = desc->Sid();
		if(sid != KNullUid)
			{
			// if the sid is there already there is no reason to add it again
			// and its presence in the list may not necessary be a registry error 
			if(iSids.Find(sid) == KErrNotFound)
				{
				iSids.AppendL(sid);
				}
			}
		}
	}

void CSisRegistryObject::RemoveFilesL(const CApplication& aApplication)
	{
	for (TInt i = 0; i < aApplication.FilesToRemove().Count(); i++)
		{
	    for (TInt j = iFileDescriptions.Count()-1; j >= 0; j--)
		    {
		    TInt res = aApplication.FilesToRemove()[i]->Target().CompareF(iFileDescriptions[j]->Target());
		    if(res == 0)	
			    {
			    TUid sid = iFileDescriptions[j]->Sid();
				if(sid != KNullUid)
					{
					iSids.Remove(iSids.Find(sid));
					}
				delete iFileDescriptions[j];
			    iFileDescriptions.Remove(j);
			    --iOwnedFileDescriptions;
			    break;	
			    }
		    }
		}	
	}

void CSisRegistryObject::StorePropertiesL(const Sis::CController& aController)
	{
	// clean properties
	iProperties.ResetAndDestroy();
	for (TInt i = 0; i < aController.Properties().Count(); i++)
		{
		// casting away const -ness, practical necessity unfortunately
		CSisRegistryProperty* property = 
			CSisRegistryProperty::NewLC((Sis::CProperty&)(aController.Properties()[i]));
		iProperties.AppendL(property);
		CleanupStack::Pop(property);
		}
	}

void CSisRegistryObject::StoreDependenciesL(const Sis::CController& aController)
	{
    // Dependencies - Prerequisites
	iDependencies.ResetAndDestroy();
	RPointerArray<Sis::CDependency> dependencies = aController.Prerequisites().Dependencies(); 	
	for (TInt i = 0; i < dependencies.Count(); i++)
		{
		CSisRegistryDependency* dependency = CSisRegistryDependency::NewLC(*dependencies[i]);
		iDependencies.AppendL(dependency);
		CleanupStack::Pop(dependency);
		}
	}
		
void CSisRegistryObject::UpdateEmbeddedAppsL(const CApplication& aApplication)
	{
	// The list of embedded packages
	TInt langIndx = 0;
	for (TInt i = 0; i < aApplication.EmbeddedApplications().Count(); i++)
		{
		const CApplication& embeddedApplication = *aApplication.EmbeddedApplications()[i];

		if (embeddedApplication.IsUninstall())
			{
			continue; // We don't want to add applications which are being uninstalled
			}
		// Upgrade or a partial upgrade should not add a new dependency - this is not a new package,
		// so it should not be a new dependency. This check is done only for embedding partial upgrades or augmentations - since
		// they are the only types which can add new dependencies to existing entries in the registry
		if ((aApplication.IsPartialUpgrade() || aApplication.IsAugmentation()) &&
			(embeddedApplication.IsUpgrade() || embeddedApplication.IsPartialUpgrade()))			
			{
			continue;
			}
        // find which is the index of the selected language so 
        // one can create a package with the proper package and vendor names acc locale
		langIndx = UserSelectedLanguageIndexL(embeddedApplication);
		const Sis::CInfo& info = embeddedApplication.ControllerL().Info();

		CSisRegistryPackage* package = CSisRegistryPackage::NewLC(	
															info.Uid().Uid(),
															info.Names()[langIndx]->Data(),
															info.UniqueVendorName().Data()
															);		
		iEmbeddedPackages.AppendL(package);
		CleanupStack::Pop(package);
		}
	}
	
void CSisRegistryObject::UpdateDrivesL()
	{
	// the constructor for token sets the drives to zero but reset to be sure
	iDrives = 0;
	RFs fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);
	for (TInt i = 0; i < iFileDescriptions.Count(); i++)
		{
		TDriveUnit drive(iFileDescriptions[i]->Target());
 	    TVolumeInfo volInfo;
 	    if (KErrNone == fs.Volume(volInfo, drive) || iFileDescriptions[i]->Operation() != Sis::EOpNull)
 		    {
 		    // update the drive bitmap
 		    iDrives |= 1 << drive;								    	
 		    }		    
		}
	CleanupStack::PopAndDestroy(&fs);
	}

void CSisRegistryObject::StoreControllerInfoL(const CMessageDigest& aMsgDigest, const Sis::CVersion& aVersion)
	{
	// calculate and store controller hash and version as they are packed together
	Set(iVersion, aVersion);

	TPtrC8 hash = const_cast<CMessageDigest&>(aMsgDigest).Final();
	CHashContainer* controllerHash = CHashContainer::NewLC(CMessageDigest::ESHA1, hash);
	CControllerInfo* controllerInfo = CControllerInfo::NewLC(iVersion, *controllerHash, iControllerInfo.Count());

	iControllerInfo.AppendL(controllerInfo);
	CleanupStack::Pop(controllerInfo);
	CleanupStack::PopAndDestroy(controllerHash); //controllerHash
	}

EXPORT_C CSisRegistryPackage* CSisRegistryObject::EmbeddedPackage(TInt aIndex)
	{
	CSisRegistryPackage* ret = NULL;
	if (aIndex >= 0 && aIndex < iEmbeddedPackages.Count())
		{
		ret = iEmbeddedPackages[aIndex];
		}
	return ret;
	}
