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
* Description:   This module contains the implementation of 
*                CIAUpdateNetworkRegistration class member functions.
*
*/



//INCLUDES

#include "iaupdatenetworkregistration.h"
#include "iaupdatenetworkregistrationobserver.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIAUpdateNetworkRegistration* CIAUpdateNetworkRegistration::NewL( 
                              MIAUpdateNetworkRegistrationObserver& aObserver )
    {
    CIAUpdateNetworkRegistration* self = new (ELeave) CIAUpdateNetworkRegistration( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// void CIAUpdateNetworkRegistration::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateNetworkRegistration::ConstructL()
    {
    iTelephony = CTelephony::NewL();
    iNetReg = new( ELeave ) CTelephony::TNetworkRegistrationV1;
   	iNetRegPkg = new( ELeave ) CTelephony::TNetworkRegistrationV1Pckg( *iNetReg );
    }    

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::CIAUpdateNetworkRegistration
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateNetworkRegistration::CIAUpdateNetworkRegistration( 
                             MIAUpdateNetworkRegistrationObserver& aObserver )  
    : CActive( CActive::EPriorityStandard ),
      iObserver ( &aObserver ) 
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::~CIAUpdateNetworkRegistration
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateNetworkRegistration::~CIAUpdateNetworkRegistration()
    {
    Cancel();
    delete iTelephony;
    delete iNetReg;
    delete iNetRegPkg;
    }

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::StartMonitoringL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIAUpdateNetworkRegistration::StartMonitoringL() 
    {
    if ( !IsActive() )
        {
    	iTelephony->GetNetworkRegistrationStatus( iStatus, *iNetRegPkg );
    	SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::DoCancel
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateNetworkRegistration::DoCancel()
    {
    iTelephony->CancelAsync( CTelephony::EGetNetworkRegistrationStatusCancel );
    iTelephony->CancelAsync( CTelephony::ENetworkRegistrationStatusChangeCancel );
    }

// ---------------------------------------------------------------------------
// CIAUpdateNetworkRegistration::RunL()
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNetworkRegistration::RunL()
    {
    if ( iStatus.Int() == KErrNotSupported ) 
        {
        // Emulator case
    	iNetReg->iRegStatus = CTelephony::ERegisteredOnHomeNetwork;
        }

    iObserver->StatusMonitored( iNetReg->iRegStatus == CTelephony::ERegisteredRoaming );
   	iTelephony->NotifyChange( iStatus,
                              CTelephony::ENetworkRegistrationStatusChange,
                              *iNetRegPkg );

   	SetActive();
    }


    
// End of File  
