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
* Description:   Contains CNcdSearchNodeFolderProxy class
*
*/


#ifndef C_NCD_SEARCH_NODE_FOLDER_PROXY
#define C_NCD_SEARCH_NODE_FOLDER_PROXY


#include <e32base.h>

#include "ncdparentoftransparentnodeproxy.h"


class MNcdLoadNodeOperationObserver;
class CNcdNodeIdentifier;
class CNcdLoadNodeOperationProxy;
class CNcdNodeSkinProxy;
class CNcdNodeSearch;
class CNcdSearchFilter;


/**
 *  Node folder contains other nodes that may also be containers.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSearchNodeFolderProxy: public CNcdParentOfTransparentNodeProxy
    {

public:

    /**
     * NewL
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeManager The manager that creates the nodes and keeps
     * track of the existing nodes.
     * @param aOperationManager This manager is used to create operations
     * for the node.
     * @param aFavoriteManager This manager is used to add or remove favorite nodes.     
     * @return CNcdSearchNodeFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeFolderProxy* NewL( MCatalogsClientServer& aSession, 
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * NewL
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aNodeManager The manager that creates the nodes and keeps
     * track of the existing nodes.
     * @param aOperationManager This manager is used to create operations
     * for the node.
     * @param aFavoriteManager This manager is used to add or remove favorite nodes.
     * @return CNcdSearchNodeFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeFolderProxy* NewLC( MCatalogsClientServer& aSession, 
                                       TInt aHandle,
                                       CNcdNodeManagerProxy& aNodeManager,
                                       CNcdOperationManagerProxy& aOperationManager,
                                       CNcdFavoriteManagerProxy& aFavoriteManager );


    /**
     * Destructor
     *
     * @note Usually Release function should be used instead of
     * directly calling delete for this class object.
     *
     */
    virtual ~CNcdSearchNodeFolderProxy();


    /**
     * Getter for search filter.
     */
    const CNcdSearchFilter& SearchFilterL() const;

protected: // CNcdNodeProxy

    /**
     * This function is called from the CNcdNodeProxy::InternalizeL
     *
     * @see CNcdNodeProxy::InternalizeNodeDataL
     */
    virtual void InternalizeNodeDataL( RReadStream& aStream );

public: // MNcdNodeContainer

    /**
     * @see MNcdNodeContainer::LoadChildrenL
     */
    virtual MNcdLoadNodeOperation* LoadChildrenL( TInt aIndex, 
                                                  TInt aSize,
                                                  TNcdChildLoadMode aMode,
                                                  MNcdLoadNodeOperationObserver& aObserver );

public: // MNcdNode 

    /**
     * @see MNcdNode::State
     */
    virtual TState State() const;
    
    /**
     * @see MNcdNode::LoadL
     */
    virtual MNcdLoadNodeOperation* LoadL( MNcdLoadNodeOperationObserver& aObserver );
                                       
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
     * @param aFavoriteManager This manager is used to add or remove favorite nodes.     
     */
    CNcdSearchNodeFolderProxy( MCatalogsClientServer& aSession,
                         TInt aHandle,
                         CNcdNodeManagerProxy& aNodeManager,
                         CNcdOperationManagerProxy& aOperationManager,
                         CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();


private:
    // Prevent if not implemented
    CNcdSearchNodeFolderProxy( const CNcdSearchNodeFolderProxy& aObject );
    CNcdSearchNodeFolderProxy& operator =( const CNcdSearchNodeFolderProxy& aObject );

protected:
    
    CNcdSearchFilter* iSearchFilter;
    };


#endif // C_NCD_SEARCH_NODE_FOLDER_PROXY
