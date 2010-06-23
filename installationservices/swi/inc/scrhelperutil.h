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
* ScrHelperUtil interface definition
* Helps the other classes on retrieving the SCR properties.
*
*/



/**
 @file 
 @released
 @internalComponent
*/
 
#ifndef __SCRHELPERUTIL_H__
#define __SCRHELPERUTIL_H__

#include <e32base.h>
#include <usif/scr/scr.h>
#include "../inc/swi/installtypes.h"

namespace Swi
{

class CSisRegistryObject;
class CControllerInfo;
class CHashContainer;
class CSisRegistryToken;
class CSisRegistryPackage;
class CSisRegistryDependency;
class CSisRegistryProperty;
class CSisRegistryFileDescription;
class TSisTrustStatus;
class CLogEntry;

namespace ScrHelperUtil
	{
	// Set of over loaded methods used to write the corresponding objects in to SCR and
	// to retrieve the same objects back from the SCR.
	// All these overloaded methods are friends to the corresponding classes.	
	void AddComponentL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType);
	void GetComponentL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject);
	void GetComponentLocalizedInfoL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject);
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryToken& aToken);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::CComponentEntry* aComponentEntry, CSisRegistryToken& aToken, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	
	// As the below set of 11 methods are declared as friend in published partner classes,
	// just to avoid unnecessary export of this header file, they all forward declared in 
	// their corresponding header files.
	// So, the changes done to these set of methods should be reflected there too. *- Otherwise it may result in run time problems -*.
	
	// Published partner friends set begin
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryPackage& aPackage);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::CComponentEntry* aComponentEntry, CSisRegistryPackage& aPackage);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aEmbeddedCompId, CSisRegistryPackage*& aPackage);
		
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TSisTrustStatus& aTrustStatus);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, TSisTrustStatus& aTrustStatus, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer& aHashContainer, TInt aIndex);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, TInt aIndex);
	
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const CHashContainer& aHashContainer, const TDesC& aFileName);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CHashContainer*& aHashContainer, const TDesC& aFileName);
			
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, CSisRegistryDependency& aRegistryDependency);
	void ReadFromScrL(Usif::CVersionedComponentId& aVersionedSupplierCompId, CSisRegistryDependency*& aRegistryDependency);
	// Published partner friends set end

	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo& aControllerInfo, TInt aIndex);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CControllerInfo*& aControllerInfo, TInt aIndex);
	
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty& aRegistryProperty, TInt aIndex);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryProperty*& aRegistryProperty, TInt aIndex);
	
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryFileDescription& aFileDescription, TBool aWildcardedFile, TInt aIndex, TInt aExecutableIndex);
	void ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryFileDescription*& aFileDescription, const TDesC& aFileName, TBool aWildcardedFile);
		
	void ExternalizeSidArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RArray<TUid>& aSidsArray);
	void ExternalizeControllerArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RPointerArray<CControllerInfo>& aControllerInfoArray);
	void ExternalizeSupplierPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, RPointerArray<CSisRegistryDependency>& aDependentPackageArray);
	void ExternalizeEmbeddedPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryPackage>& aEmbeddedPackageArray);
	void ExternalizePropertiesArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryProperty>& aRegistryPropertyArray);
	void ExternalizeFileDescriptionsArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryFileDescription>& aFileDescriptionArray);
	void ExternalizeChainIndexArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const RArray<TInt>& aChainIndexArray);
	
	void InternalizeSidArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RArray<TUid>& aSidsArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	void InternalizeControllerArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CControllerInfo>& aControllerInfoArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	void InternalizeSupplierPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType, RPointerArray<CSisRegistryDependency>& aDependentPackageArray);
	void InternalizeEmbeddedPackageArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryPackage>& aEmbeddedPackageArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	void InternalizePropertiesArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryProperty>& aRegistryPropertyArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	void InternalizeFileDescriptionsArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RPointerArray<CSisRegistryFileDescription>& aFileDescriptionArray);
	void InternalizeChainIndexArrayL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, RArray<TInt>& aChainIndexArray, RPointerArray<Usif::CPropertyEntry>* aPropertyArray=NULL);
	
	TBool IsUidPresentL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid);
	
	TBool IsUidAndNamePresentL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TDesC& aPackageName);	

	TBool GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TInt aPackageIndex, Usif::TComponentId& aComponentId);
	Usif::TComponentId GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TInt aPackageIndex);

	TBool GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TDesC& aPackageName, const TDesC& aVendorName, Usif::TComponentId& aComponentId);
	 /**
	 * Returns the component matching the given package name and vendor name  
	 * NOTE: There can be multiple such components and this API returns the first one found! 
	 */	
	Usif::TComponentId GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TDesC& aPackageName, const TDesC& aVendorName);

	void GetComponentIdListL(const Usif::RSoftwareComponentRegistry& aScrSession, RArray<Usif::TComponentId>& aComponentIdList);
	
	// Returns ETrue if the filname either ends with a '\' or contains any wildcards ('?' or '*') 
	TBool IsWildcardFile(const TDesC& aFileName);	
		
	void ExtractCompUidL(const TDesC& aCompGlobalId, TUid& aUid);	
	
	TInt FindPropertyL(const RPointerArray<Usif::CPropertyEntry>& aProperties, const TDesC& aPropertyName);
	TInt GetIntegerPropertyValueFromArrayL(const TDesC& aPropertyName, TInt aDefaultValue, const RPointerArray<Usif::CPropertyEntry>& aProperties);
	// Retrieve SWI logs
	HBufC8* GetLogInfoLC(const Usif::RSoftwareComponentRegistry& aScrSession, TInt aMaxLogEntries);
	
	void WriteToScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType);
	void AddApplicationEntryL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, const Usif::CApplicationRegistrationData& aApplicationRegistrationData);
	void DeleteApplicationEntriesL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId);
	void DeleteApplicationEntryL(Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aAppUId);
	
	} // End of name space ScrHelperUtil
	
	// Other name space wide common utility methods	
	TInt GetIntPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName, TBool aAlwaysExpectedInDb, TInt aDefaultValue = 0);
	TInt64 GetInt64PropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName,  TBool aAlwaysExpectedInDb, TInt64 aDefaultValue = 0);
	HBufC* GetStrPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName, const TLanguage aLocale = Usif::KUnspecifiedLocale, TBool aAlwaysExpectedInDb = ETrue);
	HBufC8* GetBinaryPropertyValueL(const Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, const TDesC& aPropertyName);
		
	// Creates the bit mask formated drives from the TDriveList (string) formated drive.	
	void MakeDrivesFromStringL(TDriveList& aDriveList, TUint& aDrive);
	
	// Version conversion methods.
	void VersionToString(const TVersion& aVersion, TDes& aVersionString);
	void StringToVersionL(const TDesC& aVersionString, TVersion& aVersion);

	// Format the component global Id based on the install type.
	HBufC* FormatGlobalIdLC(const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType);
} // End of name space Swi
#endif // End of __SCRHELPERUTIL_H__
