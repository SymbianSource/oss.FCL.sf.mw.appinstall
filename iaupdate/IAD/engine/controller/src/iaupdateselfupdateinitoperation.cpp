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
* Description:   CIAUpdateSelfUpdateInitOperation 
*
*/



#include <ncdnode.h>

#include "iaupdateselfupdateinitoperation.h"
#include "iaupdatenodeimpl.h"
#include "iaupdateselfupdaterinitializer.h"

#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::NewL
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateSelfUpdateInitOperation* CIAUpdateSelfUpdateInitOperation::NewL(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateSelfUpdateInitOperation* self = 
        CIAUpdateSelfUpdateInitOperation::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::NewLC
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateSelfUpdateInitOperation* CIAUpdateSelfUpdateInitOperation::NewLC(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
    {
    CIAUpdateSelfUpdateInitOperation* self =
        new( ELeave ) CIAUpdateSelfUpdateInitOperation( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::CIAUpdateSelfUpdateInitOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateSelfUpdateInitOperation::CIAUpdateSelfUpdateInitOperation(
    CIAUpdateNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::CIAUpdateSelfUpdateInitOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateSelfUpdateInitOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::ConstructL begin");
    
    CIAUpdateContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::ConstructL end");
    }
    

// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::~CIAUpdateSelfUpdateInitOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateSelfUpdateInitOperation::~CIAUpdateSelfUpdateInitOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::~CIAUpdateSelfUpdateInitOperation begin");
    
    // Notice, that this will also cancel the possible ongoing operation.
    delete iOperation;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::~CIAUpdateSelfUpdateInitOperation end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::SelfUpdateInitComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateSelfUpdateInitOperation::SelfUpdateInitComplete( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::SelfUpdateInitComplete begin");

    // Delete operation if it exists.
    delete iOperation;
    iOperation = NULL;

    if ( aError != KErrNone )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Self update completed with error: %d", aError);

        // Because an error has occurred, the install operation will not continue
        // to the end. So, update the purchase history now.        
        TRAP_IGNORE( 
            Node().SetInstallStatusToPurchaseHistoryL( aError, EFalse ) );
        }

    // Parent will handle the rest.
    OperationComplete( aError );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::~SelfUpdateInitComplete end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::StartOperationL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateSelfUpdateInitOperation::StartOperationL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::StartOperationL begin");
    
    // Initialize variables by using the parent.
    CIAUpdateContentOperation::StartOperationL();
    
    // Start new active object round.
    // Operation may already exist. If the StartL last time left.
    if ( !iOperation )
        {
        // Create new operation because old did not exist.
        // Notice, that casting is safe here, because NewLC function uses
        // CIAUpdateNode which is given to the constructor and for parent class. 
        // So, the node is CIAUpdateNode even if the parent operation class uses
        // pointer to node's parent. 
        CIAUpdateNode& node( static_cast< CIAUpdateNode& >( Node() ) );
        iOperation = CIAUpdateSelfUpdaterInitializer::NewL( node, *this );
        }
    iOperation->StartL();

    SetOperationInUse( ETrue );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::StartOperationL end");
    
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CIAUpdateSelfUpdateInitOperation::CancelOperation
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateSelfUpdateInitOperation::CancelOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::CancelOperation begin");
    
    if ( iOperation )
        {
        // This will also cancel the operation.
        // Notice, that SelfUpdateInitComplete callback is called
        // by the operation when cancel is done. Then, the purchase history
        // is also updated by the KErrCancel value.
        iOperation->Cancel();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdateInitOperation::CancelOperation end");
    }
