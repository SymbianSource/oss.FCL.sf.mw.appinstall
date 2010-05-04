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
#include "DialogWrapper.h"
#include "SWInstDebug.h"

using namespace Swi;

//TODO: remove when HB dialogs do not crash in HW/WINS
#define _SWIDAEMON_DISABLE_NOTES_

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
                  
    // By default Daemon will show all notes.
    iDisableAllNotes = EFalse;
    
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
    delete iHbProgressDialog;
    delete iHbProgressDialogForUninstaller;                          
    if ( iWatcher )
        {
        iWatcher->StopWatcher();
        delete iWatcher;
        }
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
    
    if ( iDisableAllNotes == EFalse )
        {    
        CHbDeviceNotificationDialogSymbian* notificationDialog = 
                CHbDeviceNotificationDialogSymbian::NewL( NULL );
        
        CleanupStack::PushL( notificationDialog );
        
//TODO get string from log file.  
        _LIT( KTempIconText,"note_info");
        _LIT( KTempTextTitle,"SW Silent Installer" );
        _LIT( KTempTextForErrorMessage,"Untrusted software was found." ); 
                     
        //notificationDialog->SetTimeout( KHbLongNotificationDialogTimeout );        
        
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
    
    if ( iDisableAllNotes == EFalse )
        {    
        CHbDeviceNotificationDialogSymbian* notificationDialog = 
                CHbDeviceNotificationDialogSymbian::NewL( NULL );
        
        CleanupStack::PushL( notificationDialog );
        
//TODO get string from log file.  
        _LIT( KTempIconText,"note_info");
        _LIT( KTempTextTitle,"SW Silent Installer" );
        _LIT( KTempTextForErrorMessage,"Installation was not completed." ); 
                   
        //notificationDialog->SetTimeout( KHbLongNotificationDialogTimeout );        
        
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
    
    if ( iDisableAllNotes == EFalse )
        {          
        if ( !iIsProgressDialog )
            {    
            iHbProgressDialog = CHbDeviceProgressDialogSymbian::NewL( 
                                   CHbDeviceProgressDialogSymbian::EWaitDialog, 
                                   NULL );
            iIsProgressDialog = ETrue;
            
//TODO get string from log file.            
            _LIT( KTempTextForProgressDialog,"Installing" );
            
            iHbProgressDialog->SetTextL( KTempTextForProgressDialog );            
            iHbProgressDialog->ShowL();
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
void CDialogWrapper::CancelWaitingNoteL()
	{ 
    FLOG( _L("Daemon: CDialogWrapper::CancelWaitingNoteL") );
    
	if ( iIsProgressDialog )
		{
		iHbProgressDialog->Cancel();
		delete iHbProgressDialog;
		iHbProgressDialog = NULL;
		iIsProgressDialog = EFalse;
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
    
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    if ( iDisableAllNotes == EFalse )
        {
        if ( !iIsUninstallerProgressDialog )
            {    
            iHbProgressDialogForUninstaller = 
                    CHbDeviceProgressDialogSymbian::NewL( 
                                   CHbDeviceProgressDialogSymbian::EWaitDialog, 
                                   NULL );
            iIsUninstallerProgressDialog = ETrue;
    //TODO get string from log file.            
            _LIT( KTempTextForProgressDialog,"Uninstalling" );            
            iHbProgressDialogForUninstaller->SetTextL( KTempTextForProgressDialog );            
            iHbProgressDialogForUninstaller->ShowL();
            }
        }
    else if ( iDisableAllNotes )
        {
        // Let watcher to know that waiting note should be shown 
        // after dialogs are enabled.
        iWatcher->RequestToDisplayNote();
        }
    }
//  End of File  
