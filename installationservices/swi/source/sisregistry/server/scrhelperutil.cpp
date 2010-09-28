/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* ScrHelperUtil class provides the list of functionalities needed to store and retrieve the 
* SIS Registry Object in to and from the Software Component Registry respectively.
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32uid.h>

#include "scrhelperutil.h"
#include "log.h"
#include "scrdbconstants.h"
#include "sisregistryobject.h"
#include "sisregistrytoken.h"
#include "sisregistrypackage.h"
#include "hashcontainer.h"
#include "sisproperties.h"
#include "controllerinfo.h"
#include "sisregistrydependency.h"
#include "sisregistryfiledescription.h"
#include "sisregistryproperty.h"
#include "cleanuputils.h"
#include <usif/usifcommon.h>
#include <swi/sisregistrylog.h>
#include <scs/streamingarray.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

using namespace Swi;

// Store the SIS Registry Object details in to the Software Component Registry - SCR.
// The component object should have been fully constructed before calling this method.
void ScrHelperUtil::AddComponentL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType)
	{	
	WriteToScrL(aScrSession, aCompId, aObject, aOpType);
	////StopTimerAndWriteResultL(timer, _L8(" WriteToScrL "));
	// The default value of isRemovable is true in SCR - we change it only if needed
	if(aObject.iIsRemovable == EFalse)
		{
		aScrSession.SetIsComponentRemovableL(aCompId, aObject.IsRemovable());
		}

	aScrSession.SetVendorNameL(aCompId, *(aObject.iVendorLocalizedName), aObject.Language());
	
	if(KDefaultInstallType != aObject.InstallType())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompInstallType, aObject.InstallType());	
		}		
	
	if(KDefaultIsInRom != aObject.InRom())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompInRom, aObject.InRom());
		}		
	
	if(KDefaultIsDeletablePreInstalled != aObject.DeletablePreInstalled())	
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompDeletablePreinstalled, aObject.DeletablePreInstalled());
		}		
	
	if(KDefaultIsSigned != aObject.IsSigned())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompIsSigned, aObject.IsSigned());
		}		
	
	if(KDefaultTrustValue != aObject.Trust())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrust, aObject.Trust());
		}		
	
	if(KDefaultRemoveWithLastDependent != aObject.RemoveWithLastDependent())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompIsRemoveWithLastDependent, aObject.RemoveWithLastDependent());
		}
	
	aScrSession.SetComponentPropertyL(aCompId, KCompTrustTimeStamp, aObject.TrustTimeStamp().Int64());	
	
	if(KDefaultIsSignedBySuCert != aObject.IsSignedBySuCert())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompSignedBySuCert, aObject.IsSignedBySuCert());
		}		

	if(KDefaultIsDrmProtected != aObject.IsDrmProtected())
		{
		aScrSession.SetIsComponentDrmProtectedL(aCompId, aObject.IsDrmProtected());
		}
	
	if(KDefaultIsComponentHidden != aObject.IsHidden())
		{
		aScrSession.SetIsComponentHiddenL(aCompId, aObject.IsHidden());
		}
	
	ExternalizeSupplierPackageArrayL(aScrSession, aObject.Uid(), aObject.Name(), aObject.InstallType(), aObject.iDependencies);
	ExternalizeEmbeddedPackageArrayL(aScrSession, aCompId, aObject.iEmbeddedPackages);
	ExternalizePropertiesArrayL(aScrSession, aCompId, aObject.iProperties);
	ExternalizeFileDescriptionsArrayL(aScrSession, aCompId, aObject.FileDescriptions());
	WriteToScrL(aScrSession, aCompId, aObject.TrustStatus());
	ExternalizeChainIndexArrayL(aScrSession, aCompId, aObject.InstallChainIndicies());
	}

void ScrHelperUtil::AddApplicationEntryL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, const Usif::CApplicationRegistrationData& aApplicationRegistrationData)
    {
    aScrSession.AddApplicationEntryL(aCompId, aApplicationRegistrationData);
    }

void ScrHelperUtil::DeleteApplicationEntriesL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId)
    {
    aScrSession.DeleteApplicationEntriesL(aCompId);
    }

void ScrHelperUtil::DeleteApplicationEntryL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aAppUId)
    {
    aScrSession.DeleteApplicationEntryL(aAppUId);
    }

void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType)
	{
	
	// Add the component name, it's vendor name and the supporting language in to the SCR.
	RPointerArray<Usif::CLocalizableComponentInfo> componentInfo;
	CleanupResetAndDestroyPushL(componentInfo);
	TInt matchingLanguageCount = aObject.GetLocalizedPackageNamesArray().Count();
	Usif::CLocalizableComponentInfo* component;
	if( matchingLanguageCount > 0 )
		{
		for(TInt i=0;i<matchingLanguageCount;i++)
			{
			component = Usif::CLocalizableComponentInfo::NewLC((*(aObject.GetLocalizedPackageNamesArray()[i])), (*(aObject.GetLocalizedVendorNamesArray()[i])), (TLanguage)aObject.GetSupportedLanguageIdsArray()[i]);
			componentInfo.AppendL(component);
			CleanupStack::Pop(component);
			}
		}
	else
		{
		component = Usif::CLocalizableComponentInfo::NewLC(aObject.Name(), aObject.Vendor(), aObject.Language());
		componentInfo.AppendL(component);
		CleanupStack::Pop(component);
		}
	
	HBufC* compGlobalId = NULL;
	// For all SP and PP type packages, global Id is composed with package Uid and name.
	if (aObject.Index() > 0)
		{
		const TDesC& packageName = aObject.Name();
		compGlobalId = HBufC::NewLC(KCompUidSize + packageName.Size());
		TPtr ptr(compGlobalId->Des());
		ptr.AppendFormat(KPatchGlobalIdFormat, aObject.Uid().iUid, &(aObject.Name()));
		}
	// For all other non-SP and non-PP packages the global Id is simply the package Uid.		
	else
		{
		compGlobalId = HBufC::NewLC(KCompUidSize);
		TPtr ptr(compGlobalId->Des());
		ptr.AppendFormat(KGlobalIdFormat, aObject.Uid().iUid);
		}	

	aCompId = aScrSession.AddComponentL(componentInfo, KNativeSoftwareType, compGlobalId, aOpType);
	CleanupStack::PopAndDestroy(2, &componentInfo);
	
	// Write the SIS Registry Token details
	WriteToScrL(aScrSession, aCompId, static_cast<CSisRegistryToken&>(aObject));
	}

// Store the SIS Registry Token details in to the SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryToken& aToken)
	{
	// First write the SIS Registry Package details
	WriteToScrL(aScrSession, aCompId, static_cast<CSisRegistryPackage&>(aToken));
	// Format the version to text and store in the SCR.
	TBuf<KSmlBufferSize> versionString(KEmptyString);	
	VersionToString(aToken.Version(), versionString);
	aScrSession.SetComponentVersionL(aCompId, versionString);
	// iDrives will be automatically populated by SCR
	ExternalizeSidArrayL(aScrSession, aCompId, aToken.Sids());
	ExternalizeControllerArrayL(aScrSession, aCompId, aToken.ControllerInfo());
	
	if (aToken.Language() != ELangEnglish)
		{
		// ROM stubs are typically set to English so save a few bytes in the database and a DB operation
		// for files installed as English
		aScrSession.SetComponentPropertyL(aCompId, KCompLanguage, static_cast<TInt32>(aToken.Language()));
		}
	
	if (aToken.SelectedDrive() != 0)
		{
		// ROM stubs are set to drive 0 - this optimisation avoids adding a custom property for these
		aScrSession.SetComponentPropertyL(aCompId, KCompSelectedDrive, static_cast<TInt32>(aToken.SelectedDrive()));
		}
	}

// Store the SIS Registry Package details in to the SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryPackage& aPackage)
	{
	// Store the Unique Vendor Name as a custom property
	aScrSession.SetComponentPropertyL(aCompId, KCompUniqueVendorName, *(aPackage.iVendorName));
	
	aScrSession.SetComponentPropertyL(aCompId, KCompUid, static_cast<TInt32>(aPackage.iUid.iUid));
	// The index is the Index of the component being installed. Always it will be 0 for SA type packages and
	// the index will start incrementing for each SP we install on top the base (SA) package.

	if (aPackage.Index() != KDefaultComponentIndex)
		aScrSession.SetComponentPropertyL(aCompId, KCompIndex, aPackage.Index());	
	}



