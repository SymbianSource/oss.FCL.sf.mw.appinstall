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
* Description:   This module contains the implementation of CIAUpdateInstallationListener class 
*                member functions.
*
*/


//INCLUDES
#include <e32property.h>
#include <swi/swispubsubdefs.h>

#include "iaupdateinstallationlistener.h"
#include "iaupdaterefreshobserver.h"
#include "iaupdatedebug.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateInstallationListener::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateInstallationListener* CIAUpdateInstallationListener::NewL()
    {
    CIAUpdateInstallationListener* self = CIAUpdateInstallationListener::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateInstallationListener::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateInstallationListener* CIAUpdateInstallationListener::NewLC()
    {
    CIAUpdateInstallationListener* self = new( ELeave ) CIAUpdateInstallationListener();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
//  CIAUpdateInstallationListener::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateInstallationListener::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::ConstructL() begin");
    CActiveScheduler::Add( this );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::ConstructL() end");   
    }    

// ---------------------------------------------------------------------------
// CIAUpdateInstallationListener::CIAUpdateInstallationListener
// constructor
// ---------------------------------------------------------------------------
//

CIAUpdateInstallationListener::CIAUpdateInstallationListener()  
: CActive( CActive::EPriorityStandard )
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateInstallationListener::~CIAUpdateInstallationListener
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateInstallationListener::~CIAUpdateInstallationListener()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::~CIAUpdateInstallationListener() begin");
    
    Cancel();
    iProperty.Close();
  
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::~CIAUpdateInstallationListener() end");    
    }

// ---------------------------------------------------------------------------
// CIAUpdateInstallationListener::StartListeningL()
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateInstallationListener::StartListeningL( MIAUpdateRefreshObserver* aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::StartListeningL() begin");
    
    if ( !iObserver )
        {
    	iObserver = aObserver;
	    User::LeaveIfError( iProperty.Attach( KUidSystemCategory, 
	                                          Swi::KUidSoftwareInstallKey ) );
        }
   	Cancel();
	iProperty.Subscribe( iStatus );
    SetActive();

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::StartListeningL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateInstallationListener::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateInstallationListener::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::DoCancel() begin");
    
    iProperty.Cancel();
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::DoCancel() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateInstallationListener::RunL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateInstallationListener::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::RunL() begin");
   	iProperty.Subscribe( iStatus );
    SetActive();
    TInt propertyValue = 0;
	User::LeaveIfError( RProperty::Get( KUidSystemCategory, 
                                        Swi::KUidSoftwareInstallKey, 
                                        propertyValue ) );
    // Just simple check when installer is returned to idle state.
    // Installation Status value is not checked at all because P&S event may be received by IAD 
    // so late that status value is already overwritten.
    if ( propertyValue == 0 ) 
        {
    	iObserver->HandleUiRefreshL();    	
    	}
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::RunL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateInstallationListener::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateInstallationListener::RunError( TInt /*aError*/ )
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateInstallationListener::RunError()");
	return KErrNone;
    }
    
// End of File  
