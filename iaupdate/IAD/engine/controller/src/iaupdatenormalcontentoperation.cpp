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
* Description:   CIAUpdateNormalContentOperation 
*
*/



#include <ncdoperation.h>
#include <ncdquery.h>
#include <ncdprogress.h>

#include "iaupdatenormalcontentoperation.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatedebug.h"


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation(
    CIAUpdateBaseNode& aNode,
    MIAUpdateContentOperationObserver& aObserver )
: CIAUpdateContentOperation( aNode, aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::CIAUpdateNormalContentOperation");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateNormalContentOperation::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::ConstructL() begin");

    CIAUpdateContentOperation::ConstructL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::ConstructL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::~CIAUpdateNormalContentOperation
// 
// ---------------------------------------------------------------------------
// 
CIAUpdateNormalContentOperation::~CIAUpdateNormalContentOperation()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::~CIAUpdateNormalContentOperation begin");

    // Do not call CancelOperation here because its implementation depends on 
    // child class implementations. So, this object is deleted, the child classes
    // are already deleted and their implementation is not available.
    // So, call CancelOperation in child classes.

    // Operation is created by using the HandleContentL function.
    // If operation is created, it will be released when child class calls
    // CancelOperation.

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::~CIAUpdateNormalContentOperation end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::StartOperationL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateNormalContentOperation::StartOperationL()
    {
    // Init variables by using the parent.
    CIAUpdateContentOperation::StartOperationL();
    
    // Start new active object round.
    TBool started( EFalse );

    // Start the operation and set the pointer to it.
    // Operation pointer needs to be set for the possible cancellation.
    SetOperation( HandleContentL() );        

    if ( iOperation )
        {
        // Operation exists. So, start it. 
        iOperation->StartOperationL();
        SetOperationInUse( ETrue );
        started = ETrue;
        }
        
    return started;
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::CancelOperation
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNormalContentOperation::CancelOperation()
    {
    if ( iOperation )
        {
        // Cancel the operation and release the current operation.
        // Notice, that this will result to call to ContentOperationComplete
        iOperation->CancelOperation();
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::ContentQueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNormalContentOperation::ContentQueryReceived( 
    MNcdOperation& aOperation, 
    MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::ContentQueryReceived() begin");

    // Operation query received. Always accept queries.
    // Queries should not be requested from this client.
    TInt trapError( KErrNone );
    if ( aQuery )
        {
        TRAP ( trapError, 
               aQuery->SetResponseL( MNcdQuery::EAccepted );
               aOperation.CompleteQueryL( *aQuery ); );
        // Release needs to be called to the query after it is not used.
        aQuery->Release();        
        }

    if ( ( trapError != KErrNone ) || ( !aQuery ) )
        {
        // Error occurred when query was handled.
        // So, operation can not continue.
        // Cancel operation. Notice, that OperationComplete will be called
        // by the operation when cancel is called.
        aOperation.CancelOperation();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::ContentQueryReceived() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateContentOperation::ContentOperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateNormalContentOperation::ContentOperationComplete( 
    MNcdOperation& /*aOperation*/,
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::ContentOperationComplete() begin");

    // Notice, that this releases the old operation.
    SetOperation( NULL );            
    OperationComplete( aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateContentOperation::ContentOperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateNormalContentOperation::SetOperation
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateNormalContentOperation::SetOperation( 
    MNcdOperation* aOperation )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::SetOperation() begin");
    
    if ( iOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Release old operation before setting new one");        
        iOperation->Release();
        }
    
    iOperation = aOperation;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNormalContentOperation::SetOperation() end");
    }
