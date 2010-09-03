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
* Description:   Implementation of background checker
*
*/


#include <e32std.h>
//IAD API
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <w32std.h>
#include <apgwgnam.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <bautils.h>
#include <apgtask.h>
#include <sysversioninfo.h>  //sysversioninfo
#include <featurecontrol.h>

#include <hb/hbcore/hbtextresolversymbian.h>
#include "iaupdateprivatecrkeys.h"
#include "iaupdate.hrh"

#include "iaupdatebgrefreshtimer.h"
#include "iaupdatebglogger.h"
#include "iaupdatebgfirsttimehandler.h"
#include "iaupdatebginternalfilehandler.h"
#include "iaupdatebgconst.h"

//MACROS
_LIT8( KRefreshFromNetworkDenied, "1" );
_LIT(KIAUpdateLauncherExe, "iaupdatelauncher.exe" );

// translator
_LIT(KPath, "z:/resource/qt/translations/"); 
_LIT(KFileName, "swupdate_");

// loc texts
_LIT(KTitleFirstTime, "txt_software_dpophead_update_checking");
_LIT(KTitleOneUpdate, "txt_software_dpophead_update_available");
_LIT(KTitleSeveralUpdates, "txt_software_dpophead_updates_available");
_LIT(KSecondRow, "txt_software_dpopinfo_tap_to_view");


//CONSTANTS
const TUint KIADUpdateLauncherUid( 0x2001FE2F );

// ----------------------------------------------------------
// CIAUpdateBGTimer::NewL()
// ----------------------------------------------------------
CIAUpdateBGTimer* CIAUpdateBGTimer::NewL()
    {
    CIAUpdateBGTimer* self = new(ELeave)CIAUpdateBGTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    

// ----------------------------------------------------------
// CIAUpdateBGTimer::ConstructL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
  
    iUpdate = NULL;
    iParameters = NULL;
    
    iIAUpdateCRSession = NULL; 
    iNotifyHandler = NULL;

    iControllerFile = CIAUpdateBGControllerFile::NewL();
    
    iInternalFile = CIAUpdateBGInternalFileHandler::NewL();
    
    iSoftNotification = CIAUpdateBGSoftNotification::NewL( this ); 
    
    iIndicatorNotifyHandler = CIAUpdateBGNotifyHandler::NewL();
    
    // initialize localisation text loader
    TBool res = HbTextResolverSymbian::Init(KFileName, KPath);
    if ( res != KErrNone )
        {
        // nothing to do
        }
    }
    
// ----------------------------------------------------------
// CIAUpdateBGTimer::~CIAUpdateBGTimer()
// ----------------------------------------------------------
CIAUpdateBGTimer::~CIAUpdateBGTimer()
    {
    Cancel();
    delete iUpdate; 
    delete iParameters;

    if ( iNotifyHandler ) 
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        }

    delete iIAUpdateCRSession; 
    delete iControllerFile;
    delete iInternalFile;
    delete iSoftNotification;
    delete iIndicatorNotifyHandler;
    
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::StartProcessL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::StartProcessL()
    {
    FLOG("[bgchecker] StartProcessL");
    
    // start listening indicator remove
    iIndicatorNotifyHandler->StartListeningL( this );
    
    
    iMode = ModeL();
    
    switch ( iMode )
        {
        case EFirstTimeMode:
            {
            // while not released keep in sleep mode
            int test = 0;
            if ( test == 0 )
                {
                ListenAutoUpdateSettingsL();
                iMode = ESleepMode;
                SetModeL( iMode );
                return;
                }
        
            if ( IsFirstTimeDialogDisabledL() )
                {
                //this should only happen in testing
                FLOG("[bgchecker] StartProcessL Firs time dialog is disabled, this should only happen in testing");

                //subscribe to automatic checking settings
                //subscribe to cenrep key for automatic checking for wake up from sleep mode
                if (iIAUpdateCRSession == NULL )
                    {
                    iIAUpdateCRSession = CRepository::NewL( KCRUidIAUpdateSettings ); 
                    }
                if (iNotifyHandler == NULL )
                    {
                    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iIAUpdateCRSession ); 
                    }
                FLOG("[bgchecker] StartProcessL go to sleep mode directly");    
                iNotifyHandler->StartListeningL();  


                iMode = ESleepMode;
                SetModeL( iMode );
                return;
                }

                        
            FLOG("[bgchecker] StartProcessL EFirstTimeMode");
            
            //Wait for some days before activate the first time mode
            TTimeIntervalMinutes timetowait =
                    TimeIntervalFromNextShowOfNewFeatureDialogL();
                FLOG_NUM( " time to wait = %d", timetowait.Int() );
            if (timetowait.Int() <= 0)
                {
                    FLOG("[bgchecker] StartProcessL now");
                //start immediately
                //if agreement is not accepted but auto update is enabled,
                //jump to normal mode
                if (!IsAgreementAcceptedL() && !IsAutoUpdateDisabledL())
                    {
                    iMode = ENormalMode;
                    SetModeL(iMode);
                        FLOG("[bgchecker] Agreement is NOT accepted but AutoUpdate is enabled, Go to Normal mode");
                    StartUpdatesCheckingL();
                    return;
                    }

                TTimeIntervalMinutes timetowait =
                        TimeIntervalFromNextRefreshL();

                if (IsAgreementAcceptedL() && !IsAutoUpdateDisabledL()
                        && timetowait.Int() <= 0 )
                    {
                    //in this case, background checker will connect to server for refreshing
                    //put one minute delay here.
                    StartL(StartAfterOneMin );
                    }
                else
                    {
                    StartL( StartNow );
                    }
                }
            else
                {
                FLOG("[bgchecker] StartProcessL later");
                StartL( timetowait );   
                }    
                                
                                                  
            break;
            }

        case EFirstTimeMode2:
            {
            FLOG("[bgchecker] StartProcessL EFirstTimeMode 2");
            //if user accepted the disclaimer already and reboot the phone, 
            //this mode could go to network immediately after boot
            //We put one min delay here to wait for network ready.
            StartL(StartAfterOneMin);
            break;
            }
            
        case ESleepMode:
            {
            FLOG("[bgchecker] StartProcessL EFirstTimeMode 4");
            //subscribe to automatic checking settings
            //subscribe to cenrep key for automatic checking for wake up from sleep mode
            if (iIAUpdateCRSession == NULL )
                {
                iIAUpdateCRSession = CRepository::NewL( KCRUidIAUpdateSettings ); 
                }
            if (iNotifyHandler == NULL )
                {
                iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iIAUpdateCRSession ); 
                }
                    
            iNotifyHandler->StartListeningL();  
            

            if ( !IsAgreementAcceptedL() )
                {
                FLOG("[bgchecker] EFirstTimeMode4 ShowIndicator");
                // Set number of updates; zero for first case
                iSoftNotification->SetNrOfUpdates( 0 );
                
                // do not show indicator when testing
                int test = 0;
                if ( test == 1)
                  iSoftNotification->ShowIndicatorL();
                
                }
            
            
            break;
            }

        case ENormalMode:
        case ERetryMode:
            {
            
            // Should indicator be shown ?
            TInt entries = NrOfIndicatorEntries();
            if  ( entries )
                {
                iSoftNotification->SetNrOfUpdates( entries );
                iSoftNotification->ShowIndicatorL();
                }
            
            StartUpdatesCheckingL();
            
            break;
            }
        
        default:
            break;
        }

    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::IsAgreementAcceptedL()