// Store the array of package's SID details in to the SCR.
void ScrHelperUtil::ExternalizeSidArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RArray<TUid>& aSids)
	{
	TBuf<KSmlBufferSize> compSid(KEmptyString);
	TInt sidCount = aSids.Count();
	if(KDefaultSidCount != sidCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompSidCount, sidCount);
	
	if(sidCount > 0)
		aScrSession.SetComponentPropertyL(aCompId, KCompSidsPresent, 1);
	
	for (TInt index = 0; index < sidCount; ++index)
		{
		compSid.Format(KCompSidFormat, index);
		aScrSession.SetComponentPropertyL(aCompId, compSid, static_cast<TUint>(aSids[index].iUid));
		}
	}

// Store the array of controllers in to the SCR.
void ScrHelperUtil::ExternalizeControllerArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RPointerArray<CControllerInfo>& aControllerInfoArray)
	{
	TInt controllerCount = aControllerInfoArray.Count();
	if(KDefaultControllerCount != controllerCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompControllerCount, controllerCount);
	
	for (TInt index = 0; index < controllerCount; ++index)
		{
		WriteToScrL(aScrSession, aCompId, *(aControllerInfoArray[index]), index);
		}
	}

// Store the controller's details in to the SCR.	
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo& aControllerInfo, TInt aIndex)
	{
	TBuf<KBigBufferSize> propertyName(KEmptyString);
	TBuf<KSmlBufferSize> propertyValue(KEmptyString);	
	
	propertyName.Format(KVersionNameFormat, aIndex);	
	TVersion controllerVersion = aControllerInfo.Version();
	if (controllerVersion.iMajor != KDefaultVersionMajor || controllerVersion.iMinor != KDefaultVersionMinor || 
		controllerVersion.iBuild != KDefaultVersionBuild)
		
		{
		VersionToString(controllerVersion, propertyValue);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, propertyValue);
		}
	
	// Multiple PU controllers on top of an SA will lead the offset values to be incremented by one for	each PU.	
	if (aControllerInfo.iOffset != 0)
		{
		// Most applications are SA's that have not been upgraded so use a default value of 0
		propertyName.Format(KOffsetNameFormat, aIndex);	
		aScrSession.SetComponentPropertyL(aCompId, propertyName, aControllerInfo.iOffset);
		}
	WriteToScrL(aScrSession, aCompId, *(aControllerInfo.iHashContainer), aIndex);	
	}
 
 // Store the controller's hash details in to the SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer& aHashContainer, TInt aIndex)
	{
	TBuf<KBigBufferSize> propertyName(KEmptyString);
		
	propertyName.Format(KHashAlgorithmNameFormat, aIndex);
	if (aHashContainer.Algorithm() != CMessageDigest::ESHA1)
		aScrSession.SetComponentPropertyL(aCompId, propertyName, static_cast<TInt32>(aHashContainer.Algorithm()));
	
	propertyName.Format(KHashDataNameFormat, aIndex);
	aScrSession.SetComponentPropertyL(aCompId, propertyName, aHashContainer.Data());
	}

// Store the array of supplier package dependency details in to the SCR.
// Supplier packages are the packages on which the current package is depending on.
void ScrHelperUtil::ExternalizeSupplierPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, RPointerArray<CSisRegistryDependency>& aDependentPackageArray)
	{
	for (TInt index = 0; index < aDependentPackageArray.Count(); ++index)
		{
		WriteToScrL(aScrSession, aCompUid, aCompName, aInstallType, *(aDependentPackageArray[index]));			
		}
	}

// Store the details of one of the dependency package of given component in to SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, CSisRegistryDependency& aRegistryDependency)
	{	
	TBuf <KCompUidSize> supplierGlobalIdString;
	TBuf<KSmlBufferSize> fromVersionString(KEmptyString);
	TBuf<KSmlBufferSize> toVersionString(KEmptyString);
		
	// Get the dependent details.	
	HBufC* dependentGlobalIdString = FormatGlobalIdLC(aCompUid, aCompName, aInstallType);
	
	// Get the supplier details.
	supplierGlobalIdString.AppendFormat(KGlobalIdFormat, aRegistryDependency.iUid.iUid);

	VersionToString(aRegistryDependency.iFromVersion, fromVersionString);
	VersionToString(aRegistryDependency.iToVersion, toVersionString);
	
	// Create the component global id for both dependent and supplier.
	Usif::CGlobalComponentId* supplierGlobalId = Usif::CGlobalComponentId::NewLC(supplierGlobalIdString, Usif::KSoftwareTypeNative);
	Usif::CGlobalComponentId* dependentGlobalId = Usif::CGlobalComponentId::NewLC(*dependentGlobalIdString, Usif::KSoftwareTypeNative);
	
	// Create the versioned component Id for supplier.
	Usif::CVersionedComponentId* supplierVersionedCompId = Usif::CVersionedComponentId::NewLC(*supplierGlobalId, &fromVersionString, &toVersionString);
	
	// Add the dependency details in to SCR.
	TRAPD(err, aScrSession.AddComponentDependencyL(*supplierVersionedCompId, *dependentGlobalId));
	// Since SWI allows multiple equal dependencies, we ignore KErrAlreadyExists
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	CleanupStack::PopAndDestroy(4, dependentGlobalIdString);
	}

// Store details of the array of embedded packages of the given component in to SCR.
void ScrHelperUtil::ExternalizeEmbeddedPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryPackage>& aEmbeddedPackageArray)
	{
	TInt embeddedPackageCount = aEmbeddedPackageArray.Count();
	if(KDefaultEmbeddedPackageCount != embeddedPackageCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompEmbeddedPackageCount, embeddedPackageCount);
	
	if(embeddedPackageCount > 0)
		aScrSession.SetComponentPropertyL(aCompId, KCompEmbeddedPackagePresent, 1);
	
	TBuf<KBigBufferSize> propertyName(KEmptyString);	
	for (TInt index = 0; index < embeddedPackageCount; ++index)
		{
		CSisRegistryPackage* package = aEmbeddedPackageArray[index];
		// Store the package details
		propertyName.Format(KCompEmbeddedPackageUid, index);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, package->Uid().iUid);

		propertyName.Format(KCompEmbeddedPackageName, index);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, package->Name());

		propertyName.Format(KCompEmbeddedPackageVendor, index);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, package->Vendor());

		propertyName.Format(KCompEmbeddedPackageIndex, index);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, package->Index());
		}
	}

// Store the details of one of the embedded package of the given component in to SCR.
void ScrHelperUtil::ExternalizePropertiesArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryProperty>& aRegistryPropertyArray)
	{
	TInt compPropertyCount = aRegistryPropertyArray.Count();
	if(KDefaultCompPropertyCount != compPropertyCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompPropertyCount, compPropertyCount);
	
	for (TInt index = 0; index < compPropertyCount; ++index)
		{
		WriteToScrL(aScrSession, aCompId, *(aRegistryPropertyArray[index]), index);		
		}		
	}

// Store the SIS Registry property in to SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty& aRegistryProperty, TInt aIndex)
	{
	TBuf<KBigBufferSize> propertyName(KEmptyString);
	
	propertyName.Format(KCompPropertyKey, aIndex);
	aScrSession.SetComponentPropertyL(aCompId, propertyName, aRegistryProperty.iKey);
	
	propertyName.Format(KCompPropertyValue, aIndex);
	aScrSession.SetComponentPropertyL(aCompId, propertyName, aRegistryProperty.iValue);	
	}

TInt CompareDescriptors(const TPtrC &aLhs, const TPtrC& aRhs)
	{
	return aLhs.CompareF(aRhs);
	}	
	
