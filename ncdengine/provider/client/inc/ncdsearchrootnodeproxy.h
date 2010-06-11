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
* Description:   Contains CNcdSearchRootNodeProxy class
*
*/


#ifndef C_NCD_SEARCH_ROOT_NODE_PROXY
#define C_NCD_SEARCH_ROOT_NODE_PROXY


#include <e32base.h>

#include "ncdsearchnodefolderproxy.h"


class MNcdLoadNodeOperationObserver;
class CNcdNodeIdentifier;


/**
 *  Node folder contains other nodes that may also be containers.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSearchRootNodeProxy: public CNcdSearchNodeFolderProxy 
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
     * @return CNcdSearchRootNodeProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchRootNodeProxy* NewL( MCatalogsClientServer& aSession, 
                                    TInt aHandle,
                                    CNcdNodeManagerProxy& aNodeManager,
                                    CNcdOperationManagerProxy& aOperationManager,
                                    CNcdFavoriteManagerProxy& aFavoriteManager  );

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
     * @return CNcdSearchRootNodeProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchRootNodeProxy* NewLC( MCatalogsClientServer& aSession, 
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
    virtual ~CNcdSearchRootNodeProxy();



public: // CNcdNodeProxy

    /**
     * @see CNcdNodeProxy::State
     */
    virtual TState State() const;

    /**
     * @see CNcdNodeProxy::LoadL
     */
    virtual MNcdLoadNodeOperation* LoadL( MNcdLoadNodeOperationObserver& aObserver );
    
public: // MNcdNodeContainer

    /**
     * @see MNcdNodeContainer::ChildCount
     */
    virtual TInt ChildCount() const;

    /**
     * @see MNcdNodeContainer:: ChildL
     */
    virtual MNcdNode* ChildL( TInt aIndex );

    /**
     * @see MNcdNodeContainer::LoadChildrenL
     */
    virtual MNcdLoadNodeOperation* LoadChildrenL( 
        TInt aIndex, TInt aSize, TNcdChildLoadMode aMode,
        MNcdLoadNodeOperationObserver& aObserver );

public:// MNcdLoadNodeOperationObserver
        
    /**
     * @see MNcdLoadNodeOperationObserver::OperationComplete
     * Calls the internalize function to update the node data.
     */
    virtual void OperationComplete( MNcdLoadNodeOperation& aOperation,
                                    TInt aError );
            
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
    CNcdSearchRootNodeProxy( MCatalogsClientServer& aSession,
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
    CNcdSearchRootNodeProxy( const CNcdSearchRootNodeProxy& aObject );
    CNcdSearchRootNodeProxy& operator =( const CNcdSearchRootNodeProxy& aObject );


private: // data

    };


#endif // C_NCD_SEARCH_ROOT_NODE_PROXY