// ----------------------------------------------------------
TBool CIAUpdateBGTimer::IsAgreementAcceptedL()
    {
    CIAUpdateBGFirstTimeHandler* fthandler  = CIAUpdateBGFirstTimeHandler::NewL();
    CleanupStack::PushL( fthandler );
    TBool result = fthandler->AgreementAcceptedL();
    
    CleanupStack::PopAndDestroy( fthandler );
    
    return result;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::StartUpdatesCheckingL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::StartUpdatesCheckingL()  
    {       
    // check what is current setting from user.
    //if user disable the auto checking, we simply start the cenrep listener
    if ( IsAutoUpdateDisabledL() )
        {
        ListenAutoUpdateSettingsL();
        iMode = ESleepMode;
        SetModeL( iMode );
        return;
        }
     
    FLOG("[bgchecker] checking frequency is set by user");
    
    TTimeIntervalMinutes timetowait = TimeIntervalFromNextRefreshL(); 
    FLOG_NUM(" Next checking update starts after %d minuts", timetowait.Int() );
    
    if ( timetowait.Int() <= 0 )   
        {
        FLOG( "Start checking update now.");
        
        //this supposes to start immediately but we put 1 min delay here to wait for network registration
        StartL( StartAfterOneMin );
        
        //don't turn on the reminder since the refresh is started right now
        //if there is new update, the dialog will pop up. User can start remind timer
        //via the dialog
        }
    else
        {
            FLOG_NUM( "Start checking update after %d minutes", timetowait.Int());
            StartL( timetowait );
            
         }
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::DoHandleNotifyGeneric()
// ----------------------------------------------------------
void CIAUpdateBGTimer::DoHandleNotifyGenericL( TUint32 aId )
    {
    FLOG("[bgchecker] HandleNotifyGeneric");
    if ( !IAUpdateEnabledL() )
        {
        Shutdown();
        return;
        }
    switch ( iMode )
        {
        case ESleepMode:
            {
            FLOG_NUM("[bgchecker] HandleNotifyGeneric ESleepMode with Id = %d", aId );

            if ( aId == KIAUpdateAutoUpdateCheck )
                {
                if ( IsAutoUpdateDisabledL() ) 
                    {
                    FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Autoupdate is disabled");
                    iMode = ESleepMode;
                    SetModeL( iMode );
                    ListenAutoUpdateSettingsL();
                    }
                else
                    {
                    FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Autoupdate is enabled");
                    //restart the refresh timer according to checking frequency
                    StopListeningAutoUpdateSettingsL();
                    FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Stop listenning");
                    
                    TTimeIntervalMinutes interval( TimeIntervalFromNextRefreshL() );

                    FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Start the timer according to interval counted from last refresh");                                                                           
                    if ( interval.Int() <= 0 )
                        {
                        FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Start now");
                        StartL( StartNow );
                        }
                    else
                        {
                        FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Start later");
                        StartL( interval );
                        }
                    
                    FLOG("[bgchecker] HandleNotifyGeneric ESleepMode: Set Normal mode");               
                    //in normal mode now
                    iMode = ENormalMode;
                    SetModeL( ENormalMode );  
                    
                    if ( iNotifyHandler )
                        {
                        delete iNotifyHandler;
                        iNotifyHandler = NULL;
                        }
                                   
                    if ( iIAUpdateCRSession )
                        {
                        delete iIAUpdateCRSession;
                        iIAUpdateCRSession = NULL;
                        }
                    }
                }
            }
            break;

        default:
            break;
        };        
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::HandleNotifyGeneric()
// ----------------------------------------------------------
void CIAUpdateBGTimer::HandleNotifyGeneric( TUint32 aId ) 
    {
    TRAP_IGNORE( DoHandleNotifyGenericL( aId ) );
    } 


// ----------------------------------------------------------
// CIAUpdateBGTimer::GetLastUpdateTimeL()
// ----------------------------------------------------------
TTime CIAUpdateBGTimer::GetLastRefreshTimeL()
    {    
    iControllerFile->ReadControllerDataL();
    return iControllerFile->RefreshTime();
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::SetLastUpdateTime()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::SetLastRefreshTime()
    {
    iControllerFile->SetCurrentRefreshTime();
    TRAPD( err, iControllerFile->WriteControllerDataL() );
    FLOG_NUM("SetLastRefreshTime err = %d", err );
    return err;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::GetLastTimeShowNewFeatureDialogL()
// ----------------------------------------------------------
TTime CIAUpdateBGTimer::GetLastTimeShowNewFeatureDialogL()
    {
    iInternalFile->ReadControllerDataL();
    return iInternalFile->LastTimeShowNewFeatureDialog();
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::SetTimeShowNewFeatureDialogL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::SetLastTimeShowNewFeatureDialogL( TTime aTime )
    {
    iInternalFile->SetLastTimeShowNewFeatureDialog( aTime );
    TRAPD( err,iInternalFile->WriteControllerDataL() );
    return err;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::ModeL()
// ----------------------------------------------------------
TIAUpdateBGMode CIAUpdateBGTimer::ModeL()
    {
    iInternalFile->ReadControllerDataL();
    return iInternalFile->Mode();
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::SetModeL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::SetModeL( TIAUpdateBGMode aMode )
    {
    iInternalFile->SetMode( aMode  );
    TRAPD( err,iInternalFile->WriteControllerDataL() );
    return err;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::NrOfIndicatorEntries()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::NrOfIndicatorEntries()
    {
    iInternalFile->ReadControllerDataL();
    return iInternalFile->NrOfIndicatorEntries();
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::SetNrOfIndicatorEntriesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::SetNrOfIndicatorEntriesL( TInt aEntries )
    {
    iInternalFile->SetNrOfIndicatorEntries( aEntries );
    TRAPD( err,iInternalFile->WriteControllerDataL() );
    return err;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::RetryTimesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::RetryTimesL()
    {
    iInternalFile->ReadControllerDataL();
    return iInternalFile->RetryTimes();
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::SetRetryTimesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::SetRetryTimesL( TInt aRetry )
    {
    iInternalFile->SetRetryTimes( aRetry  );
    TRAPD( err,iInternalFile->WriteControllerDataL() );
    return err;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::ConvertToMicroseconds()
// ----------------------------------------------------------
TTimeIntervalMicroSeconds32 CIAUpdateBGTimer::ConvertToMicroseconds( TTimeIntervalMinutes aInterval )
    {
    return aInterval.Int()*60*1000*1000;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::DoUpdatesCheckingL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::DoUpdatesCheckingL( const TBool& aUpdateFromServer )
    {   
    FLOG( "DoUpdatesCheckingL update");
    
    if( !iUpdate )
        {
        iUpdate = CIAUpdate::NewL( *this );
        }
    if ( !iParameters )   
        {
        iParameters = CIAUpdateParameters::NewL();
        }
    
    if ( aUpdateFromServer )
        {
        iParameters->SetRefresh( ETrue );
        
        FLOG("Polling from Server!");
        if ( !IsAgreementAcceptedL() )
            {
            FLOG("First time, bigger searching scale");
            //The bigger searching scale will be used until user see the searching result
            // meaning open IAD maiview
            iParameters->SetImportance( CIAUpdateParameters::ECritical | 
                                        CIAUpdateParameters::EMandatory |
                                        CIAUpdateParameters::ERecommended | 
                                        CIAUpdateParameters::ENormal );
            }
        else
            {
            FLOG("Not first time, smaller searching scale");
            iParameters->SetImportance( CIAUpdateParameters::ECritical | CIAUpdateParameters::EMandatory );
        	  }
        }
    else
        {
        FLOG("Use cache, use smaller searching scale!");
        iParameters->SetRefresh( EFalse );
        iParameters->SetImportance( CIAUpdateParameters::ECritical | CIAUpdateParameters::EMandatory );
        }
    
    iUpdate->CheckUpdates( *iParameters );
    //call back function will be called when checking is done
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::DoCheckUpdatesComplete()
// ----------------------------------------------------------
void CIAUpdateBGTimer::DoCheckUpdatesCompleteL( TInt aErrorCode, TInt aAvailableUpdates )
    {
    FLOG_NUM( "CheckUpdatesComplete aErrorCode = %d", aErrorCode );
    FLOG_NUM( "CheckUpdatesComplete aAvailableUpdates = %d", aAvailableUpdates );
    
    //HLa
    int test = 0;
    if ( test == 0 )
        {
        aErrorCode = 0;
        aAvailableUpdates = 0;
        }
    
    if ( !IAUpdateEnabledL() )
        {
        Shutdown();
        return;
        }
    TBool checkupdatefromserver = EFalse;

    if ( iUpdate )
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    
    if ( iParameters )
        {
        checkupdatefromserver = iParameters->Refresh();
        delete iParameters;
        iParameters = NULL;
        }
    
    if ( ModeL() == ERetryMode )
        {
        TInt retry = RetryTimesL();
        //increasing the retry times
        retry++;
        SetRetryTimesL( retry );
        }
    
    FLOG( "CheckUpdatesComplete");
    
    if ( aErrorCode == KErrNone )
        {
        FLOG( "CheckUpdatesComplete 1");
        
        //Set to normal mode
        iMode = ENormalMode;
        SetModeL( ENormalMode );
        //clear the retry times 
        SetRetryTimesL( 0 );
        
        if ( aAvailableUpdates > 0 )
            {             
            FLOG( "CheckUpdatesComplete 4");
    
            LaunchNotificationL( aAvailableUpdates );
            }

            FLOG( "CheckUpdatesComplete 5");
        
        // if update checking is from server, restart the refresh timer 
        // if update checking is from cache, do nothing
        if ( checkupdatefromserver )
            {
            // for firmware changed case, we also refresh after 1 month
            TTimeIntervalMinutes interval( CheckingFrequencyInMinutesL() );

            FLOG_NUM("interval = %d", interval.Int());
            
            FLOG( "CheckUpdatesComplete 6");
            
            //recheck after the interval
            StartL( interval );
            
            FLOG( "CheckUpdatesComplete 7");
            //update the last-checking time in iaupdate private file
            SetLastRefreshTime();
            }
        }
    else 
        {
        FLOG( "CheckUpdatesComplete Complete with error");
        
        iMode = ERetryMode;
        SetModeL( ERetryMode );
        
        TInt retry = RetryTimesL();
        
        if ( retry >= KMaxRetry )
            {
            //if 4 tries failed, stop retrymode and back to normal mode
            //just start a new interval
            iMode = ENormalMode;
            SetModeL( ENormalMode );
            
            FLOG( "CheckUpdatesComplete reach the max retry times");
            if ( checkupdatefromserver )
                {
                TTimeIntervalMinutes interval( CheckingFrequencyInMinutesL() );
                StartL( interval );
                SetLastRefreshTime();
                }
            //clear the retry time
            SetRetryTimesL( 0 ); 
            }
        else
            {
            FLOG( "CheckUpdatesComplete doesn't reach the max retry times, continue..");
            //continue retrying
            switch ( retry )
                {
                case 0:
                case 1:
                    {
                    //first or second failure, retry after 30mins
                    TTimeIntervalMinutes halfhour( KFirstSecondRetryInterval );
                    StartL( halfhour );
                    }
                    break;

                case 2:
                case 3:
                    {
                    //third failure, retry after 24 hours
                    TTimeIntervalMinutes oneday( KThirdFourthRetryInterval );
                    StartL( oneday );
                    }
                    break;
                    
                default:
                    break;
                };
            }
        }
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::CheckUpdatesComplete()
// ----------------------------------------------------------
void CIAUpdateBGTimer::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    TRAP_IGNORE( DoCheckUpdatesCompleteL( aErrorCode, aAvailableUpdates ) );
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::UpdateComplete()
// ----------------------------------------------------------
void CIAUpdateBGTimer::UpdateComplete( TInt /*aErrorCode*/, CIAUpdateResult* /*aResult*/ )
    {
    FLOG( "UpdateComplete");
    //no implementation
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::UpdateQueryComplete()
// ----------------------------------------------------------
void CIAUpdateBGTimer::UpdateQueryComplete( TInt /*aErrorCode*/, TBool /*aUpdateNow*/ )
    {
    FLOG( "UpdateQueryComplete");
    //no implementation
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::CIAUpdateBGTimer()
// ----------------------------------------------------------
CIAUpdateBGTimer::CIAUpdateBGTimer():CTimer( EPriorityStandard )
    {
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::RunL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::RunL()
    {
    FLOG("[bgchecker]RunL() ");
    if ( !IAUpdateEnabledL() )
        {
        Shutdown();
        return;
        }
    User::LeaveIfError( iStatus.Int() );
    //Check the mode again, in case the mode is changed while waiting.
    iMode = ModeL();
    
    switch ( iMode )
        {
        case ENormalMode:
        case ERetryMode:
            {     
            FLOG("[bgchecker]RunL() ENormalMode");                                    
            //roaming case is checked in IAUpdate server side
            if ( IsAutoUpdateDisabledL() )
                {
                ListenAutoUpdateSettingsL();
                
                iMode = ESleepMode;
                SetModeL( iMode );
                
                //the program is waked up when automatic checking is changed by user.
                //cenrep call back will be used.
                return;
                }                        
            
            FLOG("[bgchecker]RunL() ENormalMode 1");
            
            if(iRuns == 0)
                {
                FLOG("[bgchecker]RunL() ENormalMode 2");
                // if user did the refresh from server while waiting, restart the timer.
                // the last refresh time is read from the private file of IAUpdate server.
                TTimeIntervalMinutes timetowait =
                        TimeIntervalFromNextRefreshL();

                if (timetowait.Int() <= 0 )
                    {
                    DoUpdatesCheckingL( ETrue );
                    }
                else
                    {
                    StartL( timetowait );
                    }
                FLOG("[bgchecker]RunL() ENormalMode 3");
                }
            else
                {
                FLOG("[bgchecker]RunL() ENormalMode 4");
                TTimeIntervalMinutes timetowait = TimeIntervalFromNextRefreshL();
                   
                   if ( timetowait.Int() <= 0 )
                       {
                       StartL( StartNow );
                       }
                   else
                       {
                       StartL( timetowait );
                       }
                   FLOG("[bgchecker]RunL() ENormalMode 5");
                 }        
            }
            break;
            
        case EFirstTimeMode:
            {
            FLOG("[bgchecker] runl EFirstTimeMode");
            
            if ( iRuns == 0 )
                {
                FLOG("[bgchecker] runl run = 0");
                HandlerFirstTimeL();
                }
            else
                {
                FLOG("[bgchecker] runl still wait");
                TTimeIntervalMinutes timetowait = TimeIntervalFromNextShowOfNewFeatureDialogL();
                                   
                if ( timetowait.Int() <= 0 )
                    {
                    StartL( StartNow );
                    }
                 else
                    {
                    StartL( timetowait );
                    } 
                }
            }
            break;

        case EFirstTimeMode2:
            {
            FLOG("[bgchecker] runl EFirstTimeMode2");                 
            //disclaimer is not accepted  
           
            if ( !IsAgreementAcceptedL()  )
                {
                FLOG("[bgchecker] runl EFirstTimeMode reject disclaimer");
                //user accepte the new feature dialog but user reject sthe disclaimer
                //in sleep mode now
                iMode = ESleepMode;
                SetModeL( iMode );
                //subscribe to cenrep key for automatic checking for wake up from sleep mode
                ListenAutoUpdateSettingsL(); 
                }
            else
                {
                FLOG("[bgchecker] runl EFirstTimeMode go to normal mode");
                //Set to Normal mode
                //check autochecksetting in Runl later
                iMode = ENormalMode;
                SetModeL( iMode );
                StartL( StartNow );
                }
            }
            break;
            
        default:
            break;
        };
       
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::HandlerFirstTimeL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::HandlerFirstTimeL()
    {
    // this checking is in case when accept disclaimer from grid or application
    // during the waiting time.

    if (IsAgreementAcceptedL() || !IsAutoUpdateDisabledL())
        {
        //user accepted the disclamier during the waiting time
        //out of firsttime mode, check auto check setting in runl and 
        //do update check as normal. 
        // If autoUpdate is enabled ==> always to normal mode

        //In normal mode now.
        iMode = ENormalMode;
        SetModeL(iMode);
        StartL(StartNow);
        }
    else
        {
        //this is first time mode
        FLOG("[bgchecker]HandlerFirstTimeL 3 ");
        LaunchNotificationL( 0 );
        }
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::DoCancel()
// ----------------------------------------------------------
void CIAUpdateBGTimer::DoCancel()
    {
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::RunError()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::CheckingFrequencyInMinutesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::CheckingFrequencyInMinutesL()
    {
    FLOG("[bgchecker]CheckingFrequencyInMinutesL begin ");
    
#ifdef _DEMO    
    return KCheckingUpdateInterval;
#else
    TInt CheckingFrequency = 0;
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    TInt err = cenrep->Get( KIAUpdateCheckingFrequency, CheckingFrequency );
    
    if ( err != KErrNone )
        {
        FLOG("[bgchecker]CheckingFrequencyInMinutesL Getting checking frequency from Cenrep failed ");
        }
    
    CleanupStack::PopAndDestroy( cenrep );
    
    return CheckingFrequency*24*60; // convert into minutes
#endif
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::UpdateAvailableReminderIntervalInMinutesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::UpdateAvailableReminderIntervalInMinutesL()
    {
    FLOG("[bgchecker]UpdateAvailableReminderIntervalInMinutesL begin ");
    
#ifdef _DEMO    
    return KUpdateAvailableReminderInterval;
#else
    TInt updateAvailableReminderIntervel = 0;
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    TInt err = cenrep->Get( KIAUpdateUpdateAvailableReminderInterval, updateAvailableReminderIntervel );
    
    if ( err != KErrNone )
        {
        FLOG("[bgchecker]UpdateAvailableReminderIntervalInMinutesL interval for update available reminder from Cenrep failed ");
        }
    
    CleanupStack::PopAndDestroy( cenrep );
    
    return updateAvailableReminderIntervel*24*60; // convert into minutes
#endif
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::FirstTimeDialogDelayInMinutesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::FirstTimeDialogDelayInMinutesL()
    {
    FLOG("[bgchecker]FirstTimeDialogDelayInMinutesL begin ");
    
#ifdef _DEMO    
    return KFirstTimeDialogDelay;
#else
    TInt firstTimeDialogDelay = 0;
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    TInt err = cenrep->Get( KIAUpdateFirstTimeDialogDelay, firstTimeDialogDelay );
    
    if ( err != KErrNone )
        {
        FLOG("[bgchecker]FirstTimeDialogDelayInMinutesL Getting first time dialog delay from Cenrep failed ");
        }
    
    CleanupStack::PopAndDestroy( cenrep );
    
    return firstTimeDialogDelay*24*60; // convert into minutes
#endif
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::FirstTimeDialogReminderIntervalInMinutesL()
// ----------------------------------------------------------
TInt CIAUpdateBGTimer::FirstTimeDialogReminderIntervalInMinutesL()
    {
    FLOG("[bgchecker]FirstTimeDialogDelayInMinutesL begin ");
    
#ifdef _DEMO    
    return KFirstTimeDialogReminderInterval;
#else
    TInt firstTimeDialogReminderInterval = 0;
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    TInt err = cenrep->Get( KIAUpdateFirstTimeDialogReminderInterval, firstTimeDialogReminderInterval );
    
    if ( err != KErrNone )
        {
        FLOG("[bgchecker]FirstTimeDialogReminderIntervalInMinutesL Getting first time dialog reminder interval from Cenrep failed ");
        }
    
    CleanupStack::PopAndDestroy( cenrep );
    
    return firstTimeDialogReminderInterval*24*60; // convert into minutes
#endif
    }




// ----------------------------------------------------------
// CIAUpdateBGTimer::IsFirstTimeDialogDisabledL()
// ----------------------------------------------------------
TBool CIAUpdateBGTimer::IsFirstTimeDialogDisabledL()
    {
    FLOG("[bgchecker]IsFirstTimeDialogDisabledL begin ");
    
    TBool isDisabled = EFalse;

    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    
    TInt err = cenrep->Get( KIAUpdateFirstTimeDialogDisabled, isDisabled );

    FLOG_NUM("[bgchecker] err = %d", err );
    FLOG_NUM("[bgchecker] isDisabled = %d", isDisabled );
    
    if ( err != KErrNone )
        {
        FLOG("[bgchecker]IsFirstTimeDialogDisabledL get value from cenrep failed ");
        }
    
    CleanupStack::PopAndDestroy( cenrep );
    
    return isDisabled;
    }



// ----------------------------------------------------------
// CIAUpdateBGTimer::StartL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::StartL( TTimeIntervalMinutes aWaitTime )
    {   
    Cancel();
    /*
    RTimer::After maximum delay is 35 minutes, 47 seconds this is because it is 32bit integer. 
    Thats why aWaitTime is divided to 30min runs.  
    */
    
    iRuns = 0;

    if(aWaitTime.Int() > HalfHourInMinuts )
        {
        iRuns = aWaitTime.Int() / HalfHourInMinuts;
        TTimeIntervalMinutes halfhour( HalfHourInMinuts );    
        After( ConvertToMicroseconds( halfhour ) );  
        }
    else{
        After(ConvertToMicroseconds( aWaitTime ) ); 
        }
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::TimeIntervalFromNextRefreshL()
// ----------------------------------------------------------
TTimeIntervalMinutes CIAUpdateBGTimer::TimeIntervalFromNextRefreshL()
    { 
    FLOG("[bgchecker] TimeIntervalFromNextRefreshL 1");
    TTime lastrefreshtime = GetLastRefreshTimeL();
    
    FTIME( lastrefreshtime );
    
    TTime currenttime;
    currenttime.UniversalTime();
     
    FTIME ( currenttime ); 
    
    //get current gap
    TTimeIntervalMinutes timepast;
    currenttime.MinutesFrom( lastrefreshtime, timepast );
    FLOG("[bgchecker] TimeIntervalFromNextRefreshL 2");
    if ( timepast.Int() < 0 )
        {
        FLOG("[bgchecker] TimeIntervalFromNextRefreshL 3");
        //user set its clock backward, refresh anyway.
        return timepast;
        }
    FLOG("[bgchecker] TimeIntervalFromNextRefreshL 4");
    TTimeIntervalMinutes interval( CheckingFrequencyInMinutesL() );
    FLOG_NUM(" interval %d ", interval.Int() );
    FLOG_NUM(" timepast %d ", timepast.Int() );
    FLOG("[bgchecker] TimeIntervalFromNextRefreshL 5");
    //check if the interval is bigger than the value defined by user
    TTimeIntervalMinutes timetowait(interval.Int() - timepast.Int());
    FLOG("[bgchecker] TimeIntervalFromNextRefreshL 6");
    return timetowait;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::TimeIntervalFromNextShowOfNewFeatureDialogL()
// ----------------------------------------------------------
TTimeIntervalMinutes CIAUpdateBGTimer::TimeIntervalFromNextShowOfNewFeatureDialogL()
    {
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 1");
    TTime lastshowtime = GetLastTimeShowNewFeatureDialogL();
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 2");
    if( lastshowtime.Int64() == 0 )
        {
        FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 3");
        //first time
        //wait for some days

        TTimeIntervalMinutes firstTimeDelay(FirstTimeDialogDelayInMinutesL());
        TTime currenttime;
        currenttime.UniversalTime();

        FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 4");
        //save the next show new feature dialog time
        //in case use switch off the device and restarted.      
        User::LeaveIfError(SetLastTimeShowNewFeatureDialogL(currenttime));
            FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 5");
        return firstTimeDelay;
        }
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 6");
    TTime currenttime;
    currenttime.UniversalTime();
    
    TTimeIntervalMinutes timepass;
    currenttime.MinutesFrom( lastshowtime, timepass );
    
    if ( timepass.Int() < 0 )
         {
         FLOG("[bgchecker] TimeIntervalFromNextRefreshL 3");
         //clock is set back.
         return timepass;
         }
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 7"); 
    
    TTimeIntervalMinutes interval;
    if ( ModeL() == EFirstTimeMode )
        {
        FLOG(" current mode is FirstTimeMode ");
        interval = FirstTimeDialogDelayInMinutesL();
        }
    else
        {
        //nothing
        FLOG(" current mode is wrong!!");
        User::LeaveIfError( KErrArgument );
        }
    
    FLOG_NUM(" interval %d ", interval.Int() );
    FLOG_NUM(" timepast %d ", timepass.Int() );
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 5");
    //check if the interval is bigger than the value defined by user
    TTimeIntervalMinutes timetowait(interval.Int() - timepass.Int());
    FLOG("[bgchecker] TimeIntervalFromNextShowOfNewFeatureDialogL 6");
    return timetowait;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::DoSoftNotificationCallBackL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::DoSoftNotificationCallBackL( TBool aIsAccepted )
    {
    FLOG("[bgchecker] SoftNotificationCallBack");
    if ( !IAUpdateEnabledL() )
        {
        Shutdown();
        return;
        }
    iMode = ModeL(); 
    
      switch ( iMode )
          {
          case EFirstTimeMode:
                 {
                 
                 FLOG("[bgchecker] SoftNotificationCallBack EFirstTimeMode");
                 if ( !aIsAccepted )
                     {
                     FLOG("[bgchecker] SoftNotificationCallBack EFirstTimeMode, not accepted");
                     //in sleep mode now
                     iMode = ESleepMode;
                     SetModeL( iMode );
                     
                     //subscribe to cenrep key for automatic checking for wake up from sleep mode
                     ListenAutoUpdateSettingsL();
                     
                     }
                 else
                     {
                     FLOG("[bgchecker] SoftNotificationCallBack EFirstTimeMod accepeted");
                     StartIaupdateL();
                     //iaupdate is launched
                     //check the disclaimer acceptance and automatic checking setting in 10mins

                     StartL( KDelayForUserToAcceptDisclaimer );
                     iMode = EFirstTimeMode2;     
                     SetModeL( iMode );

                     
                     //Still in first time mode, 
                     //check user's setting and disclamier after 10mins.
                     }
                 break;
                 }
                 
          case ENormalMode:
              {
              FLOG("[bgchecker] SoftNotificationCallBack ENormalMode");
              if ( !aIsAccepted )
                  {
                  FLOG("[bgchecker] SoftNotificationCallBack ENormalMode not accpeted");
                  
                  }
              else
                  {
                  //accepted, clear indicator entries
                  SetNrOfIndicatorEntriesL( 0 );
                  // start IAD
                  StartIaupdateL();
                  }
              break;
              }

          case ESleepMode:
              {
              FLOG("[bgchecker] SoftNotificationCallBack ESleepMode");
              //donothing
              break;
              }

          default:
              break;
          }
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::SoftNotificationCallBack()
// ----------------------------------------------------------
void CIAUpdateBGTimer::SoftNotificationCallBack( TBool aIsAccepted )
    {
    TRAP_IGNORE( DoSoftNotificationCallBackL( aIsAccepted ) );
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::CurrentMode()
// ----------------------------------------------------------
TIAUpdateBGMode CIAUpdateBGTimer::CurrentMode()
    {
    return iMode;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::LaunchNotificationL( const int aNrOfUpdates )
// ----------------------------------------------------------
void CIAUpdateBGTimer::LaunchNotificationL( const int aNrOfUpdates )
    {
    
    FLOG("[bgchecker] LaunchNotificationL ");
        
    iMode = ModeL();
    
    // Resolve title text
    HBufC*  titleText;
    if ( aNrOfUpdates == 0 )
        {
        // First time case
        titleText = HbTextResolverSymbian::LoadLC( KTitleFirstTime );
        }
    else if ( aNrOfUpdates == 1 )
        {
        // one update available
        titleText = HbTextResolverSymbian::LoadLC( KTitleOneUpdate );
        }
    else
        {
        // several updates available
        titleText = HbTextResolverSymbian::LoadLC( KTitleSeveralUpdates );
        }
    
    // Resolve second text
    HBufC*  secondText;
    secondText = HbTextResolverSymbian::LoadLC( KSecondRow );
    
    // Set texts
    iSoftNotification->SetTextL( titleText->Des(), secondText->Des() );
    
    // delete text buffers
    CleanupStack::PopAndDestroy( secondText );
    CleanupStack::PopAndDestroy( titleText );
    
    // Set number of updates for dialog and internal file
    iSoftNotification->SetNrOfUpdates( aNrOfUpdates );
    SetNrOfIndicatorEntriesL( aNrOfUpdates );
    
    iSoftNotification->ShowNotificationL();
    FLOG("[bgchecker] LaunchNotificationL 1");
    
    return;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::IsAutoUpdateDisabledL()
// ----------------------------------------------------------
TBool CIAUpdateBGTimer::IsAutoUpdateDisabledL()
    {
    TInt autochecksetting;
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, autochecksetting ) );
    CleanupStack::PopAndDestroy( cenrep );     
    return (autochecksetting == EIAUpdateSettingValueDisable) ? ETrue : EFalse;
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::ListenAutoUpdateSettingsL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::ListenAutoUpdateSettingsL()
    {
    FLOG("[bgchecker] ListenAutoUpdateSettingsL() begin");
    if (iIAUpdateCRSession == NULL )  
        { 
        iIAUpdateCRSession = CRepository::NewL( KCRUidIAUpdateSettings ); 
        }
    if (iNotifyHandler == NULL )
       {
       iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iIAUpdateCRSession ); 
       }
    iNotifyHandler->StartListeningL();       
    }


// ----------------------------------------------------------
// CIAUpdateBGTimer::StopListeningAutoUpdateSettingsL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::StopListeningAutoUpdateSettingsL()
    {
    FLOG("[bgchecker] StopListeningAutoUpdateSettingsL() begin");
    if (iIAUpdateCRSession == NULL )  
        { 
        iIAUpdateCRSession = CRepository::NewL( KCRUidIAUpdateSettings ); 
        }
    if (iNotifyHandler == NULL )
       {
       iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iIAUpdateCRSession ); 
       }
    iNotifyHandler->StopListening();
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::StartIaupdateL()
// ----------------------------------------------------------
void CIAUpdateBGTimer::StartIaupdateL() const
    {
    FLOG("[bgchecker] StartIaupdateL() begin");
    RWsSession ws;   
    User::LeaveIfError( ws.Connect() == KErrNone );
    TApaTaskList tasklist(ws);   
    TApaTask task = tasklist.FindApp( TUid::Uid( KIADUpdateLauncherUid ) );
    if ( task.Exists() )
        {
        FLOG("[bgchecker] StartIaupdateL() IAD launcher process found");
        task.BringToForeground();
        FLOG("[bgchecker] StartIaupdateL() after task.BringToForeground()");
        ws.Close();
        }
    else 
        {
        ws.Close();
        RProcess process;
        CleanupClosePushL( process );
        User::LeaveIfError( 
              process.Create( KIAUpdateLauncherExe, KNullDesC ) );
              
        CApaCommandLine* commandLine = CApaCommandLine::NewLC();
        commandLine->SetDocumentNameL( KNullDesC );
        commandLine->SetExecutableNameL( _L("iaupdatelauncher.exe") );
        commandLine->SetTailEndL( KRefreshFromNetworkDenied );
        commandLine->SetProcessEnvironmentL( process );
        CleanupStack::PopAndDestroy( commandLine );
       
        process.Resume();
        CleanupStack::PopAndDestroy( &process );	
        }
    FLOG("[bgchecker] StartIaupdateL() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGTimer::IAUpdateEnabledL()
// ---------------------------------------------------------------------------
//
TBool CIAUpdateBGTimer::IAUpdateEnabledL() const
    {
    FLOG("[bgchecker] CIAUpdateBGTimer::IAUpdateEnabledL() begin");
    TBool enabled = EFalse;
    RFeatureControl featureControl;
    TInt error( KErrNone );

    error = featureControl.Connect();
    FLOG_NUM("[bgchecker] CIAUpdateBGTimer::IAUpdateEnabledL() Connection to the Feature Manager Server: %d", error );
    
    User::LeaveIfError( error );
    
    TUid iaupdatefeature;
    iaupdatefeature.iUid = KFeatureIdIAUpdate;
    
    TInt ret = featureControl.FeatureSupported( iaupdatefeature );
    if ( ret == KFeatureSupported )
        {
        enabled = ETrue;
        }
    featureControl.Close(); 
    FLOG_NUM("[bgchecker] CIAUpdateBGTimer::IAUpdateEnabledL() enabled: %d", enabled );
    return enabled;        
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGTimer::Shutdown()
// ---------------------------------------------------------------------------
//
void CIAUpdateBGTimer::Shutdown() 
    {
    FLOG("[bgchecker] Shutdown() begin");
    Cancel();
    delete iUpdate; 
    iUpdate = NULL;
    
    //delete iReminderTimer;
    //iReminderTimer = NULL;
    
    if ( iNotifyHandler ) 
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        iNotifyHandler = NULL;
        }
    
    if ( iSoftNotification )
        {
        delete iSoftNotification;
        iSoftNotification = NULL;
        }
    
    if ( iIndicatorNotifyHandler )
        {
        delete iIndicatorNotifyHandler;
        iIndicatorNotifyHandler = NULL;
        }
    
    CActiveScheduler::Stop();
        
    FLOG("[bgchecker] Shutdown() end");
    }   
// ---------------------------------------------------------------------------
// CIAUpdateBGTimer::HandleIndicatorRemoveL()
// ---------------------------------------------------------------------------
//
void CIAUpdateBGTimer::HandleIndicatorRemoveL() 
    {
    // remove indicator from indicator menu
    iSoftNotification->RemoveIndicatorL();
    
    //clear nr of indicator entries in internal file
    SetNrOfIndicatorEntriesL( 0 );
    }

// End of file