// Store the array of owned file's details in to the SCR.
// Wild carded files are stored as part of component custom property group. Where as the normal files are stored 
// as part of files property group. The wild carded files are used only in stub files and they are used only during
// checking for eclipse validation during ROM upgrade. The wild carded files won't give file ownership information.
void ScrHelperUtil::ExternalizeFileDescriptionsArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryFileDescription>& aFileDescriptionArray)
	{
	// As SCR won't support the wildcard characters on file names, if the file name has wildcard characters, 
	// dont' store them as part file properties. Rather, store them as component custom properties only.
	TInt wildCardFileCount = 0;
	TInt executableFileIndex = 0;
	TInt64 componentSize = 0;
	// Since SCR does not check for duplicate entries, and they may occur in SWI, we filter them here
	// The file names are stored in a temporary array and compared with while iterating over file descriptions. Duplicates are skipped
	// Cannot use CDesC16ArrayFlat as it is in BAFL which the TCB sisregistryserver cannot link to

	RArray<TPtrC> filenamesArray;
	CleanupClosePushL(filenamesArray);

	TInt fileDescriptionsCount = aFileDescriptionArray.Count();
	TLinearOrder<TPtrC> descriptorsComparator(&CompareDescriptors);
	for (TInt index = 0; index < fileDescriptionsCount; ++index)
		{
				
		const TDesC& fileTarget = aFileDescriptionArray[index]->Target();
		TPtrC fileTargetPtr(fileTarget);
		if (filenamesArray.SpecificFindInOrder(fileTargetPtr, descriptorsComparator, EArrayFindMode_First) != KErrNotFound)
			continue; // Skip duplicates
		filenamesArray.InsertInOrderL(fileTargetPtr, descriptorsComparator);
		
		if (IsWildcardFile(fileTarget))
			{
			// Calls the method with ETrue if it's wild carded file.
			WriteToScrL(aScrSession, aCompId, *(aFileDescriptionArray[index]), ETrue, wildCardFileCount, -1);
			++wildCardFileCount;
			}
		else
			{
			// If the file's SID value is not zero (only for EXEs)
			if (aFileDescriptionArray[index]->Sid().iUid != 0)
				{
				WriteToScrL(aScrSession, aCompId, *(aFileDescriptionArray[index]), EFalse, -1, executableFileIndex);
				++executableFileIndex;
				}
			else
				{
				WriteToScrL(aScrSession, aCompId, *(aFileDescriptionArray[index]), EFalse, -1, -1);
				}			
			// Get the each file size and accumulate it to get the whole component size.
			componentSize += aFileDescriptionArray[index]->UncompressedLength();
			}
		}

	if (KDefaultWildCardFileCount != wildCardFileCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompWildCardFileCount, wildCardFileCount);
	if (0 != componentSize) // For ROM components, the typical size is 0 which is also the default value at the SCR
		aScrSession.SetComponentSizeL(aCompId, componentSize);
		
	CleanupStack::PopAndDestroy(&filenamesArray);
	}

// Store the owned files details in to the SCR.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryFileDescription& aFileDescription, 
								TBool aIsWildcardedFile, TInt aIndex, TInt aExecutableFileIndex)
	{	
	if (aIsWildcardedFile)
		{
		// SCR doesn't support the wildcard cahracters in file names.
		// So, store the object as a component custom property.
		TBuf<KBigBufferSize> propertyName(KEmptyString);
		propertyName.Format(KCompWildCardFile, aIndex);
		// For wild-carded files, all properties apart from the target are meaningless
		aScrSession.SetComponentPropertyL(aCompId, propertyName, aFileDescription.Target());				
		return;
		}
		
	// For normal file, store it using regular SCR file management APIs
	
	// Store the file name as component property to get the FileName related to a specific SID easily.
	// CSisRegistryPackage::iSids & CSisRegistryObject::iFileDescriptions are to be in sync. Otherwise, this mapping will fail.
	if (aExecutableFileIndex != -1) // Value will be -1 for non executables.
		{
		TBuf<KBigBufferSize> propertyName(KEmptyString);
		propertyName.Format(KCompSidFileNameFormat, aExecutableFileIndex);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, aFileDescription.Target());		
		}

	TBool considerFileInDriveList = (aFileDescription.Operation() != Swi::Sis::EOpNull);
	// For OpNull, we shouldn't update the drive list, as the file does not actually exist, it is only registered for uninstall
	aScrSession.RegisterComponentFileL(aCompId, aFileDescription.Target(), considerFileInDriveList);
	
	if (aFileDescription.MimeType().Size() > 0)
		{
		RBuf8 mimeType8;
		mimeType8.CreateL(aFileDescription.MimeType().Size());
		CleanupClosePushL(mimeType8);
		mimeType8.Copy(aFileDescription.MimeType());
		aScrSession.SetFilePropertyL(aCompId, aFileDescription.Target(), KFileMimeType, mimeType8);
		CleanupStack::PopAndDestroy(&mimeType8);
		}
	
	if (aFileDescription.Operation() != Swi::Sis::EOpInstall)
		{
		aScrSession.SetFilePropertyL(aCompId, aFileDescription.Target(), KFileSisFileOperation, aFileDescription.Operation());
		}
	
	if (aFileDescription.OperationOptions())
		{
		aScrSession.SetFilePropertyL(aCompId, aFileDescription.Target(), KFileFileOperationOptions, aFileDescription.OperationOptions());
		}
	
	if (aFileDescription.Sid().iUid)
		{
		aScrSession.SetFilePropertyL(aCompId, aFileDescription.Target(), KFileSid, aFileDescription.Sid().iUid);		
		}

	WriteToScrL(aScrSession, aCompId, aFileDescription.Hash(), aFileDescription.Target());		
	}

void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const CHashContainer& aHashContainer, const TDesC& aFileName)
	{	
	if (aHashContainer.Algorithm() != CMessageDigest::ESHA1)
		{
		aScrSession.SetFilePropertyL(aCompId, aFileName, KFileHashAlgorithm, static_cast<TInt32>(aHashContainer.Algorithm()));
		}
  		
	if (aHashContainer.Data().Length() > 0)
		{
		aScrSession.SetFilePropertyL(aCompId, aFileName, KFileHashData, aHashContainer.Data());
		}		
	}

// Store the SIS Trust Status to the Software Component Registry.
void ScrHelperUtil::WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TSisTrustStatus& aTrustStatus)
	{
	if(KDefaultValidationStatus != aTrustStatus.iValidationStatus) 
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustValidationStatus, aTrustStatus.iValidationStatus);
		}		
	
	if(KDefaultRevocationStatus != aTrustStatus.iRevocationStatus)
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustRevocationStatus, aTrustStatus.iRevocationStatus);
		}		
	
	if(KDefaultGeneralDate != aTrustStatus.iResultDate.Int64())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustResultDate, aTrustStatus.iResultDate.Int64());
		}
			
	if(KDefaultGeneralDate != aTrustStatus.iLastCheckDate.Int64())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustLastCheckDate, aTrustStatus.iLastCheckDate.Int64());
		}		
	
	if(KDefaultIsQuarantined != aTrustStatus.iQuarantined)
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustIsQuarantined, aTrustStatus.iQuarantined);
		}		
	
	if(KDefaultGeneralDate != aTrustStatus.iQuarantinedDate.Int64())
		{
		aScrSession.SetComponentPropertyL(aCompId, KCompTrustQuarantinedDate, aTrustStatus.iQuarantinedDate.Int64());
		}
			
	// Update the 'Origin Verified' & 'Known Revoked' fields of the component 
	// based on this object's validation status and revocation status respectively.	
	if (aTrustStatus.iValidationStatus >= Swi::EValidatedToAnchor)
		{
		aScrSession.SetIsComponentOriginVerifiedL(aCompId, ETrue);
		}
		
	if (aTrustStatus.iRevocationStatus == Swi::EOcspRevoked)
		{
		aScrSession.SetIsComponentKnownRevokedL(aCompId, ETrue);
		}
	}

// Store the Chain Index  array elements to the Software Component Registry.
void ScrHelperUtil::ExternalizeChainIndexArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RArray<TInt>& aChainIndexArray)
	{
	TBuf<KBigBufferSize> propertyName(KEmptyString);	
	TInt chainIndexCount = aChainIndexArray.Count();
	if(KDefaultChainIndex != chainIndexCount)
		aScrSession.SetComponentPropertyL(aCompId, KCompInstallChainIndexCount, chainIndexCount);
	
	for (TInt index = 0; index < chainIndexCount; ++index)
		{
		propertyName.Format(KCompInstallChainIndex, index);
		aScrSession.SetComponentPropertyL(aCompId, propertyName, aChainIndexArray[index]);
		}
	}

