/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSWInstAppUi 
*                class member functions.
*
*/


// INCLUDE FILES

#include <aknenv.h>
#include <SWInstDefs.h>
#include <f32file.h>

#include "SWInstAppUi.h"
#include "SWInstInstallRequest.h"
#include "SWInstServer.h"
#include "SWInstDefs.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstAppUi::CSWInstAppUi
// -----------------------------------------------------------------------------
//
CSWInstAppUi::CSWInstAppUi()
    {
    SetFullScreenApp( EFalse );
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstAppUi::ConstructL()
    {
    BaseConstructL( EStandardApp | ENoScreenFurniture | ENonStandardResourceFile
            | EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    // Create private path for the process
    iEikonEnv->FsSession().CreatePrivatePath( EDriveC );
    iOkToExit = ETrue;    
    }

// Destructor
CSWInstAppUi::~CSWInstAppUi()
    {    
    if ( iReqObj )
        {
        iReqObj->Cancel();
        delete iReqObj;        
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::RunFileL
// Process file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstAppUi::InstallL( const TDesC& aFileName )
    {
    // Currently only installation supported from here
    __ASSERT_ALWAYS( !iReqObj, User::Leave( KSWInstErrBusy ) );

    TInstallReq params;        
    iReqObj = CSWInstInstallRequest::NewL( ERequestInstall );
    iReqObj->SetCallback( this );
    static_cast<CSWInstInstallRequest*>(iReqObj)->Install( aFileName, params );
    }

void CSWInstAppUi::InstallL( RFile& aFile )
    {
    // Currently only installation supported from here
    __ASSERT_ALWAYS( !iReqObj, User::Leave( KSWInstErrBusy ) );

    TInstallReq params;        
    iReqObj = CSWInstInstallRequest::NewL( ERequestInstall );
    iReqObj->SetCallback( this );

    RFile file;
    file.Duplicate( aFile );    
    
    static_cast<CSWInstInstallRequest*>(iReqObj)->Install( file, params ); // Ownership of file transfers
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::RequestCompleted
// Called when request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstAppUi::RequestCompleted( TInt /*aResult*/ )
    {
    iOkToExit = ETrue;    
   
    RunAppShutter();
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::HandleCommandL
// Handles user commands.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstAppUi::HandleCommandL( TInt aCommand )
    {
    if ( aCommand == EEikCmdExit )
        {
        CSWInstServer* server = reinterpret_cast<CSWInstServer*>(iEikonEnv->AppServer());        
               
        if ( iReqObj && iReqObj->IsActive() && !iCancelInProgress )
            {
            // In this case we have no server running.
            iCancelInProgress = ETrue;                            
            iOkToExit = EFalse;

            // We must cancel the request before we can exit
            iReqObj->Cancel();   
            }
        else if ( server && !iCancelInProgress )
            {            
            // In this case we might have an pending request in the server side.
//            iCancelInProgress = ETrue;
            iOkToExit = EFalse;
            
            // Prepare the server for exit. This will eventually call RequestCompletedL
            // when all requests have been cancelled.
            server->PrepareForExit( this );
            }

        if ( iOkToExit )
            {
            Exit();
            }        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::ProcessCommandParametersL
// Processes shell commands.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSWInstAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/,
                                               TFileName& /*aDocumentName*/,
                                               const TDesC8& /*aTail*/)
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSWInstAppUi::HandleSystemEventL
// Handles system events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstAppUi::HandleSystemEventL( const TWsEvent &aEvent )
    {
    switch (*(TApaSystemEvent*)(aEvent.EventData()))
        {
	case EApaSystemEventShutdown:
            // We need to exit
            HandleCommandL( EEikCmdExit );            
            break;
	default:
            CAknAppUi::HandleSystemEventL(aEvent);
            break;
        }
    }


//  End of File  
