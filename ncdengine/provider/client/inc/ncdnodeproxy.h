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
* Description:   Contains CNcdNodeProxy class
*
*/


#ifndef NCD_NODE_PROXY_H
#define NCD_NODE_PROXY_H


// For streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnode.h"
#include "ncdloadnodeoperationobserver.h"
#include "ncdnodechildoftransparent.h"
#include "ncdnodefavorite.h"
#include "catalogsarray.h"


class CNcdNodeManagerProxy;
class CNcdOperationManagerProxy;
class MNcdOperation;
class CNcdNodeIdentifier;
class CNcdNodeMetadataProxy;
class CNcdNodeUserDataProxy;
class CNcdFavoriteManagerProxy;
class CNcdNodeSeenProxy;


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeProxy: public CNcdInterfaceBaseProxy,
                     public MNcdNode,
                     public MNcdLoadNodeOperationObserver,
                     public MNcdNodeChildOfTransparent,
                     public MNcdNodeFavorite
                     
    {

public:

    /**
     * Destructor
     *
     * @note Usually Release function should be used instead of
     * directly calling delete for this class object.
     *
     * Informs the node manager that this node is deleted.
     */
    virtual ~CNcdNodeProxy();


    /**
     * @return CNcdNodeIdentifier& Identifier of this node.
     */
    CNcdNodeIdentifier& NodeIdentifier() const;


    /** 
     * @return CNcdNodeManagerProxy& Node manager may be used to get
     * or create nodes.
     */
    CNcdNodeManagerProxy& NodeManager() const;


    /** 
     * @return CNcdOperationManagerProxy& Operation manager may be used
     * to get or create operations.
     */
    CNcdOperationManagerProxy& OperationManager() const;
    


    // Getter functions to get the interface class objects.
    // These are provided here so the client side may use
    // all the functionality provided by the classes instead
    // of asking only API interfaces.

    /**
     * @return CNcdNodeMetadataProxy* Metadata object if it exists. 
     * NULL if the metadata has not been set. Ownership is not transferred.
     */
    CNcdNodeMetadataProxy* Metadata() const;


    /**
     * Calls all the internalization functions to set the data for the node.
     * If child classes have additional internalization functions, the child
     * should overload this function. So, all the functions will be called.
     * At least InternalizeNodeDataL is called to initialize this node.
     */ 
    virtual void InternalizeL();
    
    
    /**
     * Adds the node to favorite nodes. If the node is already
     * a favorite node, does nothing.
     *
     * @param aRemoveOnDisconnect If true, the node is removed from favorites when
     * client-server session is closed.
     * @exception Leave System wide error code 
     */    
    void AddToFavoritesL( TBool aRemoveOnDisconnect );
    

public: // MNcdNode

    /**
     * @see MNcdNode::State
     */
    virtual TState State() const;

    
    /**
     * @see MNcdNode::Id
     */
    virtual const TDesC& Id() const;


    /**
     * @see MNcdNodeMetaData::Namespace
     */
    virtual const TDesC& Namespace() const;


    /**
     * @see MNcdNode::CatalogSourceName
     */
    virtual const TDesC& CatalogSourceName() const;


    /**
     * @see MNcdNode::ParentL
     */
    virtual MNcdNodeContainer* ParentL() const;
    

    /**
     * @see MNcdNode::LoadL
     */
    virtual MNcdLoadNodeOperation* LoadL( MNcdLoadNodeOperationObserver& aObserver );


    /**
     * @see MNcdNode::OperationsL
     */
    virtual RCatalogsArray< MNcdOperation > OperationsL() const;


    /**
     * @see MNcdNode::AddToFavoritesL
     */
    virtual void AddToFavoritesL();    


public:// MNcdLoadNodeOperationObserver

    // This interface is used just to check when the load operation
    // is complete. So, the node may update its data from server side
    // by calling the internalize function.
    
    /**
     * @see MNcdLoadNodeOperationObserver::NodesUpdated
     */
    virtual void NodesUpdated( MNcdLoadNodeOperation& aOperation,
                               RCatalogsArray< MNcdNode >& aNodes );
    
    /**
     * @see MNcdLoadNodeOperationObserver::QueryReceived
     */
    virtual void QueryReceived( MNcdLoadNodeOperation& aOperation,
                                MNcdQuery* aQuery );

    /**
     * @see MNcdLoadNodeOperationObserver::OperationComplete     
     */
    virtual void OperationComplete( MNcdLoadNodeOperation& aOperation,
                                    TInt aError );
                                    

public: // MNcdNodeChildOfTransparent
    
    virtual MNcdNode* TransparentParentL() const;
    
    
public: // MNcdNodeFavorite

    /**
     * @see MNcdNodeFavorite::RemoveFromFavoritesL.
     */
    virtual void RemoveFromFavoritesL();
                               
    /**
     * @see MNcdNodeFavorite::SetDisclaimerL.
     */
    virtual void SetDisclaimerL( MNcdNode* aDisclaimerOwner );
    
    /**
     * @see MNcdNodeFavorite::DisclaimerL.
     */
    virtual MNcdQuery* DisclaimerL() const;
    
    
protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeManager The manager that creates the nodes and keeps
     * track of the existing nodes.
     * @param aOperationManager This manager is used to create operations
     * for the node.     
     */
    CNcdNodeProxy( MCatalogsClientServer& aSession,
                   TInt aHandle, 
                   CNcdNodeManagerProxy& aNodeManager,
                   CNcdOperationManagerProxy& aOperationManager,
                   CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * ConstructL
     */
    virtual void ConstructL( );


    /**
     * @exception ETrue if link handle exists. In other words
     * if this node can connect to the link data of the server side.
     * EFalse if the link handle is not set.
     */
    TBool LinkHandleSet() const;
    

    /**
     * @return TInt link handle.
     * @exception KErrNotReady if link does not exist
     */
    TInt LinkHandleL() const;
    
    /**
     * 
     */
    void SetLinkHandle( TInt aHandle );


    /**
     * @return TTime gives the time when this node expires.
     */
    TTime ExpiredTime() const;
    
    /**
     * @return CNcdNodeIdentifier* The parent identifier.
     * Ownership is not transferred.
     */
    CNcdNodeIdentifier* ParentIdentifier() const;


    /** 
     * @return TBool ETrue if the node is remote node (for example, if the
     * node is a catalog instead of normal folder). Else EFalse. 
     */
    TBool IsRemote() const;
    

    // These function are called to update proxy data by getting data
    // from the server side.
     
    /**
     * Internalizes the link data if it can be found from
     * the server side. Uses InternalizeNodeLinkDataL.
     */
    void InternalizeLinkL();
    
    
    // These functions are used to update the data of this class object
 
    /**
     * Initializes the node. At least identifier data should be set.
     * InternalizeL calls this function.
     *
     * @param aStream Contains data that is used to initialize the node.
     */
    virtual void InternalizeNodeDataL( RReadStream& aStream );

    /**
     * Initializes the node link data.
     * InternalizeL calls this function.
     *
     * @param aStream Contains data that is used to initialize 
     * the node link information.
     */
    virtual void InternalizeNodeLinkDataL( RReadStream& aStream );


    // These functions are used to update data of the class objects 
    // owned by this node.
    
    void InternalizeMetadataL();
    
    void InternalizeNodeSeenL(); 


private:
    // Prevent if not implemented
    CNcdNodeProxy( const CNcdNodeProxy& aObject );
    CNcdNodeProxy& operator =( const CNcdNodeProxy& aObject );
            
private: // data

    // Node manager is used to create the nodes. And it keeps track of
    // the existing nodes. Thus, when this node is ready to be destroyed, 
    // it should inform the manager about it. And, the manager may then
    // destroy this node proxy. This is done by checking the reference count
    // of the node when Release is called. When the reference count is one
    // after Release, the manager should be informed. The manager may
    // call the Release and this node is delete when reference count is zero. 
    //
    // Not owned
    CNcdNodeManagerProxy& iNodeManager;
    
    // Operation manager is used to create operations for this node.
    //
    // Not owned
    CNcdOperationManagerProxy& iOperationManager;
    
    // Favorite manager is used to add and remove nodes from favorite list.
    //
    // Not owned
    CNcdFavoriteManagerProxy& iFavoriteManager;


    CNcdNodeIdentifier* iNodeIdentifier;

    // This is the parent shown in UI.
    CNcdNodeIdentifier* iParentIdentifier;
    
    // This is the real parent in engine (may be transparent folder).
    CNcdNodeIdentifier* iRealParentIdentifier;

    // This flag informs if the node is originally a remote node.
    // For example if the node were a catalog instead of a normal node, the
    // flag would be ETrue.
    TBool iRemoteFlag;    


    // MNceNode data
    
    HBufC* iTimestamp;
    HBufC* iCatalogSourceName;
    TTime iExpiredTime;

    TInt iLinkHandle;
    TBool iLinkHandleSet;
        
    // These are the possible additional interface objects that this node
    // may use. If the pointer value is NULL then the node does not provide
    // functionality for that interface at that moment. Mainly these objects
    // are created after certain operations have been done for the node.

    // Node metadata
    CNcdNodeMetadataProxy* iMetadata;
    
    // Node seen
    CNcdNodeSeenProxy* iNodeSeen;

    };


#endif // NCD_NODE_PROXY_H