// Retrieve the SIS Registry Object details of the given component from Software Component Registry - SCR.
void ScrHelperUtil::GetComponentL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject)
	{
	// First retrieve the user selected language to use it as the locale to retrive localized strings
	TLanguage userSelectedLanguage = static_cast<TLanguage>(GetIntPropertyValueL(aScrSession, aCompId, KCompLanguage, EFalse, ELangEnglish));
	
	//Retrieve the locale information from ComponentLocalizable
	GetComponentLocalizedInfoL(aScrSession, aCompId, aObject);
    
	// Then get all properties of the component
	RPointerArray<Usif::CPropertyEntry> propertyArray;
	aScrSession.GetComponentPropertiesL(aCompId, propertyArray);
	CleanupResetAndDestroyPushL(propertyArray);
		
	// Get the component entry of the given Component Id and construct the registry object hierarchy.
	Usif::CComponentEntry* componentEntry = Usif::CComponentEntry::NewLC();
	aScrSession.GetComponentL(aCompId, *componentEntry, userSelectedLanguage);
	
	// First read the SIS Registry Token
	ReadFromScrL(aScrSession, componentEntry, static_cast<CSisRegistryToken&>(aObject), &propertyArray);
	
	aObject.iIsRemovable = componentEntry->IsRemovable();
	
	// Internalize other simple member variables of the object.
	// The values retrieved from the data base are casted according to the variable types.
	aObject.iVendorLocalizedName = componentEntry->Vendor().AllocL();
	
	// Internalize the DRM protection status on the component
	aObject.iIsDrmProtected = componentEntry->IsDrmProtected();
	
	// Internalize the 'hidden' nature of the component
	aObject.iIsHidden = componentEntry->IsHidden();
	CleanupStack::PopAndDestroy(componentEntry);
	
	aObject.iTrustTimeStamp = TTime(GetInt64PropertyValueL(aScrSession, aCompId, KCompTrustTimeStamp, ETrue));
	// Assign default values
	aObject.iInstallType = KDefaultInstallType;
	aObject.iInRom = KDefaultIsInRom;
	aObject.iDeletablePreInstalled = KDefaultIsDeletablePreInstalled; 
	aObject.iSigned = KDefaultIsSigned;
	aObject.iTrust = KDefaultTrustValue;
	aObject.iRemoveWithLastDependent = KDefaultRemoveWithLastDependent;
	aObject.iSignedBySuCert = KDefaultIsSignedBySuCert;
	
	// Check the retrieved properties to see if the properties assigned with default values exist.
	// Update the existing properties with the retrieved property values.
	TInt propertyCount = propertyArray.Count();
	for(TInt i=0; i<propertyCount; ++i)
		{
		const TDesC& propertyName = propertyArray[i]->PropertyName();
		if(Usif::CPropertyEntry::EIntProperty != propertyArray[i]->PropertyType())
			continue; // In this loop, only integer properties are checked. Ignore binary and localizable properties.
		
		Usif::CIntPropertyEntry* property = static_cast<Usif::CIntPropertyEntry*>(propertyArray[i]);
		TInt value = property->IntValue();
		
		if(KCompInstallType() == propertyName)
			{
			aObject.iInstallType =  static_cast<Sis::TInstallType>(value);
			}
		else if(KCompInRom() == propertyName)
			{
			aObject.iInRom = static_cast<TBool>(value);
			}
		else if(KCompDeletablePreinstalled() == propertyName)
			{
			aObject.iDeletablePreInstalled = static_cast<TBool>(value);
			}
		else if(KCompIsSigned() == propertyName)
			{
			aObject.iSigned = static_cast<TBool>(value);
			}
		else if(KCompTrust() == propertyName)
			{
			aObject.iTrust = static_cast<TSisPackageTrust>(value);
			}
		else if(KCompIsRemoveWithLastDependent() == propertyName)
			{
			aObject.iRemoveWithLastDependent = value;
			}
		else if(KCompSignedBySuCert() == propertyName)
			{
			aObject.iSignedBySuCert = static_cast<TBool>(value);
			}
		}
		
	// Internalize the other array members of the object.	
	InternalizeSupplierPackageArrayL(aScrSession, aObject.Uid(), aObject.Name(), aObject.iInstallType, aObject.iDependencies);	
	InternalizeEmbeddedPackageArrayL(aScrSession, aCompId, aObject.iEmbeddedPackages, &propertyArray);
	InternalizePropertiesArrayL(aScrSession, aCompId, aObject.iProperties, &propertyArray);
	InternalizeFileDescriptionsArrayL(aScrSession, aCompId, aObject.iFileDescriptions);	
	aObject.iOwnedFileDescriptions = aObject.iFileDescriptions.Count();		
	ReadFromScrL(aScrSession, aCompId, aObject.iTrustStatus, &propertyArray);
	InternalizeChainIndexArrayL(aScrSession, aCompId, aObject.iInstallChainIndices, &propertyArray);
	CleanupStack::PopAndDestroy(&propertyArray);
	}

// Retrieve the localizable details of a given component from Software Component Registry - SCR.	
void ScrHelperUtil::GetComponentLocalizedInfoL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject)
    {
    DEBUG_PRINTF2(_L8("Retrieve the localized information related to component with CompId:%d"),aCompId);
    RPointerArray<Usif::CLocalizableComponentInfo> compLocalizedInfoArray;
    CleanupResetAndDestroyPushL(compLocalizedInfoArray);
    aScrSession.GetComponentLocalizedInfoL(aCompId, compLocalizedInfoArray);
    for(TInt i=0;i<compLocalizedInfoArray.Count();i++)
        {
        aObject.iLocalizedPackageNames.AppendL((compLocalizedInfoArray[i]->NameL()).AllocL());
        aObject.iLocalizedVendorNames.AppendL((compLocalizedInfoArray[i]->VendorL()).AllocL());
        aObject.iSupportedLanguageIds.AppendL(compLocalizedInfoArray[i]->Locale());
        }
    CleanupStack::Pop(&compLocalizedInfoArray);
    compLocalizedInfoArray.ResetAndDestroy();
    }
TBool ScrHelperUtil::IsUidPresentL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid)
	{
	Usif::TComponentId dummyComponentId(0);
	return GetComponentIdL(aScrSession, aPackageUid, CSisRegistryPackage::PrimaryIndex, dummyComponentId); // If the base package for the UID does not exist, then it cannot be present on the system
	}
	
TBool ScrHelperUtil::IsUidAndNamePresentL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aUid, const TDesC& aPackageName)
	{
	// Get all components for the given UID and perform a comparison with package names 
	// Note that we cannot apply package names on the filter
	// Get the right component id matching with the package UID, name and index.	
	Usif::CComponentFilter* componentFilter = Usif::CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	componentFilter->AddPropertyL(KCompUid, aUid.iUid);	
	
	Usif::RSoftwareComponentRegistryView subSession;
	CleanupClosePushL(subSession); 	
	subSession.OpenViewL(aScrSession, componentFilter);	
	Usif::CComponentEntry* compEntry = NULL;
	TBool found = EFalse;
	while (!found && (compEntry = subSession.NextComponentL()) != NULL)
		{
		CleanupStack::PushL(compEntry);
		if (compEntry->Name() == aPackageName)
			{
			found = ETrue;
			}
		CleanupStack::PopAndDestroy(compEntry);
		}
	CleanupStack::PopAndDestroy(2, componentFilter);	// subSession

	if (!found)
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - Retrieving Component ID for the given UID (%d) failed."), aUid.iUid);
		DEBUG_PRINTF2(_L8("Error : %d. Component not found in the SCR."), KErrNotFound);
		return EFalse;
		}

	return ETrue;
	}

// Queries the SCR for a component ID matching with the given values for Package UID and Package Index
// Returns EFalse, if not found. Returns ETrue if found, and returns the component id in the [out] parameter in such case
TBool ScrHelperUtil::GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TInt aPackageIndex, Usif::TComponentId& aComponentId)
	{
	// Setup a filter based on the input parameters
	Usif::CComponentFilter* compFilter = Usif::CComponentFilter::NewLC();
	compFilter->AddPropertyL(KCompUid, aPackageUid.iUid);	

	compFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	
	RArray<Usif::TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	aScrSession.GetComponentIdsL(componentIdList, compFilter);
	
	if(componentIdList.Count() == 0)
		{
		// No components are found in the SCR. So, return false
		CleanupStack::PopAndDestroy(2, compFilter);	// componentIdList
		DEBUG_PRINTF2(_L8("Sis Registry Server - Unable to find component for UID: %x"), aPackageUid.iUid);
		return EFalse;
		}
	// Filtering on KCompIndex is expensive in the SCR, as many values would be zero and the index is ineffective causing many comparisons in  the DB. Usually the number of
	// components matching the UID is low (as these are the base package + augmentations). 
	TBool matchingIndexFound = EFalse;
	for (TInt i = 0; i < componentIdList.Count(); ++i)
		{
		TInt componentIndex = GetIntPropertyValueL(aScrSession, componentIdList[i], KCompIndex, EFalse, KDefaultComponentIndex);
		if (componentIndex == aPackageIndex)
			{
			aComponentId = componentIdList[i];
			matchingIndexFound = ETrue;
			break;
			}
		}

	CleanupStack::PopAndDestroy(2, compFilter);	// componentIdList
	if (!matchingIndexFound)
		{
		DEBUG_PRINTF3(_L8("Sis Registry Server - Unable to find matching package for UID %d and index %d"), aPackageUid.iUid, aPackageIndex);
		return EFalse;		
		}
		
	return ETrue;
	}

// Queries the SCR for a component ID matching with the given values for Package UID and Package Index
// Leaves with KErrNone if the a corresponding component cannot be found

Usif::TComponentId ScrHelperUtil::GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TInt aPackageIndex)
	{
	Usif::TComponentId compId;
	if (!GetComponentIdL(aScrSession, aPackageUid, aPackageIndex, compId))
		User::Leave(KErrNotFound);

	return compId;
	}

