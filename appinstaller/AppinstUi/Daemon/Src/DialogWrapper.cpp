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
// TODO needs to be removed in 10.1
//#include <AknGlobalNote.h> 
//#include <avkon.rsg>
// TODO maybe removed 10.1
//#include <bautils.h>  // file operations like FileMan
//#include <data_caging_path_literals.hrh> // resource paths
// TODO probably not needed in QT.
//#include <swidaemon.rsg>

#include "DialogWrapper.h"
#include "SWInstDebug.h"

//_LIT( KDaemonResourceFile, "swidaemon.rsc" );

using namespace Swi;

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
//TODO: All resoureces nees to be rewriten for device dialogs (QT)    
    // Get resource file path
    //TFileName fileName;
    //fileName.Copy( TParsePtrC( RProcess().FileName() ).Drive() );
    //fileName.Append( KDC_RESOURCE_FILES_DIR );
    //fileName.Append( KDaemonResourceFile );
    
    // Get language of resource file        
    //BaflUtils::NearestLanguageFile( iFs, fileName );

    // Open resource file
    //iResourceFile.OpenL( iFs, fileName );
    //iResourceFile.ConfirmSignatureL();
  
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
    //iResourceFile.Close();
    
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
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)    
    // Let watcher to know that waiting note is canceled.
    //iWatcher->CancelNoteRequest();
    
    //if ( iDisableAllNotes == EFalse )
    //    {    
    //    HBufC* string = ReadResourceLC( R_DAEMON_UNTRUSTED_FOUND );    
    //    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    //    note->ShowNoteL( EAknGlobalInformationNote, *string );   
    //    CleanupStack::PopAndDestroy( 2, string ); 
    //    }
    }

// -----------------------------------------------------------------------------
// CDialogWrapper::ShowErrorResultL
// Show global result dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CDialogWrapper::ShowErrorResultL()
    { 
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    // Let watcher to know that waiting note is canceled.
    iWatcher->CancelNoteRequest();   
    
    if ( iDisableAllNotes == EFalse )
        {
        /*
        HBufC* string = ReadResourceLC( R_DAEMON_INSTALLATION_ERROR );    
        CAknGlobalNote* note = CAknGlobalNote::NewLC();
        note->ShowNoteL( EAknGlobalInformationNote, *string );   
        CleanupStack::PopAndDestroy( 2, string );
        */  
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
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    if ( iDisableAllNotes == EFalse )
        {
        /*
        if ( iNoteId == 0 )
            {            
            HBufC* string = ReadResourceLC( R_DAEMON_INSTALLING );   
            CAknGlobalNote* note = CAknGlobalNote::NewLC();
            note->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
            iNoteId = note->ShowNoteL( EAknGlobalWaitNote, *string );
            CleanupStack::PopAndDestroy( 2, string );           
            }
        */    
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
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    /*
	if ( iNoteId )
		{
		CAknGlobalNote* note = CAknGlobalNote::NewLC();
		note->CancelNoteL( iNoteId );
		iNoteId = 0;
		CleanupStack::PopAndDestroy();
		}
	*/	
    // Let watcher to know that waiting note is canceled.
	iWatcher->CancelNoteRequest();
 	}

// -----------------------------------------------------------------------------
// CDialogWrapper::LoadResourceLC  
// Read resource string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
HBufC* CDialogWrapper::ReadResourceLC( TInt aResourceId )
    {
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    /*
    TResourceReader reader;
    HBufC8* buff = iResourceFile.AllocReadLC( aResourceId );    
    reader.SetBuffer( buff );
    HBufC* text = reader.ReadHBufCL();
    CleanupStack::PopAndDestroy( buff );
    CleanupStack::PushL( text );
    return text;
    */
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
    //TODO: All resoureces nees to be rewriten for device dialogs (QT)   
    if ( iDisableAllNotes == EFalse )
        {
        /*
        if ( iNoteId == 0 )
            {
            HBufC* string = ReadResourceLC( R_UNINSTALLER_INSTALL );   
            CAknGlobalNote* note = CAknGlobalNote::NewLC();
            note->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
            iNoteId = note->ShowNoteL( EAknGlobalWaitNote, *string );
            CleanupStack::PopAndDestroy( 2, string );
            }
        */    
        }
    else if ( iDisableAllNotes )
        {
        // Let watcher to know that waiting note should be shown 
        // after dialogs are enabled.
        iWatcher->RequestToDisplayNote();
        }
    }
//  End of File  
