/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "catalogshttptransactionmanagerimpl.h"

#include "catalogshttpoperation.h"
#include "catalogsutils.h"
#include "catalogshttptransaction.h"
#include "catalogshttpconfigimpl.h"
#include "catalogshttpsessionmanager.h"
#include "catalogshttpstack.h"
#include "catalogshttpconnectionmanager.h"
#include "catalogshttpsessionimpl.h"

#include "catalogsdebug.h"
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//
CCatalogsHttpTransactionManager* CCatalogsHttpTransactionManager::NewL( 
    MCatalogsHttpSessionManager& aSessionManager, 
    CCatalogsHttpSession& aSession,
    CCatalogsHttpConnectionManager& aConnectionManager, 
    TInt32 aSessionId )
    {
    CCatalogsHttpTransactionManager* self = new ( ELeave ) 
        CCatalogsHttpTransactionManager( 
            aSessionManager,
            aSession, 
            aConnectionManager, 
            aSessionId );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpTransactionManager::~CCatalogsHttpTransactionManager()
    {
    iCancellingAll = ETrue;
    // Release transactions
    for( TInt i = 0; i < iTransactions.Count(); ++i ) 
        {
        while( iTransactions[i]->Release() )
            {
            }
        }
    
    iHttpStacks.ResetAndDestroy();
    
    iTransactions.Reset();    
    delete iDefaultConfig;
    iManager.Release();
    }


// ---------------------------------------------------------------------------
// Transaction creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpTransactionManager::CreateTransactionL( 
    const TDesC16& aUrl, MCatalogsHttpObserver* aObserver )
    {
    HBufC8* buf = ConvertUnicodeToUtf8L( aUrl );
    CleanupStack::PushL( buf );
    MCatalogsHttpOperation* operation = CreateTransactionL( *buf, aObserver );
    CleanupStack::PopAndDestroy( buf );
    return operation;
    }
    

// ---------------------------------------------------------------------------
// Transaction creator
// ---------------------------------------------------------------------------
//
MCatalogsHttpOperation* CCatalogsHttpTransactionManager::CreateTransactionL( 
    const TDesC8& aUrl, MCatalogsHttpObserver* aObserver )
    {
   
    // Create a new transaction
    CCatalogsHttpTransaction* transaction = CCatalogsHttpTransaction::NewL( 
        *this, *iDefaultConfig, 
        TCatalogsTransportOperationId( iSessionId, NextOperationId() ) );
    
    // If the given observer != NULL, set it as the observer for the download
    if ( aObserver ) 
        {        
        transaction->Config().SetObserver( aObserver );        
        }
        
    CleanupStack::PushL( transaction );

    // Set the URI for the transaction
    transaction->SetUriL( aUrl );

    // Insert the transaction in id order
    TLinearOrder<MCatalogsHttpOperation> order( 
        &CCatalogsHttpTransactionManager::CompareTransactions );
    
    // Insert the download to the list of downloads    
    User::LeaveIfError( iTransactions.InsertInOrder( transaction, order ) );
    
    CleanupStack::Pop( transaction );
    
    return transaction;        
    }
    

// ---------------------------------------------------------------------------
// Creates a download transaction
// ---------------------------------------------------------------------------
//        
MCatalogsHttpOperation* CCatalogsHttpTransactionManager::CreateDlTransactionL( 
    const TDesC8& aUrl, MCatalogsHttpObserver& aObserver,
    const CCatalogsHttpConfig& aConfig )
    {    
    // Create a new transaction
    CCatalogsHttpTransaction* transaction = CCatalogsHttpTransaction::NewL( 
        *this, aConfig, 
        TCatalogsTransportOperationId( iSessionId, NextOperationId() ), 
        ECatalogsHttpTransactionDownload );
    
    // If the given observer != NULL, set it as the observer for the download
    transaction->Config().SetObserver( &aObserver );        

        
    CleanupStack::PushL( transaction );

    // Set the URI for the transaction
    transaction->SetUriL( aUrl );

    // Insert the transaction in id order
    TLinearOrder<MCatalogsHttpOperation> order( 
        &CCatalogsHttpTransactionManager::CompareTransactions );
    
    // Insert the download to the list of downloads    
    User::LeaveIfError( iTransactions.InsertInOrder( transaction, order ) );
    
    CleanupStack::Pop( transaction );
    
    return transaction;            
    }
    

