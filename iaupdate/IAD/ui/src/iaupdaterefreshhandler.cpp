/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateRefreshHandler class 
*                member functions.
*
*/


//INCLUDES
#include <e32property.h>
#include <apgwgnam.h>

#include "iaupdaterefreshhandler.h"
#include "iaupdaterefreshobserver.h"
#include "iaupdateinstallationlistener.h"
#include "iaupdateuids.h"
#include "iaupdatedebug.h"

//CONSTANTS
const TUid KPSUid = { KIAUpdateUiUid };
const TUint32 KIAUpdateUiRefresh = 0x00000002;
const TUid KSid = { KIAUpdateUiUid };

//MACROS
_LIT_SECURITY_POLICY_PASS( KReadPolicy );
_LIT_SECURITY_POLICY_S0( KWritePolicy, (TSecureId ) KSid );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateRefreshHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateRefreshHandler* CIAUpdateRefreshHandler::NewL()
    {
    CIAUpdateRefreshHandler* self = CIAUpdateRefreshHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateRefreshHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateRefreshHandler* CIAUpdateRefreshHandler::NewLC()
    {
    CIAUpdateRefreshHandler* self = new( ELeave ) CIAUpdateRefreshHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
//  CIAUpdateRefreshHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateRefreshHandler::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::ConstructL() begin");
    CActiveScheduler::Add( this );
    TInt err = RProperty::Define( KPSUid, 
                                  KIAUpdateUiRefresh, 
                                  RProperty::EInt,
                                  KReadPolicy,
                                  KWritePolicy );	
    if ( err != KErrAlreadyExists )
        {		
        User::LeaveIfError( err );	
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::ConstructL() end");   
    }    

// ---------------------------------------------------------------------------
// CIAUpdateRefreshHandler::CIAUpdateRefreshHandler
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateRefreshHandler::CIAUpdateRefreshHandler()  
: CActive( CActive::EPriorityStandard )
    {
    iEikEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------------------------
// CIAUpdateRefreshHandler::~CIAUpdateRefreshHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateRefreshHandler::~CIAUpdateRefreshHandler()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::~CIAUpdateRefreshHandler() begin");
    
    Cancel();
    iProperty.Close();
    delete iInstallationListener;
  
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::~CIAUpdateRefreshHandler() end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateRefreshHandler::InformRefreshL()
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateRefreshHandler::InformRefreshL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::InformRefreshL() begin");
	TInt wgId = iEikEnv->RootWin().Identifier();
	User::LeaveIfError( RProperty::Set( KPSUid, 
                                        KIAUpdateUiRefresh, 
                                        wgId ) );
	
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::InformRefreshL() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateRefreshHandler::StartListeningL()
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateRefreshHandler::StartListeningL( MIAUpdateRefreshObserver* aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::StartListeningL() begin");
    
    if ( !iObserver )
        {
    	iObserver = aObserver;
	    User::LeaveIfError( iProperty.Attach( KPSUid, KIAUpdateUiRefresh ) );
        }
 
   	Cancel();
	iProperty.Subscribe( iStatus );
    SetActive();
    
    if ( !iInstallationListener )
        {
    	iInstallationListener = CIAUpdateInstallationListener::NewL();
        }
    iInstallationListener->StartListeningL( aObserver );

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::StartListeningL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateRefreshHandler::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateRefreshHandler::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::DoCancel() begin");
    
    iProperty.Cancel();
    
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::DoCancel() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateRefreshHandler::RunL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateRefreshHandler::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::RunL() begin");
   	iProperty.Subscribe( iStatus );
    SetActive();
    TInt wgId = 0;
	User::LeaveIfError( RProperty::Get( KPSUid, 
                                        KIAUpdateUiRefresh, 
                                        wgId ) );
    if (  wgId != iEikEnv->RootWin().Identifier() )
        {
    	// only requests from other IAD instances are handled
    	iObserver->HandleUiRefreshL();    	
    	}
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::RunL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateRefreshHandler::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateRefreshHandler::RunError( TInt /*aError*/ )
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateRefreshHandler::RunError()");
	return KErrNone;
    }
    
// End of File  
