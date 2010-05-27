/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "dialogwatcher.h"
#include "DialogWrapper.h"
#include "SWInstDebug.h"

using namespace Swi;

CDialogWatcher* CDialogWatcher::NewL( CDialogWrapper* aDialog )
    {
    CDialogWatcher* self = CDialogWatcher::NewLC( aDialog );
    CleanupStack::Pop( self );
    return self;
    }

CDialogWatcher* CDialogWatcher::NewLC( CDialogWrapper* aDialog )
    {
    CDialogWatcher* self = new(ELeave) CDialogWatcher();
    CleanupStack::PushL( self );
    self->ConstructL( aDialog );
    return self;    
    }

void CDialogWatcher::ConstructL( CDialogWrapper* aDialog )
    {
    iDialogWrapper = aDialog;
    iNoteActive = EFalse;
    iRequestToDisplayNote = EFalse;
    
    FLOG( _L("[CDialogWatcher] ConstructL: iProperty.Attach") );     
    User::LeaveIfError( iProperty.Attach(            
        KPSUidSWInstallerUiNotification, 
        KSWInstallerDisableDaemonNotes ) );
    
    FLOG( _L("[CDialogWatcher] ConstructL: CActiveScheduler::Add") );  
    CActiveScheduler::Add( this );
    }

CDialogWatcher::~CDialogWatcher()
    {
    FLOG( _L("[CDialogWatcher] ~CDialogWatcher") ); 
    FLOG( _L("[CDialogWatcher] ~CDialogWatcher: Cancel subscribe") ); 
    iProperty.Cancel();
    FLOG( _L("[CDialogWatcher] ~CDialogWatcher: Close property") ); 
    iProperty.Close();

    if( IsAdded() ) 
         { 
         FLOG( _L("[CDialogWatcher] ~CDialogWatcher: CActive Deque()") ); 
         // Cancel outstanding request and remove from active scheduler.
         Deque();         
         }                            
    }
 
CDialogWatcher::CDialogWatcher() : CActive( CActive::EPriorityStandard )   
    {
    }

void CDialogWatcher::CancelNoteRequest()
    {
    FLOG( _L("[CDialogWatcher] CancelNoteRequest") ); 
    iRequestToDisplayNote = EFalse;
    }
 
void CDialogWatcher::RequestToDisplayNote()
    {
    FLOG( _L("[CDialogWatcher] RequestToDisplayNote") ); 
    iRequestToDisplayNote = ETrue;
    }

void CDialogWatcher::StartWatcher()
    {
    FLOG( _L("[CDialogWatcher] StartWatcher") );  
    if( !IsActive() )
        {         
        FLOG( _L("[CDialogWatcher] StartWatcher: Start subscribe") );
        // Request PS key change event.
        iProperty.Subscribe( iStatus );
        FLOG( _L("[CDialogWatcher] StartWatcher: SetActive") );
        SetActive();        
        }
    }

void CDialogWatcher::StopWatcher()
    {
    FLOG( _L("[CDialogWatcher] StopWatcher: Cancel subscribe ") );  
    iProperty.Cancel();      
    Cancel();
    iRequestToDisplayNote = EFalse;  
    }

TInt CDialogWatcher::GetPSKeyForUI( TInt& aPSKeyValue )
    {   
    FLOG( _L("[CDialogWatcher] GetPSKeyForUI") ); 
    TInt err = RProperty::Get( 
        KPSUidSWInstallerUiNotification, 
        KSWInstallerDisableDaemonNotes,
        aPSKeyValue );  
    
    FLOG_1( _L("[CDialogWatcher] RProperty::Get error = %d "), err );
    FLOG_1( _L("[CDialogWatcher] PS Key value = %d "), aPSKeyValue );   
    return err;
    }

void CDialogWatcher::DoCancel()
    {
    FLOG( _L("[CDialogWatcher] DoCancel ") ); 
    iProperty.Cancel();
    }

void CDialogWatcher::RunL()
    {
    FLOG( _L("[CDialogWatcher] RunL ") ); 
    TInt psKey = KErrNotFound;
    
    // Get the current PS key.
    TInt err = GetPSKeyForUI( psKey );
    if ( err )
        {
        // If error, let's try ones more.
        err = GetPSKeyForUI( psKey );
        }
    if ( err == KErrNone )
        {        
        if ( psKey )
            {
            FLOG( _L("[CDialogWatcher] RunL: PSKey true -> Cancel notes ") ); 
            // If key is true, cancel all dialogs.
            iDialogWrapper->CancelWaitingNote(); 
            iDialogWrapper->CancelWaitingNoteForUninstaller();
            iDialogWrapper->SetUIFlag( psKey );
            // Issue new request.
            StartWatcher();
            }
        else if ( !psKey && iRequestToDisplayNote )
            {
            FLOG( _L("[CDialogWatcher] RunL: PSKey false -> Show note") ); 
            // If key is false and note should be displayed, 
            // start showing the waiting note.
            iDialogWrapper->SetUIFlag( psKey );
            iDialogWrapper->ShowWaitingNoteL(); 
            iRequestToDisplayNote = EFalse;
            // Issue new request.
            StartWatcher();
            }
        else if ( !psKey )
            {
            FLOG( _L("[CDialogWatcher] RunL: PSKey false -> Set UI flag") ); 
            // If key is true, set UI flag to true and let 
            // dialog wrapper show notes/warnings if needed.
            iDialogWrapper->SetUIFlag( psKey );
            StartWatcher();
            }
        }
    else
        {
        FLOG_1( _L("[CDialogWatcher] RunL: PSKey get error = %d"), err ); 
        FLOG( _L("[CDialogWatcher] RunL: Cancel notes") ); 
        // If we can not read the the PS Key, let's close dialogs, 
        // so we do not block the start applications.
        iDialogWrapper->CancelWaitingNote(); 
        iDialogWrapper->CancelWaitingNoteForUninstaller();
        iDialogWrapper->SetUIFlag( ETrue );
        // Issue new request.
        StartWatcher();
        }            
    }

TInt CDialogWatcher::RunError( TInt aError )
    {
    FLOG_1( _L("[CDialogWatcher] RunError error = %d "), aError );
    return aError;
    }

//EOF

