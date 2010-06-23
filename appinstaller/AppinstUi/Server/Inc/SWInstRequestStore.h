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
* Description:   This file contains the header file of the CSWInstRequestStore 
*                class.
*
*                This class stores all outstanding requests and makes sure that
*                no more than allowed number of simultaneous requests may be
*                issued.
*
*                Currently only 1 outstanding request is supported.
*
*/


#ifndef SWINSTREQUESTSTORE_H
#define SWINSTREQUESTSTORE_H

//  INCLUDES
#include <e32base.h>

namespace SwiUI
{
class CSWInstRequestObject;

const TUint KMaxRequestCount = 1;

// CLASS DECLARATION

/**
*  This class stores all outstanding requests. Currently only 1 simultaneous 
*  request is supported.
*
* @since 3.0
*/
class CSWInstRequestStore : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSWInstRequestStore* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSWInstRequestStore();

    public: // New functions
        
        /**
        * Returns the request object based on the given id.
        * @since 3.0
        * @return Request object. NULL if not found. Client must not delete the object.
        */
        CSWInstRequestObject* GetRequest( TInt aRequestId );
        
        /**
        * Returns the current total request count.
        * @since 3.0
        * @return Request count.
        */
        TInt RequestCount();

        /**
        * Returns the current outstanding request count.
        * @since 3.0
        * @return Request count.
        */
        TInt PendingRequestCount();
        
        /**
        * Adds the given request to the store. Leaves with KSWInstErrBusy if 
        * the maximum number of outstanding requests is exceeded.
        * @since 3.0
        * @param aRequest - The request object.
        */
        void AddRequestL( CSWInstRequestObject* aRequest );

        /**
        * Destroy a request and remove it from the store.
        * @since 3.0
        * @param aIpc - Request id of the object.
        * @return KErrNone, if no error, KErrNotFound if the object is not in the store.
        */
        TInt DestroyRequest( TInt aRequestId );  

        /**
        * Destroys all completed requests.
        * @since 3.0
        */
        void Flush();

        /**
        * Cancels all requests. Asynchronous.
        * @since 3.0
        */
        void CancelAllRequests();        

    private:

        /**
        * C++ default constructor.
        */
        CSWInstRequestStore();
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();        

    private: // Data

        RPointerArray<CSWInstRequestObject> iRequests;
    };
}

#endif      // SWINSTREQUESTSTORE_H   
            
// End of File