// Queries the SCR for a component ID matching with the given values for Package Name and Package Vendor Name.
// Returns the matching component Id, if succeeds. Otherwise, it leaves with relevant error code.
// NOTE: There can be multiple such components and this API returns the first one found! 
Usif::TComponentId ScrHelperUtil::GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TDesC& aPackageName, const TDesC& aVendorName)
	{
	Usif::TComponentId compId;
	if (!GetComponentIdL(aScrSession, aPackageName, aVendorName, compId))
		User::Leave(KErrNotFound);

	return compId;
	}

// Queries the SCR for a component ID matching with the given values for Package Name and Package Vendor Name.
// Returns true iff the component exists. Uses the out parameter for setting the component's id in such case
// NOTE: There can be multiple such components and this API returns the first one found! 
TBool ScrHelperUtil::GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TDesC& aPackageName, const TDesC& aVendorName, Usif::TComponentId& aComponentId)
	{
	// Get the right component id matching with the package UID, name and vendor name.	
	Usif::CComponentFilter* componentFilter = Usif::CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	componentFilter->SetNameL(aPackageName);
	componentFilter->AddPropertyL(KCompUniqueVendorName, aVendorName, Usif::KNonLocalized);
		
	RArray<Usif::TComponentId> componentIdList;
	CleanupClosePushL(componentIdList);
	aScrSession.GetComponentIdsL(componentIdList, componentFilter);
	
	if(componentIdList.Count() == 0)
		{
		// No components are found in the SCR. So, return FALSE
		CleanupStack::PopAndDestroy(2, componentFilter); // componentIdList
		DEBUG_PRINTF3(_L("Sis Registry Server - Component ID not found for the given name (%S) and vendor (%S)."), 
				&aPackageName, &aVendorName);
		return EFalse;
		}
	// If found, get the corresponding Component Id and return it.
	aComponentId = componentIdList[0];

	CleanupStack::PopAndDestroy(2, componentFilter); // componentIdList
	return ETrue;
	}

// Retrieve the SIS Registry Token details.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::CComponentEntry* aComponentEntry, CSisRegistryToken& aToken, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	// First read the SIS Registry Package
	ReadFromScrL(aScrSession, aComponentEntry, static_cast<CSisRegistryPackage&>(aToken));
	
	InternalizeSidArrayL(aScrSession, aComponentEntry->ComponentId(), aToken.iSids, aPropertyArray);
	InternalizeControllerArrayL(aScrSession, aComponentEntry->ComponentId(), aToken.iControllerInfo, aPropertyArray);

	// Reset the Installed Drives
	aToken.iDrives = 0;
	// Convert the TDriveList format to the BitMask format.
	TDriveList driveList = aComponentEntry->InstalledDrives();
	MakeDrivesFromStringL(driveList, aToken.iDrives);
	// Since the wild carded files are not stored as part of FilesProperties,
	// SCR won't populate the installed drives itself. 
	// So, for wild carded ROM packages, we will manually populate the iDrives with Z.
	TInt wildCardFileCount = GetIntPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompWildCardFileCount, EFalse, KDefaultWildCardFileCount);
	if (wildCardFileCount > 0)
		{
		aToken.iDrives |= 1 << EDriveZ;
		}
	
	// Get the version.	
	StringToVersionL(aComponentEntry->Version(), aToken.iVersion);
		
	aToken.iLanguage = static_cast<TLanguage>(GetIntPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompLanguage, EFalse, ELangEnglish));	
	aToken.iSelectedDrive = static_cast<TChar>(GetIntPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompSelectedDrive, EFalse));
	}

// Retrieve the SIS Registry Package details.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::CComponentEntry* aComponentEntry, CSisRegistryPackage& aPackage)
	{	
	aPackage.iUid.iUid = GetIntPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompUid, ETrue);
	aPackage.iPackageName = aComponentEntry->Name().AllocL();
	aPackage.iVendorName = GetStrPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompUniqueVendorName);
	aPackage.iIndex = GetIntPropertyValueL(aScrSession, aComponentEntry->ComponentId(), KCompIndex, EFalse, KDefaultComponentIndex); 
	}

// Retrieve the component's array of SID details from SCR.
void ScrHelperUtil::InternalizeSidArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RArray<TUid>& aSidsArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	TBuf<KSmlBufferSize> compSid(KEmptyString);
	TInt sidCount = 0;
	if(aPropertyArray)
		sidCount = GetIntegerPropertyValueFromArrayL(KCompSidCount, KDefaultSidCount, *aPropertyArray);
	else
		sidCount = GetIntPropertyValueL(aScrSession, aCompId, KCompSidCount, EFalse, KDefaultSidCount);
	
	for (TInt index = 0; index < sidCount; ++index)
		{
		compSid.Format(KCompSidFormat, index);
		TUid tUid = TUid::Uid(GetIntPropertyValueL(aScrSession, aCompId, compSid, ETrue));
		aSidsArray.AppendL(tUid);
		}
	}

// Retrieve the array of controlller's details for the given component from SCR.
void ScrHelperUtil::InternalizeControllerArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CControllerInfo>& aControllerInfoArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	TInt controllerCount = 0;
	if(aPropertyArray)
		controllerCount = GetIntegerPropertyValueFromArrayL(KCompControllerCount, KDefaultControllerCount, *aPropertyArray);
	else
		controllerCount = GetIntPropertyValueL(aScrSession, aCompId, KCompControllerCount, EFalse, KDefaultControllerCount);

	for (TInt index = 0; index < controllerCount; ++index)
		{		
		CControllerInfo* controllerInfo = NULL;
		ReadFromScrL(aScrSession, aCompId, controllerInfo, index);
		CleanupStack::PushL(controllerInfo);
		aControllerInfoArray.AppendL(controllerInfo);
		CleanupStack::Pop(controllerInfo);
		}	
	}

// Retrieve the given controller details from SCR.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo*& aControllerInfo, TInt aIndex)
	{
	TVersion version(KDefaultVersionMajor, KDefaultVersionMinor, KDefaultVersionBuild);
 	TInt offset;
	CHashContainer* hashContainer = NULL;
		
	TBuf<KBigBufferSize> controllerVersionName(KEmptyString);
	TBuf<KBigBufferSize> controllerOffsetName(KEmptyString);	
	HBufC* controllerVersionPointer = NULL;

	controllerVersionName.Format(KVersionNameFormat, aIndex);
	controllerVersionPointer = GetStrPropertyValueL(aScrSession, aCompId, controllerVersionName, Usif::KUnspecifiedLocale, EFalse);	
	if (controllerVersionPointer) // The value is optional in the DB - assumed default if not present
		{
		StringToVersionL(controllerVersionPointer->Des(), version);
		delete controllerVersionPointer;
		}
	
	controllerOffsetName.Format(KOffsetNameFormat, aIndex);	
	offset = GetIntPropertyValueL(aScrSession, aCompId, controllerOffsetName, EFalse, 0);	
	ReadFromScrL(aScrSession, aCompId, hashContainer, aIndex);
	CleanupStack::PushL(hashContainer);	
	aControllerInfo = CControllerInfo::NewL(version, *hashContainer, offset);
	CleanupStack::PopAndDestroy(hashContainer);	
	}

// Retrieve the hash details of the controller.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, TInt aIndex)
	{		
	CMessageDigest::THashId algorithm;	
	TBuf<KBigBufferSize> propertyName(KEmptyString);	
			
	propertyName.Format(KHashAlgorithmNameFormat, aIndex);	
	algorithm = static_cast<CMessageDigest::THashId>(GetIntPropertyValueL(aScrSession, aCompId, propertyName, EFalse, CMessageDigest::ESHA1));
	
	propertyName.Format(KHashDataNameFormat, aIndex);
	HBufC8* data = GetBinaryPropertyValueL(aScrSession, aCompId, propertyName);
	CleanupStack::PushL(data);	
	aHashContainer = CHashContainer::NewL(algorithm, *data);	
	CleanupStack::PopAndDestroy(data);
	}
	
// Retrieve the array of supplier package details for the given component.
// Supplier packages are the packages on which the current package is depending on.
void ScrHelperUtil::InternalizeSupplierPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, RPointerArray<CSisRegistryDependency>& aDependentPackageArray)
	{	
	RPointerArray<Usif::CVersionedComponentId> supplierCompList;
	CleanupResetAndDestroyPushL(supplierCompList);
	
	HBufC* dependentGlobalIdString = FormatGlobalIdLC(aCompUid, aCompName, aInstallType);
		
	Usif::CGlobalComponentId* dependentGlobalId = Usif::CGlobalComponentId::NewLC(*dependentGlobalIdString, Usif::KSoftwareTypeNative);
	
	aScrSession.GetSupplierComponentsL(*dependentGlobalId, supplierCompList);
	CleanupStack::PopAndDestroy(2, dependentGlobalIdString);
	
	for (TInt index = 0; index < supplierCompList.Count(); ++index)
		{
		// Create the object from SCR and append it to the array.
		CSisRegistryDependency* dependency = NULL;
		ReadFromScrL(*(supplierCompList[index]), dependency);
		CleanupStack::PushL(dependency);
		aDependentPackageArray.AppendL(dependency);
		CleanupStack::Pop(dependency);
		}
	CleanupStack::PopAndDestroy(&supplierCompList);
	}

