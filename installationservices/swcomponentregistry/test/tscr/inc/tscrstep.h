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
	
	// Read component names from config file and add a new component
	Usif::TComponentId AddNonLocalisableComponentL();
	
	// Read filter properties from config file
	Usif::CComponentFilter* ReadComponentFilterFromConfigLC();
	void ReadFilterPropertiesL(Usif::CComponentFilter* aFilter, TInt aPropertiesCount);
	void StartTimer();
	
protected:
	CScrTestServer& iParent;
	Usif::RSoftwareComponentRegistry iScrSession;

private:
	TInt ReadSharedComponentIdL(TInt aOffset);
	void StopTimerAndPrintResultL();
	void PrintPerformanceLog(TTime aTime);

protected:
	// For plugin management test cases, the time is measured externally because they are performed via another accessor server (scr accessor).
	// The actual test time is measured on the scr accessor server side and returned to the test code.
	TInt iTimeMeasuredExternally;
	
private:
	TBool iIsPerformanceTest;
	TTime iStartTime;
	};

#endif /* TSCRSTEP_H */
