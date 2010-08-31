/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @internalTechnology 
*/

#include "tsifnotifierteststep.h"
#include "tsifsuitedefs.h"
#include <usif/sif/sifcommon.h>
#include <usif/sif/sif.h>
#include <scs/cleanuputils.h>
#include <ct/rcpointerarray.h>

using namespace Usif;

_LIT(KOperationCount, "OperationCount");
_LIT(KOperation, "Operation");
_LIT(KGlobalCompId, "GlobalCompId");
_LIT(KComponentName, "ComponentName");
_LIT(KComponentIcon, "ComponentIcon");
_LIT(KAppNameCount, "AppNameCount");
_LIT(KAppName, "AppName");
_LIT(KAppIconCount, "AppIconCount");
_LIT(KAppIcon, "AppIcon");
_LIT(KComponentSize, "ComponentSize");
_LIT(KIconPath, "IconPath");
_LIT(KSoftwareType, "SoftwareType");
_LIT(KErrMsg, "ErrMsg");
_LIT(KErrMsgDetails, "ErrMsgDetails");
_LIT(KErrCategory, "ErrCategory");
_LIT(KErrCode, "ErrCode");
_LIT(KPhase, "Phase");
_LIT(KSubPhase, "SubPhase");
_LIT(KCurrentProgress, "CurrentProgress");
_LIT(KTotal, "Total");
_LIT(KSubscriberNumber, "SubscriberNumber");
_LIT(KKeyNumber, "KeyNumber");
_LIT (KSubscribeKey, "SubscribeKey");
_LIT (KSubscribeForProgress, "SubscribeForProgress");
_LIT (KEndData, "EndData");
_LIT (KProgressData, "ProgressData");
_LIT (KDelay, "Delay");
_LIT (KTotalStartCount, "StartCount");
_LIT (KTotalProgressCount, "ProgressCount");
_LIT (KTotalEndCount, "EndCount");
_LIT (KSubscribeShutdownTimer, "SubscribeShutdownTimer");
_LIT (KPublisherDelay, "PublisherDelay");
_LIT (KPrintDetails, "PrintDetails");
CSifSubscribeTestStep::~CSifSubscribeTestStep()
/**
* Destructor
*/
    {
    INFO_PRINTF1(_L("Cleanup in CSifSubscribeTestStep::~CSifSubscribeTestStep()"));
    }

CSifSubscribeTestStep::CSifSubscribeTestStep()
/**
* Constructor
*/
    {
    SetTestStepName(KSifSubscribeTestStep);
    }

void CSifSubscribeTestStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {
    INFO_PRINTF1(_L("I am in CSifSubscribeTestStep::ImplTestStepPreambleL()."));
    iSched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iSched);
    SetTestStepResult(EPass);
    }


void CSifSubscribeTestStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
    {   
    //Get the  Subscriber number
    GetIntFromConfig(ConfigSection(),KSubscriberNumber, iSubscriberNum);
    
    // Delay the Startup ?
    TInt delay(0);
    //Get the  delay value if present
    GetIntFromConfig(ConfigSection(),KDelay, delay);
    if(delay !=0)
        {
        User::After(delay * 1000000);
        }
    
    
    iNotifier = CSifOperationsNotifier::NewL(*this);
    TInt shutdownTimer(0);
    GetIntFromConfig(ConfigSection(), KSubscribeShutdownTimer, shutdownTimer);
    if(shutdownTimer ==0)
        {
        shutdownTimer = 10; //default value
        }
    iTimer = CStopTimer::NewL(iNotifier, shutdownTimer* 1000000);
    CActiveScheduler::Start();

    //Verify Counts
    TInt startCount(0);
    GetIntFromConfig(ConfigSection(), KTotalStartCount, startCount);
    
    if(startCount != iStartMessageCounter)
        {
        INFO_PRINTF1(_L("Total number of start handler notifications received does not match expected value"));
        SetTestStepResult(EFail);
        }

    TInt progressCount(0);
    GetIntFromConfig(ConfigSection(), KTotalProgressCount, progressCount);
    
    if(progressCount != iProgressMessageCounter)
        {
        INFO_PRINTF1(_L("Total number of progress handler notifications received does not match expected value"));
        SetTestStepResult(EFail);
        }
    
    TInt endCount(0);
    GetIntFromConfig(ConfigSection(), KTotalEndCount, endCount);
    
    if(endCount != iEndMessageCounter)
        {
        INFO_PRINTF1(_L("Total number of end handler notifications received does not match expected value"));
        SetTestStepResult(EFail);
        }    
    
    }

void CSifSubscribeTestStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {   
    INFO_PRINTF1(_L("I am in CSifPublishTestStep::ImplTestStepPostambleL()."));
    delete iNotifier;
    delete iSched;
    delete iTimer;
    }

void CSifSubscribeTestStep::StartOperationHandler(TUint aKey, const CSifOperationStartData& aStartData)
    { 
    TBuf<20> Key;
    Key = KKeyNumber;
    GenerateIndexedAttributeNameL(Key, ++iStartMessageCounter);
    TPtrC keyFromIni;
    GetStringFromConfig(ConfigSection(), Key, keyFromIni);
    HBufC* retrievedKeyFromIni = keyFromIni.AllocLC();
    
    //Received Key
    HBufC* receivedKey = HBufC::NewLC(20);
    receivedKey->Des().Format(_L("%d"), aKey );
    
    if(*receivedKey != *retrievedKeyFromIni)
        {
        INFO_PRINTF1(_L("Key Mismatch !") );
        SetTestStepResult(EFail);
        }
    else
        {
        INFO_PRINTF1(_L("Received Correct Key!") );
        }
    
    
    //Subscribe to key ?
    TBuf<20> SubscribeKey;
    SubscribeKey = KSubscribeKey;
    GenerateIndexedAttributeNameL(SubscribeKey, ++iSubscribeCounter);
    
    TPtrC SubscribeKeyFromIni;
    if(GetStringFromConfig(ConfigSection(),SubscribeKey, SubscribeKeyFromIni))
        {
        HBufC* retrievedSubscribeKeyFromIni = SubscribeKeyFromIni.AllocLC();
        
        
        if(*retrievedSubscribeKeyFromIni == *receivedKey)
            {
            INFO_PRINTF3(_L("Subscribing to Key %d from Subscriber%d ."), aKey, iSubscriberNum);
            
            TBuf<35> progressNotifier;
            progressNotifier = KSubscribeForProgress;
            GenerateIndexedAttributeNameL(progressNotifier, iSubscribeCounter);
            
            TBool progressNotifierVal;
            GetBoolFromConfig(ConfigSection(),progressNotifier, progressNotifierVal);
            iNotifier->SubscribeL(aKey, progressNotifierVal);
            }
 
        CleanupStack::PopAndDestroy(retrievedSubscribeKeyFromIni);
        }   
    
    CleanupStack::PopAndDestroy(2, retrievedKeyFromIni);
    //Global Component Id check
    TBuf<20> GlobalId;
    GlobalId = KGlobalCompId;
    GenerateIndexedAttributeNameL(GlobalId, iStartMessageCounter);
    
    TPtrC globalIdFromIni;
    GetStringFromConfig(ConfigSection(),GlobalId, globalIdFromIni);
    
    HBufC* retrievedGlobalIdFromIni = globalIdFromIni.AllocLC();
    
    
    if(*retrievedGlobalIdFromIni != aStartData.GlobalComponentId())
        {       
        INFO_PRINTF1(_L("Global Component Id mismatch !") );
        SetTestStepResult(EFail); 
        }
    else
        {
        INFO_PRINTF1(_L("Received Correct Global Component Id !") );     
        }   
    CleanupStack::PopAndDestroy(retrievedGlobalIdFromIni);
    
    //Print received data ?
    TBool print;
    if(GetBoolFromConfig(ConfigSection(),KPrintDetails, print))
        {
        INFO_PRINTF2(_L("Global Component Id : %S"),  &aStartData.GlobalComponentId());
        INFO_PRINTF2(_L("Component Name: %S"),  &aStartData.ComponentName());
        
        for(TInt i=0; i<aStartData.ApplicationNames().Count(); ++i)
            {
            INFO_PRINTF3(_L("Application name %d : %S"), i,  aStartData.ApplicationNames()[i]);
            }
        for(TInt i=0; i<aStartData.ApplicationIcons().Count(); ++i)
            {
            INFO_PRINTF3(_L("Application icons %d : %S"), i,  aStartData.ApplicationIcons()[i]);
            }       
        INFO_PRINTF2(_L("Component Size : %d"),  aStartData.ComponentSize());
        INFO_PRINTF2(_L("Icon Path : %S"),  &aStartData.IconPath());
        INFO_PRINTF2(_L("Component Icon : %S"),  &aStartData.ComponentIcon());
        INFO_PRINTF2(_L("Software Type: %S"),  &aStartData.SoftwareType());
        INFO_PRINTF2(_L("Operation Phase : %d"), (TInt)aStartData.OperationPhase());
        
        }
    }


