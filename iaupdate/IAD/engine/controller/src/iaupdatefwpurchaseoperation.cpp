/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateFwPurchaseOperation
*
*/



#include <ncdquery.h>
#include <ncdprogress.h>
#include <ncdoperation.h>
#include <ncdpurchaseoperation.h>
#include <ncdpurchaseoption.h>
#include <ncdnodepurchase.h>
#include <ncdnode.h>
#include <catalogsutils.h>

#include "iaupdatefwpurchaseoperation.h"
#include "iaupdatefwnodeimpl.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::NewL
// 
// ---------------------------------------------------------------------------
//
CIAUpdateFwPurchaseOperation* CIAUpdateFwPurchaseOperation::NewL(
    CIAUpdateFwNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateFwPurchaseOperation* self = 
        CIAUpdateFwPurchaseOperation::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateFwPurchaseOperation* CIAUpdateFwPurchaseOperation::NewLC(
    CIAUpdateFwNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateFwPurchaseOperation* self =
        new( ELeave ) CIAUpdateFwPurchaseOperation( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::CIAUpdateFwPurchaseOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdateFwPurchaseOperation::CIAUpdateFwPurchaseOperation(
    CIAUpdateFwNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateNormalContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::CIAUpdateFwPurchaseOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwPurchaseOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::ConstructL begin");
    
    CIAUpdateNormalContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::ConstructL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::~CIAUpdateFwPurchaseOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdateFwPurchaseOperation::~CIAUpdateFwPurchaseOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::~CIAUpdateFwPurchaseOperation begin");

    // This will also reset and delete all the necessary data if needed.
    CancelOperation();
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::~CIAUpdateFwPurchaseOperation end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::PurchaseProgress
// Content purchase functions
// ---------------------------------------------------------------------------
//
void CIAUpdateFwPurchaseOperation::PurchaseProgress( 
    MNcdPurchaseOperation& /*aOperation*/, 
    TNcdProgress /*aProgress*/ )
    {
    // Purchase progress is not observed.
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::QueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwPurchaseOperation::QueryReceived( 
    MNcdPurchaseOperation& aOperation, 
    MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::QueryReceived() begin");

    ContentQueryReceived( aOperation, aQuery );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::QueryReceived() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::OperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFwPurchaseOperation::OperationComplete( 
    MNcdPurchaseOperation& aOperation, 
    TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateFwPurchaseOperation::OperationComplete() begin: %d",
                     aError);

    if ( aError != KErrNone )
        {
        IAUPDATE_TRACE("[IAUPDATE] Purchase error. Update purchase history.");
        // In a purchase operation error case, NCD Engine does not update the
        // purchase history information. To make sure that purchase history 
        // contains the error code for the operation, insert it here.
        TRAP_IGNORE( Node().SetIdleErrorToPurchaseHistoryL( aError, EFalse ) );
        }

    ContentOperationComplete( aOperation, aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::OperationComplete() end");
    }
        

// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::HandleContentL
// 
// ---------------------------------------------------------------------------
//
MNcdOperation* CIAUpdateFwPurchaseOperation::HandleContentL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::HandleContentL begin");

    MNcdOperation* operation( PurchaseL() );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::HandleContentL end");

    return operation;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFwPurchaseOperation::PurchaseL
// 
// ---------------------------------------------------------------------------
//
MNcdOperation* CIAUpdateFwPurchaseOperation::PurchaseL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::PurchaseL() begin");

    MNcdOperation* operation( NULL );
    
    MNcdNodePurchase* purchase( 
        Node().Node().QueryInterfaceLC< MNcdNodePurchase >() );
    
    if ( purchase )
        {
        // Purchase interface exists. So, we can do purchasing.
        if ( !purchase->IsPurchased() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Item was not purchased yet");

            // Only do purchase operation if we have not already done purchasing
            // and the purchase option is usable and not obsolete.
            RCatalogsArray< MNcdPurchaseOption > options( 
                purchase->PurchaseOptionsL() );
            CleanupResetAndDestroyPushL( options );
            
            // Notice, IsUsableL() will leave with KNcdErrorObsolete if the option
            // is obsolete. Let it leave then, because we could not purchase then
            // anyways.
            if( options.Count() == 1 
                && options[ 0 ]->IsFree() 
                && options[ 0 ]->IsUsableL() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Purchase option was correct.");

                operation = purchase->PurchaseL( *options[ 0 ], *this );
                
                if( !operation )
                    {
                    // Could not create the operation for some reason.
                    IAUPDATE_TRACE("[IAUPDATE] ERROR Could not create the operation.");
                    User::Leave( KErrGeneral );
                    }
                }
            else
                {
                // Wrong kind of purchase information gotten from the server.
                IAUPDATE_TRACE("[IAUPDATE] ERROR Wrong purchase info from server.");
                User::Leave( KErrGeneral );
                }
            CleanupStack::PopAndDestroy( &options );
            }
        CleanupStack::PopAndDestroy( purchase );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Purchase interface was not found.");
        User::Leave( KErrNotFound );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateFwPurchaseOperation::PurchaseL() end");

    return operation;
    }

