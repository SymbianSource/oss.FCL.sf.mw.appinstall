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


#ifndef C_CATALOGSHTTPTRANSACTIONMANAGER_H
#define C_CATALOGSHTTPTRANSACTIONMANAGER_H

#include <e32base.h>
#include "catalogshttptransactionmanager.h"
#include "catalogstransportoperationid.h"
#include "catalogshttpsession.h"    // RCatalogsHttpOperationArray

class MCatalogsHttpConfig;
class CCatalogsHttpConfig;
class CCatalogsHttpTransaction;
class MCatalogsHttpOperation;
class MCatalogsHttpObserver;
class MCatalogsHttpSessionManager;
class CCatalogsHttpStack;
class CCatalogsHttpConnectionManager;
class CCatalogsHttpSession;

/**
* HTTP transaction manager
*/
class CCatalogsHttpTransactionManager : public CBase,
    public MCatalogsHttpTransactionManager
    {
    public: // Constructors and destructor
    
        /**
        * Creator
        *
        * @param aSessionId Session ID
        * @return A new download manager
        */
        static CCatalogsHttpTransactionManager* NewL( 
            MCatalogsHttpSessionManager& aSessionManager, 
            CCatalogsHttpSession& aSession,
            CCatalogsHttpConnectionManager& aConnectionManager,
            TInt32 aSessionId );
        
        
        /**
        * Destructor
        */
        ~CCatalogsHttpTransactionManager();
        
    public: // New methods
    
    
	    /**
	    * Creates a new transaction operation
	    * 
	    * @param aUrl URL to connect to
	    * @param aObserver Observer for the transaction. Must be given if the 
	    * default observer in the session configuration is NULL. Default: NULL
	    * 
	    * @return A new transaction operation
	    */
        MCatalogsHttpOperation* CreateTransactionL( const TDesC16& aUrl,
            MCatalogsHttpObserver* aObserver );

        MCatalogsHttpOperation* CreateTransactionL( const TDesC8& aUrl,
            MCatalogsHttpObserver* aObserver );
        
                
        /**
        * Transaction getter
        *
        * Returns the transaction that matches the id or NULL
        * 
        * @param aId Transaction ID
        * @return Transaction or NULL
        */
        MCatalogsHttpOperation* Transaction( 
            const TCatalogsTransportOperationId& aId ) const;
        
        
        /**
        * Returns a list of current transactions
        * @return Transaction list
        */
        const RCatalogsHttpOperationArray& CurrentTransactions() const;
        
        
        /**
        * Cancels all transactions
        */
        void CancelAll();
        
        /**
         * 
         */                
        void NotifyCancelAll();
        
        /**
        * Default transaction configuration getter
        *
        * @return Default transaction configuration
        */
        MCatalogsHttpConfig& DefaultConfig() const;
        
        
    public:        
        
        static TInt CompareTransactions( 
            const MCatalogsHttpOperation& aFirst,
            const MCatalogsHttpOperation& aSecond );
    
        static TBool AreTransactionsEqual( 
            const MCatalogsHttpOperation& aFirst, 
            const MCatalogsHttpOperation& aSecond );
       
    
    public: // From MCatalogsHttpTransactionManager
    
        void AddRef();
        TInt Release();    
        
        /**
        * Session ID getter
        *
        * @return Session id
        */
        TInt32 SessionId() const;
    
        /**
        * Removes the transaction from the manager 
        * @param aTransaction Transaction to remove from the manager
        */
        void RemoveOperation( MCatalogsHttpOperation* aOperation );
           
        /**
         * @see MCatalogsHttpTransactionManager::HttpStackL()
         */    
        CCatalogsHttpStack* HttpStackL(
            MCatalogsHttpStackObserver& aObserver );
        

        /**
         * @see MCatalogsHttpTransactionManager::ReleaseHttpStackL()
         */    
        void ReleaseHttpStackL( CCatalogsHttpStack* aStack );
                
        
        /**
        * @see MCatalogsHttpManager::StartOperation()
        */        
        TInt StartOperation( MCatalogsHttpOperation* aOperation );
                
        /**
        * @see MCatalogsHttpManager::CompleteOperation()
        */                
        TInt CompleteOperation( MCatalogsHttpOperation* aOperation );
    

        /**
        * @see MCatalogsHttpManager::ReportConnectionStatus()
        */                
        void ReportConnectionStatus( TBool aActive );
    
        /**
        * @see MCatalogsHttpManager::OperationPriorityChanged()
        */
        TInt OperationPriorityChanged( MCatalogsHttpOperation* aOperation );


        /**
        * @see MCatalogsHttpManager::ConnectionCreatorL
        */
        CCatalogsHttpConnectionCreator& ConnectionCreatorL();

        /**
        * @see MCatalogsHttpManager::ConnectionManager
        */
        CCatalogsHttpConnectionManager& ConnectionManager();


        /**
        * @see MCatalogsHttpManager::CreateDlTransactionL()
        */
        MCatalogsHttpOperation* CreateDlTransactionL( const TDesC8& aUrl,
            MCatalogsHttpObserver& aObserver,
            const CCatalogsHttpConfig& aConfig );
        
    private: // Constructors
    
        /**
        * Constructor
        */        
        CCatalogsHttpTransactionManager( 
            MCatalogsHttpSessionManager& aSessionManager,
            CCatalogsHttpSession& aSession, 
            CCatalogsHttpConnectionManager& aConnectionManager,
            TInt32 aSessionId );
        
        /**
        * 2nd phase constructor
        */
        void ConstructL();
        
        
    private:
    
        /**
        * Searches for the given transaction from the given array and returns
        * an index to it or KErrNotFound.
        *
        * @param aArray Array to search from
        * @param aTransaction Transaction to find
        * @return Index to the transaction in the given array
        */      
        TInt FindTransaction( 
            const RCatalogsHttpOperationArray& aArray,
            MCatalogsHttpOperation* aTransaction ) const;


        /**
        * Updates the state of the transaction to the manager
        * @param aTransaction The updates transaction
        * @param aRunning Running state of the transaction
        */
        void SetTransactionState( MCatalogsHttpOperation* aTransaction, 
            TBool aRunning );
        
        
        
        /**
        * Returns the next operation id
        * @return Operation id         
        */
        TInt NextOperationId();
        
        
    private:
    
        MCatalogsHttpSessionManager& iManager;
        CCatalogsHttpSession& iSession;
        CCatalogsHttpConnectionManager& iConnectionManager;
        TInt32 iSessionId;
        
        // Transactions
        RCatalogsHttpOperationArray iTransactions;
        
        RPointerArray<CCatalogsHttpStack> iHttpStacks;
        
        // Queued transactions
        CCatalogsHttpConfig* iDefaultConfig;        
        TBool iCancellingAll;        
        TInt iOperationId;
        TInt iRefCount;
        
    };

#endif // C_CATALOGSHTTPTRANSACTIONMANAGER_H