void CSifSubscribeTestStep::EndOperationHandler(const CSifOperationEndData& aEndData)
    {
    TBuf<20> endData;
    endData = KEndData;
    GenerateIndexedAttributeNameL(endData, ++iEndMessageCounter);
    
    TPtrC endDataFromIni;
    if(GetStringFromConfig(ConfigSection(),endData, endDataFromIni))
        {
        HBufC* retrievedEndDataFromIni = endDataFromIni.AllocLC();
        
        if(*retrievedEndDataFromIni != aEndData.GlobalComponentId())
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(_L("End Data Mismatch !") );
            }
        else
            {
            INFO_PRINTF1(_L("Correct end data received !") );
            }     
        CleanupStack::PopAndDestroy(retrievedEndDataFromIni);
        }
    else
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(_L("End Data Mismatch !") );
        }
    
    //Print received data ?
    TBool print;
    if(GetBoolFromConfig(ConfigSection(),KPrintDetails, print))
        {
        INFO_PRINTF2(_L("Global Component Id : %S"),  &aEndData.GlobalComponentId());
        INFO_PRINTF2(_L("Error Category : %d"),  (TInt)aEndData.ErrorCategory());
        INFO_PRINTF2(_L("Error code : %d"),  aEndData.ErrorCode());
        INFO_PRINTF2(_L("Error message : %S"),  &aEndData.ErrorMessage());
        INFO_PRINTF2(_L("Error message details : %S"),  &aEndData.ErrorMessageDetails());
        }
    }

void CSifSubscribeTestStep::ProgressOperationHandler(const CSifOperationProgressData& aProgressData)
    {
    TBuf<20> progressData;
    progressData = KProgressData;
    GenerateIndexedAttributeNameL(progressData, ++iProgressMessageCounter);
    
    TPtrC progressDataFromIni;
    if(GetStringFromConfig(ConfigSection(),progressData, progressDataFromIni))
        {
        HBufC* retrievedprogressDataFromIni = progressDataFromIni.AllocLC();
        
        if(*retrievedprogressDataFromIni != aProgressData.GlobalComponentId())
            {
            SetTestStepResult(EFail);
            INFO_PRINTF1(_L("Progress Data Mismatch !") );
            }
        else
            {
            INFO_PRINTF1(_L("Correct progress data received !") );
            }
        
        CleanupStack::PopAndDestroy(retrievedprogressDataFromIni);       
        }
    else
        {
        SetTestStepResult(EFail);
        INFO_PRINTF1(_L("Progress Data Mismatch !") );
        }   
    
    //Print received data ?
    TBool print;
    if(GetBoolFromConfig(ConfigSection(),KPrintDetails, print))
        {
        INFO_PRINTF2(_L("Global Component Id : %S"),  &aProgressData.GlobalComponentId());
        INFO_PRINTF2(_L("Phase : %d"),  (TInt)aProgressData.Phase());
        INFO_PRINTF2(_L("Sub Phase : %d"),  (TInt)aProgressData.SubPhase());
        INFO_PRINTF2(_L("Current Progress : %d"),  aProgressData.CurrentProgress());
        INFO_PRINTF2(_L("Total Value : %d"),  aProgressData.Total());
        
        }
    }

void CSifSubscribeTestStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
    {
    const TInt MAX_INT_STR_LEN = 8;
    TBuf<MAX_INT_STR_LEN> integerAppendStr;
    integerAppendStr.Format(_L("%d"), aIndex);
    aInitialAttributeName.Append(integerAppendStr);
    }

CSifPublishTestStep::~CSifPublishTestStep()
/**
* Destructor
*/
    {
    INFO_PRINTF1(_L("Cleanup in CSifPublishTestStep::~CSifPublishTestStep()"));
    delete iTimer;
    delete iPublisher;
    delete iSched;
    }

