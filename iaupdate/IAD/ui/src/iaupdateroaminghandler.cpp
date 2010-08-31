/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of 
*                CIAUpdateRoamingHandler class member functions.
*
*/



//INCLUDES
#include <centralrepository.h>

#include "iaupdateroaminghandler.h"
#include "iaupdateroaminghandlerobserver.h"
#include "iaupdatenetworkregistration.h"
#include "iaupdateprivatecrkeys.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateRoamingHandler* CIAUpdateRoamingHandler::NewL()
    {
    CIAUpdateRoamingHandler* self = new (ELeave) CIAUpdateRoamingHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// void CIAUpdateRoamingHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateRoamingHandler::ConstructL()
    {
    iNwReg = CIAUpdateNetworkRegistration::NewL( *this );
    }    

// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::CIAUpdateRoamingHandler
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateRoamingHandler::CIAUpdateRoamingHandler()
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::~CIAUpdateRoamingHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateRoamingHandler::~CIAUpdateRoamingHandler()
    {
    CancelPreparing();
    delete iNwReg;
    }

// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::StatusMonitored
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateRoamingHandler::StatusMonitored( TBool aRoaming )
    {
	iRoaming = aRoaming;
	if ( iPreparing )
	    {
		iObserver->RoamingHandlerPrepared();
		iObserver = NULL;
		iPreparing = EFalse;
		iPrepared = ETrue;
	    }
    }



// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::PrepareL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateRoamingHandler::PrepareL( MIAUpdateRoamingHandlerObserver& aObserver ) 
    {
    iObserver = &aObserver;
    iPreparing = ETrue; 
    iNwReg->StartMonitoringL();
    }

// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::CancelPreparing
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateRoamingHandler::CancelPreparing()
    {
    if ( iNwReg )
        {
        iNwReg->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::Prepared()
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateRoamingHandler::Prepared() const
    {
    return iPrepared;	
    }


// ---------------------------------------------------------------------------
// CIAUpdateRoamingHandler::IsRoaming()
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateRoamingHandler::IsRoaming()
    {
	return iRoaming;
    }
    


    
// End of File  