// Retrieve the package dependency details of the given component.
void ScrHelperUtil::ReadFromScrL(Usif::CVersionedComponentId& aVersionedSupplierCompId, CSisRegistryDependency*& aRegistryDependency)
	{
	TUid tUid;
	TVersion fromVersion;
	TVersion toVersion;	
	
	// Get the supplier package details. The supplier Uid if extracted from it's global Id.
	const Usif::CGlobalComponentId &supplierGlobalId = aVersionedSupplierCompId.GlobalId();	
	ExtractCompUidL(supplierGlobalId.GlobalIdName(), tUid);

	StringToVersionL(*(aVersionedSupplierCompId.VersionFrom()), fromVersion);	
	StringToVersionL(*(aVersionedSupplierCompId.VersionTo()), toVersion);
		
	aRegistryDependency = CSisRegistryDependency::NewL(tUid, fromVersion, toVersion);		
	}
	
// Retrieve the array of embedded packages of the given component.
void ScrHelperUtil::InternalizeEmbeddedPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryPackage>& aEmbeddedPackageArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	TInt embedCount = 0;
	if(aPropertyArray)
		embedCount = GetIntegerPropertyValueFromArrayL(KCompEmbeddedPackageCount, KDefaultEmbeddedPackageCount, *aPropertyArray);
	else
		embedCount = GetIntPropertyValueL(aScrSession, aCompId, KCompEmbeddedPackageCount, EFalse, KDefaultEmbeddedPackageCount);

	for (TInt index = 0; index < embedCount; ++index)
		{
		// Get the package details
		TBuf<KBigBufferSize> propertyName(KEmptyString);
		propertyName.Format(KCompEmbeddedPackageUid, index);
		TUid packageUid = TUid::Uid(GetIntPropertyValueL(aScrSession, aCompId, propertyName, ETrue));

		propertyName.Format(KCompEmbeddedPackageName, index);
		HBufC16* packageName = GetStrPropertyValueL(aScrSession, aCompId, propertyName);
		CleanupStack::PushL(packageName);

		propertyName.Format(KCompEmbeddedPackageVendor, index);
		HBufC* packageVendor = GetStrPropertyValueL(aScrSession, aCompId, propertyName);
		CleanupStack::PushL(packageVendor);

		propertyName.Format(KCompEmbeddedPackageIndex, index);
		TInt packageIndex = GetIntPropertyValueL(aScrSession, aCompId, propertyName, ETrue);

		CSisRegistryPackage *package = CSisRegistryPackage::NewLC(packageUid, *packageName, *packageVendor);
		package->SetIndex(packageIndex);
		aEmbeddedPackageArray.AppendL(package);
		CleanupStack::Pop(package);
		CleanupStack::PopAndDestroy(2, packageName);
		}	
	}

// Retrieve the package details of the given component from the SCR.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryPackage*& aPackage)
	{
	TUid compUid = TUid::Uid(GetIntPropertyValueL(aScrSession, aCompId, KCompUid, ETrue));
	TInt componentIndex = GetIntPropertyValueL(aScrSession, aCompId, KCompIndex, EFalse, KDefaultComponentIndex);

	// Unique vendor name is stored as a custom property
	HBufC* uniqueVendor = GetStrPropertyValueL(aScrSession, aCompId, KCompUniqueVendorName);
	CleanupStack::PushL(uniqueVendor);
	Usif::CComponentEntry* componentEntry = Usif::CComponentEntry::NewLC();
	aScrSession.GetComponentL(aCompId, *componentEntry);
	aPackage = CSisRegistryPackage::NewL(compUid, componentEntry->Name(), *uniqueVendor);	
	aPackage->iIndex = componentIndex;
	CleanupStack::PopAndDestroy(2, uniqueVendor);	// componentEntry
	}

// Retrieve the array of SIS regiatry properties related to the given component from SCR.
void ScrHelperUtil::InternalizePropertiesArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryProperty>& aPropertyArray, RPointerArray<Usif::CPropertyEntry>* aCompPropertyArray)
	{
	TInt propertyCount = 0;
	if(aCompPropertyArray)
		propertyCount = GetIntegerPropertyValueFromArrayL(KCompPropertyCount, KDefaultCompPropertyCount, *aCompPropertyArray);
	else
		propertyCount = GetIntPropertyValueL(aScrSession, aCompId, KCompPropertyCount, EFalse, KDefaultCompPropertyCount);

	for (TInt index = 0; index < propertyCount; ++index)
		{
		// Create the object from SCR and append it to the array.
		CSisRegistryProperty* property = NULL;
		ReadFromScrL(aScrSession, aCompId, property, index);		
		aPropertyArray.AppendL(property);
		CleanupStack::Pop(property);		
		}	
	}

// Retrieve the SIS registry property details from the SCR.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty*& aRegistryProperty, TInt aIndex)
	{
	TBuf<KBigBufferSize> propertyName(KEmptyString);
	
	aRegistryProperty = new(ELeave) CSisRegistryProperty();
	CleanupStack::PushL(aRegistryProperty);
	propertyName.Format(KCompPropertyKey, aIndex);
	aRegistryProperty->iKey = GetIntPropertyValueL(aScrSession, aCompId, propertyName, ETrue);
	
	propertyName.Format(KCompPropertyValue, aIndex);
	aRegistryProperty->iValue = GetIntPropertyValueL(aScrSession, aCompId, propertyName, ETrue);		
	}

// Retrieve the files owned by the given component from the Software Component Registry.	
void ScrHelperUtil::InternalizeFileDescriptionsArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryFileDescription>& aFileDescriptionArray)
	{
	HBufC* fileName = NULL;	
	// First internalize all the wildcarded files (if any).
	TInt wildCardFileCount = GetIntPropertyValueL(aScrSession, aCompId, KCompWildCardFileCount, EFalse, KDefaultWildCardFileCount);
	for (TInt index = 0; index < wildCardFileCount; index++)
		{
		TBuf<KBigBufferSize> propertyName(KEmptyString);
		propertyName.Format(KCompWildCardFile, index);
		fileName = GetStrPropertyValueL(aScrSession, aCompId, propertyName);
		CleanupStack::PushL(fileName);
		CSisRegistryFileDescription* fileDescription;
		ReadFromScrL(aScrSession, aCompId, fileDescription, *fileName, ETrue); // ETrue for wild carded files
		CleanupStack::PushL(fileDescription);		
		aFileDescriptionArray.AppendL(fileDescription);
		CleanupStack::Pop(fileDescription);
		CleanupStack::PopAndDestroy(fileName);
		}
	// Internalize the normal files (non wildcarded).
	Usif::RSoftwareComponentRegistryFilesList subSession;
	CleanupClosePushL(subSession); 	
	subSession.OpenListL(aScrSession, aCompId);
	
	for (;;)
		{
		fileName = subSession.NextFileL();
		if (!fileName)
			{
			break;
			}
		CleanupStack::PushL(fileName);
		CSisRegistryFileDescription* fileDescription = NULL;
		ReadFromScrL(aScrSession, aCompId, fileDescription, *fileName, EFalse); // EFalse for normal (non wild carded) files
		CleanupStack::PushL(fileDescription);		
		aFileDescriptionArray.AppendL(fileDescription);
		CleanupStack::Pop(fileDescription);
		CleanupStack::PopAndDestroy(fileName);
		}	
	CleanupStack::PopAndDestroy(&subSession);
	}

