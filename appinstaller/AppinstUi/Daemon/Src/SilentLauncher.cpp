/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
    iConnected = EFalse;   
    iSifOptions = Usif::COpaqueNamedParams::NewL();
    iSifResults = Usif::COpaqueNamedParams::NewL();
    
    // Set needed parameters for silent install.      
    FLOG( _L("Daemon: CSilentLauncher::ConstructL: InstallSilently: ETrue") );
    // Note InstallSilently is not a policy so use boolean ETure.
    iSifOptions->AddIntL( Usif::KSifInParam_InstallSilently, ETrue );

    iSifOptions->AddIntL( Usif::KSifInParam_PerformOCSP, Usif::ENotAllowed );   
    // Note if upgrade is allowed, see NeedsInstallingL function.
    iSifOptions->AddIntL( Usif::KSifInParam_AllowUpgrade, Usif::ENotAllowed );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowUntrusted, Usif::ENotAllowed );
    iSifOptions->AddIntL( Usif::KSifInParam_GrantCapabilities, Usif::ENotAllowed ); 
    // Defined for the install.
    iSifOptions->AddIntL( Usif::KSifInParam_InstallOptionalItems, Usif::EAllowed );          
    iSifOptions->AddIntL( Usif::KSifInParam_IgnoreOCSPWarnings, Usif::EAllowed );            
    iSifOptions->AddIntL( Usif::KSifInParam_AllowAppShutdown, Usif::EAllowed );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowDownload, Usif::EAllowed );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowOverwrite, Usif::EAllowed );        
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

// -----------------------------------------------------------------------------
// CSilentLauncher::~CSilentLauncher()
// Destructor.
// -----------------------------------------------------------------------------
//    
CSilentLauncher::~CSilentLauncher()
    {   
    delete iSifOptions;
    delete iSifResults;
    
    if ( iConnected )
        {
        iSWInstallerFW.Close();
        }
    
    delete iDrive;
    }

// -----------------------------------------------------------------------------
// CSilentLauncher::InstallL
// Perform installation with file handle.
// -----------------------------------------------------------------------------
//
void CSilentLauncher::InstallL( RFile& aFileHandle, TRequestStatus& aStatus )
    {
    FLOG( _L("Daemon: CSilentLauncher::InstallL (aFileHandle) START") );
    
    if ( !iConnected )
         {
         FLOG( _L("Daemon: Connect to sif installer server") );    
         User::LeaveIfError( iSWInstallerFW.Connect() );             
         iConnected = ETrue;            
         }
        
    // Get drive number from file path. by default SWI Daemon installs
    // package to drive where PA sis or SA sis is found.
    TInt driveNumber = 0;
    TDriveInfo driveInfo;
    aFileHandle.Drive( driveNumber, driveInfo );
    FLOG_1( _L("Daemon: Drive number = %d"), driveNumber );

    RArray<TInt> driveArray;
    CleanupClosePushL( driveArray );
    User::LeaveIfError( driveArray.Append( driveNumber ) );        
    iSifOptions->AddIntArrayL( Usif::KSifInParam_Drive, driveArray );
    CleanupStack::PopAndDestroy( &driveArray );
                                     
    FLOG( _L("Daemon: Launch install") );
    iSWInstallerFW.Install( aFileHandle, 
                           *iSifOptions, 
                           *iSifResults,
                           aStatus,
                           ETrue );
          
    FLOG( _L("Daemon: CSilentLauncher::InstallL (aFileHandle) END") );       
    }
       
// -----------------------------------------------------------------------------
// CSilentLauncher::Cancel
// Cancel the current installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSilentLauncher::Cancel()
    {
    FLOG( _L("Daemon: Cancel Install operation") );            
    iSWInstallerFW.CancelOperation();
    }

//  End of File  
