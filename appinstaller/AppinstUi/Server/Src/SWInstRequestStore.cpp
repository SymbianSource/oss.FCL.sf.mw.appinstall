/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CSWInstRequestStore 
*                class member functions.
*
*/


// INCLUDE FILES
#include <SWInstDefs.h>

#include "SWInstRequestStore.h"
#include "SWInstRequestObject.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstRequestStore::CSWInstRequestStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstRequestStore::CSWInstRequestStore()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstRequestStore::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstRequestStore* CSWInstRequestStore::NewL()
    {
    CSWInstRequestStore* self = new ( ELeave ) CSWInstRequestStore();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }
    
// Destructor
CSWInstRequestStore::~CSWInstRequestStore()
    {    
    iRequests.ResetAndDestroy();    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::GetRequest
// Returns the request object based on the given id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSWInstRequestObject* CSWInstRequestStore::GetRequest( TInt aRequestId )
    {
    CSWInstRequestObject* obj = NULL;    
    
    for ( TInt index = 0; index < iRequests.Count(); index++ )
        {
        if ( iRequests[index]->ObjectId() == aRequestId )
            {
            obj = iRequests[index];
            break;            
            }
        }

    return obj;
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::RequestCount
// Returns the current total request count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSWInstRequestStore::RequestCount()
    {
    return iRequests.Count();    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::PendingRequestCount
// Returns the current outstanding request count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSWInstRequestStore::PendingRequestCount()
    {
    TInt result( 0 );
    
    for ( TInt index = 0; index < iRequests.Count(); index++ )
        {
        if ( iRequests[index]->IsActive() )
            {
            ++result;        
            }
        }

    return result;    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::AddRequestL
// Adds the given request to the store.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstRequestStore::AddRequestL( CSWInstRequestObject* aRequest )
    {
    if ( iRequests.Count() == KMaxRequestCount )
        {
        User::Leave( KSWInstErrBusy );
        }
    
    if ( !GetRequest( aRequest->ObjectId() ) )
        {
        iRequests.Append( aRequest );        
        }    
   
    else
        {
        User::Leave( KSWInstErrBusy );
        }
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::DestroyRequest
// Destroy a request and remove it from the store.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSWInstRequestStore::DestroyRequest( TInt aRequestId ) 
    {
    TInt result( KErrNotFound );

    for ( TInt index = 0; index < iRequests.Count(); index++ )
        {
        if ( iRequests[index]->ObjectId() == aRequestId )
            {
            CSWInstRequestObject* obj = iRequests[index];
            iRequests.Remove( index );
            delete obj;
            result = KErrNone;
            break;            
            }
        }

    return result;    
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::Flush
// Destroys all completed requests.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CSWInstRequestStore::Flush()
    {
    TInt count = iRequests.Count();
    TInt index( 0 );
        
    while ( count )
        {
        CSWInstRequestObject* obj = iRequests[index];
        if ( !obj->IsActive() )
            {
            iRequests.Remove( index );
            delete obj;
            }        
        else
            {
            ++index;            
            }        
        --count;        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstRequestStore::CancelAllRequests
// Cancels all requests. Asynchronous.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CSWInstRequestStore::CancelAllRequests()
    {
    for ( TInt index = 0; index < iRequests.Count(); index++ )
        {
        if ( iRequests[index]->IsActive() )
            {
            iRequests[index]->Cancel();
            }
        }
    }

//  End of File  
