/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CDialogWrapper
*
*/



#include "iaupdaterdialog.h"
#include "iaupdaterdefs.h"
#include "iaupdatercancelobserver.h"
#include "iaupdatedebug.h"

#include <hbtextresolversymbian.h>


_LIT(KFilename, "iaupdate.ts");
_LIT(KPath, "z://data");
_LIT(KInstalling, "txt_software_info_installing_1"); 

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::CIAUpdaterDialog
// C++ default constructor 
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog::CIAUpdaterDialog( MIAUpdaterCancelObserver& aObserver )
: iObserver ( &aObserver ) 
    {

    }



// -----------------------------------------------------------------------------
// CIAUpdaterDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog* CIAUpdaterDialog::NewL( MIAUpdaterCancelObserver& aObserver )
    {
    CIAUpdaterDialog* self =
        CIAUpdaterDialog::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog* CIAUpdaterDialog::NewLC( MIAUpdaterCancelObserver& aObserver )
    {
    CIAUpdaterDialog* self = 
        new( ELeave ) CIAUpdaterDialog( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdaterDialog::ConstructL()
    {
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ConstructL begin");
    
    // Get resource file path
    iIsResolverSuccess = HbTextResolverSymbian::Init(KFilename, KPath);
    
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ConstructL end");
    }

 
// -----------------------------------------------------------------------------
// CIAUpdaterDialog::~CIAUpdaterDialog()
// C++ Destructor
// -----------------------------------------------------------------------------
//
CIAUpdaterDialog::~CIAUpdaterDialog()
    {
    DestroyGlobalWaitNote();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterDialog::ShowWaitingNoteL  
// Show global waiting note during installing.
// -----------------------------------------------------------------------------
// 
void CIAUpdaterDialog::ShowWaitingNoteL( const TDesC& aName, TInt /*aIndex*/, TInt /*aTotalCount*/ )
	{	
	IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ShowWaitingNoteL begin");
	
	DestroyGlobalWaitNote();
    
    iGlobalWaitNote = CHbDeviceProgressDialogSymbian::NewL(
            CHbDeviceProgressDialogSymbian::EWaitDialog );
    
    // loc: Load string 
    iGlobalResource = HbTextResolverSymbian::LoadL( KInstalling, aName );
    if ( iGlobalResource )
        iGlobalWaitNote->SetTextL( iGlobalResource->Des() );

    // Icon ?
    //iGlobalWaitNote->SetIconNameL(const TDesC& aIconName);
    
    // Button ?
    iGlobalWaitNote->SetButton(ETrue);
    
    iGlobalWaitNote->SetObserver( this );
    iGlobalWaitNote->SetAutoClose(EFalse);
    iGlobalWaitNote->ShowL();
    
	IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::ShowWaitingNoteL end");
	}

	
// -----------------------------------------------------------------------------
// CIAUpdaterDialog::CancelWaitingNoteL  
// Cancel global waiting note after installing.
// -----------------------------------------------------------------------------
// 
void CIAUpdaterDialog::CancelWaitingNoteL()
	{
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::CancelWaitingNoteL begin");
    
    DestroyGlobalWaitNote();
    
    IAUPDATE_TRACE("[IAUpdater] CIAUpdaterDialog::CancelWaitingNoteL end");
	}

void CIAUpdaterDialog::ProgressDialogCancelled(
    const CHbDeviceProgressDialogSymbian* /*aProgressDialog*/ )
    {
    
    iObserver->UserCancel();
    
    }

void CIAUpdaterDialog::ProgressDialogClosed(
    const CHbDeviceProgressDialogSymbian* /*aProgressDialog*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CIAUpdaterDialog::DestroyGlobalWaitNote
// -----------------------------------------------------------------------------
void CIAUpdaterDialog::DestroyGlobalWaitNote()
    {
    if ( iGlobalWaitNote )
        {
        iGlobalWaitNote->Close();
        delete iGlobalWaitNote;
        iGlobalWaitNote = NULL;
        delete iGlobalResource;
        iGlobalResource = NULL;
        }
    }
    
//  EOF  
