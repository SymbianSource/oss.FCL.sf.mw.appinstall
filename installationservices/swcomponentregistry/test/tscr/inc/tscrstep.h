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
* Defines the basic test step for the Software Component Registry test harness
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCRSTEP_H
#define TSCRSTEP_H

#include <scs/oomteststep.h>
#include <usif/usifcommon.h>
#include <usif/scr/scr.h>
#include <usif/scr/appreginfo.h>

class CScrTestServer;

// Shared operators for comparing properties
TBool operator !=(Usif::CPropertyEntry& lhsEntry, Usif::CPropertyEntry& rhsEntry);
TBool operator ==(Usif::CPropertyEntry& lhsEntry, Usif::CPropertyEntry& rhsEntry);
TBool operator ==(const Usif::CComponentEntry& aLhsEntry, const Usif::CComponentEntry& aRhsEntry);
TBool operator !=(const Usif::CComponentEntry& aLhsEntry, const Usif::CComponentEntry& aRhsEntry);

class CScrTestStep : public COomTestStep
/**
 	Base class used to provide handy functions available to test step classes.
 */
	{
	friend class CScrPerformanceTestStep;
public:
	virtual ~CScrTestStep();
	void MarkAsPerformanceStep();
	
protected:
	CScrTestStep(CScrTestServer& aParent);
	
	// From COomTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	RScsClientBase* ClientHandle();
	
	// Utility Functions
	void PrintErrorL(const TDesC& aMsg, TInt aErrNum,...);
	
	// Shared code for comparing properties
	TBool ComparePropertiesL(RPointerArray<Usif::CPropertyEntry>& aFoundProperties, RPointerArray<Usif::CPropertyEntry>& aExpectedProperties);
	static void GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex);
	void CompareComponentIdsL(RArray<Usif::TComponentId>& aFoundComponents, RArray<Usif::TComponentId>& aExpectedComponents);
	
	// Helper functions for getting common test step attributes	
	void AppendSharedComponentIdL(Usif::TComponentId aComponentId);
	void GetSoftwareTypeNameL(TPtrC& aSwTypeName);
	TBool GetGlobalIdNameL(TPtrC& aGlobalIdName);
	Usif::CGlobalComponentId* GetGlobalComponentIdLC();
	Usif::CGlobalComponentId* GetGlobalComponentIdLC(const TDesC& aGlobalIdName, const TDesC& aSwTypeName);
	HBufC* GetVersionFromConfigL(const TDesC& aVersionName);
	void GetExpectedVersionedComponentIdListL(RPointerArray<Usif::CVersionedComponentId>& aVerCompIdList);
	TBool CompareVersionedComponentIdsL(RPointerArray<Usif::CVersionedComponentId>& aFoundVerCompIdList, RPointerArray<Usif::CVersionedComponentId>& aExpectedVerCompIdList);
	TBool GetLocaleFromConfigL(const TDesC& aParamName, TLanguage &aLocale);
	TBool GetLocaleFromConfigL(TLanguage &aLocale);
	void GetFileNameFromConfigL(TPtrC& aFileName);
	void GetFileNameListFromConfigL(RPointerArray<HBufC>& aFileList);
	TInt GetComponentIdL();
	void ReadAllSharedComponentIdsL(RArray<TInt>& aComponentList);
	void GetComponentNameFromConfigL(TPtrC& aComponentName);
	void GetVendorNameFromConfigL(TPtrC& aVendorName);
	Usif::CComponentEntry* GetComponentEntryFromConfigLC(TBool aIsSingle=ETrue, TInt aIndex=0); 
	void GetPropertiesFromConfigL(RPointerArray<Usif::CPropertyEntry>& aProperties, TBool aSupportLocalized=EFalse);
	Usif::CPropertyEntry* GetPropertyFromConfigLC(TBool aIsSingle, TInt aIndex = 0, TBool aSupportLocalized=EFalse);
	void GetComponentIdsFromConfigL(RArray<Usif::TComponentId>& aComponentIds);
	void GetComponentLocalizablesFromConfigL(TInt aIndex, TPtrC& aName, TPtrC& aVendor, TLanguage& aLocale);
	void GetLocalisedComponentsFromConfigL(RPointerArray<Usif::CLocalizableComponentInfo>& aComponentInfo);
	TBool GetInstalledDrivesFromConfigL(TDriveList& aDriveList, const TDesC& aAttributeName);
	TBool GetScomoStateFromConfigL(Usif::TScomoState& aScomoState, const TDesC& aAttributeName);
	TInt GetSetSizeFromConfigL();
	TBool Get64BitIntegerFromConfigL(const TDesC& aConfigKeyName, TInt64& aRetVal);
	Usif::CApplicationRegistrationData* GetAppRegInfoFromConfigLC();
	void GetAppOwnedFilesL(RPointerArray<HBufC>& aOwnedFileArray);
	void GetAppServiceInfoL(RPointerArray<Usif::CServiceInfo>& aServiceInfoArray);
	void GetAppLocalizableInfoL(RPointerArray<Usif::CLocalizableAppInfo>& aLocalizableAppInfoArray);
	void GetAppOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aAppOpaqueDataInfoArray);
	void GetServiceOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aServiceOpaqueDataInfoArray, TInt aServiceOpaqueDataInfoCount, TInt aStartingIndex);
	Usif::TComponentId GetCompIdFromConfigL();
    TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid);
	
	TUid GetServiceUidL();
	void GetAppUidL(TUid& aAppUid);
	void GetViewDataInfoFromConfigL(RPointerArray<Usif::CAppViewData>& aAppViewInfoArray);
	TBool NotEqual(const Usif::CCaptionAndIconInfo *aLhsEntry, const Usif::CCaptionAndIconInfo *aRhsEntry) const;
	TBool NotEqualL(const RPointerArray<Usif::CServiceInfo>& aLhsEntry, const RPointerArray<Usif::CServiceInfo>& aRhsEntry) const;
	
	// Read component names from config file and add a new component
	Usif::TComponentId AddNonLocalisableComponentL(Usif::RSoftwareComponentRegistry& aScrSession);
	
	// Read filter properties from config file
	Usif::CComponentFilter* ReadComponentFilterFromConfigLC();
	void ReadFilterPropertiesL(Usif::CComponentFilter* aFilter, TInt aPropertiesCount);
	// Read Application info filter from config file
	void ReadAppInfoFilterFromConfigL(Usif::CAppInfoFilter** aFilter);
	Usif::TAppRegInfo* CScrTestStep::GetAppInfoFromConfigL(TBool aIsSingle , TInt aIndex=0 );
	
	void StartTimer();
	void StopTimerAndPrintResultL();
	void PrintPerformanceLog(TTime aTime);

private:
	TInt ReadSharedComponentIdL(TInt aOffset);	

protected:
	CScrTestServer& iParent;
	Usif::RSoftwareComponentRegistry iScrSession;
	TBool iIsPerformanceTest;
	
	// For plugin management test cases, the time is measured externally because they are performed via another accessor server (scr accessor).
	// The actual test time is measured on the scr accessor server side and returned to the test code.
	TInt iTimeMeasuredExternally;
		
private:	
	TTime iStartTime;
	};

#endif /* TSCRSTEP_H */
