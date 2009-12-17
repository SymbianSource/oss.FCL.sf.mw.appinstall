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
* Description:   This module contains the implementation of CSWInstServer class 
*                member functions.
*
*/


// INCLUDE FILES
#include <coemain.h>

#include "SWInstServer.h"
#include "SWInstSession.h"
#include "SWInstPrivateUid.h"
#include "SWInstAppUi.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstServer::CSWInstServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstServer::CSWInstServer()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstServer::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstServer* CSWInstServer::NewL()
    {
    CSWInstServer* self = new (ELeave) CSWInstServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();    
    return self;
    }
    
// Destructor
CSWInstServer::~CSWInstServer()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstServer::PrepareForExit
// Prepare the session for exit.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstServer::PrepareForExit( MRequestCallback* aExitCallback )
    {
    // We may assume that this server can have only one connected session
    iSessionIter.SetToFirst();
    CSWInstSession* session = reinterpret_cast<CSWInstSession*>(iSessionIter++);
    if ( session )
        {
        session->PrepareForExit( aExitCallback );        
        }    
    else
        {
        // No connected sessions
        aExitCallback->RequestCompleted( KErrNone );
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstServer::CreateServiceL
// Service creation function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CApaAppServiceBase* CSWInstServer::CreateServiceL( TUid aServiceType ) const
    {
    if ( aServiceType == TUid::Uid( KSWInstInstallServiceUid ) )
        {
        return new (ELeave) CSWInstSession();        
        }
    else if ( aServiceType == TUid::Uid( KSWInstSilentInstallServiceUid ) )
        {
        CCoeEnv* coe = CCoeEnv::Static();
        CSWInstAppUi* appui = reinterpret_cast<CSWInstAppUi*>( coe->AppUi() );
        appui->HideApplicationFromFSW( ETrue );

        return new (ELeave) CSWInstSession();        
        }
    else
        {
        return CAknAppServer::CreateServiceL( aServiceType );        
        }   
    }

// -----------------------------------------------------------------------------
// CSWInstServer::CreateServiceL
// Security check for services.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CPolicyServer::TCustomResult CSWInstServer::CreateServiceSecurityCheckL( TUid aServiceType, 
                                                                         const RMessage2& aMsg, 
                                                                         TInt& aAction, 
                                                                         TSecurityInfo& aMissing )
    {
    if ( aServiceType == TUid::Uid( KSWInstInstallServiceUid ) )
        {
        // No capabilities needed for normal installation
        return CPolicyServer::EPass;
        }
    else if ( aServiceType == TUid::Uid( KSWInstSilentInstallServiceUid ) )
        {
        if ( aMsg.HasCapability( TCapability( ECapabilityTrustedUI ) ) )
            {
            return CPolicyServer::EPass;                
            }
        else
            {
            return CPolicyServer::EFail;
            }          
        }
    else
        {
        return CAknAppServer::CreateServiceSecurityCheckL( aServiceType, aMsg, aAction, aMissing );
        }    
    }



//  End of File  
