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


#ifndef C_NCD_RIGHTS_OBJECT_OPERATION_PROXY_H	
#define C_NCD_RIGHTS_OBJECT_OPERATION_PROXY_H

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdrightsobjectoperation.h"


class MNcdRightsObjectOperationObserver;

class CNcdRightsObjectOperationProxy : 
    public CNcdOperation< MNcdRightsObjectOperation >
    {
public:

    /**
     * Constructor.
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aNodeManager
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aObserver Observer to receive operation progress.
     */    
    static CNcdRightsObjectOperationProxy* NewL(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdRightsObjectOperationObserver& aObserver );

    /**
     * Constructor.
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aNodeManager
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aObserver Observer to receive operation progress.
     */    
    static CNcdRightsObjectOperationProxy* NewLC(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdRightsObjectOperationObserver& aObserver );

public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;

protected: // Constructor and destructor

    /**
     * Default constructor protected.
     */
    CNcdRightsObjectOperationProxy( MNcdRightsObjectOperationObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CNcdRightsObjectOperationProxy();

    /**
     * ConstructL
     */
    void ConstructL(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler );
    
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

    /** Operation observer, for callbacks. */    
    MNcdRightsObjectOperationObserver& iObserver;
    
    };

#endif // C_NCD_RIGHTS_OBJECT_OPERATION_PROXY_H