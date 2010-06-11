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
* Declares the class which implements SCR's session and sub-session service requests.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRREQUESTIMPL_H
#define SCRREQUESTIMPL_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries.h"
#include "appregentries.h"
#include "appreginfo.h"
#include <usif/scr/screntries_platform.h>

namespace Usif
	{
	// constants
	static const TLanguage KLangNone = TLanguage(0);
	static const TInt KMaxSwTypeIdDigits = 4; 		///< Maximum number of digits a software type id may have.
	static const TInt KMaxSwTypeIdValue = 0xFFFF;   ///< Maximum value of a software type id.	
	
	// forward declaration
	class CDatabase;
	class CStatement;
	class CScrAsyncRequestImpl;
	class CScrSession;
	class CScrSubsession;
	class CCompViewSubsessionContext;
	class CAppInfoViewSubsessionContext;
	class CRegInfoForApplicationSubsessionContext;
	class CAppRegistrySubsessionContext;
	class CApplicationRegInfoSubsessionContext; 

	class CScrRequestImpl : public CBase
		{
	public:
		static CScrRequestImpl* NewL(RFs& aFs, RFile& aDatabaseFile, RFile& aJournalFile);
		~CScrRequestImpl();
		
		// Transaction Requests
		void CreateTransactionL();
		void RollbackTransactionL();
		void CommitTransactionL();
		
		// Component Management Requests
		void AddComponentL(const RMessage2& aMessage);
		void AddComponentDependencyL(const RMessage2& aMessage);
		void SetComponentLocalizablePropertyL(const RMessage2& aMessage);
		void SetComponentBinaryPropertyL(const RMessage2& aMessage);		
		void SetComponentIntPropertyL(const RMessage2& aMessage);
		void RegisterComponentFileL(const RMessage2& aMessage); 
		void SetFileStrPropertyL(const RMessage2& aMessage);
		void SetFileIntPropertyL(const RMessage2& aMessage);
		void SetComponentNameL(const RMessage2& aMessage);
		void SetVendorNameL(const RMessage2& aMessage);
		void SetIsComponentRemovableL(const RMessage2& aMessage);
		void SetIsComponentDrmProtectedL(const RMessage2& aMessage);
		void SetIsComponentHiddenL(const RMessage2& aMessage);
		void SetIsComponentKnownRevokedL(const RMessage2& aMessage);
		void SetIsComponentOriginVerifiedL(const RMessage2& aMessage);
		void SetComponentSizeL(const RMessage2& aMessage);
		void SetComponentVersionL(const RMessage2& aMessage);
		void DeleteComponentPropertyL(const RMessage2& aMessage);
		void DeleteFilePropertyL(const RMessage2& aMessage);
		void UnregisterComponentFileL(const RMessage2& aMessage);
		void DeleteComponentL(const RMessage2& aMessage);
		void DeleteComponentDependencyL(const RMessage2& aMessage);
		
		// Component Query requests
		void GetComponentEntrySizeL(const RMessage2& aMessage) const;							
		void GetComponentEntryDataL(const RMessage2& aMessage) const;
		void GetComponentLocalizedEntrySizeL(const RMessage2& aMessage) const;
		void GetComponentLocalizedEntryDataL(const RMessage2& aMessage) const;
		void GetFilePropertiesSizeL(const RMessage2& aMessage) const;
		void GetFilePropertiesDataL(const RMessage2& aMessage) const;
		void GetSingleFilePropertySizeL(const RMessage2& aMessage) const;
		void GetSingleFilePropertyDataL(const RMessage2& aMessage) const;
		void GetFileComponentsSizeL(const RMessage2& aMessage) const;
		void GetFileComponentsDataL(const RMessage2& aMessage) const;
		void GetComponentPropertiesSizeL(const RMessage2& aMessage) const;
		void GetComponentPropertiesDataL(const RMessage2& aMessage) const;
		void GetComponentSinglePropertySizeL(const RMessage2& aMessage) const;
		void GetComponentSinglePropertyDataL(const RMessage2& aMessage) const;
		void GetComponentIdListSizeL(const RMessage2& aMessage) const;
		void GetComponentIdListDataL(const RMessage2& aMessage) const;
		void GetComponentFilesCountL(const RMessage2& aMessage) const;
		void GetComponentIdL(const RMessage2& aMessage) const;
		void GetComponentWithGlobalIdSizeL(const RMessage2& aMessage) const;
		void GetComponentWithGlobalIdDataL(const RMessage2& aMessage) const;
		void GetSupplierComponentsSizeL(const RMessage2& aMessage) const;
		void GetSupplierComponentsDataL(const RMessage2& aMessage) const;
		void GetDependantComponentsSizeL(const RMessage2& aMessage) const;
		void GetDependantComponentsDataL(const RMessage2& aMessage) const;
		void GetComponentSupportedLocalesListSizeL(const RMessage2& aMessage) const;
		void GetComponentSupportedLocalesListDataL(const RMessage2& aMessage) const;
				
		// Application Manager Requests
		void GetIsMediaPresentL(const RMessage2& aMessage) const;
		void GetIsComponentOrphanedL(const RMessage2& aMessage) const;
		void GetLogFileHandleL(const RMessage2& aMessage) const;
		void FlushLogEntriesArrayL();
		void GetIsComponentOnReadOnlyDriveL(const RMessage2& aMessage) const;
		void GetIsComponentPresentL(const RMessage2& aMessage) const;
		TBool IsComponentPresentL(TComponentId aComponentId) const;
		void SetIsComponentPresentL(const RMessage2& aMessage);
		
		// SIF Requests
		void SetScomoStateL(const RMessage2& aMessage);
		void GetPluginUidWithMimeTypeL(const RMessage2& aMessage) const;
		void GetPluginUidWithComponentIdL(const RMessage2& aMessage) const;
		
		// Security Layer Requests
		TBool GetInstallerOrExecutionEnvSidsForComponentL(TComponentId aComponentId, RArray<TSecureId>& aSids) const;
		TBool GetSidsForSoftwareTypeIdL(TInt aSoftwareTypeId, RArray<TSecureId>& aSids) const;
		TBool GetSidsForSoftwareTypeL(const HBufC* aSoftwareTypeName, RArray<TSecureId>& aSids) const;
		TBool IsInstallerOrExecutionEnvSidL(TSecureId& aSid) const;
		TBool GetIsComponentOrphanedL(TComponentId aComponentId) const;
		TInt GetSoftwareTypeForComponentL(TComponentId aComponentId) const;
		
		// AppArc Requests
		void GetApplicationLaunchersSizeL(const RMessage2& aMessage) const;
		void GetApplicationLaunchersDataL(const RMessage2& aMessage) const;
		
		// Component's view operations
		CComponentFilter* ReadComponentFilterL(const RMessage2& aMessage) const;
		CStatement* OpenComponentViewL(CComponentFilter& aFilter, RArray<TComponentId>& aComponentFilterSuperset, TBool& aFilterSupersetInUse) const;
		void NextComponentSizeL(const RMessage2& aMessage, CStatement* aStmt, CComponentFilter* aFilter, CComponentEntry*& aEntry, CCompViewSubsessionContext* aSubsessionContext) const;
		void NextComponentDataL(const RMessage2& aMessage, CComponentEntry*& aEntry) const;
		void NextComponentSetSizeL(const RMessage2& aMessage, CStatement* aStmt, CComponentFilter* aFilter, RPointerArray<CComponentEntry>& aEntryList, CCompViewSubsessionContext* aSubsessionContext) const;
		void NextComponentSetDataL(const RMessage2& aMessage, RPointerArray<CComponentEntry>& aEntryList) const;
		
		// Files's list operations
		CStatement* OpenFileListL(const RMessage2& aMessage) const; 	 
		void NextFileSizeL(const RMessage2& aMessage, CStatement* aStmt, HBufC*& aFilePath) const;
		void NextFileDataL(const RMessage2& aMessage, HBufC*& aFilePath) const;
		void NextFileSetSizeL(const RMessage2& aMessage, CStatement* aStmt, RPointerArray<HBufC>& aFileList) const;
		void NextFileSetDataL(const RMessage2& aMessage, RPointerArray<HBufC>& aFileList) const;
		
		// Static utility functions - code reuse between SCRServer and this class
		static TComponentId GetComponentIdFromMsgL(const RMessage2& aMessage);
		static HBufC* GetSoftwareTypeNameFromMsgLC(const RMessage2& aMessage);
		static HBufC* ReadFileNameFromMsgLC(const RMessage2& aMessage);
		
		// Sofwtare type management operations
		void AddSoftwareTypeL(const RMessage2& aMessage);
		void DeleteSoftwareTypeL(const RMessage2& aMessage);
		void GetDeletedMimeTypesL(const RMessage2& aMessage) const;
		
		// AppInfo's view operations
		//CAppRegistrySubsession
        void OpenApplicationRegistrationViewL(const RMessage2& aMessage, CAppRegistrySubsessionContext*  aSubsessionContext);
        void OpenApplicationRegistrationForAppUidsViewL(const RMessage2& aMessage, CAppRegistrySubsessionContext*  aSubsessionContext);
        void NextApplicationRegistrationInfoSizeL(const RMessage2& aMessage, CApplicationRegistrationData*& aApplicationRegistration, CAppRegistrySubsessionContext*  aSubsessionContext);
        void NextApplicationRegistrationInfoDataL(const RMessage2& aMessage, CApplicationRegistrationData*& aApplicationRegistration);
        
		//CAppInfoViewSubsession
        CAppInfoFilter* ReadAppInfoFilterL(const RMessage2& aMessage) const;
        void OpenAppInfoViewL(CAppInfoFilter& aFilter, CAppInfoViewSubsessionContext* aSubsessionContext);
        TBool DoesAppWithScreenModeExistL(TUid aUid, TInt aScreenMode, TLanguage aLocale) const;
        void GetAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TBool aScreenModePresent = EFalse) const;
        void GetEmbeddableAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TBool aScreenModePresent = EFalse) const;
        void GetServerAppUidsL(CAppInfoViewSubsessionContext* aSubsessionContext, TUid aServiceUid, TBool aScreenModePresent = EFalse) const;
        void GetAppUidsWithEmbeddabilityFilterL(CAppInfoViewSubsessionContext* aSubsessionContext, TEmbeddableFilter& aFilter, TBool aScreenModePresent = EFalse) const;
        void GetAppUidsWithCapabilityMaskAndValueL(CAppInfoViewSubsessionContext* aSubsessionContext,TUint aCapabilityAttrFilterMask, TUint aCapabilityAttrFilterValue, TBool aScreenModePresent = EFalse) const;
        void GetAppServicesL(TUid aAppUid, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const;
        void GetServiceImplementationsL(TUid aServiceUid, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const;
        void GetServiceImplementationsL(TUid aServiceUid, TDesC& aDataType, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const;
        void GetAppServiceOpaqueDataL(TUid aAppUid, TUid aServiceUid, RPointerArray<CServiceInfo>& aServiceInfoArray, TLanguage aLocale) const;
        void GetOpaqueDataArrayL(TUid aAppUid, TUid aServiceUid, RPointerArray<COpaqueData>& aOpaqueDataArray, TLanguage aLanguage) const;
        void NextAppInfoSizeL(const RMessage2& aMessage, TAppRegInfo*& aAppInfo, CAppInfoViewSubsessionContext* aSubsessionContext);
        void NextAppInfoDataL(const RMessage2& aMessage, TAppRegInfo*& aAppInfo);

        //CRegInfoForApplicationSubsession
        void GetServiceUidSizeL(const RMessage2& aMessage, TUid aAppUid,CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetServiceUidDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void SetLocaleForRegInfoForApplicationSubsessionContextL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext);
        void GetApplicationLanguageL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetDefaultScreenNumberL(const RMessage2& aMessage, TUid aAppUid) const;
        void GetNumberOfOwnDefinedIconsL(const RMessage2& aMessage, TUid aAppUid,CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetViewSizeL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetViewDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetViewsL(RPointerArray<Usif::CAppViewData>& aViewInfoArray,TUid aAppUid, TLanguage aLanguage)const;
        void GetAppOwnedFilesSizeL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext)const ;
        void GetAppOwnedFilesDataL(const RMessage2& aMessage, CRegInfoForApplicationSubsessionContext *aSubsessionContext)const ;
        void GetAppCharacteristicsL(const RMessage2& aMessage, TUid aAppUid) const;
        void GetAppIconForFileNameL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
        void GetAppViewIconFileNameL(const RMessage2& aMessage, TUid aAppUid, CRegInfoForApplicationSubsessionContext *aSubsessionContext) const;
		void GetAppPropertiesInfoL(CApplicationRegistrationData& aApplicationRegistrationaApplicationRegistration,TUid aAppUid, TLanguage aLanguage);
		//CApplicationRegInfoSubsession
        void GetAppForDataTypeAndServiceL(const RMessage2& aMessage) const;
        void GetAppForDataTypeL(const RMessage2& aMessage) const;
        void GetAppServiceInfoSizeL(const RMessage2& aMessage, CApplicationRegInfoSubsessionContext *aSubsessionContext) const;
        void GetAppServiceInfoDataL(const RMessage2& aMessage, CApplicationRegInfoSubsessionContext *aSubsessionContext) const;
        void GetApplicationInfoL(const RMessage2& aMessage);
        
        //General functions to retrieve various application information, mostly used by CAppRegistrySubsession
        TBool GetApplicationRegistrationInfoL(CApplicationRegistrationData& aApplicationRegistrationaApplicationRegistration,TUid aAppUid) const;
        void GetFileOwnershipInfoL(CApplicationRegistrationData& aApplicationRegistrationaApplicationRegistration,TUid aAppUid) const;
        void GetDataTypesL(RPointerArray<Usif::CDataType> & aDataTypes,TInt aServiceId)const;
        void GetServiceInfoL(CApplicationRegistrationData& aApplicationRegistrationaApplicationRegistration,TUid aAppUid,TLanguage aLanguage) const;
        void GetLocalizableAppInfoL(CApplicationRegistrationData& aApplicationRegistrationaApplicationRegistration,TUid aAppUid, TLanguage aLanguage);
        CCaptionAndIconInfo* GetCaptionAndIconInfoL(TInt aCaptionAndIconId)const;
        void GetAppRegOpaqueDataL(CApplicationRegistrationData& aApplicationRegistration,TUid aAppUid,TLanguage aLanguage) const;
        
        //General functions
		void GetLocalesForAppIdL(RArray<TInt>& aLocales, TUid aAppUid) const;
		TBool CheckIfAppUidExistsL(const TUid aAppUid) const;
        CStatement* CreateStatementForAppInfoL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const;
        void GetCaptionAndShortCaptionInfoForLocaleL(TUid aAppUid, TLanguage aLocale, TAppCaption& aShortCaption, TAppCaption& aCaption);
        void GenerateNonNativeAppUidL(const RMessage2& aMessage);

		//Application related
		void AddApplicationEntryL(const RMessage2& aMessage);		
		void DeleteAllAppsWithinPackageL(const RMessage2& aMessage);
		void DeleteAllAppsWithinPackageInternalL(const TComponentId aComponentId);
		void DeleteApplicationEntryL(const RMessage2& aMessage);
		void DeleteApplicationEntryInternalL(const TInt aAppUid);
		void GetComponentIdForAppL(const RMessage2& aMessage) const;
	    TBool GetComponentIdForAppInternalL(TUid aAppUid, TComponentId& aComponentId) const;
		void GetAppUidsForComponentSizeL(const RMessage2& aMessage) const;
		void GetAppUidsForComponentDataL(const RMessage2& aMessage)const; 

		void AddFileOwnershipInfoL(TUid aAppUid, const TDesC& aFileName);
		void AddServiceInfoL(TUid aAppUid, CServiceInfo*  aAppServiceInfoEntry);
		void AddServiceDataTypeL(TInt aServiceUid, Usif::CDataType* aDataTypeEntry);
		void AddLocalizableAppInfoL(TUid aAppUid, Usif::CLocalizableAppInfo* aLocalizableAppInfoEntry);
		void AddViewDataL(TInt aLocalAppInfoId, Usif::CAppViewData* aViewDataEntry);
		TInt AddCaptionAndIconInfoL(CCaptionAndIconInfo* aCaptionAndIconEntry);
		void AddPropertyL(TUid aAppUid, Usif::CPropertyEntry* aAppPropertiesEntry);
		void AddOpaqueDataL(TUid aAppUid, Usif::COpaqueData*  aOpaqueDataEntry, TUid aServiceUid = TUid::Null());
		void DeleteFromTableL(const TDesC& aTableName, const TDesC& aAttribute, const TInt aValue);		
	private:
		CScrRequestImpl(RFs& aFs);
		void ConstructL(RFile& aDatabaseFile, RFile& aJournalFile);
	
	private:
		enum TValueType
			{
			EValueInteger 	 = 0x01,
			EValueString 	 = 0x02,
			EValueInteger64  = 0x03,
			EValueLanguage   = 0x04,
			EValueBinary = 0x05,
			};
		
		enum TPropertyType
			{
			EPropertyUndefined = 0,	  // The property type is not defined yet
			EPropertyInteger   = 1,   // The property type is integer
			EPropertyLocalizable    = 2,    // The property type is a localizable string
			EPropertyBinary    = 3    // The property type is a non-localizable 8-bit descriptor
			};
		
		enum TFileOperationType
			{
			EFileRegistered,
			EFileUnregistered
			};
		
		class TRollbackParams
			{
		public:
			TRollbackParams(TInt aIdColumnVal, CScrRequestImpl& aReqImplHandle)
				: iIdColumnVal(aIdColumnVal), iReqImplHandle(aReqImplHandle){}
		
			TInt iIdColumnVal;
			CScrRequestImpl& iReqImplHandle;
			};
		
	    static HBufC* ReadDescLC(const RMessage2& aMessage, TInt aSlot);
	    static HBufC8* ReadDesc8LC(const RMessage2& aMessage, TInt aSlot);
		static TUint32 HashCaseInsensitiveL(const TDesC& aName);
		static TUint32 HashCaseSensitiveL(const TDesC& aName);
		static void ComponentRollback(TAny* aParam);
		static void ComponentLocalizablesRollback(TAny* aParam);
		
		HBufC* FormatStatementLC(const TDesC& aStatement, TInt aFormatLength,...) const;
		HBufC* GenerateGlobalIdL(const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId) const;
		TComponentId CommonAddComponentL(const TDesC& aUniqueSwTypeName, const TDesC& aGlobalId);
		void AddComponentLocalizablesL(TComponentId aCompId, TLanguage aLocale, const TDesC& aName, const TDesC& aVendor);
		HBufC* ReadAndGetGlobalIdLC(const RMessage2& aMessage, TInt aSlot) const;
		TUint32 ReadAndHashGlobalIdL(const RMessage2& aMessage, TInt aGlobalIdNameSlot, TInt aSwTypeNameSlot) const;
		TUint32 HashGlobalIdsL(const TDesC& aDependantId, const TDesC& aSupplierId) const;		
		CStatement* CreateGeneralPropertyStatementWithLocaleL(const TDesC& aStmtStr, TInt aIdColumnValue, TLanguage aLocale, const TDesC& aPropName, TBool aDoLocaleResolving=ETrue) const;
		TInt FindGeneralPropertyNoLocaleDowngradeL(const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropertyName, TLanguage aLocale, TPropertyType& aPropertyType) const;
		void SetGeneralLocalizablePropertyL(CScrRequestImpl::TPropertyType aPropType, TInt aPropertyId, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName, TLanguage aLocale, const TDesC& aPropValue);
		void SetGeneralBinaryPropertyL(CScrRequestImpl::TPropertyType aPropType, TInt aPropertyId, const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName, const TDesC8& aPropValue);
		void SetGeneralIntPropertyL(TPropertyType aPropType, TInt aPropertyId, const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName, TInt64 aPropValue);
		void ExecuteStatementL(TRefByValue<const TDesC>, TInt aValuesNum=0,...);
		void GetFileComponentsL(const TDesC& aFileName, RArray<TComponentId>& aComponents) const;
		TInt GetComponentFileIdL(const TDesC& aFileName, TComponentId aComponentId) const;
		TInt FindComponentFileL(const TDesC& aFileName, TComponentId aComponentId) const;
		void SetComponentLocalizableL(TComponentId aComponentId, TLanguage aLocale, const TDesC& aColumnName, const TDesC& aName, const TDesC& aVendor);
		CPropertyEntry* GetGeneralSinglePropertyL(const TDesC& aTableName, const TDesC& aIdColumnName , TInt aIdColumnValue, const TDesC& aPropName, TLanguage aLocale) const;
		CPropertyEntry* GetPropertyEntryL(CStatement& aStmt, const TDesC& aPropName, TInt aStartingIndex) const;
		void AddGeneralPropertiesArrayWithLocaleL(const TDesC& aStmtStr, TLanguage aLocale, TComponentId aIdColumnValue, RPointerArray<CPropertyEntry>& aProperties) const;
		void GetGeneralPropertiesArrayL(const TDesC& aTableName, const TDesC& aIdColumnName , TComponentId aIdColumnValue, TLanguage aLocale, RPointerArray<CPropertyEntry>& aProperties) const;
		TBool BindAndProcessStatementObjectL(CStatement& aStatementObj, TLanguage aLanguage, TInt aValuesNum, VA_LIST aList) const;
		CStatement* CreateStatementObjectWithoutLocaleL(const TDesC& aStatement, TInt aValuesNum,...) const;		
		CStatement* CreateStatementObjectWithLocaleL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const;
		CStatement* CreateStatementObjectWithLocaleNoDowngradeL(const TDesC& aStatement, TLanguage aLocale, TInt aValuesNum,...) const;
		void BindStatementValuesL(CStatement& aStatement, TLanguage aLanguage, TInt aValuesNum, VA_LIST aList) const;
		void GetComponentIdsHavingThePropertiesL(RArray<TComponentId>& aComponentIds, RPointerArray<CPropertyEntry>& aProperties, RArray<CComponentFilter::TPropertyOperator>& aPropertyOperatorList, 
                                                     TBool aDoIntersect) const;
		CStatement* CreateStatementObjectForComponentLocalizablesLC(const TDesC& aName, const TDesC& aVendor, TUint aSetFlag, TComponentId aComponentId  = 0 ) const;
		void GetComponentsHavingNameVendorL(RArray<TComponentId>& aComponentIds, const TDesC& aName, const TDesC& aVendor, TUint16 aSetFlag, TBool aDoIntersect) const;
		CComponentEntry* GetNextComponentEntryL(CStatement& aStmt, CComponentFilter& aFilter, TLanguage aLocale, CCompViewSubsessionContext* aSubsessionContext) const;
		HBufC* GetNextFilePathL(CStatement& aStmt) const;
		CComponentEntry* CreateComponentEntryFromStatementHandleL(CStatement& aStmt) const;
		CStatement* ExecuteLocalizableStatementL(const TDesC& aLocalizableStmtStr, const TDesC& aAnyValueStmtStr, TInt aConditionIntValue, TLanguage aConditionLocale) const;
		void AddComponentEntryLocalizablesL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale) const;
		void AddComponentEntryLocalizablesL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale, const CComponentFilter& aFilter) const;
		void AddSoftwareTypeNameToComponentEntryL(CStatement& aStmt, CComponentEntry& aEntry, TLanguage aLocale) const;
		TBool IsSoftwareTypeExistingL(TInt aSoftwareTypeId) const;
		void GetGeneralComponentEntrySizeL(const RMessage2& aMessage, const TDesC& aConditionColumn, TUint32 aConditionValue, TInt aReturnSizeSlot, TLanguage aLocale, CComponentEntry*& aComponentEntry) const;
        TInt GetDriveFromFilePath(const TDesC& aFilePath, TDriveUnit& aDriveUnit) const;
		TInt InstalledDrivesToBitmaskL(const TDriveList& aDriveList) const;
		void UpdateInstalledDrivesL(TComponentId aComponentId, const TDesC& aFilePath, TFileOperationType aType);
		TBool GetSifPluginUidIInternalL(TInt aSoftwareTypeId, TInt& aValue) const;
		TInt GetInstalledDrivesBitmaskL(TComponentId aComponentId) const;
		CGlobalComponentId* ParseGlobalComponendIdLC(const TDesC& aGlobalId) const;
		void GetGeneralDependencyListL(const TDesC& aSelectColumn, const TDesC& aConditionColumn, const TDesC& aConditionValue, RPointerArray<CVersionedComponentId> &aVerCompIdList) const;
		void ReadAndSetCommonComponentPropertyL(const RMessage2& aMessage, const TDesC& aPropertyColumn);
		TBool HasFilesOnDriveL(TDriveUnit aDrive, TComponentId aComponentId);
		CStatement* OpenFileListStatementL(TComponentId aComponentId) const;
		TBool IsSoftwareTypeExistingL(TUint32 aSwTypeId, TUint32 aSifPluginUid, RArray<TCustomAccessInfo>& aSidArray, const RPointerArray<HBufC>& aMimeTypesArray, const RPointerArray<CLocalizedSoftwareTypeName>& aLocalizedNamesArray, const TDesC& aLauncherExecutable);
		void SubsessionAddLocalizableSoftwareTypeNameL(CStatement& aStmt, CComponentEntry& aEntry, TLanguage aLocale, CCompViewSubsessionContext* aSubsessionContext) const;
		TBool IsDriveReadOnlyL(TInt driveIndex) const;
		TBool CheckForMediaPresenceL(TComponentId aComponentId) const;
		
		TBool GetIntforConditionL(const TDesC& aSelectColumn, const TDesC& aTableInfo, const TDesC& aConditionColumn,TInt aConditionValue,TInt& aRetrievedValue) const;
		TInt  GetServiceIdForDataTypeL(const TDesC& aType) const;
		TBool GetAppUidForServiceIdL(const TInt ServiceId, TUid& aAppUid) const;
		TBool GetNearestAppLanguageL(TLanguage aRequiredLocale,TUid aAppUid,TLanguage& aFinalAppLocale) const;
		void GetNearestAppLanguageForOpaqueDataL(TLanguage aRequiredLocale,TUid aAppUid,TUid aServiceUid,TLanguage& aFinalAppLocale) const;
		void GetOperatorStringL(CComponentFilter::TDbOperator aOperator, HBufC*& aOperatorString) const;

		// DB Version management
		void InitializeDbVersionL();
		void VerifyDbVersionCompatibilityL() const;
		
	private:
		RFs& iFs;	///< Reference to the file handle. It is worth to consume a word of memory instead of passing the file handle to many functions.
		CDatabase* iDbHandle; ///< Handle to the SCR database object.
		TVersion iDbVersion;
		mutable CComponentEntry* iComponentEntry; 			///< The component entry whose size calculated and sent to the client.
		mutable CPropertyEntry* iSingleProperty;
		mutable RArray<TComponentId> iFileComponents;
		mutable RArray<TComponentId> iComponentIdList;
		mutable RPointerArray<CPropertyEntry> iProperties;
		mutable RPointerArray<CVersionedComponentId> iVerCompIdList; // The list of supplier/dependant versioned component IDs
		mutable RPointerArray<HBufC> iDeletedMimeTypes; // The list of MIME types deleted as the result of software type deletion.
		mutable RPointerArray<CScrLogEntry> iLogEntries; // The list of log entries recorded in a session
		mutable RArray<TLanguage> iMatchingSupportedLanguageList; // The list of matching supported language list recorded in a session
		mutable RPointerArray<CLocalizableComponentInfo> iCompLocalizedInfoArray; //Component's localized information (name, vendor, locale)
		mutable RArray<TUid> iComponentAppUids;  //List of Application Uids associated with a component
		mutable RPointerArray<CLauncherExecutable> iLaunchers;
		};
	
	} // End of namespace Usif

#endif /* SCRREQUESTIMPL_H */
