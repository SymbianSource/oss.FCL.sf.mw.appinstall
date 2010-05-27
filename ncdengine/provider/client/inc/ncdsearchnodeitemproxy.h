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
* Description:   Contains CNcdSearchNodeItemProxy class
*
*/


#ifndef NCD_SEARCH_NODE_ITEM_PROXY_H
#define NCD_SEARCH_NODE_ITEM_PROXY_H


#include <e32base.h>

#include "ncdnodeitemproxy.h"


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSearchNodeItemProxy: public CNcdNodeItemProxy
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
     * @return CNcdSearchNodeItemProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeItemProxy* NewL( MCatalogsClientServer& aSession, 
                                    TInt aHandle,
                                    CNcdNodeManagerProxy& aNodeManager,
                                    CNcdOperationManagerProxy& aOperationManager,
                                    CNcdFavoriteManagerProxy& aFavoriteManager );

    /**
     * NewLC
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
     * @return CNcdSearchNodeItemProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeItemProxy* NewLC( MCatalogsClientServer& aSession, 
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
    virtual ~CNcdSearchNodeItemProxy();


public: // MNcdNode

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
    CNcdSearchNodeItemProxy( MCatalogsClientServer& aSession,
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
    CNcdSearchNodeItemProxy( const CNcdSearchNodeItemProxy& aObject );
    CNcdSearchNodeItemProxy& operator =( const CNcdSearchNodeItemProxy& aObject );


private: // data
    
    };


#endif // NCD_SEARCH_NODE_ITEM_PROXY_H