// Retrieve information of the given file from Software Component Registry.
// Wild carded file's details are read from component custom properties group and 
// Normal file's details are read from the file properties group.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryFileDescription*& aFileDescription, const TDesC& aFileName, TBool aIsWildcardedFile)
	{
	aFileDescription = CSisRegistryFileDescription::NewLC();
	aFileDescription->iTarget = aFileName.AllocL();
	// If the file name has any wild card charactors on it, the object should be read from component custom porperties.
	if (aIsWildcardedFile)
		{
		// A wildcard applies to MULTIPLE files so discard all values that only apply to single files
		aFileDescription->iMimeType = HBufC16::NewL(0);
		aFileDescription->iHash = CHashContainer::NewL(CMessageDigest::ESHA1, KNullDesC8);
		aFileDescription->iOperation = Swi::Sis::EOpInstall;
		CleanupStack::Pop(aFileDescription);
		return;
		}

	// Else, read the object from file properties 
	RPointerArray<Usif::CPropertyEntry> propertyArray;
	CleanupResetAndDestroyPushL(propertyArray);
	aScrSession.GetFilePropertiesL(aCompId, aFileDescription->Target(), propertyArray);
	aFileDescription->iOperation = Swi::Sis::EOpInstall; // EOpInstall is the default value - we record the operation in the SCR only if it is different
	// Not all file properties are present for every file. We use default values for the non-mandatory ones, such as SID
	for (TInt i = 0; i < propertyArray.Count(); ++i)
		{
		const TDesC& propertyName = propertyArray[i]->PropertyName();
		if (propertyName == KFileMimeType)
			{
			Usif::CBinaryPropertyEntry* binaryPropertyEntry = static_cast<Usif::CBinaryPropertyEntry *>(propertyArray[i]);
			const TDesC8& mimeType8 = binaryPropertyEntry->BinaryValue();
			aFileDescription->iMimeType = HBufC16::NewL(mimeType8.Length());
			aFileDescription->iMimeType->Des().Copy(mimeType8);
			}

		Usif::CIntPropertyEntry* intPropertyEntry = static_cast<Usif::CIntPropertyEntry *>(propertyArray[i]);	
		if (propertyName == KFileSisFileOperation)
			{
			aFileDescription->iOperation = static_cast<Sis::TSISFileOperation>(intPropertyEntry->IntValue());
			}
		else if (propertyName == KFileFileOperationOptions)
			{
			aFileDescription->iOperationOptions = static_cast<Sis::TSISFileOperationOptions>(intPropertyEntry->IntValue());
			}
		else if (propertyName == KFileSid)
			{
			aFileDescription->iSid = TUid::Uid(intPropertyEntry->IntValue());
			}
		}
		
	if (!aFileDescription->iMimeType)
		{
		aFileDescription->iMimeType = HBufC16::NewL(0);
		}
	CleanupStack::PopAndDestroy(&propertyArray);
	ReadFromScrL(aScrSession, aCompId, aFileDescription->iHash, aFileName);

	CleanupStack::Pop(aFileDescription);
	}

// Retrieve the Hash values related to the given file from the Software Component Registry
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, const TDesC& aFileName)
	{	
	HBufC8* data = NULL;
	CMessageDigest::THashId algorithm = CMessageDigest::ESHA1;

	RPointerArray<Usif::CPropertyEntry> propertyArray;
	CleanupResetAndDestroyPushL(propertyArray);
	aScrSession.GetFilePropertiesL(aCompId, aFileName, propertyArray);
	for (TInt i = 0; i < propertyArray.Count(); ++i)
		{
		const TDesC& propertyName = propertyArray[i]->PropertyName();
		if (propertyName == KFileHashData)
			{
			Usif::CBinaryPropertyEntry* binaryPropertyEntry = static_cast<Usif::CBinaryPropertyEntry *>(propertyArray[i]);
			data = HBufC8::NewLC(binaryPropertyEntry->BinaryValue().Length());
			data->Des().Copy(binaryPropertyEntry->BinaryValue());
			}
		else if (propertyName == KFileHashAlgorithm)
			{
			Usif::CIntPropertyEntry* intPropertyEntry = static_cast<Usif::CIntPropertyEntry *>(propertyArray[i]);
			algorithm = static_cast<CMessageDigest::THashId>(intPropertyEntry->IntValue());
			}
		}
		
	if (data == NULL)
		{
		data = HBufC8::NewLC(0);
		}

	CleanupStack::Pop(data);
	CleanupStack::PopAndDestroy(&propertyArray);
	CleanupStack::PushL(data);		
	aHashContainer = CHashContainer::NewL(algorithm, *data);
	CleanupStack::PopAndDestroy(data);
	}

// Retrieve the SIS Trust Status from the Software Component Registry.
void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, TSisTrustStatus& aTrustStatus, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	RPointerArray<Usif::CPropertyEntry> *localCopyOfPropertyArray;
	RPointerArray<Usif::CPropertyEntry> localPropertyArray;
	
	if(aPropertyArray)
		{
		localCopyOfPropertyArray = aPropertyArray;
		}
	else
		{ // if the properties array parameter is not passed, retrieve all properties of the component in question.
		aScrSession.GetComponentPropertiesL(aCompId, localPropertyArray);
		CleanupResetAndDestroyPushL(localPropertyArray);
		localCopyOfPropertyArray = &localPropertyArray;
		}
	
	// Then find and assign values from the component properties array
	aTrustStatus.iValidationStatus = KDefaultValidationStatus;
	aTrustStatus.iRevocationStatus = KDefaultRevocationStatus;
	aTrustStatus.iResultDate = TTime(KDefaultGeneralDate);
	aTrustStatus.iLastCheckDate = TTime(KDefaultGeneralDate);		  	
	aTrustStatus.iQuarantined = KDefaultIsQuarantined; 
	aTrustStatus.iQuarantinedDate = TTime(KDefaultGeneralDate);
	
	RPointerArray<Usif::CPropertyEntry>& propertyArray = *localCopyOfPropertyArray;
	TInt propertyCount = propertyArray.Count();
	for(TInt i=0; i<propertyCount; ++i)
		{
		if(Usif::CPropertyEntry::EIntProperty != propertyArray[i]->PropertyType())
			continue; // In this loop, only integer properties are checked. Ignore binary and localizable properties.
		
		const TDesC& propertyName = propertyArray[i]->PropertyName();	
		Usif::CIntPropertyEntry* property = static_cast<Usif::CIntPropertyEntry*>(propertyArray[i]);
			
		if(KCompTrustValidationStatus() == propertyName)
			{
			aTrustStatus.iValidationStatus =  static_cast<TValidationStatus>(property->IntValue());
			}
		else if(KCompTrustRevocationStatus() == propertyName)
			{
			aTrustStatus.iRevocationStatus = static_cast<TRevocationStatus>(property->IntValue());
			}
		else if(KCompTrustResultDate() == propertyName)
			{
			aTrustStatus.iResultDate = TTime(property->Int64Value());		
			}
		else if(KCompTrustLastCheckDate() == propertyName)
			{
			aTrustStatus.iLastCheckDate = TTime(property->Int64Value());		
			}
		else if(KCompTrustIsQuarantined() == propertyName)
			{
			aTrustStatus.iQuarantined = static_cast<TBool>(property->IntValue());		
			}
		else if(KCompTrustQuarantinedDate() == propertyName)
			{
			aTrustStatus.iQuarantinedDate = TTime(property->Int64Value());	
			}
		}	
	
	if(!aPropertyArray)
		{ // Delete the local array if it has been populated
		CleanupStack::PopAndDestroy(&localPropertyArray);
		}
	}

// Retrieve the Chain Index  array elements from the Software Component Registry - SCR.
void ScrHelperUtil::InternalizeChainIndexArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RArray<TInt>& aChainIndexArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray)
	{
	TInt chainIndex = 0;
	TBuf<KBigBufferSize> propertyName(KEmptyString);	
	
	TInt chainIndexCount = 0;
	if(aPropertyArray)
		chainIndexCount = GetIntegerPropertyValueFromArrayL(KCompInstallChainIndexCount, KDefaultChainIndex, *aPropertyArray);
	else
		chainIndexCount = GetIntPropertyValueL(aScrSession, aCompId, KCompInstallChainIndexCount, EFalse, KDefaultChainIndex);

	for (TInt index = 0; index < chainIndexCount; ++index)
		{
		propertyName.Format(KCompInstallChainIndex, index);
		chainIndex = GetIntPropertyValueL(aScrSession, aCompId, propertyName, ETrue);
		aChainIndexArray.AppendL(chainIndex);
		}
	}

// Get the value for given integer (64 bit) type property related to the given component.
TInt64 Swi::GetInt64PropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName,  TBool aAlwaysExpectedInDb, TInt64 aDefaultValue)
	{	
	TInt64 propertyValue = 0;	
	Usif::CIntPropertyEntry* propertyEntry = NULL;
	
	propertyEntry = static_cast<Usif::CIntPropertyEntry *>(aScrSession.GetComponentPropertyL(aCompId, aPropertyName));				

	__ASSERT_ALWAYS(propertyEntry || !aAlwaysExpectedInDb, User::Leave(KErrAbort));
	
	propertyValue = propertyEntry ? propertyEntry->Int64Value() : aDefaultValue;
	delete propertyEntry;
	
	return (propertyValue);	
	}

// Get the value for given integer type property related to the given component.
TInt Swi::GetIntPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName, TBool aAlwaysExpectedInDb, TInt aDefaultValue)
	{	
	TInt propertyValue = 0;	
	Usif::CIntPropertyEntry* propertyEntry = NULL;
	
	propertyEntry = static_cast<Usif::CIntPropertyEntry *>(aScrSession.GetComponentPropertyL(aCompId, aPropertyName));
	
	__ASSERT_ALWAYS(propertyEntry || !aAlwaysExpectedInDb, User::Leave(KErrAbort));		
		
	propertyValue = propertyEntry ? propertyEntry->Int64Value() : aDefaultValue;
	delete propertyEntry;
	
	return (propertyValue);	
	}

