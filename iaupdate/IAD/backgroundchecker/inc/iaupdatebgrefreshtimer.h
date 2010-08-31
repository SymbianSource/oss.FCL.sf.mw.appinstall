/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   The main header file of background timer
*
*/


#ifndef IAUPDATEBGREFRESHTIMER_H
#define IAUPDATEBGREFRESHTIMER_H

// INCLUDE FILES 

#include <cenrepnotifyhandler.h>
#include <centralrepository.h>
#include <iaupdateobserver.h>
#include <barsc.h>


#include "iaupdatebginternalfilehandler.h"
#include "iaupdatebgcontrollerfile.h"
#include "iaupdatebgsoftnotification.h"
#include "iaupdatebgcheckermode.h"

#include "iaupdatebgnotifyhandler.h"

// FORWARD DECLARATIONS
class CIAUpdate;
class CIAUpdateParameters;
class CInternalFileHandler;
class CIdleObserver;

// CLASS DECLARATION
/**
 * Main class for background timer
 * 
 * @lib 
 * @since 
 */

class CIAUpdateBGTimer : public CTimer, 
                 public MIAUpdateObserver, 
                 public MCenRepNotifyHandlerCallback, 
                 public MIAUpdateBGSoftNotificationCallBack,
                 public MIAUpdateBGNotifyObserver
            
    {
    public:
   
         static CIAUpdateBGTimer* NewL();  
        
         virtual ~CIAUpdateBGTimer();   
         
         void StartProcessL();

    public://MCenRepNotifyHandlerCallback

        void HandleNotifyGeneric( TUint32 aId );
        
        
    public://MSoftNotificationCallBack

        void SoftNotificationCallBack( TBool aIsAccepted );

        TIAUpdateBGMode CurrentMode();

    public://MIAUpdateObserver

        void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );

        void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult );

        void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );
   
    public: //MReminderTimerCallBack

        void ReminderTimerCallBack();
    
    private: // From MIAUpdateBGNotifyObserver   

         void HandleIndicatorRemoveL();
        
    private:
    
        CIAUpdateBGTimer();

        void ConstructL();  
        
        void RunL();
  
        void DoCancel();

        TInt RunError(TInt aError);
        
        void DoUpdatesCheckingL( const TBool& aFromServer );
        
        TTimeIntervalMicroSeconds32 ConvertToMicroseconds( /*TTimeIntervalDays*/ TTimeIntervalMinutes aInterval );
        
        TTime GetLastRefreshTimeL();
        
        TInt SetLastRefreshTime();
       
        TInt CheckingFrequencyInMinutesL();

        TInt UpdateAvailableReminderIntervalInMinutesL();

        TInt FirstTimeDialogDelayInMinutesL();

        TInt FirstTimeDialogReminderIntervalInMinutesL();

        TBool IsFirstTimeDialogDisabledL();
        
        void StartL( TTimeIntervalMinutes aWaitTime );
        
        TTimeIntervalMinutes TimeIntervalFromNextRefreshL();
        
        TTimeIntervalMinutes TimeIntervalFromNextShowOfNewFeatureDialogL();
        
        TBool IsAgreementAcceptedL();
       
        void HandlerFirstTimeL();
        
        TTime GetLastTimeShowNewFeatureDialogL();

        TInt SetLastTimeShowNewFeatureDialogL( TTime aTime );
       
        TBool GetUserDecisionL();
 
        TInt SetUserDecisionL( TBool aDecision );
        
        TIAUpdateBGMode ModeL();

        TInt SetModeL( TIAUpdateBGMode aMode );
        
        TInt NrOfIndicatorEntries();

        TInt SetNrOfIndicatorEntriesL( TInt aEntries );
        
        TInt RetryTimesL();

        TInt SetRetryTimesL( TInt aRetry );
          
        void StartIaupdateL() const;
        
        void LaunchNotificationL( const int aNrOfUpdates );
        
        TBool IsAutoUpdateDisabledL();
        
        void ListenAutoUpdateSettingsL();
        
        void StopListeningAutoUpdateSettingsL();
        
        void DoHandleNotifyGenericL( TUint32 aId );
        
        void DoCheckUpdatesCompleteL( TInt aErrorCode, TInt aAvailableUpdates );
        
        void DoSoftNotificationCallBackL( TBool aIsAccepted );
        
        void StartUpdatesCheckingL();    
        
        TBool IAUpdateEnabledL() const;
        
        void Shutdown();
            
    private:
        // Data       
        CIAUpdate* iUpdate; 
        CIAUpdateParameters* iParameters; 
        CRepository* iIAUpdateCRSession; 
        CCenRepNotifyHandler* iNotifyHandler; 
        TInt iRuns;
        TInt iLastRunLength;
        CIAUpdateBGControllerFile* iControllerFile;
        CIAUpdateBGInternalFileHandler* iInternalFile;
        TIAUpdateBGMode iMode;
        CIAUpdateBGSoftNotification* iSoftNotification;
        
        CIAUpdateBGNotifyHandler* iIndicatorNotifyHandler;
    };        

#endif //IAUPDATEBGREFRESHTIMER_H
