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
    FLOG( _L("Daemon: CSilentLauncher::ConstructL: InstallSilently") );
    iSifOptions->AddIntL( Usif::KSifInParam_InstallSilently, ETrue );

    iSifOptions->AddIntL( Usif::KSifInParam_PerformOCSP, EFalse );   
    // Note if upgrade is allowed, see NeedsInstallingL function.
    iSifOptions->AddIntL( Usif::KSifInParam_AllowUpgrade, EFalse );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowUntrusted, EFalse );
    iSifOptions->AddIntL( Usif::KSifInParam_GrantCapabilities, EFalse ); 
    // Defined for the install.
    iSifOptions->AddIntL( Usif::KSifInParam_InstallOptionalItems, ETrue );          
    iSifOptions->AddIntL( Usif::KSifInParam_IgnoreOCSPWarnings, ETrue );   
    iSifOptions->AddIntL( Usif::KSifInParam_DisplayPackageInfo, ETrue );     
    iSifOptions->AddIntL( Usif::KSifInParam_AllowAppShutdown, ETrue );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowDownload, ETrue );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowOverwrite, ETrue );
    iSifOptions->AddIntL( Usif::KSifInParam_AllowOverwrite, ETrue );
    
// TODO: is this defined in USIF?    
    //iSifOptions->AddIntL( Usif::KSifInParam_Languages, NULL );
    
// TODO: remove old params.
    /*
    // Old swinstdefs.inl defines for install.   
    iUpgrade( EPolicyAllowed ),
    iOptionalItems( EPolicyAllowed ),
    iOCSP( EPolicyAllowed ),    
    iIgnoreOCSPWarnings( EPolicyAllowed ),
    iUntrusted( EPolicyNotAllowed ),
    iPackageInfo( EPolicyAllowed ),
    iCapabilities( EPolicyAllowed ),
    iKillApp( EPolicyAllowed ),
    iDownload( EPolicyAllowed ),
    iOverwrite( EPolicyAllowed ),
    iDrive( 'C' ),
    iLang( ELangNone ),
    iUsePhoneLang( ETrue ),
    iUpgradeData( EPolicyAllowed )
    */        
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
void CSilentLauncher::InstallL( RFile& aFileHandle, 
                                const TDesC& aFile, 
                                TRequestStatus& aStatus )
    {
    FLOG( _L("Daemon: CSilentLauncher::InstallL (aFileHandle) START") );
    
    if ( !iConnected )
         {
         FLOG( _L("Daemon: Connect to sif installer server") );    
         User::LeaveIfError( iSWInstallerFW.Connect() );             
         iConnected = ETrue;            
         }
        
    // Set drive for installer.
//    delete iDrive  
//    iDrive = NULL;
//    iDrive = HBufC::NewLC( 8 );
//    TPtr drivePtr = iDrive->Des();
//     
//    TInt driveNumber = 0;
//    TDriveInfo driveInfo;
//    aFileHandle.Drive( driveNumber, driveInfo );
// TODO: how is this used? Is this drive letter?      
//    iSifOptions->AddStringL( Usif::KSifInParam_Drive , *drive );
                                 
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
