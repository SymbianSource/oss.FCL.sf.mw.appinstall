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
* Description:  
*
*/


#ifndef C_NCDCREATEACCESSPOINTOPERATIONPROXY_H	
#define C_NCDCREATEACCESSPOINTOPERATIONPROXY_H	

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdcreateaccesspointoperation.h"


class MNcdCreateAccessPointOperationObserver;

class CNcdCreateAccessPointOperationProxy : 
    public CNcdOperation< MNcdCreateAccessPointOperation >
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
    static CNcdCreateAccessPointOperationProxy* NewL(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdCreateAccessPointOperationObserver& aObserver );

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
    static CNcdCreateAccessPointOperationProxy* NewLC(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdCreateAccessPointOperationObserver& aObserver );

public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;
    
public: // From MNcdCreateAccessPointOperation

    /**
     * @see MNcdOperation::AccessPoint()
     */
    TUint32 AccessPoint() const;

        

protected: // Constructor and destructor

    /**
     * Default constructor protected.
     */
    CNcdCreateAccessPointOperationProxy( MNcdCreateAccessPointOperationObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CNcdCreateAccessPointOperationProxy();

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
    
    void HandleCompletedMessage(
        TNcdOperationMessageCompletionId aCompletionId,
        RReadStream& aReadStream,
        TInt aDataLength );
    
        
    
private: // data

    /** Operation observer, for callbacks. */    
    MNcdCreateAccessPointOperationObserver& iObserver;
    TUint32 iAccessPoint;    
    };

#endif // C_NCDCREATEACCESSPOINTOPERATIONPROXY_H
