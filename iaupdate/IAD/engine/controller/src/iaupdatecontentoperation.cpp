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
* Description:   CIAUpdateContentOperation 
*
*/



#include "iaupdatecontentoperation.h"
#include "iaupdatecontentoperationobserver.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatenodedetails.h"

#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::CIAUpdateContentOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperation::CIAUpdateContentOperation( 
    CIAUpdateBaseNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CBase(),
  iNode( aNode ),
  iObserver( aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::CIAUpdateContentOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::ConstructL");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::~CIAUpdateContentOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateContentOperation::~CIAUpdateContentOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::~CIAUpdateContentOperation begin");

    // Do not call CancelOperation here because its implementation depends on 
    // child class implementations. So, this object is deleted, the child classes
    // are already deleted and their implementation is not available.
    // So, call CancelOperation in child classes.

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::~CIAUpdateContentOperation end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::StartL
// 
// ---------------------------------------------------------------------------
//     
TBool CIAUpdateContentOperation::StartOperationL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::StartL begin");
    
    if ( OperationInUse() )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR Operation already existed");
        User::Leave( KErrInUse );
        }

    if ( Node().Details().EmbededDegrades() )
        {
        // Do not continue operation if embedded content degrades already
        // installed content.
        IAUPDATE_TRACE("[IAUPDATE] Embeded content degrades. Leave with KErrPermissionDenied.");
        User::Leave( KErrPermissionDenied );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::StartL end");

    // No operation is started here. So, return EFalse.
    // Child classes should provide their own implementation.
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::OperationComplete
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperation::OperationComplete( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::OperationComplete() begin");

    // Reset and set everything before calling observer complete because
    // the observer may delete this object immediately.

    // Operation has completed. So, it is not in use anymore.
    SetOperationInUse( EFalse );


    // Inform the observer about the completion of the operation. 
    // Give the main node as a parameter.
    Observer().ContentOperationComplete( Node(), aError );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::OperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::OperationProgress
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperation::OperationProgress( 
    TInt aProgress, TInt aMaxProgress )
    {
    Observer().ContentOperationProgress( Node(), aProgress, aMaxProgress );
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::Node
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateBaseNode& CIAUpdateContentOperation::Node() const
    {
    return iNode;
    }
    
    
// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::Observer
// 
// ---------------------------------------------------------------------------
// 
MIAUpdateContentOperationObserver& CIAUpdateContentOperation::Observer() const
    {
    return iObserver;
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::OperationInUse
// 
// ---------------------------------------------------------------------------
// 
TBool CIAUpdateContentOperation::OperationInUse() const
    {
    return iInUse;
    }

// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::SetOperationInUse
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateContentOperation::SetOperationInUse( TBool aInUse )
    {
    iInUse = aInUse;
    }
