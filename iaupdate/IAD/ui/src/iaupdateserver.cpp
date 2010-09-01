/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateServer class 
*                member functions.
*
*/



// INCLUDE FILES
#include <coemain.h>

#include "iaupdateserver.h"
#include "iaupdatesession.h"
#include "iaupdateappui.h"
#include "iaupdateuicontroller.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIAUpdateServer::CIAUpdateServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateServer::CIAUpdateServer()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateServer::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateServer* CIAUpdateServer::NewL()
    {
    CIAUpdateServer* self = new (ELeave) CIAUpdateServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateServer::~CIAUpdateServer
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateServer::~CIAUpdateServer()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateServer::CreateServiceL
// Service creation function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CApaAppServiceBase* CIAUpdateServer::CreateServiceL( TUid /*aServiceType*/ ) const
    {
   	return new (ELeave) CIAUpdateSession();	
    }

// -----------------------------------------------------------------------------
// CIAUpdateServer::HandleAllClientsClosed
// All clients are closed. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateServer::HandleAllClientsClosed()
    {
    TBool closingAllowed = ETrue;
    CIAUpdateAppUi* appUi = 
        static_cast<CIAUpdateAppUi*>( CEikonEnv::Static()->EikAppUi() );   
    if ( appUi )
        {
    	if ( appUi->Controller() )
    	    {
    		closingAllowed = appUi->Controller()->ClosingAllowedByClient();
    	    }
        }
    if ( closingAllowed ) 
        {
    	CAknAppServer::HandleAllClientsClosed();
        }
    else
        {
        if ( appUi )
            {
    	    TRAP_IGNORE( appUi->HandleAllClientsClosedL() );
            }	
        }
    }
    

	
//  End of File  
