/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateBGNotifyHandler class 
*                member functions.
*
*/


//INCLUDES
#include <e32property.h>

#include "iaupdatebgnotifyhandler.h"
#include "iaupdateuids.h"
#include "iaupdatebglogger.h"

//MACROS
_LIT_SECURITY_POLICY_PASS( KReadPolicy );
_LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityWriteDeviceData );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateBGNotifyHandler* CIAUpdateBGNotifyHandler::NewL()
    {
    CIAUpdateBGNotifyHandler* self = CIAUpdateBGNotifyHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateBGNotifyHandler* CIAUpdateBGNotifyHandler::NewLC()
    {
    CIAUpdateBGNotifyHandler* self = new( ELeave ) CIAUpdateBGNotifyHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
//  CIAUpdateBGNotifyHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateBGNotifyHandler::ConstructL()
    {
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::ConstructL() begin");
    CActiveScheduler::Add( this );
    TInt err = RProperty::Define( KPSUidBgc, 
                                  KIAUpdateBGNotifyIndicatorRemove, 
                                  RProperty::EInt,
                                  KReadPolicy,
                                  KWritePolicy );	
    if ( err != KErrAlreadyExists )
        {		
        User::LeaveIfError( err );	
        }
    
    // If not definined, set initial (dummy) value
    if ( err == KErrNone )
        {
        RProperty::Set(KPSUidBgc, KIAUpdateBGNotifyIndicatorRemove, 0);
        }
    
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::ConstructL() end");   
    }    

// ---------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::CIAUpdateBGNotifyHandler
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateBGNotifyHandler::CIAUpdateBGNotifyHandler()  
: CActive( CActive::EPriorityStandard )
    {

    }

// ---------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::~CIAUpdateBGNotifyHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateBGNotifyHandler::~CIAUpdateBGNotifyHandler()
    {
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::~CIAUpdateBGNotifyHandler() begin");
    
    Cancel();
    iProperty.Close();
  
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::~CIAUpdateBGNotifyHandler() end");    
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::StartListeningL()
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateBGNotifyHandler::StartListeningL( MIAUpdateBGNotifyObserver* aObserver )
    {
    
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::StartListeningL() begin");
    
    if ( !iObserver )
        {
    	iObserver = aObserver;
    	User::LeaveIfError( 
    	        iProperty.Attach( KPSUidBgc, KIAUpdateBGNotifyIndicatorRemove ) );
        }
 
   	Cancel();
	iProperty.Subscribe( iStatus );
    SetActive();
    
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::StartListeningL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateBGNotifyHandler::DoCancel()
    {
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::DoCancel() begin");
    
    iProperty.Cancel();
    
    
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::DoCancel() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::RunL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateBGNotifyHandler::RunL()
    {
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::RunL() begin");
   	iProperty.Subscribe( iStatus );
    SetActive();
    
    /* No need to read value so far
    TInt value = 0;
	User::LeaveIfError( RProperty::Get( KPSUidBgc, 
	                                    KIAUpdateBGNotifyIndicatorRemove, 
                                        value ) );
    */
    
    iObserver->HandleIndicatorRemoveL();
    
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::RunL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGNotifyHandler::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateBGNotifyHandler::RunError( TInt /*aError*/ )
    {
    FLOG("[IAUPDATE] CIAUpdateBGNotifyHandler::RunError()");
	return KErrNone;
    }
    
// End of File  
