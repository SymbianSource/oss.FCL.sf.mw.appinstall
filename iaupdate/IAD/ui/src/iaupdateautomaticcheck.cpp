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
* Description:   This module contains the implementation of CIAUpdateAutomaticCheck class 
*                member functions.
*
*/



//INCLUDES

#include "iaupdateautomaticcheck.h"
#include "iaupdatefirsttimeinfo.h"
#include "iaupdate.hrh"
#include "iaupdateprivatecrkeys.h"



#include <avkon.hrh>
#include <centralrepository.h> 


// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateAutomaticCheck* CIAUpdateAutomaticCheck::NewL()
    {
    CIAUpdateAutomaticCheck* self = CIAUpdateAutomaticCheck::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateAutomaticCheck* CIAUpdateAutomaticCheck::NewLC()
    {
    CIAUpdateAutomaticCheck* self = new( ELeave ) CIAUpdateAutomaticCheck();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
//  CIAUpdateAutomaticCheck::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateAutomaticCheck::ConstructL()
    {
    }    

// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::CIAUpdateAutomaticCheck
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateAutomaticCheck::CIAUpdateAutomaticCheck()  
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::~CIAUpdateAutomaticCheck
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateAutomaticCheck::~CIAUpdateAutomaticCheck()
    {
    }


// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::AcceptAutomaticCheckL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateAutomaticCheck::AcceptAutomaticCheckL()
    {
    TBool acceptChecks = ETrue;
	CIAUpdateFirstTimeInfo* firstTimeInfo = CIAUpdateFirstTimeInfo::NewLC();
	if ( !firstTimeInfo->AutomaticUpdateChecksAskedL() )
	    {
	    if ( AutoUpdateCheckEnabledL() )
	        {
	        // if automatic update check configured as enabled, confirmation is not asked  
	        firstTimeInfo->SetAutomaticUpdatesAskedL();
	        }
	    else
	        {
	        /*TInt ret = IAUpdateDialogUtil::ShowConfirmationQueryL( 
	                                          R_IAUPDATE_TURN_ON_AUTOUPD_CHECKS, 
	                                          R_AVKON_SOFTKEYS_YES_NO );*/ 
	        TInt ret = EAknSoftkeyYes;
	        firstTimeInfo->SetAutomaticUpdatesAskedL();
	        if ( ret == EAknSoftkeyYes )
                {
	            EnableAutoUpdateCheckL( ETrue );
	            }
	        }
	    }
	CleanupStack::PopAndDestroy( firstTimeInfo ); 
    return acceptChecks;
    }

// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::AutoUpdateCheckEnabledL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateAutomaticCheck::AutoUpdateCheckEnabledL()
    {
    TBool enabled = EFalse;
    TInt autoUpdateCheckValue( 0 );
    CRepository* cenrep(  CRepository::NewLC( KCRUidIAUpdateSettings ) );
    User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, 
                                     autoUpdateCheckValue ) );
    CleanupStack::PopAndDestroy( cenrep );
    if ( autoUpdateCheckValue == EIAUpdateSettingValueDisableWhenRoaming ||
         autoUpdateCheckValue == EIAUpdateSettingValueEnable )
        {
        enabled = ETrue;
        }
    return enabled;
    }    
   
// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::EnableAutoUpdateCheckL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAutomaticCheck::EnableAutoUpdateCheckL( TBool aEnable )
    {    
    CRepository* cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
    TInt err = cenrep->StartTransaction( CRepository::EReadWriteTransaction );
    User::LeaveIfError( err );
    cenrep->CleanupCancelTransactionPushL();

    if ( aEnable )
        {
        err = cenrep->Set( KIAUpdateAutoUpdateCheck, 
                           EIAUpdateSettingValueDisableWhenRoaming );
        }
    else
        {
        err = cenrep->Set( KIAUpdateAutoUpdateCheck, 
                           EIAUpdateSettingValueDisable );
        }
    User::LeaveIfError( err );
    
    TUint32 ignore = KErrNone;
    User::LeaveIfError( cenrep->CommitTransaction( ignore ) );
    CleanupStack::PopAndDestroy(); // CleanupCancelTransactionPushL()
    CleanupStack::PopAndDestroy( cenrep );
    }

    
// End of File  