// Get the value for given string type property related to the given component.
HBufC* Swi::GetStrPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName, const TLanguage aLocale/*= Usif::KUnspecifiedLocale*/, TBool aAlwaysExpectedInDb /*=ETrue*/)
	{
	HBufC *propertyValue = NULL;
	Usif::CLocalizablePropertyEntry* propertyEntry = NULL;
	
	propertyEntry = static_cast<Usif::CLocalizablePropertyEntry *>(aScrSession.GetComponentPropertyL(aCompId, aPropertyName, aLocale));
	
	__ASSERT_ALWAYS(propertyEntry || !aAlwaysExpectedInDb, User::Leave(KErrAbort));
	if (!propertyEntry)
		return NULL;
	
	CleanupStack::PushL(propertyEntry);
	// Ownership of the descriptor is transfered to the caller.
	propertyValue = propertyEntry->StrValue().AllocL();
	CleanupStack::PopAndDestroy(propertyEntry);
	
	return (propertyValue);
	}

// Get the binary string value for given property related to the given component.
HBufC8* Swi::GetBinaryPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName)
	{
	HBufC8 *propertyValue = NULL;
	Usif::CBinaryPropertyEntry* propertyEntry = NULL;
	propertyEntry = static_cast<Usif::CBinaryPropertyEntry *>(aScrSession.GetComponentPropertyL(aCompId, aPropertyName));
		
	__ASSERT_ALWAYS((propertyEntry != NULL), User::Leave(KErrAbort));

	CleanupStack::PushL(propertyEntry);
	// Ownership of the descriptor is transfered to the caller.
	propertyValue = propertyEntry->BinaryValue().AllocL();
	CleanupStack::PopAndDestroy(propertyEntry);
	
	return (propertyValue);
	}


	
// Creates the bit mask formated drives from the TDriveList (string) formated drive.
void Swi::MakeDrivesFromStringL(TDriveList& aDriveList, TUint& aDrive)
	{
	if(aDriveList.Length() == 0 )
		{
		return;
		}
	
	for(TInt driveNum=EDriveA; driveNum<=EDriveZ; ++driveNum)
		{
		if(aDriveList[driveNum])
			{
			aDrive |= 1<<driveNum;
			}
		}	
	}

// Write the Version object in to string format.
void Swi::VersionToString(const TVersion& aVersion, TDes& aVersionString) 
	{
	aVersionString.AppendFormat(KVersionFormat, aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
	}
	
// Create the Version object from the string format.
void Swi::StringToVersionL(const TDesC& aVersionString, TVersion& aVersion)
	{
	_LIT(KDot, ".");	
	TInt dotPos1 = aVersionString.FindF(KDot);
	TInt dotPos2 = 0;
	if(dotPos1 > 0)
		{
		dotPos2 = aVersionString.Mid(dotPos1 + 1).FindF(KDot);
		}
	
	if (dotPos1 <= 0 || dotPos2 <= 0)
		{
		DEBUG_PRINTF2(_L8("Sis Registry Server - The version format is not recognized for a SIS file. Expected format: n.n.n  But, Got : %S"), &aVersionString);		
		User::Leave(KErrNotSupported);
		}	
	TLex lex(aVersionString);
	User::LeaveIfError(lex.Val(aVersion.iMajor));

	lex = aVersionString.Mid(dotPos1 + 1);
	User::LeaveIfError(lex.Val(aVersion.iMinor));

	lex = aVersionString.Mid(dotPos1 + dotPos2 + 2);
	User::LeaveIfError(lex.Val(aVersion.iBuild));
	}
	
void ScrHelperUtil::GetComponentIdListL(const Usif::RSoftwareComponentRegistry& aScrSession, RArray<Usif::TComponentId>& aComponentIdList)
	{
	// Create a filter.
	Usif::CComponentFilter* componentFilter = Usif::CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	
	// Retrieve the componentId of all the components from the SCR.
	aScrSession.GetComponentIdsL(aComponentIdList, componentFilter);
	CleanupStack::PopAndDestroy(componentFilter);
	}

TBool ScrHelperUtil::IsWildcardFile(const TDesC& aFileName)
	{
	return (aFileName.Locate('?') != KErrNotFound || 
			aFileName.Locate('*') != KErrNotFound || 
			aFileName[aFileName.Length() - 1] == '\\');
	}

// Extracts the UID of the component from it's global Id data.
// Just an string to TUid conversion.
void ScrHelperUtil::ExtractCompUidL(const TDesC& aGlobalIdStr, TUid& aUid)
	{
	TInt64 uidNum;	
	TLex lex(aGlobalIdStr);
	TInt error = lex.Val(uidNum, EHex);
	__ASSERT_ALWAYS((error == KErrNone), User::Leave(error));
	aUid = TUid::Uid(uidNum);
	}

TBool ComparePropertyNames(const Usif::CPropertyEntry& aLhs,  const Usif::CPropertyEntry& aRhs)
	{
	return (aLhs.PropertyName() == aRhs.PropertyName());
	}

TInt ScrHelperUtil::FindPropertyL(const RPointerArray<Usif::CPropertyEntry>& aProperties, const TDesC& aPropertyName)
	{
	Usif::CPropertyEntry *dummyProperty = Usif::CIntPropertyEntry::NewLC(aPropertyName, 0);
	TInt ret = aProperties.Find(dummyProperty, TIdentityRelation<Usif::CPropertyEntry>(ComparePropertyNames));
	CleanupStack::PopAndDestroy(dummyProperty);
	return ret;
	}

TInt ScrHelperUtil::GetIntegerPropertyValueFromArrayL(const TDesC& aPropertyName, TInt aDefaultValue, const RPointerArray<Usif::CPropertyEntry>& aProperties)
	{
	TInt propertyValue = aDefaultValue;
	TInt pos = FindPropertyL(aProperties, aPropertyName);
	if(KErrNotFound != pos)
		{
		propertyValue = static_cast<Usif::CIntPropertyEntry*>(aProperties[pos])->IntValue();
		}
	return propertyValue;
	}

HBufC8* ScrHelperUtil::GetLogInfoLC(const Usif::RSoftwareComponentRegistry& aScrSession, TInt aMaxLogEntries)
	{
	RPointerArray<Usif::CScrLogEntry> logEntries;
	CleanupResetAndDestroyPushL(logEntries);
	const TDesC& swType = Usif::KSoftwareTypeNative();
	aScrSession.RetrieveLogEntriesL(logEntries, &swType);
	
	RPointerArray<CLogEntry> swiLogEntries;
	CleanupResetAndDestroyPushL(swiLogEntries);
	
	TInt logCount = logEntries.Count();
	TInt surplusCount = logCount - aMaxLogEntries;
	TInt startIndex = surplusCount>0 ? surplusCount : 0;
	
	for(TInt i=startIndex; i<logCount; ++i)
		{
		Usif::CScrLogEntry *scrLog = logEntries[i];
		CLogEntry *swiLog = new(ELeave)CLogEntry();
		CleanupStack::PushL(swiLog);
		
		swiLog->iPackageName = scrLog->ComponentName().AllocL();
		TVersion version;
		StringToVersionL(scrLog->ComponentVersion(), version);
		swiLog->iMajorVersion = version.iMajor;
		swiLog->iMinorVersion = version.iMinor;
		swiLog->iBuildVersion = version.iBuild;
		swiLog->iEvent = scrLog->OperationTime();
		
		switch (scrLog->OperationType())
			{
			case Usif::EScrCompUnInstall:
				swiLog->iInstallType = ESwiLogUnInstall;
				break;
			case Usif::EScrCompUpgrade:
				swiLog->iInstallType = ESwiLogUpgrade;
				break;
			case Usif::EScrCompInstall:
			default:
				swiLog->iInstallType = ESwiLogInstall;
			}
		
		TBuf<KSmlBufferSize> uidString(KEmptyString);	
		uidString.Copy(scrLog->GlobalId().Ptr(), 8);
		ExtractCompUidL(uidString, swiLog->iUid);
		
		swiLogEntries.AppendL(swiLog);
		CleanupStack::Pop(swiLog); // Ownership has been transferred
		}
	
	HBufC8 *logsBuf = ExternalizePointersArrayLC(swiLogEntries, EFalse); // EFalse -> Don't externalize the array length
	CleanupStack::Pop(logsBuf);
	CleanupStack::PopAndDestroy(2, &logEntries); // logEntries, swiLogEntries, 
	CleanupStack::PushL(logsBuf);
	return logsBuf;
	}
