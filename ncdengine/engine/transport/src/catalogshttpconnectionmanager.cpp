/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "catalogshttpconnectionmanager.h"

#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogstransportoperationid.h"
#include "catalogshttpconnectioncreator.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========
 
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCatalogsHttpConnectionManager* CCatalogsHttpConnectionManager::NewL(
    MCatalogsHttpSession& aSession )
    {
    CCatalogsHttpConnectionManager* self = new( ELeave ) 
        CCatalogsHttpConnectionManager( aSession );
    return self;        
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpConnectionManager::~CCatalogsHttpConnectionManager()
    {
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpConnectionManager::SetConnectionConfirmationObserver( 
    MCatalogsHttpConnectionConfirmationObserver* aObserver )
    {
    iConfirmationObserver = aObserver;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpConnectionManager::SetConnectionErrorObserver( 
    MCatalogsHttpConnectionErrorObserver* aObserver )
    {
    iErrorObserver = aObserver;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpConnectionManager::SetConnectionConfirmationState( 
    const TCatalogsHttpConnectionConfirmationState& aState )
    {
    DLTRACEIN(("aState: %d", aState));
    iConfirmationState = aState;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TCatalogsHttpConnectionConfirmationState 
    CCatalogsHttpConnectionManager::ConnectionConfirmationState()
    {
    DLTRACEIN(("State: %d", iConfirmationState));
    return iConfirmationState;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TCatalogsConnectionMethod& 
    CCatalogsHttpConnectionManager::DefaultConnectionMethod() const
    {    
    return iDefaultAccessPoint;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpConnectionManager::SetDefaultConnectionMethod( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));
    DLTRACE(("Setting default as %d, %u, %u",
        aMethod.iType,
        aMethod.iId,
        aMethod.iApnId ));
        
    iDefaultAccessPoint = aMethod;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpConnectionManager::SetAccessPointForDefaultConnectionMethod( 
    const TCatalogsConnectionMethod& aMethod )
    {
    if ( aMethod.Match( DefaultConnectionMethod() ) && 
         !iDefaultAccessPoint.iApnId ) 
        {
        iDefaultAccessPoint.iApnId = aMethod.iApnId;
        iCurrentAccessPoint = aMethod.iApnId;
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TUint32 CCatalogsHttpConnectionManager::CurrentAccessPoint() const
    {    
    return iCurrentAccessPoint;
    }

    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CCatalogsHttpConnectionManager::AskConnectionConfirmation( 
    const TCatalogsConnectionMethod& aMethod )
    {
    DLTRACEIN((""));    
    TBool confirmed = ETrue;
    
    if ( iConfirmationState == ECatalogsHttpConnectionDenied ) 
        {
        confirmed = EFalse;
        }
    else if ( 
        iConfirmationState == ECatalogsHttpConnectionConfirmationRequired &&
        iConfirmationObserver )
        {
        DLTRACE(("Confirmation required and observer exists"));
        TRAPD( err, iConfirmationState = 
            iConfirmationObserver->HandleConnectionConfirmationRequestL(
                iSession,
                aMethod ) );
                
        if ( iConfirmationState == ECatalogsHttpConnectionDenied || 
            err != KErrNone )
            {
            DLINFO(("Connection denied, err: %d", err));
            confirmed = EFalse;
            }
        }
    
    DLTRACEOUT(("Connection confirmed: %d", confirmed ));
    return confirmed;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TInt CCatalogsHttpConnectionManager::ReportConnectionError( 
    const TCatalogsConnectionMethod& aMethod,
    TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    TInt err = KErrNone;
    if ( iErrorObserver ) 
        {
        DLTRACE(("Calling error observer"));
        TRAP( err, iErrorObserver->HandleConnectionErrorL( 
            iSession, aMethod, aError ) );
        }
    return err;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CCatalogsHttpConnectionManager::CCatalogsHttpConnectionManager(
    MCatalogsHttpSession& aSession ) :
    iSession( aSession ),
    iConfirmationState( ECatalogsHttpConnectionConfirmationRequired )    
    {
    }
