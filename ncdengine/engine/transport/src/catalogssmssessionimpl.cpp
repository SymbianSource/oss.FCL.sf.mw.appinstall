/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "catalogssmssessionimpl.h"
#include "catalogssmsoperationimpl.h"
#include "catalogstransportimpl.h"
#include "catalogstransporttypes.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsSmsSession* CCatalogsSmsSession::NewL( TInt32 aSessionId, 
    CCatalogsTransport& aOwner )
    {
    CCatalogsSmsSession* self = new ( ELeave ) CCatalogsSmsSession(
        aSessionId, aOwner );
    return self;	
    }
	

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//		
CCatalogsSmsSession::~CCatalogsSmsSession()
    {    
    DLTRACEIN(( KNullDesC() ));
    iRemoveAll = ETrue;
    
    for ( TInt i = 0; i < iOperations.Count(); ++i ) 
        {
        // Release operations if the client doesn't
        while ( iOperations[i]->Release() )
            {
            }
        }
    
    // Removes this session from Catalogs Transport
    iOwner.RemoveSession( this );
    iOperations.Close();
    }
		

// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsSession::AddRef()
    {
    iRefCount++;
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsSmsSession::Release()
    {
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }
        

// ---------------------------------------------------------------------------
// Reference count
// ---------------------------------------------------------------------------
//        
TInt CCatalogsSmsSession::RefCount() const
    {
    return iRefCount;
    }

		
// ---------------------------------------------------------------------------
// Operation creator
// ---------------------------------------------------------------------------
//	
MCatalogsSmsOperation* CCatalogsSmsSession::CreateSmsL( const TDesC& aRecipient, 
    const TDesC& aBody, MCatalogsSmsObserver* aObserver )
    {
    DLTRACEIN(( _L("Recipient: %S, body: %S"), &aRecipient, &aBody ));
    // Create a new operation. Give this object as the owner
    CCatalogsSmsOperation* operation = CCatalogsSmsOperation::NewL( *this,
        TCatalogsTransportOperationId( iSessionId, iNextId ),
        aObserver );
    
    
    // Add the operation to operation list    
    if ( iOperations.Append( operation ) != KErrNone ) 
        {
        operation->Release();
        return NULL;
        }
    
    
    // Increase id count after the operation has been succesfully created
    iNextId++;
    
    CleanupStack::PushL( operation );
    operation->StartL( aRecipient, aBody );
    CleanupStack::Pop( operation );
    return operation;
    }


// ---------------------------------------------------------------------------
// Operation getter
// ---------------------------------------------------------------------------
//	
MCatalogsSmsOperation* CCatalogsSmsSession::SmsOperation( 
    const TCatalogsTransportOperationId& aId ) const
    {
    for ( TInt i = 0; i < iOperations.Count(); ++i )
        {
        if( iOperations[i]->OperationId() == aId ) 
            {
            return iOperations[i];
            }
        }
    return NULL;
    }


// ---------------------------------------------------------------------------
// Operation list getter
// ---------------------------------------------------------------------------
//	
const RCatalogsSmsOperationArray& 
    CCatalogsSmsSession::CurrentSmsOperations() const
    {
    return iOperations;
    }


// ---------------------------------------------------------------------------
// Session ID getter
// ---------------------------------------------------------------------------
//	
TInt32 CCatalogsSmsSession::SessionId() const
    {
    return iSessionId;
    }


// ---------------------------------------------------------------------------
// Session type getter
// ---------------------------------------------------------------------------
//	
TInt CCatalogsSmsSession::SessionType() const
    {
    return KCatalogsTransportSmsInterface;
    }


// ---------------------------------------------------------------------------
// Operation remover
// ---------------------------------------------------------------------------
//	
void CCatalogsSmsSession::RemoveOperation( MCatalogsSmsOperation* aOperation )
    {
    DLTRACEIN(( KNullDesC() ));
    DASSERT( aOperation );
    
    if ( !iRemoveAll ) 
        {
        TInt index = iOperations.Find( aOperation );    
        ASSERT( index != KErrNotFound );
        
        iOperations.Remove( index );
        }
    DLTRACEOUT(( KNullDesC() ));
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsSmsSession::CCatalogsSmsSession( TInt32 aSessionId, 
    CCatalogsTransport& aOwner ) : iOwner( aOwner ), iSessionId( aSessionId ),
    iRefCount( 1 ), iNextId( 1 )
    {
    }    
