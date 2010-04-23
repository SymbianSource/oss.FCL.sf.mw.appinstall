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
* Description:   Contains CNcdSearchNodeBundleProxy class
*
*/


#ifndef C_NCDSEARCHNODEBUNDLEPROXY_H
#define C_NCDSEARCHNODEBUNDLEPROXY_H

#include "ncdsearchnodefolderproxy.h"
#include "ncdbundle.h"

class CNcdSearchNodeBundleProxy : public CNcdSearchNodeFolderProxy
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
     * @return CNcdSearchNodeBundleProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeBundleProxy* NewL( MCatalogsClientServer& aSession, 
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
     * @return CNcdSearchNodeBundleProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeBundleProxy* NewLC( MCatalogsClientServer& aSession, 
                                         TInt aHandle,
                                         CNcdNodeManagerProxy& aNodeManager,
                                         CNcdOperationManagerProxy& aOperationManager,
                                         CNcdFavoriteManagerProxy& aFavoriteManager );
                                         
    virtual ~CNcdSearchNodeBundleProxy();    
                                         
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
    CNcdSearchNodeBundleProxy( MCatalogsClientServer& aSession,
                           TInt aHandle,
                           CNcdNodeManagerProxy& aNodeManager,
                           CNcdOperationManagerProxy& aOperationManager,
                           CNcdFavoriteManagerProxy& aFavoriteManager );

};

#endif