CSifPublishTestStep::CSifPublishTestStep()
/**
* Constructor
*/
    {
    SetTestStepName(KSifPublishTestStep);
    }

void CSifPublishTestStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {
    INFO_PRINTF1(_L("I am in CSifPublishTestStep::ImplTestStepPreambleL()."));
    
    TInt delay(0);
    //Get the  delay value if present
    GetIntFromConfig(ConfigSection(),KDelay, delay);
    if(delay !=0)
        {
        User::After(delay * 1000000);
        }
   
    iSched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iSched);
    iPublisher = CPublishSifOperationInfo::NewL();
    }

void CSifPublishTestStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
    {
    const TInt MAX_INT_STR_LEN = 8;
    TBuf<MAX_INT_STR_LEN> integerAppendStr;
    integerAppendStr.Format(_L("%d"), aIndex);
    aInitialAttributeName.Append(integerAppendStr);
    }

void CSifPublishTestStep::PublishDataL(HBufC* aConfigSection)
    {
    HBufC* configsection;
    if(aConfigSection == NULL)
       configsection = ConfigSection().AllocLC();
    else
       configsection = aConfigSection->AllocLC();
        
	TPtrC op;
	GetStringFromConfig(*configsection,KOperation, op);
    if(op.Compare(_L("start")) == 0)
		{
		TPtrC globalCompId, compName, compIcon, appName, appIcon, iconPath, softwareType;
		TInt appNameCount =0;
		TInt appIconCount =0;
		TInt compSize = 0;
		TInt phase = 0;
		GetStringFromConfig(*configsection,KGlobalCompId, globalCompId);
		GetStringFromConfig(*configsection,KComponentName, compName);
		GetStringFromConfig(*configsection,KComponentIcon, compIcon);
		GetIntFromConfig(*configsection, KAppNameCount, appNameCount);
		RPointerArray<HBufC> appNameArray;
		CleanupResetAndDestroyPushL(appNameArray);
		for (TUint i = 0; i < appNameCount; ++i)
			{
			TPtrC tAppName;
			TBuf<20> appName;
			appName = KAppName;
			GenerateIndexedAttributeNameL(appName, i);
			GetStringFromConfig(*configsection, appName, tAppName);
			HBufC* applicationName = tAppName.AllocLC();
			appNameArray.AppendL(applicationName);
			CleanupStack::Pop(applicationName);
			}
		GetIntFromConfig(*configsection, KAppIconCount, appIconCount);
		RPointerArray<HBufC> appIconArray;
		CleanupResetAndDestroyPushL(appIconArray);
		for (TUint i = 0; i < appIconCount; ++i)
			{
			TPtrC tAppIcon;
			TBuf<20> appIcon;
			appIcon = KAppIcon;
			GenerateIndexedAttributeNameL(appIcon, i);
			GetStringFromConfig(*configsection, appIcon, tAppIcon);
			HBufC* applicationIcon = tAppIcon.AllocLC();
			appIconArray.AppendL(applicationIcon);
			CleanupStack::Pop(applicationIcon);
			}
       
		GetIntFromConfig(*configsection, KComponentSize, compSize);
		GetStringFromConfig(*configsection, KIconPath, iconPath);
		GetStringFromConfig(*configsection, KSoftwareType, softwareType);
		GetIntFromConfig(*configsection, KPhase, phase);

		TSifOperationPhase enumPhase = static_cast<TSifOperationPhase>(phase);
        CSifOperationStartData* startdata = CSifOperationStartData::NewLC(globalCompId, compName, appNameArray, appIconArray, compSize, iconPath, compIcon, softwareType, enumPhase);
        iPublisher->PublishStartL(*startdata);   
        CleanupStack::PopAndDestroy(3, &appNameArray);
        }
    else if(op.Compare(_L("progress")) == 0)
        {
		TInt phase=0, subPhase=0, curProgress=0, total=0;
		TPtrC globalCompIdProgress;
		GetStringFromConfig(*configsection,KGlobalCompId, globalCompIdProgress);
		GetIntFromConfig(*configsection, KPhase, phase);
		GetIntFromConfig(*configsection, KSubPhase, subPhase);
		GetIntFromConfig(*configsection, KCurrentProgress, curProgress);
		GetIntFromConfig(*configsection, KTotal, total);

		TSifOperationPhase enumPhase = static_cast<TSifOperationPhase>(phase);
		TSifOperationSubPhase enumSubPhase = static_cast<TSifOperationSubPhase>(subPhase);
        CSifOperationProgressData* progressdata = CSifOperationProgressData::NewLC(globalCompIdProgress, enumPhase, enumSubPhase, curProgress, total);
        iPublisher->PublishProgressL(*progressdata);    
        CleanupStack::PopAndDestroy(progressdata);
        }
    else if(op.Compare(_L("end")) == 0)
        {
		TPtrC errMsg, errMsgDetails;
		TInt errCat, errCode;
		TPtrC globalCompIdEnd;
		GetStringFromConfig(*configsection,KGlobalCompId, globalCompIdEnd);
		GetStringFromConfig(*configsection, KErrMsg, errMsg);
		GetStringFromConfig(*configsection, KErrMsgDetails, errMsgDetails);
		GetIntFromConfig(*configsection, KErrCategory, errCat);
		GetIntFromConfig(*configsection, KErrCode, errCode);

		TErrorCategory enumErrCat = static_cast<TErrorCategory>(errCat);
		CSifOperationEndData* enddata = CSifOperationEndData::NewLC(globalCompIdEnd, enumErrCat, errCode, errMsg, errMsgDetails);
        iPublisher->PublishCompletionL(*enddata);
        CStopTimer* timer = CStopTimer::NewL(NULL, 5000000);
        CleanupStack::PushL(timer);
        CActiveScheduler::Start();    
        CleanupStack::PopAndDestroy(2, enddata);
        }   
    else if (op.Compare(_L("delay")) == 0)
        {
        TInt delay(0);
        GetIntFromConfig(*configsection, KPublisherDelay, delay);
        User::After(delay*1000000);
        }
        
        
    CleanupStack::PopAndDestroy(configsection);
    }

void CSifPublishTestStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
    {
    RSoftwareInstall sif;
    User::LeaveIfError(sif.Connect());
    CleanupClosePushL(sif);
    
    TInt operationCount = 0;
    TPtrC configSectionName(ConfigSection());
    HBufC* cnfSecName = NULL;
        
    GetIntFromConfig(ConfigSection(), KOperationCount, operationCount);
    for (TUint i = 0; i < operationCount; ++i)
        {
        TBuf<20> configSection = _L("configsection");
        GenerateIndexedAttributeNameL(configSection, i);
        GetStringFromConfig(ConfigSection(),configSection, configSectionName);
        cnfSecName = configSectionName.AllocLC();
        PublishDataL(cnfSecName);
        CleanupStack::PopAndDestroy(cnfSecName);
        }
    CStopTimer* timer = CStopTimer::NewL(NULL, 5000000);
    CleanupStack::PushL(timer);
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy(2, &sif);
    }

void CSifPublishTestStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {   
    delete iPublisher;
    iPublisher=0;
    delete iSched;
    iSched=0;
    }




CStopTimer::CStopTimer(CSifOperationsNotifier* aNotifier) : 
    CTimer(-1),
    iNotifier(aNotifier)
    {
    CActiveScheduler::Add(this);
    }

CStopTimer::~CStopTimer()
    {
    Cancel();
    }

void CStopTimer::ConstructL(TInt aSeconds)
    {
    CTimer::ConstructL();
    iSeconds = aSeconds;
    Start();
    }

void CStopTimer::Start()
    {
    After(iSeconds);
    }
CStopTimer* CStopTimer::NewL(CSifOperationsNotifier* aNotifier, TInt aSeconds)
    {
    CStopTimer* self = new(ELeave) CStopTimer(aNotifier);
    CleanupStack::PushL(self);
    self->ConstructL(aSeconds);
    CleanupStack::Pop(self);
    return self;
    }

void CStopTimer::RunL()
    {
    if(iNotifier)
        {
        //To improve coverage, call cancel subscribe on a non existent key.
        iNotifier->CancelSubscribeL(123);
        
        //cancel all subscriptions.
        iNotifier->CancelSubscribeL();           
        }
    CActiveScheduler::Stop();
    }


TInt CStopTimer::RunError(TInt aError)
    {
    (void)aError;
    return KErrNone;
    }
