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
* Description:   Contains CNcdBundleFolderProxy class
*
*/


#ifndef C_NCDBUNDLEFOLDERPROXY_H
#define C_NCDBUNDLEFOLDERPROXY_H

#include "ncdparentoftransparentnodeproxy.h"
#include "ncdbundle.h"


/**
 *  Node folder contains other nodes that may also be containers.
 *  Root may also contain children of the transparent child folder.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdBundleFolderProxy : public CNcdParentOfTransparentNodeProxy,
                              public MNcdBundle 
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
     * @return CNcdBundleFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdBundleFolderProxy* NewL( MCatalogsClientServer& aSession, 
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
     * @return CNcdBundleFolderProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdBundleFolderProxy* NewLC( MCatalogsClientServer& aSession, 
                                         TInt aHandle,
                                         CNcdNodeManagerProxy& aNodeManager,
                                         CNcdOperationManagerProxy& aOperationManager,
                                         CNcdFavoriteManagerProxy& aFavoriteManager );
                                         
    virtual ~CNcdBundleFolderProxy();    

                                         
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

                                         
public: // MNcdBundle

    virtual const TDesC& ViewType() const;


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
    CNcdBundleFolderProxy( MCatalogsClientServer& aSession,
                           TInt aHandle,
                           CNcdNodeManagerProxy& aNodeManager,
                           CNcdOperationManagerProxy& aOperationManager,
                           CNcdFavoriteManagerProxy& aFavoriteManager );
                           
    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * @see CNcdNodeFolder::IsSearchSupportedL
     */
    virtual TBool IsSearchSupportedL();
    
private:

    // Prevent these if not implemented
    CNcdBundleFolderProxy( const CNcdBundleFolderProxy& aObject );
    CNcdBundleFolderProxy& operator =( const CNcdBundleFolderProxy& aObject );


private: // data

    HBufC* iViewType;
};

#endif
