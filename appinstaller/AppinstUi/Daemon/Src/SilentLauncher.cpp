/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSilentLauncher
*                class member functions.
*
*/


// INCLUDE FILES
#include "SilentLauncher.h"
#include "SWInstDebug.h"
#include "SWInstDefs.h"

using namespace Swi;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSilentLauncher::CSilentLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSilentLauncher::CSilentLauncher( RFs& aFs )
    : iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CSilentLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSilentLauncher::ConstructL()
    {
    iOptions.iUpgrade = SwiUI::EPolicyNotAllowed;
    iOptions.iOCSP = SwiUI::EPolicyNotAllowed;
#ifndef RD_MULTIPLE_DRIVE 
    iOptions.iDrive = 'E';
#endif       
    iOptions.iUntrusted = SwiUI::EPolicyNotAllowed; 
    iOptions.iCapabilities = SwiUI::EPolicyNotAllowed;    
    iOptionsPckg = iOptions;    
    }

// -----------------------------------------------------------------------------
// CSilentLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSilentLauncher* CSilentLauncher::NewL( RFs& aFs )
    {
    CSilentLauncher* self = new( ELeave ) CSilentLauncher( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSilentLauncher::~CSilentLauncher()
    {
    iLauncher.Close();
    }

// -----------------------------------------------------------------------------
// CSilentLauncher::InstallL
// Perform installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSilentLauncher::InstallL( const TDesC& aFile, TRequestStatus& aStatus )
    {
    if ( !iConnected )
        {
        FLOG( _L("Daemon: Connect to installer server") );    
        User::LeaveIfError( iLauncher.Connect() );    
        FLOG( _L("Daemon: Connected to installer server") );
        iConnected = ETrue;            
        }
        
#ifdef RD_MULTIPLE_DRIVE 
    FLOG( _L("Daemon: InstallL: Set drive for install") );   
    // Set drive for installer.          
    iOptions.iDrive = aFile[0];
#endif                

    // Launch the installation
    FLOG( _L("Daemon: Launch install") );
    iLauncher.SilentInstall( aStatus, aFile, iOptionsPckg );
    }
       
// -----------------------------------------------------------------------------
// CSilentLauncher::Cancel
// Cancel the current installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSilentLauncher::Cancel()
    {
    FLOG( _L("Daemon: Install cancel requested") );        
    iLauncher.CancelAsyncRequest( SwiUI::ERequestSilentInstall );   
    }

//  End of File  
