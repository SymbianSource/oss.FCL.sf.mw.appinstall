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
#include "SWInstDefs.h"
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
    iOptionsPckg = SilentInstallOptionsL();
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
    }

// -----------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::InstallL
// Perform installation.
// -----------------------------------------------------------------------------
//
void CIAUpdaterSilentLauncher::InstallL( const TDesC& aFile, TRequestStatus& aStatus )
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
    iLauncher.SilentInstall( aStatus, aFile, iOptionsPckg );    

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

    iLauncher.CancelAsyncRequest( SwiUI::ERequestSilentInstall );   

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::Cancel() end"); 
    }


// ---------------------------------------------------------------------------
// CIAUpdaterSilentLauncher::SilentInstallOptionsL
// 
// ---------------------------------------------------------------------------
//
SwiUI::TInstallOptions CIAUpdaterSilentLauncher::SilentInstallOptionsL() const
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::SilentInstallOptionsL() begin"); 
    
    SwiUI::TInstallOptions options;

    // Upgrades are allowed        
    options.iUpgrade = SwiUI::EPolicyAllowed;

    // Install all if optional packets exist.
    options.iOptionalItems = SwiUI::EPolicyAllowed;

    // Prevent online cert revocation check.
    options.iOCSP = SwiUI::EPolicyNotAllowed;
    
    // See iOCSP setting above
    options.iIgnoreOCSPWarnings = SwiUI::EPolicyAllowed;

    // Do not allow installation of uncertified packages.
    options.iUntrusted = SwiUI::EPolicyNotAllowed;

    // If filetexts are included in SIS package. Then, show them.
    options.iPackageInfo = SwiUI::EPolicyUserConfirm;
    
    // Automatically grant user capabilities.
    // See also iUntrusted above.
    options.iCapabilities = SwiUI::EPolicyAllowed;

    // Open application will be closed.
    options.iKillApp = SwiUI::EPolicyAllowed;
    
    // Files can be overwritten.
    options.iOverwrite = SwiUI::EPolicyAllowed;
    
    // This only affects Java applications.
    options.iDownload = SwiUI::EPolicyAllowed;

    // Notice! Here we use always the same drive for the IAD.
    // So, this will not change automatically according to the disk spaces 
    // and when new memory cards are inserted into the phone.    
    options.iDrive = IAUpdaterDefs::KIAUpdaterDrive;
    
    // Choose the phone language.
    options.iLang = User::Language();
    
    // If language is asked, then use the current phone language.
    options.iUsePhoneLang = ETrue;
    
    // Does not affect SISX. This is for Java.
    options.iUpgradeData = SwiUI::EPolicyAllowed;

    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterSilentLauncher::SilentInstallOptionsL() end"); 
    
    return options;
    }


// ======== GLOBAL FUNCTIONS ========

// EOF 
