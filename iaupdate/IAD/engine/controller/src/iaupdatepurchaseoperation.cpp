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
* Description:   CIAUpdatePurchaseOperation 
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

#include "iaupdatepurchaseoperation.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::NewL
// 
// ---------------------------------------------------------------------------
//
CIAUpdatePurchaseOperation* CIAUpdatePurchaseOperation::NewL(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdatePurchaseOperation* self = 
        CIAUpdatePurchaseOperation::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdatePurchaseOperation* CIAUpdatePurchaseOperation::NewLC(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdatePurchaseOperation* self =
        new( ELeave ) CIAUpdatePurchaseOperation( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::CIAUpdatePurchaseOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdatePurchaseOperation::CIAUpdatePurchaseOperation(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateNormalContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::CIAUpdatePurchaseOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdatePurchaseOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::ConstructL begin");
    
    CIAUpdateNormalContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::ConstructL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::~CIAUpdatePurchaseOperation
// 
// ---------------------------------------------------------------------------
//
CIAUpdatePurchaseOperation::~CIAUpdatePurchaseOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::~CIAUpdatePurchaseOperation begin");

    // This will also reset and delete all the necessary data if needed.
    CancelOperation();
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::~CIAUpdatePurchaseOperation end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::PurchaseProgress
// Content purchase functions
// ---------------------------------------------------------------------------
//
void CIAUpdatePurchaseOperation::PurchaseProgress( 
    MNcdPurchaseOperation& /*aOperation*/, 
    TNcdProgress /*aProgress*/ )
    {
    // Purchase progress is not observed.
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::QueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdatePurchaseOperation::QueryReceived( 
    MNcdPurchaseOperation& aOperation, 
    MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::QueryReceived() begin");

    ContentQueryReceived( aOperation, aQuery );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::QueryReceived() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::OperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdatePurchaseOperation::OperationComplete( 
    MNcdPurchaseOperation& aOperation, 
    TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdatePurchaseOperation::OperationComplete() begin: %d",
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
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::OperationComplete() end");
    }
        

// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::HandleContentL
// 
// ---------------------------------------------------------------------------
//
MNcdOperation* CIAUpdatePurchaseOperation::HandleContentL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::HandleContentL begin");

    MNcdOperation* operation( NULL );

    // If node is already installed, then do not purchase it again.
    // Notice, that casting is safe here, because NewLC function uses
    // CIAUpdateNode which is given to the constructor and for parent class. 
    // So, the node is CIAUpdateNode even if the parent operation class uses
    // pointer to node's parent. 
    CIAUpdateNode& node( static_cast< CIAUpdateNode& >( Node() ) );
    if ( !node.IsInstalled() )
        {
        IAUPDATE_TRACE("[IAUPDATE] Create purchase operation.");
        operation = PurchaseL();
        }

    return operation;
    }


// ---------------------------------------------------------------------------
// CIAUpdatePurchaseOperation::PurchaseL
// 
// ---------------------------------------------------------------------------
//
MNcdOperation* CIAUpdatePurchaseOperation::PurchaseL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::PurchaseL() begin");

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

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdatePurchaseOperation::PurchaseL() end");

    return operation;
    }

