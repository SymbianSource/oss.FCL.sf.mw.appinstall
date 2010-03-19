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
		void SetIsComponentPresentL(const RMessage2& aMessage);
		
		// SIF Requests
		void SetScomoStateL(const RMessage2& aMessage);
		void GetPluginUidWithMimeTypeL(const RMessage2& aMessage) const;
		void GetPluginUidWithComponentIdL(const RMessage2& aMessage) const;
		
		// Security Layer Requests
		TBool GetInstallerSidForComponentL(TComponentId aComponentId, TSecureId& aSid) const;
		TBool GetSidsForSoftwareTypeL(const HBufC* aSoftwareTypeName, TSecureId& aInstallerSid, TSecureId& aExecutableEnvSid) const;
		TBool GetExecutionEnvSidForComponentL(TComponentId aComponentId, TSecureId& aSid) const;
		TBool IsInstallerOrExecutionEnvSidL(TSecureId& aSid) const;
		TBool GetIsComponentOrphanedL(TComponentId aComponentId) const;
		
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
		void GetComponentIdsHavingThePropertiesL(RArray<TComponentId>& aComponentIds, RPointerArray<CPropertyEntry> aProperties, TBool aDoIntersect) const;
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
		TBool GetIntSoftwareTypeDataForComponentLC(TComponentId aComponentId, const TDesC& aColumnName, TInt& aValue) const;
		TInt GetInstalledDrivesBitmaskL(TComponentId aComponentId) const;
		CGlobalComponentId* ParseGlobalComponendIdLC(const TDesC& aGlobalId) const;
		void GetGeneralDependencyListL(const TDesC& aSelectColumn, const TDesC& aConditionColumn, const TDesC& aConditionValue, RPointerArray<CVersionedComponentId> &aVerCompIdList) const;
		void ReadAndSetCommonComponentPropertyL(const RMessage2& aMessage, const TDesC& aPropertyColumn);
		TBool HasFilesOnDriveL(TDriveUnit aDrive, TComponentId aComponentId);
		CStatement* OpenFileListStatementL(TComponentId aComponentId) const;
		TBool IsSoftwareTypeExistingL(TUint32 aSwTypeId, TUint32 aSifPluginUid, TUint32 aInstallerSecureId, TUint32 aExecutionLayerSecureId, const RPointerArray<HBufC>& aMimeTypesArray, const RPointerArray<CLocalizedSoftwareTypeName>& aLocalizedNamesArray);
		void SubsessionAddLocalizableSoftwareTypeNameL(CStatement& aStmt, CComponentEntry& aEntry, TLanguage aLocale, CCompViewSubsessionContext* aSubsessionContext) const;
		TBool IsDriveReadOnlyL(TInt driveIndex) const;
		TBool CheckForMediaPresenceL(TComponentId aComponentId) const;
		
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
		};
	
	} // End of namespace Usif

#endif /* SCRREQUESTIMPL_H */
