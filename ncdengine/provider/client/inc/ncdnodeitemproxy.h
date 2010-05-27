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
* Description:   Contains CNcdNodeItemProxy class
*
*/


#ifndef NCD_NODE_ITEM_PROXY_H
#define NCD_NODE_ITEM_PROXY_H


#include <e32base.h>

#include "ncdnodeproxy.h"


/**
 * Base class for the proxies.
 * Provides basic methods for client server session function calls.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeItemProxy: public CNcdNodeProxy
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
     * @return CNcdNodeItemProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemProxy* NewL( MCatalogsClientServer& aSession, 
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
     * @return CNcdNodeItemProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemProxy* NewLC( MCatalogsClientServer& aSession, 
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
    virtual ~CNcdNodeItemProxy();


protected: // CNcdNodeProxy

    virtual void InternalizeNodeDataL( RReadStream& aStream );
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
    CNcdNodeItemProxy( MCatalogsClientServer& aSession,
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
    CNcdNodeItemProxy( const CNcdNodeItemProxy& aObject );
    CNcdNodeItemProxy& operator =( const CNcdNodeItemProxy& aObject );


private: // data
    
    };


#endif // NCD_NODE_ITEM_PROXY_H
