/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32property.h>
#include <SWInstallerInternalPSKeys.h>

#include "programstatus.h"
#include "SWInstDebug.h"

using namespace Swi;


// -----------------------------------------------------------------------
// Two-phased constructor
// -----------------------------------------------------------------------
//        
CProgramStatus* CProgramStatus::NewL( TInt aProcessStatus )
    {
    CProgramStatus* self = NewLC( aProcessStatus );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------
// Two-phased constructor
// -----------------------------------------------------------------------
//      
CProgramStatus* CProgramStatus::NewLC( TInt aProcessStatus )
    {
    CProgramStatus* self = new (ELeave) CProgramStatus;
    CleanupStack::PushL( self );
    self->ConstructL( aProcessStatus );
    return self;    
    }
    

// -----------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------
//      
void CProgramStatus::ConstructL( TInt aProcessStatus )
    {                           
    iGeneralProcessStatus = aProcessStatus;   
    // Define PS keys for UI. 
    DefinePSKeyForUI();
    }


// -----------------------------------------------------------------------
// C++ destructor
// -----------------------------------------------------------------------
//      
CProgramStatus::~CProgramStatus()
    {
    }


// -----------------------------------------------------------------------
// Set program status.
// -----------------------------------------------------------------------
//      
void CProgramStatus::SetProgramStatus( TInt aProcessStatus )
    {
    iGeneralProcessStatus = aProcessStatus;
    }


// -----------------------------------------------------------------------
// Get program status.
// -----------------------------------------------------------------------
//      
TInt CProgramStatus::GetProgramStatus()
    {
    return iGeneralProcessStatus;
    }


// -----------------------------------------------------------------------
// Set program status to idle.
// -----------------------------------------------------------------------
//       
void CProgramStatus::SetProgramStatusToIdle()
    {
    iGeneralProcessStatus = EStateIdle;
    }

// -----------------------------------------------------------------------
// Define PS key for UI and set the key value. 
// -----------------------------------------------------------------------
//      
void CProgramStatus::DefinePSKeyForUI()
    {                
    _LIT_SECURITY_POLICY_C1( KReadProtection, ECapability_None );
    _LIT_SECURITY_POLICY_C1( KWriteProtection, ECapabilityWriteDeviceData );
    
    // Define the property.
    TInt err = RProperty::Define( 
        KPSUidSWInstallerUiNotification, 
        KSWInstallerDisableDaemonNotes, 
        RProperty::EInt,
        KReadProtection,
        KWriteProtection );
    
    FLOG_1( _L("[CProgramStatus] RProperty::Define error = %d "), err );
           
    // If key is not set when Daemon starts, set value.
    if ( err == KErrNone )
        {  
        // Key can be set to False at this point. Correct PS key will 
        // be read in AO's RunL.
        TInt keyValue = EFalse;
        
        err = RProperty::Set( 
            KPSUidSWInstallerUiNotification, 
            KSWInstallerDisableDaemonNotes, 
            keyValue );
        
        FLOG_1( _L("[CProgramStatus] RProperty::Set error = %d "), err );
        }                  
    }

  
//EOF
    
