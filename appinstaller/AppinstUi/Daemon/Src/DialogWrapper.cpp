/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CDialogWrapper
*                class member functions.
*
*/


// INCLUDE FILES
#include <hb/hbwidgets/hbdeviceprogressdialogsymbian.h>
#include <hb/hbwidgets/hbdevicenotificationdialogsymbian.h>
#include <hb/hbcore/hbindicatorsymbian.h>
#include <hb/hbcore/hbsymbianvariant.h>
//#include <ssm/ssmstateawaresession.h> // For system state
//#include <ssm/ssmstate.h>
//#include <ssm/ssmdomaindefs.h> // KUIFrameworkDomain
#include "DialogWrapper.h"
#include "SWInstDebug.h"

using namespace Swi;

// Time interval for progress dialog.
const TUint KDialogTimeOut = 5000000;
// Indicator type
_LIT( KIndicatorTypeSWIDaemon, 
        "com.nokia.sisxsilentinstall.indicatorplugin/1.0" );

//TODO: remove when HB dialogs do not crash in HW/WINS
//#define _SWIDAEMON_DISABLE_NOTES_

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDialogWrapper::CDialogWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDialogWrapper::CDialogWrapper( RFs& aFs )
    : iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDialogWrapper::ConstructL()
    {
    iIsProgressDialog = EFalse;
    iHbProgressDialog = NULL;
    iIsUninstallerProgressDialog = EFalse;
    iHbProgressDialogForUninstaller = NULL;
    iIsIndicator = EFalse;
    iHbIndicator = NULL;                        
    // By default Daemon will show all notes.
    iDisableAllNotes = EFalse;    
    iTimeOffDisableProgress = EFalse;    
    // Create watcher AO for PS Key.
    iWatcher = CDialogWatcher::NewL( this );              
    // Get current PS Key 
    TInt err = iWatcher->GetPSKeyForUI( iDisableAllNotes );
    if ( err )
        {
        // If we can not get PS key, let's disable all notes.
        iDisableAllNotes = ETrue;
        }
    // Start AO
    iWatcher->StartWatcher();    
    // Create dialog timer for progress dialog.
    iTimer = CDialogTimer::NewL( this );
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDialogWrapper* CDialogWrapper::NewL( RFs& aFs )
    {
    CDialogWrapper* self = new( ELeave ) CDialogWrapper( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::~CDialogWrapper
// Destructor
// -----------------------------------------------------------------------------
//    
CDialogWrapper::~CDialogWrapper()
    {
    FLOG( _L("Daemon: CDialogWrapper::~CDialogWrapper") );
    delete iTimer;
    
    // If installer's RunL leaves make sure that dialogs are closed.
    if ( iIsProgressDialog && iHbProgressDialog )
            {
            iHbProgressDialog->Close();
            }    
    delete iHbProgressDialog;
    
    // If uninstaller's RunL leaves make sure that dialogs are closed.
    if ( iIsUninstallerProgressDialog && iHbProgressDialogForUninstaller )
            {
            iHbProgressDialogForUninstaller->Close();
            }    
    delete iHbProgressDialogForUninstaller;
    
    if ( iIsIndicator && iHbIndicator )
            {             
            iHbIndicator->Deactivate( KIndicatorTypeSWIDaemon );                                     
            }
    delete iHbIndicator;
    
    if ( iWatcher )
        {
        iWatcher->StopWatcher();        
        }       
    delete iWatcher;
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ShowUntrustedResultL
// Show global result dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ShowUntrustedResultL()
    {
    FLOG( _L("Daemon: CDialogWrapper::ShowUntrustedResultL") );
        
#ifdef _SWIDAEMON_DISABLE_NOTES_
    FLOG( _L("Daemon: CDialogWrapper: iDisableAllNotes = ETrue") );
    iDisableAllNotes = ETrue;
#endif                
    
    // Let watcher to know that waiting note is canceled.
    iWatcher->CancelNoteRequest();            
        
    // Inform watcher that we have request to show note. 
    iWatcher->CancelNoteRequest(); 
            
    if ( !iDisableAllNotes  )
        {    
        CHbDeviceNotificationDialogSymbian* notificationDialog = 
                CHbDeviceNotificationDialogSymbian::NewL( NULL );
        
        CleanupStack::PushL( notificationDialog );
        
//TODO get string from log file.  
        _LIT( KTempIconText,"note_info");
        _LIT( KTempTextTitle,"SW Silent Installer" );
        _LIT( KTempTextForErrorMessage,"Untrusted software was found." ); 
                                         
        notificationDialog->NotificationL( KTempIconText, 
                                           KTempTextTitle , 
                                           KTempTextForErrorMessage );
                    
        CleanupStack::PopAndDestroy( notificationDialog );        
        }   
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ShowErrorResultL
// Show global result dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ShowErrorResultL()
    { 
    FLOG( _L("Daemon: CDialogWrapper::ShowErrorResultL") );
    
#ifdef _SWIDAEMON_DISABLE_NOTES_
    FLOG( _L("Daemon: CDialogWrapper: iDisableAllNotes = ETrue") );
    iDisableAllNotes = ETrue;
#endif  
    
    // Inform watcher that we have request to show note. 
    iWatcher->CancelNoteRequest();   
            
    if ( !iDisableAllNotes )
        {    
        CHbDeviceNotificationDialogSymbian* notificationDialog = 
                CHbDeviceNotificationDialogSymbian::NewL( NULL );
        
        CleanupStack::PushL( notificationDialog );
        
//TODO get string from log file.  
        _LIT( KTempIconText,"note_info");
        _LIT( KTempTextTitle,"SW Silent Installer" );
        _LIT( KTempTextForErrorMessage,"Installation was not completed." ); 
                                        
        notificationDialog->NotificationL( KTempIconText, 
                                           KTempTextTitle , 
                                           KTempTextForErrorMessage );
               
        CleanupStack::PopAndDestroy( notificationDialog ); 
        }
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ShowWaitingNoteL  
// Show global waiting note during installing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ShowWaitingNoteL()
	{
    FLOG( _L("Daemon: CDialogWrapper::ShowWaitingNoteL") );
           
#ifdef _SWIDAEMON_DISABLE_NOTES_
    FLOG( _L("Daemon: CDialogWrapper: iDisableAllNotes = ETrue") );
    iDisableAllNotes = ETrue;
#endif
    
    FLOG_1( _L("Daemon: iDisableAllNotes: %d"), iDisableAllNotes );
    FLOG_1( _L("Daemon: iIsProgressDialog: %d"), iIsProgressDialog );
    FLOG_1( _L("Daemon: iTimeOffDisableProgress: %d"), iTimeOffDisableProgress );
            
    if ( !iDisableAllNotes )
        {          
        if ( !iIsProgressDialog && !iTimeOffDisableProgress )
            {    
            iHbProgressDialog = CHbDeviceProgressDialogSymbian::NewL( 
                                   CHbDeviceProgressDialogSymbian::EWaitDialog, 
                                   NULL );
            iIsProgressDialog = ETrue;
                       
            _LIT( KTempTextForProgressDialog,"Installing" );
            
            iHbProgressDialog->SetTextL( KTempTextForProgressDialog ); 
            FLOG( _L("Daemon: ShowWaitingNoteL: ShowL") );
            iHbProgressDialog->ShowL();            
            FLOG( _L("Daemon: ShowWaitingNoteL: StartDialogTimer") );
            iTimer->StartDialogTimer( KDialogTimeOut );
            }
        }
    else if ( iDisableAllNotes )
        {
        // Let watcher to know that waiting note should be shown 
        // after dialogs are enabled.
        iWatcher->RequestToDisplayNote();
        }
	}
	
// -----------------------------------------------------------------------------
// CDialogWrapper::CancelWaitingNoteL  
// Cancel global waiting note after installing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::CancelWaitingNote()
	{ 
    FLOG( _L("Daemon: CDialogWrapper::CancelWaitingNoteL") );
    
	if ( iIsProgressDialog )
		{
		iHbProgressDialog->Close();
		delete iHbProgressDialog;
		//Make sure not to delete twice in destructor.
		iHbProgressDialog = NULL;
		iIsProgressDialog = EFalse;
		
		iTimeOffDisableProgress = ETrue;
		}
		
    // Let watcher to know that waiting note is canceled.
	iWatcher->CancelNoteRequest();
 	}

// -----------------------------------------------------------------------------
// CDialogWrapper::LoadResourceLC  
// Read resource string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
HBufC* CDialogWrapper::ReadResourceLC( TInt /*aResourceId*/ )
    {
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::SetUIFlag  
// Sets UI flag for dialog wrapper. If flag is ture, dialogs are disabled.
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::SetUIFlag( TInt aUIFlag )
    {
    iDisableAllNotes = aUIFlag;
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ShowWaitingNoteForUninstallerL  
// Show global waiting note during uninstall.
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ShowWaitingNoteForUninstallerL()
    {
    FLOG( _L("Daemon: CDialogWrapper::ShowWaitingNoteForUninstallerL") );
    
#ifdef _SWIDAEMON_DISABLE_NOTES_
    FLOG( _L("Daemon: CDialogWrapper: iDisableAllNotes = ETrue") );
    iDisableAllNotes = ETrue;
#endif
    
    FLOG_1( _L("Daemon: iDisableAllNotes: %d"), iDisableAllNotes );
    FLOG_1( _L("Daemon: iIsProgressDialog: %d"), iIsProgressDialog );
    FLOG_1( _L("Daemon: iTimeOffDisableProgress: %d"), iTimeOffDisableProgress );
              
    if ( !iDisableAllNotes )
        {
        if ( !iIsUninstallerProgressDialog && !iTimeOffDisableProgress )
            {    
            iHbProgressDialogForUninstaller = 
                    CHbDeviceProgressDialogSymbian::NewL( 
                                   CHbDeviceProgressDialogSymbian::EWaitDialog, 
                                   NULL );
            iIsUninstallerProgressDialog = ETrue;
    //TODO get string from log file.            
            _LIT( KTempTextForProgressDialog,"Uninstalling" );            
            iHbProgressDialogForUninstaller->SetTextL( KTempTextForProgressDialog ); 
            FLOG( _L("Daemon: ShowWaitingNoteForUninstallerL: ShowL") );
            iHbProgressDialogForUninstaller->ShowL();
            FLOG( _L("Daemon: ShowWaitingNoteForUninstallerL: StartDialogTimer") );
            iTimer->StartDialogTimer( KDialogTimeOut );
            }
        }
    else if ( iDisableAllNotes )
        {
        // Let watcher to know that waiting note should be shown 
        // after dialogs are enabled.
        iWatcher->RequestToDisplayNote();
        }
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::CancelWaitingNoteL  
// Cancel global waiting note after installing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::CancelWaitingNoteForUninstaller()
    { 
    FLOG( _L("Daemon: CDialogWrapper::CancelWaitingNoteForUninstallerL") );
    
    if ( iIsUninstallerProgressDialog )
        {
        iHbProgressDialogForUninstaller->Close();
        delete iHbProgressDialogForUninstaller;
        //Make sure not to delete twice in destructor.
        iHbProgressDialogForUninstaller = NULL; 
        iIsUninstallerProgressDialog = EFalse;
        
        iTimeOffDisableProgress = ETrue;
        }
        
    // Let watcher to know that waiting note is canceled.
    iWatcher->CancelNoteRequest();
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ActivateIndicatorL()  
// 
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ActivateIndicatorL( TReal aProcessValue )
    {  
    FLOG( _L("Daemon: CDialogWrapper::ActivateIndicatorL") );
                  
    if ( !iIsIndicator )
        {
        FLOG( _L("Daemon: CHbIndicatorSymbian::NewL") );
        iHbIndicator = CHbIndicatorSymbian::NewL();
        iIsIndicator = ETrue;             
        }
    
    TInt value = static_cast<TInt>( aProcessValue );
    FLOG_1( _L("Daemon: ActivateIndicatorL: precent value: %d"), value );
          
    CHbSymbianVariant* hbParam = CHbSymbianVariant::NewL( 
                                                 &value,
                                                 CHbSymbianVariant::EInt );    
    CleanupStack::PushL( hbParam );
    
    FLOG( _L("Daemon: ActivateIndicatorL: Activate") );
    iHbIndicator->Activate( KIndicatorTypeSWIDaemon, hbParam );
    
    CleanupStack::PopAndDestroy( hbParam );         
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::SetModeToIndicatorL()  
// 
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::SetModeToIndicatorL( TInt aMode )
    {  
    FLOG_1( _L("Daemon: CDialogWrapper::SetModeToIndicatorL: aMode: %d"),
            aMode );
                  
    if ( !iIsIndicator )
        {
        FLOG( _L("Daemon: CHbIndicatorSymbian::NewL") );
        iHbIndicator = CHbIndicatorSymbian::NewL();
        iIsIndicator = ETrue;        
        }
    // Set mode.
    TReal mode = aMode;    
    CHbSymbianVariant* hbParam = CHbSymbianVariant::NewL( 
                                                 &mode,
                                                 CHbSymbianVariant::EReal );    
    CleanupStack::PushL( hbParam );    
    FLOG( _L("Daemon: SetModeToIndicatorL: Activate") );
    iHbIndicator->Activate( KIndicatorTypeSWIDaemon, hbParam );
    
    CleanupStack::PopAndDestroy( hbParam );         
    }


// -----------------------------------------------------------------------------
// CDialogWrapper::CancelIndicatorL  
// 
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::CancelIndicatorL()
    {
    FLOG( _L("Daemon: CDialogWrapper::CancelIndicatorL") );
    
    if ( iIsIndicator )
        { 
        FLOG( _L("Daemon: CancelIndicatorL: Deactivate") );
        iHbIndicator->Deactivate( KIndicatorTypeSWIDaemon );              
        delete iHbIndicator;
        iHbIndicator = NULL; //Make sure not to delete twice in destructor.
        iIsIndicator = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::CheckSystemState()  
// 
// -----------------------------------------------------------------------------
// 
/*
void CDialogWrapper::CheckSystemState()
    {
    FLOG( _L("Daemon: CDialogWrapper::CheckSystemState TEST") );

    // We need to check this only if system is not ready to show 
    // dialogs e.g. UI is not up.
    if ( !iSystemReadyToShowDialogs )
        {    
        RSsmStateAwareSession systemStateSession;
         
        TInt err = systemStateSession.Connect( KUIFrameworkDomain3 );
        FLOG_1( _L("Daemon: systemStateSession.Connect err = %d"), err ); 
        
        if ( err == KErrNone )
            {
            TSsmState currentState = systemStateSession.State();
            systemStateSession.Close();
            
            FLOG_1( _L("Daemon: System main state = %d"), currentState.MainState() ); 
            FLOG_1( _L("Daemon: System sub state = %d"), currentState.SubState() ); 
            
            
            if ( currentState.MainState() == ESsmNormal )
                {
                iSystemReadyToShowDialogs = ETrue;            
                }
            else
                {
                iSystemReadyToShowDialogs = EFalse;            
                }
            }
        else
            {
            iSystemReadyToShowDialogs = EFalse;
            }
        }
    FLOG_1( _L("Daemon: iSystemReadyToShowDialogs = %d"), 
            iSystemReadyToShowDialogs );      
    }
*/

//-------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CDialogTimer::CDialogTimer()
//
// -----------------------------------------------------------------------------
//
CDialogTimer::CDialogTimer() : CActive( EPriorityNormal )
    {
    }

// -----------------------------------------------------------------------------
// CDialogTimer::~CDialogTimer()
//
// -----------------------------------------------------------------------------
//
CDialogTimer::~CDialogTimer()
    {
    FLOG( _L("Daemon: CDialogTimer::~CDialogTimer") );
    // Cancel the outstanding request. Calls the active object’s
    // DoCancel function if request is outstanding.
    Cancel();
    // Delete RTimer
    iRTimer.Close();
    }

// -----------------------------------------------------------------------------
// CDialogTimer::NewL()
//
// -----------------------------------------------------------------------------
//
CDialogTimer* CDialogTimer::NewL( CDialogWrapper* aDialog )
    {
    CDialogTimer* activeTimer = new (ELeave) CDialogTimer();
    CleanupStack::PushL( activeTimer );
    activeTimer->ConstructL( aDialog );
    CleanupStack::Pop();
    return activeTimer;
    }

// -----------------------------------------------------------------------------
// CDialogTimer::ConstructL()
//
// -----------------------------------------------------------------------------
//
void CDialogTimer::ConstructL( CDialogWrapper* aDialog )
    {  
    if ( aDialog == NULL )
        {
        User::Leave( KErrArgument );
        }  
    iDialog = aDialog;
    CActiveScheduler::Add( this );                 
    iRTimer.CreateLocal();                                     
    }

// -----------------------------------------------------------------------------
// CDialogTimer::StartDialogTimer()
//
// -----------------------------------------------------------------------------
//
void CDialogTimer::StartDialogTimer( TUint32 aRefreshTime )
    {
    FLOG( _L("Daemon: CDialogTimer::StartDialogTimer") );
    // Check first that we do not have request outstanding.
    if ( !IsActive() )
        {
        // Set time interval for dialog.
        TimerSet( aRefreshTime );     
        }
    }

// -----------------------------------------------------------------------------
// CDialogTimer::TimerSet()
//
// -----------------------------------------------------------------------------
//
void CDialogTimer::TimerSet( TUint32 aRefreshTime )
    {  
    FLOG_1( _L("Daemon: CDialogTimer::TimerSet time: %d"), aRefreshTime );
    // Set timer interval.    
    iRTimer.After( iStatus, aRefreshTime );               
    // Set active. Start wait for timer.
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDialogTimer::RunL()
//
// -----------------------------------------------------------------------------
//
void CDialogTimer::RunL()
    {
    FLOG( _L("Daemon: CDialogTimer::RunL: Cancel waiting note") );
    iDialog->CancelWaitingNote();
    iDialog->CancelWaitingNoteForUninstaller();
    }

// -----------------------------------------------------------------------------
// CDialogTimer::DoCancel()
//
// -----------------------------------------------------------------------------
//
void CDialogTimer::DoCancel()
    {
    // Cancel outstanding request for a timer event.
    iRTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CDialogTimer::RunError()
//
// -----------------------------------------------------------------------------
//
TInt CDialogTimer::RunError( TInt aError )
    {                        
    return aError;       
    }


//  End of File  
