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


#ifndef C_CATALOGSSMSSESSION_H
#define C_CATALOGSSMSSESSION_H

#include <e32base.h>

#include "catalogssmssession.h"


class MCatalogsSmsOperation;    // SMS operation
class MCatalogsSmsObserver;     // SMS observer
class CCatalogsTransport;       // Catalogs Transport


class CCatalogsSmsSession : public CBase, public MCatalogsSmsSession
    {
    public: // Constructors & Destructor
	
	    /**
	    * Creator 
	    * 
	    * @param aSessionId Session ID
	    * @param aOwner Owning object
	    */
        static CCatalogsSmsSession* NewL( TInt32 aSessionId, 
            CCatalogsTransport& aOwner );
		
		/**
		* Destructor
		*/
		~CCatalogsSmsSession();
		
		
    public: // From MCatalogsSmsSession
		        
        TInt AddRef();		
        
        TInt Release();
                
        TInt RefCount() const;    
        		
	    /**
	    * Creates a new SMS operation and starts it
	    * @param aRecipient Recipient's address
	    * @param aBody Message body
	    * @param aObserver Observer for SMS events. Default: NULL
	    * @return A new SMS operation that sends the given message
	    */
        MCatalogsSmsOperation* CreateSmsL( const TDesC& aRecipient, 
            const TDesC& aBody, MCatalogsSmsObserver* aObserver );
            

        /**
        * Returns the SMS operation that matches the given id or returns
        * NULL if no such operation was found.
        *
        * @param aId Operation ID
        *
        * @return SMS operation that matches the ID or NULL
        */
        MCatalogsSmsOperation* SmsOperation( 
            const TCatalogsTransportOperationId& aId ) const;

            
        /**
        * Returns an array of the session's SMS operations
        * @return Array of currently existing SMS operations
        */
        const RCatalogsSmsOperationArray& CurrentSmsOperations() const;  
    
        
    public: // From MCatalogsTransportSession            

        /**
        * Session ID getter
        *
        * @return Session ID
        */
        TInt32 SessionId() const;
    
    
        /**
        * Session type getter
        *
        * @return Session type
        */
        TInt SessionType() const;
    
    
    public: // New operations
    
        /**
        * Removes the operation from the session.
        * Called by the operation's destructor
        */
        void RemoveOperation( MCatalogsSmsOperation* aOperation );
    
    
    protected:
    
        /**
        * Constructor
        */
        CCatalogsSmsSession( TInt32 aSessionId, CCatalogsTransport& aOwner );
        
        
    private:
    
        CCatalogsTransport& iOwner;
        TInt32 iSessionId;
    
        // Current operations
        RCatalogsSmsOperationArray iOperations;
        TInt iRefCount;
        TInt32 iNextId;
        TBool iRemoveAll;
    };


#endif // C_CATALOGSSMSSESSION_H
