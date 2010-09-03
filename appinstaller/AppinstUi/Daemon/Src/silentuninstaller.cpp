/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include "silentuninstaller.h"
#include "SWInstDefs.h"
#include "SWInstDebug.h"


using namespace Swi;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CSilentUninstaller::CSilentUninstaller( RFs& aFs )
    : iFs( aFs )
    {
    }


// -----------------------------------------------------------------------------
// 2nd constructor
// -----------------------------------------------------------------------------
//
void CSilentUninstaller::ConstructL()
    {
    iConnected = EFalse;     
    iSifOptions = Usif::COpaqueNamedParams::NewL();
    iSifResults = Usif::COpaqueNamedParams::NewL();    
    // Set parameters for silent uninstall.    
    iSifOptions->AddIntL( Usif::KSifInParam_InstallSilently, ETrue );       
    iSifOptions->AddIntL( Usif::KSifInParam_AllowAppShutdown, Usif::EAllowed );    
    iSifOptions->AddIntL( Usif::KSifInParam_AllowAppBreakDependency, Usif::ENotAllowed );     
    }


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSilentUninstaller* CSilentUninstaller::NewL( RFs& aFs )
    {
    CSilentUninstaller* self = new( ELeave ) CSilentUninstaller( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CSilentUninstaller::~CSilentUninstaller()
    {
    FLOG( _L("[CSilentUninstaller] ~CSilentUninstaller") );

    delete iSifOptions;
    delete iSifResults;
    
    if ( iConnected )
        {
        iSWInstallerFW.Close();
        iRegistrySession.Close();
        }    
    }


// -----------------------------------------------------------------------------
// Perform uninstallation.
// -----------------------------------------------------------------------------
//
void CSilentUninstaller::UninstallL( 
    TUid& aUid, 
    TRequestStatus& aReqStatus, 
    TDesC& aMIME )
    {
    FLOG_1( _L("Daemon: UninstallL: UID = 0x%x"), aUid.iUid );
    
    if ( !iConnected )
        {               
        FLOG( _L("[CSilentUninstaller] Connect to sif installer server") );    
        User::LeaveIfError( iSWInstallerFW.Connect() );                         
        FLOG( _L("[CSilentUninstaller] Connect to SisRegistery") );      
        User::LeaveIfError( iRegistrySession.Connect() );                   
        iConnected = ETrue;   
        }
      
    // Set MIME type.
    iSifOptions->AddStringL( Usif::KSifInParam_MimeType, aMIME );  
           
    // Usif need the component ID, so we need to map the package UID to 
    // component ID. To do this simple we need SisRegistry.           
    Usif::TComponentId componentId;
    componentId = iRegistrySession.GetComponentIdForUidL( aUid );
    FLOG_1( _L("Daemon: UninstallL: ComponentId = %d"), componentId );
                                   
    // Launch the installation
    FLOG( _L("[CSilentUninstaller] Launch uninstall") );                 
    iSWInstallerFW.Uninstall( componentId, 
                              *iSifOptions,
                              *iSifResults, 
                              aReqStatus,
                              ETrue );
    }
  

// -----------------------------------------------------------------------------
// Cancel the current installation.
// -----------------------------------------------------------------------------
// 
void CSilentUninstaller::Cancel()
    {
    FLOG( _L("[CSilentUninstaller] Cancel silet uninstall") );              
    iSWInstallerFW.CancelOperation(); 
    }

//  End of File  
