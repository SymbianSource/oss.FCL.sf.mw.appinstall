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
#include "SWInstDebug.h"
#include "SWInstDefs.h"

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
    // Silently kill the app. if needed.
    FLOG(_L("[CSilentUninstaller] iKillApp = SwiUI::EPolicyAllowed;"));
    iOptions.iKillApp = SwiUI::EPolicyAllowed; 
    FLOG(_L("[CSilentUninstaller] iBreakDependency = SwiUI::EPolicyNotAllowed"));
    iOptions.iBreakDependency = SwiUI::EPolicyNotAllowed;         
    iOptionsPckg = iOptions;      
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
    iLauncher.Close();
    }


// -----------------------------------------------------------------------------
// Perform uninstallation.
// -----------------------------------------------------------------------------
//
void CSilentUninstaller::UninstallL( 
    TUid& aUid, 
    TRequestStatus& aReqStatus, 
    TDesC8& aMIME )
    {
    if ( !iConnected )
        {
        FLOG( _L("[CSilentUninstaller] Connect to server") );
        User::LeaveIfError( iLauncher.Connect() );
        iConnected = ETrue;            
        }                
    // Launch the installation
    FLOG( _L("[CSilentUninstaller] Launch uninstall") );
    iLauncher.SilentUninstall( aReqStatus, aUid, iOptionsPckg, aMIME );
    }
  

// -----------------------------------------------------------------------------
// Cancel the current installation.
// -----------------------------------------------------------------------------
// 
void CSilentUninstaller::Cancel()
    {
    FLOG( _L("[CSilentUninstaller] Cancel silet uninstall") );     
    iLauncher.CancelAsyncRequest( SwiUI::ERequestSilentUninstall );   
    }

//  End of File  
