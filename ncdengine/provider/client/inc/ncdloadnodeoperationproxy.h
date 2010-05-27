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


#ifndef C_NCDLOADNODEOPERATIONPROXY_H
#define C_NCDLOADNODEOPERATIONPROXY_H

#include <e32base.h>

#include "catalogsbaseimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdsearchoperation.h"
#include "ncdoperationimpl.h"
#include "ncdconfigurationdata.h"

class CDesC16ArrayFlat;
class CNcdNodeIdentifier;
class CNcdNodeProxy;
class CNcdNodeManagerProxy;

/**
 *  Load node operation proxy.
 *
 *  Client side implementation for load node operation.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdLoadNodeOperationProxy : 
    public CNcdOperation< MNcdSearchOperation >,
    public MNcdConfigurationData
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
     * @param aNode The node this operation was started from.
     * @param aLocalizer The client localizer.
     */    
    static CNcdLoadNodeOperationProxy* NewL( MCatalogsClientServer& aSession,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        TBool aSearch,
                        MNcdClientLocalizer* aLocalizer );
    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aLocalizer The client localizer.
     */
    static CNcdLoadNodeOperationProxy* NewLC( MCatalogsClientServer& aSession,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        TBool aSearch,
                        MNcdClientLocalizer* aLocalizer );

    /**
     * Adds an observer.
     */
    virtual void AddObserverL( MNcdLoadNodeOperationObserver* aObserver );
    
    /**
     * Removes an observer.
     */
    virtual TInt RemoveObserver( MNcdLoadNodeOperationObserver* aObserver );
    
public: // from MNcdConfigurationData
    
    /**
     * @see MNcdConfigurationData
     */
    virtual HBufC8* ProtocolResponseDataL();
     

public: // from MNcdOperation
    
    /**
     * @see MNcdOperation::OperationType
     */
    TNcdInterfaceId OperationType() const;
    
public: // from MNcdSearchOperation

    /**
     * @see MNcdSearchOperation::SearchRootNodeL
     */
    MNcdNodeContainer* SearchRootNodeL();
    
public: // from MNcdOperation



public: // from CCatalogsBase
    const TAny* QueryInterfaceL( TInt aInterfaceId ) const;

protected:
    
    /**
     * Constructor
     *
     */
    CNcdLoadNodeOperationProxy( TBool aSearch, MNcdClientLocalizer* aLocalizer );
    
    virtual ~CNcdLoadNodeOperationProxy();


    /**
     * ConstructL
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */         
    void ConstructL( MCatalogsClientServer& aSession,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager );

protected: // from CNcdBaseOperationProxy

    /**
     * @see CNcdBaseOperationProxy::HandleCompletedMessage
     */
    virtual void HandleCompletedMessage( 
        TNcdOperationMessageCompletionId aCompletionId,
        RReadStream& aReadStream,
        TInt aDataLength );
    
    /**
     * @see CNcdBaseOperationProxy::ProgressCallback
     */
    virtual void ProgressCallback();
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback
     */
    virtual void QueryReceivedCallback( CNcdQuery* aQuery );
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback
     */
    virtual void CompleteCallback( TInt aError );
    

private: // data
    
    /**
     * Operation observers, for callbacks.
     * Observers not own.
     */    
    RPointerArray<MNcdLoadNodeOperationObserver> iObservers;
    
    /**
     * Recently loaded nodes's ids are temporarily stored here.
     */
    RPointerArray<CNcdNodeIdentifier> iLoadedNodes;
    
    CNcdNodeManagerProxy* iNodeManager;
    
    /**
     * Flag that indicates whether an op is a plain load or search.
     */
    TBool iSearch;
   

    };

#endif // C_NCDLOADNODEOPERATIONPROXY_H
