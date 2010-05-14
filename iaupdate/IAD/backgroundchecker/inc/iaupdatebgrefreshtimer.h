/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "iaupdatebgremindertimer.h"
#include "iaupdatebgcontrollerfile.h"
#include "iaupdatebgsoftnotification.h"
#include "iaupdatebgcheckermode.h"

// FORWARD DECLARATIONS
class CIAUpdate;
class CIAUpdateParameters;
// class CAknSoftNotifier;
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
                 public MIAUpdateBGReminderTimerCallBack
            
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
        
        TBool IsAskedAlreadyL();
        
        void HandlerFirstTimeL();
        
        TTime GetLastTimeShowNewFeatureDialogL();

        TInt SetLastTimeShowNewFeatureDialogL( TTime aTime );
       
        TBool GetUserDecisionL();
 
        TInt SetUserDecisionL( TBool aDecision );
        
        TBool ReminderOnL();

        TInt SetReminderL( TBool aOn );
        
        TTime NextRemindTimeL();

        TInt SetNextRemindTimeL( TTime aTime );
        
        TIAUpdateBGMode ModeL();

        TInt SetModeL( TIAUpdateBGMode aMode );
        
        TInt RetryTimesL();

        TInt SetRetryTimesL( TInt aRetry );
          
        // HBufC* ReadResourceLC( TInt aResourceId );
        
        void StartIaupdateL() const;
        
        // void LaunchSoftNotificationL( const TInt& aResourceId, const TInt& SK1, const TInt& SK2 );
        
        void LaunchNotificationL( const int aNrOfUpdates );
        
        TBool IsAutoUpdateDisabledL();
        
        void ListenAutoUpdateSettingsL();
        
        void StopListeningAutoUpdateSettingsL();
        
        void DoHandleNotifyGenericL( TUint32 aId );
        
        void DoCheckUpdatesCompleteL( TInt aErrorCode, TInt aAvailableUpdates );
        
        void DoSoftNotificationCallBackL( TBool aIsAccepted );
        
        void DoReminderTimerCallBackL();
        
        void StartUpdatesCheckingL();    
        
        // HBufC8* LoadFileLC(const TDesC& aFile);
        
        TInt GetPrivatePathL( TFileName& aPath );
        
        TInt SetSessionPrivatePathL( RFs& aFs, const TDesC& aPath ) const;
        
        void SetPrivateDriveL( RFs& aFs, const TDesC& aFileName ) const;
        
        TBool IAUpdateEnabledL() const;
        
        void Shutdown();
            
    private:
        // Data       
        CIAUpdate* iUpdate; 
        CIAUpdateParameters* iParameters; 
        CIAUpdateBGReminderTimer* iReminderTimer;
        CRepository* iIAUpdateCRSession; 
        CCenRepNotifyHandler* iNotifyHandler; 
        TInt iRuns;
        TInt iLastRunLength;
        CIAUpdateBGControllerFile* iControllerFile;
        CIAUpdateBGInternalFileHandler* iInternalFile;
        TIAUpdateBGMode iMode;
        CIAUpdateBGSoftNotification* iSoftNotification;
        // RResourceFile iResourceFile;
        RFs iFs;
    };        

#endif //IAUPDATEBGREFRESHTIMER_H
