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
* Description:   Contains CNcdNodeFolderProxy class
*
*/


#ifndef C_NCD_NODE_FOLDER_PROXY
#define C_NCD_NODE_FOLDER_PROXY


#include <e32base.h>

#include "ncdnodeproxy.h"
#include "ncdnodecontainer.h"


class MNcdLoadNodeOperationObserver;
class CNcdNodeIdentifier;
class CNcdLoadNodeOperationProxy;
class CNcdNodeSearch;
class CNcdChildEntity;
class CNcdFavoriteManagerProxy;
class CNcdNodeSeenFolderProxy;

/**
 *  Node folder contains other nodes that may also be containers.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeFolderProxy: public CNcdNodeProxy,
                           public MNcdNodeContainer 
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
     * @return CNcdNodeFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolderProxy* NewL( MCatalogsClientServer& aSession, 
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
     * @return CNcdNodeFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFolderProxy* NewLC( MCatalogsClientServer& aSession, 
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
    virtual ~CNcdNodeFolderProxy();



    /**
     * When link information has been loaded for the node, the number
     * of the children that the container will have is given.
     */
    TInt ExpectedChildCount() const;


public: // MNcdNode

    /**
     * @see MNcdNode::LoadL
     */
    virtual MNcdLoadNodeOperation* LoadL( MNcdLoadNodeOperationObserver& aObserver );


public: // MNcdNodeContainer

    /**
     * @see MNcdNodeContainer
     * 
     * When the container node metadata is loaded then the node contains
     * the ids of its children. This function returns the count of the
     * children whose ids are given for this container.
     *
     * @note ChildExpectedCount and this function should give the same
     * results after both link and metadata have been loaded.
     *
     */
    virtual TInt ChildCount() const;

    /**
     * @see MNcdNodeContainer:: ChildL
     */
    virtual MNcdNode* ChildL( TInt aIndex );

    /**
     * @see MNcdNodeContainer::LoadChildrenL
     */
    virtual MNcdLoadNodeOperation* LoadChildrenL( TInt aIndex, 
                                                  TInt aSize,
                                                  TNcdChildLoadMode aMode,
                                                  MNcdLoadNodeOperationObserver& aObserver );

    /**
     * @see MNcdNodeContainer::ContainerType
     */
    virtual TNcdContainerType ContainerType() const;
    
    /**
     * @see MNcdLoadNodeOperationObserver::OperationComplete
     * Calls the internalize function to update the node data.
     */
    virtual void OperationComplete( MNcdLoadNodeOperation& aOperation,
                                    TInt aError );
    
                                                  
public: // CNcdNodeProxy
    
    /**
     * This function calls additional folder specific
     * internalization functions that have not been called 
     * from the CNcdNodeProxy::InternalizeL.
     *
     * @see CNcdNodeProxy::InternalizeL
     */
    virtual void InternalizeL();
    
    
protected: // CNcdNodeProxy

    /**
     * This function is called from the CNcdNodeProxy::InternalizeL
     *
     * @see CNcdNodeProxy::InternalizeNodeDataL
     */
    virtual void InternalizeNodeDataL( RReadStream& aStream );

    /**
     * This function is called from the CNcdNodeProxy::InternalizeL
     *
     * @see CNcdNodeProxy::InternalizeNodeLinkDataL
     */
    virtual void InternalizeNodeLinkDataL( RReadStream& aStream );


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
    CNcdNodeFolderProxy( MCatalogsClientServer& aSession,
                         TInt aHandle,
                         CNcdNodeManagerProxy& aNodeManager,
                         CNcdOperationManagerProxy& aOperationManager,
                         CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    /**
     * Checks whether search is supported for this folder.
     *
     * @return ETrue if search is supported, EFalse otherwise.
     */
    virtual TBool IsSearchSupportedL();
    
    /**
     * Internalizes the MNcdNodeSeenFolder interface.
     */
    virtual void InternalizeNodeSeenFolderL();


private:
    // Prevent if not implemented
    CNcdNodeFolderProxy( const CNcdNodeFolderProxy& aObject );
    CNcdNodeFolderProxy& operator =( const CNcdNodeFolderProxy& aObject );


protected: // data
    
    // This is the count that the link information gives.
    TInt iExpectedChildCount;
    
    // This is the count that informs how many children are actually
    // set to the node.
    RPointerArray<CNcdChildEntity> iChildren;

    // These are the possible additional interface objects that this node
    // may use. If the pointer value is NULL then the node does not provide
    // functionality for that interface at that moment. Mainly these objects
    // are created after certain operations have been done for the node.

    CNcdNodeSearch* iSearch;
    
    // Implements the MNcdNodeSeenFolder interface for this object.
    CNcdNodeSeenFolderProxy* iNodeSeenFolder;
        
    };


#endif // C_NCD_NODE_FOLDER_PROXY
