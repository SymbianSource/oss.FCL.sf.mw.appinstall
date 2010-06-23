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


#ifndef M_CATALOGSSMSSESSION_H
#define M_CATALOGSSMSSESSION_H

// INTERNAL INCLUDES
#include "catalogstransportsession.h"
#include "catalogssmstypes.h"
#include "catalogstransportoperationid.h"

// FORWARD DECLARATIONS
class MCatalogsSmsOperation;    // SMS operation
class MCatalogsSmsObserver;     // SMS observer


/**
* Interface for SMS sessions
*/
class MCatalogsSmsSession : public MCatalogsTransportSession
    {            
    public: // New operations
	
	    /**
	    * Creates a new SMS operation and starts it
	    * @param aRecipient Recipient's address
	    * @param aBody Message body
	    * @param aObserver Observer for SMS events. Default: NULL
	    * @return A new SMS operation that sends the given message
	    */
        virtual MCatalogsSmsOperation* CreateSmsL( const TDesC& aRecipient, 
            const TDesC& aBody, MCatalogsSmsObserver* aObserver = NULL) = 0;
            
            
        /**
        * Returns an array of the session's SMS operations
        * @return Array of currently existing SMS operations
        */
        virtual const RCatalogsSmsOperationArray& 
            CurrentSmsOperations() const = 0;
            
        
        /**
        * Returns the SMS operation that matches the given id or returns
        * NULL if no such operation was found.
        *
        * @param aId Operation ID
        *
        * @return SMS operation that matches the ID or NULL
        */
        virtual MCatalogsSmsOperation* SmsOperation( 
            const TCatalogsTransportOperationId& aId ) const = 0;
            
    };

#endif // M_CATALOGSSMSSESSION_H