// ---------------------------------------------------------------------------
// Transaction getter
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CCatalogsHttpTransactionManager::Transaction( 
    const TCatalogsTransportOperationId& aId ) const
    {
    TInt count = iTransactions.Count();
    for ( TInt i = 0; i < count; ++i ) 
        {
        if ( iTransactions[i]->OperationId() == aId ) 
            {
            return iTransactions[i];
            }
        }
    return NULL;
    }
    
    
// ---------------------------------------------------------------------------
// Current transactions getter
// ---------------------------------------------------------------------------
//    
const RCatalogsHttpOperationArray& 
    CCatalogsHttpTransactionManager::CurrentTransactions() const
    {
    return iTransactions;
    }


// ---------------------------------------------------------------------------
// Cancels all transactions
// ---------------------------------------------------------------------------
//
void CCatalogsHttpTransactionManager::CancelAll()
    {
    iCancellingAll = ETrue;
        
    for ( TInt i = iTransactions.Count() - 1; i >= 0; --i ) 
        {
        iTransactions[i]->Cancel();
        }
         
    iTransactions.Reset();

    iCancellingAll = EFalse;            
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpTransactionManager::NotifyCancelAll()
    {
    for ( TInt i = iTransactions.Count() - 1; i >= 0; --i ) 
        {
        iTransactions[i]->NotifyCancel();
        }
    }


// ---------------------------------------------------------------------------
// Returns the default configuration for transactions
// ---------------------------------------------------------------------------
//
MCatalogsHttpConfig& CCatalogsHttpTransactionManager::DefaultConfig() const
    {
    return *iDefaultConfig;
    }


// ---------------------------------------------------------------------------
// Removes the transaction from the manager
// ---------------------------------------------------------------------------
//
void CCatalogsHttpTransactionManager::RemoveOperation( 
    MCatalogsHttpOperation* aOperation )
    {
    DLTRACEIN((""));
    TInt index = KErrNotFound;
    
    // If cancelling all, we don't want to remove them separately 
    // from the list
    if ( !iCancellingAll ) 
        {        
        // Search from transactions
        index = FindTransaction( iTransactions, aOperation );
        
        if ( index != KErrNotFound ) 
            {
            DLTRACE( ( _L("Removing tr: %i"), 
                aOperation->OperationId().Id() ) );
            iTransactions.Remove( index );            
            }
#ifdef _DEBUG
        else 
            {
            DLTRACE( ( _L("Couldn't find tr: %i"), 
                aOperation->OperationId().Id() ) );
            }
#endif            
        }
    DLTRACEOUT( ( _L("Total TRs: %i"), iTransactions.Count() ) );
    }



// ---------------------------------------------------------------------------
// HTTP stack getter
// ---------------------------------------------------------------------------
//
CCatalogsHttpStack* CCatalogsHttpTransactionManager::HttpStackL(
    MCatalogsHttpStackObserver& aObserver )
    {
    DLTRACEIN((""));
    CCatalogsHttpStack* stack = NULL;
    if ( iHttpStacks.Count() ) 
        {
        DLTRACE(("Returning an existing stack"));
        #ifdef CATALOGS_BUILD_CONFIG_DEBUG
        for (TInt i = 0 ; i < iHttpStacks.Count() ; i++ )
            {
            DLINFO(("http stack: %x, index: %d, ap: %d", iHttpStacks[i], i, iHttpStacks[i]->AccessPointID()))
            }
        #endif
        TInt index = iHttpStacks.Count() - 1;
        stack = iHttpStacks[index];
        iHttpStacks.Remove( index );
        stack->SetObserver( &aObserver );
        }
    else 
        {
        DLTRACE(("Creating a new stack"));
        /*
        stack = CCatalogsHttpStack::NewL( &aObserver,
            iManager.ConnectionCreatorL() );
            */
        }
    return stack;
    }


// ---------------------------------------------------------------------------
// Release HTTP stack
// ---------------------------------------------------------------------------
//
void CCatalogsHttpTransactionManager::ReleaseHttpStackL( 
    CCatalogsHttpStack* aStack )
    {
    DLTRACEIN(( "Stack-ptr: %x", aStack ));
    iHttpStacks.AppendL( aStack );
    aStack->SetObserver( NULL );
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
        for (TInt i = 0 ; i < iHttpStacks.Count() ; i++ )
            {
            DLINFO(("http stack: %x, index: %d, ap: %d", iHttpStacks[i], i, iHttpStacks[i]->AccessPointID()))
            }
    #endif
    }


// ---------------------------------------------------------------------------
// Compares two transactions
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpTransactionManager::CompareTransactions( 
    const MCatalogsHttpOperation& aFirst, 
    const MCatalogsHttpOperation& aSecond )
    {    
    return aFirst.OperationId().Id() - aSecond.OperationId().Id();
    }


// ---------------------------------------------------------------------------
// Identity relation
// ---------------------------------------------------------------------------
//
TBool CCatalogsHttpTransactionManager::AreTransactionsEqual( 
    const MCatalogsHttpOperation& aFirst, 
    const MCatalogsHttpOperation& aSecond )
    {    
    return aFirst.OperationId().Id() == aSecond.OperationId().Id();
    }


// ---------------------------------------------------------------------------
// Add a reference
// ---------------------------------------------------------------------------
//
void CCatalogsHttpTransactionManager::AddRef()
    {
    iRefCount++;    
    }


// ---------------------------------------------------------------------------
// Release a reference
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpTransactionManager::Release()
    {
    DLTRACEIN(( "" ));
    iRefCount--;
    if ( !iRefCount ) 
        {
        delete this;
        return 0;
        }
    return iRefCount;
    }


// ---------------------------------------------------------------------------
// Returns the session id
// ---------------------------------------------------------------------------
//
TInt32 CCatalogsHttpTransactionManager::SessionId() const
    {
    return iSessionId;
    }    
    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCatalogsHttpTransactionManager::CCatalogsHttpTransactionManager( 
    MCatalogsHttpSessionManager& aSessionManager,
    CCatalogsHttpSession& aSession,
    CCatalogsHttpConnectionManager& aConnectionManager,
    TInt32 aSessionId ) : 
    iManager( aSessionManager ), 
    iSession( aSession ),
    iConnectionManager( aConnectionManager ), 
    iSessionId( aSessionId ),
    iCancellingAll( EFalse ), 
    iRefCount( 1 )
    {
    iManager.AddRef();
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//    
void CCatalogsHttpTransactionManager::ConstructL()
    {     
    DLTRACEIN((""));
    iDefaultConfig = CCatalogsHttpConfig::NewL();    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// Finds the index for the given transaction in the given array
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpTransactionManager::FindTransaction( 
    const RCatalogsHttpOperationArray& aArray,
    MCatalogsHttpOperation* aTransaction ) const
    {
    DASSERT( aTransaction );
    
    if ( &aArray == &iTransactions ) 
        {       
        // Search for the download
        TLinearOrder<MCatalogsHttpOperation> order( 
            &CCatalogsHttpTransactionManager::CompareTransactions );
        
        TInt index = aArray.FindInOrder( aTransaction, order );    

        return index;
        }
    else
        {
        // Search for the download
        TIdentityRelation<MCatalogsHttpOperation> order( 
            &CCatalogsHttpTransactionManager::AreTransactionsEqual );
        
        TInt index = aArray.Find( aTransaction, order );    

        return index;        
        }
    }


// ---------------------------------------------------------------------------
// Decides whether the download can start or not
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpTransactionManager::StartOperation( 
    MCatalogsHttpOperation* aOperation )
    {        
    return iManager.StartOperation( aOperation, ETrue );
    }


// ---------------------------------------------------------------------------
// Complete the download and resume a transaction from the queue if any
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpTransactionManager::CompleteOperation( 
    MCatalogsHttpOperation* aOperation )
    {    
    DLTRACE(( "Total TRs: %i", iTransactions.Count() ));    
    return iManager.CompleteOperation( aOperation );
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//            
void CCatalogsHttpTransactionManager::ReportConnectionStatus( TBool aActive )
    {
    iSession.ReportConnectionStatus( aActive );
    }


// ---------------------------------------------------------------------------
// Handles priority change
// ---------------------------------------------------------------------------
//            
TInt CCatalogsHttpTransactionManager::OperationPriorityChanged( 
    MCatalogsHttpOperation* aOperation )
    {
    return iManager.OperationPriorityChanged( aOperation );
    }


// -----------------------------------------------------    ----------------------
// 
// ---------------------------------------------------------------------------
//            
CCatalogsHttpConnectionCreator& 
    CCatalogsHttpTransactionManager::ConnectionCreatorL()
    {
    return iManager.ConnectionCreatorL();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//            
CCatalogsHttpConnectionManager& CCatalogsHttpTransactionManager::ConnectionManager()
    {
    return iConnectionManager;
    }


// ---------------------------------------------------------------------------
// Returns the id for the next operation
// ---------------------------------------------------------------------------
//                
TInt CCatalogsHttpTransactionManager::NextOperationId()
    {
    return iOperationId++;
    }

