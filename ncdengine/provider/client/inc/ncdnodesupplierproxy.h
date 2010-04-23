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
* Description:   Contains CNcdNodeSupplierProxy class
*
*/


#ifndef NCDNODESUPPLIERPROXY_H
#define NCDNODESUPPLIERPROXY_H

#include "ncdnodeproxy.h"

class CNcdNodeSupplierProxy : public CNcdNodeProxy 
{
public:
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
     */
    static CNcdNodeSupplierProxy* NewLC(
        MCatalogsClientServer& aSession,
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
    virtual ~CNcdNodeSupplierProxy();
    
        
public: // MNcdNode

    /**
     * @see MNcdNode::State
     */
    virtual TState State() const;
    

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

    
public: // MNcdLoadNodeOperationObserver

    /**
     * @see MNcdLoadNodeOperationObserver::OperationComplete     
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
    CNcdNodeSupplierProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle, 
        CNcdNodeManagerProxy& aNodeManager,
        CNcdOperationManagerProxy& aOperationManager,
        CNcdFavoriteManagerProxy& aFavoriteManager );

    
private: // data

    // This node class will be set as a interface parent for iActualNode. So,
    // this class object will contain also the reference counts of the actual node.
    // So, when total reference count reaches zero and this class object will be deleted,
    // it can also delete iActualNode, because its reference count is also zero then.    
    CNcdNodeProxy* iActualNode;
};

#endif // NCDNODESUPPLIERPROXY_H