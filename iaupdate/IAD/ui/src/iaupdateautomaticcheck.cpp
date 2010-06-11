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
#include "iaupdatedialogutil.h"

#include <centralrepository.h> 
#include <hbaction.h>


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
    if ( mDialogUtil )
        {
        delete mDialogUtil;
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::AcceptAutomaticCheckL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAutomaticCheck::AcceptAutomaticCheckL()
    {
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
	        if ( !mDialogUtil )
	            {
	        	mDialogUtil = new IAUpdateDialogUtil(NULL, this);
	            }
	        if ( mDialogUtil )
	            {
	            mPrimaryAction = NULL;
	            mPrimaryAction = new HbAction("Yes");
	            HbAction *secondaryAction = NULL;
	            secondaryAction = new HbAction("No");
	            mDialogUtil->showQuestion(QString("Turn on setting for Automatic update checks?"), mPrimaryAction, secondaryAction);
	            }
 	        }
	    }
	CleanupStack::PopAndDestroy( firstTimeInfo ); 
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

// ---------------------------------------------------------------------------
// CIAUpdateAutomaticCheck::dialogFinished
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAutomaticCheck::dialogFinished(HbAction *action)
    {
    if ( action == mPrimaryAction )
        {
        EnableAutoUpdateCheckL( ETrue ); 
        }
    CIAUpdateFirstTimeInfo* firstTimeInfo = CIAUpdateFirstTimeInfo::NewLC();
    firstTimeInfo->SetAutomaticUpdatesAskedL();
    CleanupStack::PopAndDestroy( firstTimeInfo );
    }
// End of File  
