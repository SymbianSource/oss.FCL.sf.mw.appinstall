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
* Description:   This module contains the implementation of CIAUpdateGlobalLockHandler class 
*                member functions.
*
*/


//INCLUDES
#include <e32property.h>
#include <apgwgnam.h>

#include "iaupdategloballockhandler.h"
#include "iaupdateuids.h"
#include "iaupdatedebug.h"

//CONSTANTS
const TUid KPSUid = { KIAUpdateUiUid };
const TUint32 KIAUpdateGlobalLock = 0x00000001;
const TUid KSid = { KIAUpdateUiUid };

//MACROS
_LIT_SECURITY_POLICY_PASS( KReadPolicy );
_LIT_SECURITY_POLICY_S0( KWritePolicy, (TSecureId ) KSid );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateGlobalLockHandler* CIAUpdateGlobalLockHandler::NewL()
    {
    CIAUpdateGlobalLockHandler* self = CIAUpdateGlobalLockHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateGlobalLockHandler* CIAUpdateGlobalLockHandler::NewLC()
    {
    CIAUpdateGlobalLockHandler* self = new( ELeave ) CIAUpdateGlobalLockHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
//  CIAUpdateGlobalLockHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateGlobalLockHandler::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::ConstructL() begin");
    TInt err = RProperty::Define( KPSUid, 
                                  KIAUpdateGlobalLock, 
                                  RProperty::EInt,
                                  KReadPolicy,
                                  KWritePolicy );	
    if ( err != KErrNone && err != KErrAlreadyExists )
        {		
        User::Leave( err );	
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::ConstructL() end");   
    }    

// ---------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::CIAUpdateGlobalLockHandler
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateGlobalLockHandler::CIAUpdateGlobalLockHandler()  
    {
    iEikEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::~CIAUpdateGlobalLockHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateGlobalLockHandler::~CIAUpdateGlobalLockHandler()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::~CIAUpdateGlobalLockHandler() begin");
    if ( iSetAsLocked )
        {
    	TRAP_IGNORE( SetToInUseForAnotherInstancesL( EFalse ) );
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::~CIAUpdateGlobalLockHandler() end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::InUseByAnotherInstanceL
// 
// ---------------------------------------------------------------------------
//  
TBool CIAUpdateGlobalLockHandler::InUseByAnotherInstanceL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::InUseByAnotherInstanceL() begin");
	TBool inUse = EFalse;
	TInt wgId = 0;  //zero value means that global lock is off 
	User::LeaveIfError( RProperty::Get( KPSUid, 
                                        KIAUpdateGlobalLock, 
                                        wgId ) );
    if ( wgId > 0 && wgId != iEikEnv->RootWin().Identifier() )
        {
    	// another iaupdate instance has set a lock
    	// to be checked that instance is still running
    	TInt prevWgId = 0;
    	while ( ( prevWgId != KErrNotFound ) && ( !inUse ) )
    	    {
    	    CApaWindowGroupName::FindByAppUid( KSid, iEikEnv->WsSession(), prevWgId );	
    	    if ( prevWgId == wgId )
    	        {
    	    	inUse = ETrue;
    	    	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::InUseByAnotherInstanceL() IAUPDATE locked by another instance");
    	        }
    	    }
    	}
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::InUseByAnotherInstanceL() end");
	return inUse;
    }
    
// ---------------------------------------------------------------------------
// CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL
// 
// ---------------------------------------------------------------------------
//  
void CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL( TBool aInUse )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL() begin");
    TInt wgId = 0;  //zero value means that global lock is off   
    if ( aInUse )
        {
    	wgId = iEikEnv->RootWin().Identifier();
        }
    
    User::LeaveIfError( RProperty::Set( KPSUid, 
                                        KIAUpdateGlobalLock, 
                                        wgId ) );
    if ( wgId == 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL() Set as unlocked");
    	iSetAsLocked = EFalse;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL() Set as locked");
        iSetAsLocked = ETrue; 	
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateGlobalLockHandler::SetToInUseForAnotherInstancesL() end");
    }
    
// End of File  
