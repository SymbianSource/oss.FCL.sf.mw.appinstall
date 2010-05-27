/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef APPREGINFOAPPARCSTEPS_H_
#define APPREGINFOAPPARCSTEPS_H_

#include <scs/oomteststep.h>
#include <usif/scr/scr.h>

using namespace Usif;

class CScrApparcTestServer;

_LIT(KScrApplicationRegistrationViewSubsessionStep,"ScrGetApplicationRegistrationView");
_LIT(KScrMultipleSubsessionsForAppRegistryViewStep,"ScrMultipleSubsessionsForAppRegistryView");

class CScrGetApplicationRegistrationViewSubsessionStep : public COomTestStep
/**
    TEF test step which exercises the SCR GetApplicationRegistrationView interface
 */
    {
public:
    CScrGetApplicationRegistrationViewSubsessionStep();
    ~CScrGetApplicationRegistrationViewSubsessionStep();
    
    void MarkAsPerformanceStep();
    
protected:    
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();   
    
    // Utility Functions
    void PrintErrorL(const TDesC& aMsg, TInt aErrNum,...);
    
    static void GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex);
    TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid);
    // For reading data from ini file
    Usif::CApplicationRegistrationData* GetAppRegInfoFromConfigLC(HBufC* aConfigSection = NULL);
    void GetAppOwnedFilesL(RPointerArray<HBufC>& aOwnedFileArray, HBufC* aConfigSection = NULL);
    void GetAppServiceInfoL(RPointerArray<Usif::CServiceInfo>& aServiceInfoArray, HBufC* aConfigSection = NULL);
    void GetAppLocalizableInfoL(RPointerArray<Usif::CLocalizableAppInfo>& aLocalizableAppInfoArray, HBufC* aConfigSection = NULL);
    void GetAppOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aAppOpaqueDataInfoArray, HBufC* aConfigSection = NULL);
    void GetServiceOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aServiceOpaqueDataInfoArray, TInt aServiceOpaqueDataInfoCount, TInt aStartingIndex, HBufC* aConfigSection = NULL);
    TBool Get64BitIntegerFromConfigL(const TDesC& aConfigKeyName, TInt64& aRetVal,HBufC* aConfigsection = NULL);
    CPropertyEntry* GetPropertyFromConfigLC(TBool aIsSingle, TInt aIndex, TBool aSupportLocalized, HBufC* aConfigsection = NULL);
    void GetAppPropertiesL(RPointerArray<Usif::CPropertyEntry>& aAppPropertyArray, TBool aSupportLocalized, HBufC* aConfigSection = NULL );
    void GetAppUidL(TUid& aAppUid, HBufC* aConfigsection = NULL);
        
    // For comparing data 
    TBool CompareApplicationRegistrationDataL(Usif::CApplicationRegistrationData *aActualData, Usif::CApplicationRegistrationData *aExpectedData);
    TBool CompareFileOwnershipInfo(RPointerArray<HBufC> aActualData, RPointerArray<HBufC> aExpectedData);
    TBool CompareServiceInfoL(RPointerArray<Usif::CServiceInfo> aActualData,  RPointerArray<Usif::CServiceInfo> aExpectedData);
    TBool CompareDataType(RPointerArray<Usif::CDataType> aActualData,  RPointerArray<Usif::CDataType> aExpectedData);
    TBool CompareLocalizableAppInfo(RPointerArray<Usif::CLocalizableAppInfo> aActualData,  RPointerArray<Usif::CLocalizableAppInfo> aExpectedData);
    TBool CompareCaptionandIconInfo(const CCaptionAndIconInfo* aActualData, const CCaptionAndIconInfo* aExpectedData);
    TBool CompareViewData(RPointerArray<Usif::CAppViewData> aActualData,  RPointerArray<Usif::CAppViewData> aExpectedData);
    TBool CompareOpaqueDataL(RPointerArray<Usif::COpaqueData> aActualData,  RPointerArray<Usif::COpaqueData> aExpectedData);
    
    void GetApplicationRegistrationDataFromConfigL(RPointerArray<Usif::CApplicationRegistrationData>& aEntries);
    
    void StartTimer();
    void StopTimerAndPrintResultL();
    void PrintPerformanceLog(TTime aTime);
    
protected:    
    Usif::RSoftwareComponentRegistry iScrSession;
    TBool iIsPerformanceTest;
    
private:    
    TTime iStartTime;
    };


class CScrMultipleSubsessionsForAppRegistryViewStep : public CScrGetApplicationRegistrationViewSubsessionStep
/**
    TEF test step which exercises the SCR CScrAddConcurrentApplicationRegistrationViewEntryStep interface
 */
    {
public:
    CScrMultipleSubsessionsForAppRegistryViewStep();
    ~CScrMultipleSubsessionsForAppRegistryViewStep();
    
protected:      
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

#endif /* APPREGINFOAPPARCSTEPS_H_ */


