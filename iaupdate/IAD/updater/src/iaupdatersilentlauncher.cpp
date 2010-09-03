/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles the launching of silent installation.
*
*/



#include "iaupdatersilentlauncher.h"
#include "iaupdaterdefs.h"
#include "iaupdatedebug.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::CIAUpdaterSilentLauncher
// C++ default constructor 
// -----------------------------------------------------------------------------
//
CIAUpdaterSilentLauncher::CIAUpdaterSilentLauncher( RFs& aFs ): iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdaterSilentLauncher::ConstructL()
    {
    
    iOptionsPckg = Usif::COpaqueNamedParams::NewL();
    
    UsifSilentInstallOptionsL( iOptionsPckg );    

    }

// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdaterSilentLauncher* CIAUpdaterSilentLauncher::NewL( RFs& aFs )
    {
    CIAUpdaterSilentLauncher* self = new( ELeave ) CIAUpdaterSilentLauncher( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::~CIAUpdaterSilentLauncher
// C++ Destructor
// -----------------------------------------------------------------------------
//
CIAUpdaterSilentLauncher::~CIAUpdaterSilentLauncher()
    {
    iLauncher.Close();
    
    delete iOptionsPckg;
    
    }

// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::InstallL
// Perform installation.
// -----------------------------------------------------------------------------
//
void CIAUpdaterSilentLauncher::InstallL ( const TDesC& aFile, 
        TRequestStatus& aStatus, Usif::COpaqueNamedParams* aResults  )
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::InstallL() begin"); 
        
    if ( !iConnected )
        {        
        IAUPDATE_TRACE("[IAUpdater] Connect to launcher");
        User::LeaveIfError( iLauncher.Connect() );                            
        iConnected = ETrue;            
        }

    // Launch the installation   
    IAUPDATE_TRACE("[IAUpdater] Launch silent install");                    
    iLauncher.Install( aFile, *iOptionsPckg, *aResults, aStatus );

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::InstallL() end"); 
    }
       
// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::Cancel
// Cancel the current installation.
// -----------------------------------------------------------------------------
// 
void CIAUpdaterSilentLauncher::Cancel()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::Cancel() begin");       

    iLauncher.CancelOperation();

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::Cancel() end"); 
    }

// ---------------------------------------------------------------------------
// IAUpdateUtils::UsifSilentInstallOptionsL
// 
// ---------------------------------------------------------------------------

void  CIAUpdaterSilentLauncher::UsifSilentInstallOptionsL( 
        Usif::COpaqueNamedParams * aOptions )
    {

    aOptions->AddIntL( Usif::KSifInParam_InstallSilently, ETrue );

    // Upgrades are allowed 
    aOptions->AddIntL( Usif::KSifInParam_AllowUpgrade, Usif::EAllowed );
    
    // Install all if optional packets exist.
    aOptions->AddIntL( Usif::KSifInParam_InstallOptionalItems, Usif::EAllowed );
    
    // Prevent online cert revocation check.
    aOptions->AddIntL( Usif::KSifInParam_PerformOCSP, Usif::ENotAllowed );
    
    // See iOCSP setting above
    aOptions->AddIntL( Usif::KSifInParam_IgnoreOCSPWarnings, Usif::EAllowed );
    
    // Do not allow installation of uncertified packages.
    aOptions->AddIntL( Usif::KSifInParam_AllowUntrusted, Usif::ENotAllowed );
    
    // If filetexts are included in SIS package, show them.
    aOptions->AddIntL( Usif::KSifInParam_PackageInfo, Usif::EAllowed );
    
    // Automatically grant user capabilities.
    // See also KSifInParam_AllowUntrusted above.
    aOptions->AddIntL( Usif::KSifInParam_GrantCapabilities, Usif::EAllowed );
    
    // Open application will be closed.
    aOptions->AddIntL( Usif::KSifInParam_AllowAppShutdown, Usif::EAllowed );
    
    // Files can be overwritten.
    aOptions->AddIntL( Usif::KSifInParam_AllowOverwrite, Usif::EAllowed  );
    
    // Incompatible allowed
    aOptions->AddIntL( Usif::KSifInParam_AllowIncompatible, Usif::EAllowed  );
    
    // This only affects Java applications.
    aOptions->AddIntL( Usif::KSifInParam_AllowDownload, Usif::EAllowed  );
    
    // Where to save.
    // Notice! Here we use always the same drive for the IAD.
    // So, this will not change automatically according to the disk spaces 
    // and when new memory cards are inserted into the phone. 
    // TODO: remove comment when parameter buffer increased in USIF
    TDriveUnit driveUnit(IAUpdaterDefs::KIAUpdaterDrive);
    //aOptions->AddIntL( Usif::KSifInParam_Drive, driveUnit );
    
    // Choose the phone language.
    // TODO: remove comment when parameter buffer increased in USIF
    TLanguage lang = User::Language();
    //aOptions->AddIntL( Usif::KSifInParam_Languages, lang ); 
    
    }


// ======== GLOBAL FUNCTIONS ========

// EOF 
