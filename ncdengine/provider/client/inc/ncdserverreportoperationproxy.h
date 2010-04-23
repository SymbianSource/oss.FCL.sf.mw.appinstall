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


#ifndef C_NCD_SERVER_REPORT_OPERATION_PROXY_H	
#define C_NCD_SERVER_REPORT_OPERATION_PROXY_H


#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdserverreportoperation.h"

class MNcdServerReportOperationObserver;


class CNcdServerReportOperationProxy : 
    public CNcdOperation< MNcdServerReportOperation >
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
    static CNcdServerReportOperationProxy* NewL(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdServerReportOperationObserver& aObserver );

    /**
     * @see CNcdServerReportOperationProxy::NewL
     */    
    static CNcdServerReportOperationProxy* NewLC(
        MCatalogsClientServer& aSession,
        CNcdNodeManagerProxy* aNodeManager,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler& aRemoveHandler,
        MNcdServerReportOperationObserver& aObserver );


public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;


protected: // Constructor and destructor

    /**
     * Default constructor protected.
     */
    CNcdServerReportOperationProxy( MNcdServerReportOperationObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CNcdServerReportOperationProxy();

    /**
     * ConstructL
     */
    virtual void ConstructL(
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
    MNcdServerReportOperationObserver& iObserver;
    
    };

#endif // C_NCD_SERVER_REPORT_OPERATION_PROXY_H
