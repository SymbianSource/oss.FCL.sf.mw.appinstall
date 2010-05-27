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


#ifndef C_CATALOGSSMSOPERATION_H
#define C_CATALOGSSMSOPERATION_H

#include <e32base.h>

#include "catalogssmsoperation.h"
#include "catalogssmstypes.h"


class MCatalogsSmsObserver; // SMS observer
class CCatalogsSmsSession;  // SMS session
class CCatalogsSmsSender;   // SMS sender


/**
* Implementation of the MCatalogsSmsOperation-interface
*/
class CCatalogsSmsOperation : public CBase, 
    public MCatalogsSmsOperation
    {
    public: // Constructors & Destructor


        /**
        * Creator
        * 
        * @param aOwner Owner of the operation
        * @param aObserver SMS observer
        *
        * @return A new SMS operation
        */
        static CCatalogsSmsOperation* NewL( CCatalogsSmsSession& aOwner,
            const TCatalogsTransportOperationId& aId,
        	MCatalogsSmsObserver* aObserver );


        /**
        * Destructor
        */
        ~CCatalogsSmsOperation();			


    public: // MCatalogsSmsOperation		
	
        TInt AddRef();		
        
        TInt Release();
                
        TInt RefCount() const;    

	
	    /**
	    * @note Cancelling is done synchronously so it may take a while
	    * @see MCatalogsTransportOperation::Cancel()
	    */
        TInt Cancel();

        TCatalogsTransportProgress Progress() const;	

        /**
        * Returns operation's ID
        *
        * @return Operation ID
        */
        const TCatalogsTransportOperationId& OperationId() const;

    public: // New methods
    
        /**
        * Start the operation
        *
        * @param aRecipient Message recipient
        * @param aBody Message body
        */  
        void StartL( const TDesC& aRecipient, const TDesC& aBody );
    
    
    public:
    
        /**
        * Handles events from CCatalogsSmsSender
        * @param aError Symbian error code
        */
        void HandleSmsSenderEvent( TInt aError );
		
		
    private:	// Constructors

        /**
        * Constructor
        * 
        * @param aOwner Owner of the operation
        * @param aObserver Operation observer
        */
        CCatalogsSmsOperation( CCatalogsSmsSession& aOwner, 
            const TCatalogsTransportOperationId& aId,
            MCatalogsSmsObserver* aObserver );
    
    
    
    private: // Private data
    
        CCatalogsSmsSession& iOwner;        
        TCatalogsTransportOperationId iId;
        MCatalogsSmsObserver* iObserver;     
        CCatalogsSmsSender* iSender;
        TInt iRefCount;   
        TCatalogsSmsEvent iState;  

    };
	
	
#endif // C_CATALOGSSMSOPERATION_H