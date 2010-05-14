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
* Description:   ?Description
*
*/


#ifndef C_NCDSENDHTTPREQUESTOPERATIONPROXY_H
#define C_NCDSENDHTTPREQUESTOPERATIONPROXY_H

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdsendhttprequestoperation.h"
#include "ncdoperationdatatypes.h"

class MNcdSendHttpRequestOperationObserver;


class CNcdSendHttpRequestOperationProxy :
    public CNcdOperation< MNcdSendHttpRequestOperation >
    {
public:

        
    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     */
    static CNcdSendHttpRequestOperationProxy* NewLC(        
        MCatalogsClientServer& aSession,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        CNcdNodeManagerProxy& aNodeManager,
        MNcdSendHttpRequestOperationObserver& aObserver );



        
public: // From MNcdSendHttpRequestOperation
    
    /**
     * @see MNcdSendHttpRequestOperation::ResponseL()
     */
    HBufC8* ResponseL() const;
    

public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;

    
protected: // Constructor and destructor

    /**
     * Constructor
     *
     */
    CNcdSendHttpRequestOperationProxy( 
        MNcdSendHttpRequestOperationObserver& aObserver );
    
    virtual ~CNcdSendHttpRequestOperationProxy();


    /**
     * ConstructL
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     */
    void ConstructL( MCatalogsClientServer& aSession,
                     TInt aHandle,
                     MNcdOperationProxyRemoveHandler& aRemoveHandler,
                     CNcdNodeManagerProxy& aNodeManager );
    
    
protected: // From CNcdBaseOperationProxy


    /**
     * @see CNcdBaseOperationProxy::ProgressCallback()
     */
    void ProgressCallback();
    
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback()
     */
    void QueryReceivedCallback( CNcdQuery* aQuery );
    
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback()
     */
    void CompleteCallback( TInt aError );
        
    
private: // data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    MNcdSendHttpRequestOperationObserver& iObserver;
        
    };

#endif // C_NCDSENDHTTPREQUESTOPERATIONPROXY_H
