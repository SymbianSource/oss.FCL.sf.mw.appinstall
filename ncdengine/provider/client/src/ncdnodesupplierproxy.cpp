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


#include "ncdnodesupplierproxy.h"
#include "ncdnodemanagerproxy.h"
#include "catalogsdebug.h"
#include "ncderrors.h"

CNcdNodeSupplierProxy* CNcdNodeSupplierProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle, 
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    DLTRACEIN((""));
    CNcdNodeSupplierProxy* self = new ( ELeave ) CNcdNodeSupplierProxy(
        aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    

CNcdNodeSupplierProxy::CNcdNodeSupplierProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle, 
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) :
    CNcdNodeProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager ) 
    {
    }


CNcdNodeSupplierProxy::~CNcdNodeSupplierProxy()
    {
    if ( iActualNode != NULL )
        {
        // No need to call Release here. We, can just directly delete the actual node.
        // This is because, this class was set as a parent for the actual node. So,
        // when this class is deleted, also reference counts of its children are zero.
        // And, so they can also be deleted. The destructor, of the CNcdNodeProxy parent class 
        // will automatically inform the nodemanager that these nodes can be removed from 
        // the cache.
        delete iActualNode;
        iActualNode = NULL;
        }
    }


MNcdNode::TState CNcdNodeSupplierProxy::State() const
    {
    DLTRACEIN((""));
    if ( iActualNode ) 
        {
        return iActualNode->State();
        }
    else 
        {
        return MNcdNode::EStateNotInitialized;
        }
    }

const TDesC& CNcdNodeSupplierProxy::CatalogSourceName() const 
    {
    DLTRACEIN((""));
    if ( iActualNode ) 
        {
        return iActualNode->CatalogSourceName();
        }
    else 
        {
        return CNcdNodeProxy::CatalogSourceName();
        }
    }
    
MNcdNodeContainer* CNcdNodeSupplierProxy::ParentL() const 
    {
    DLTRACEIN((""));
    if ( iActualNode ) 
        {
        return iActualNode->ParentL();
        }
    else 
        {
        return CNcdNodeProxy::ParentL();
        }
    }
    

MNcdLoadNodeOperation* CNcdNodeSupplierProxy::LoadL(
    MNcdLoadNodeOperationObserver& aObserver ) 
    {
    DLTRACEIN((""));
    if ( iActualNode ) 
        {
        return iActualNode->LoadL( aObserver );
        }
    else 
        {
        return CNcdNodeProxy::LoadL( aObserver );
        }
    }
    

RCatalogsArray< MNcdOperation > CNcdNodeSupplierProxy::OperationsL() const 
    {
    DLTRACEIN((""));
    if ( iActualNode ) 
        {
        return iActualNode->OperationsL();
        }
    else 
        {
        return CNcdNodeProxy::OperationsL();
        }
    }
    
void CNcdNodeSupplierProxy::OperationComplete(
    MNcdLoadNodeOperation& /*aOperation*/, TInt aError ) 
    {
    DLTRACEIN((""));

    if ( iActualNode != NULL )
        {
        DLINFO(("iActualNode has already been created. Nothing to do here."))
        return;
        }
        
    if ( aError == KErrNone || aError == KNcdErrorSomeCatalogsFailedToLoad )
        {
        DLINFO(("Set iActualNode"))

        // The actual node is loaded and can be requested from nodemanager.
        // This will replace the supplier node with the temporary node in nodemanager cache.
        // After that, if nodemanager is asked for the node, the temporary node is directly
        // given. But, if this node is used then this will forward, the requests to that temporary
        // node
        TRAPD( err, iActualNode = &NodeManager().ReplaceCacheNodeL( NodeIdentifier() ) );
        (void) &err; // prevents compiler warning of unreferenced variable

        // The actual node should always be created in server side if we come here.
        // So, above should have found it.
        DASSERT( err == KErrNone );
        
        if ( iActualNode != NULL )
            {
            // SetParentL is a safe call here because nodes are top level parents
            // as the default value. So, we can safely set a new parent for the
            // actual node. Notice, that this setting will insert the interfaces from
            // the actual node to this node. So, for example MNcdNode-interface pointer
            // will be replaced by the interface pointer of the actual node gotten above.
            TRAP(err, iActualNode->SetParentL( this ) );
            DASSERT( err == KErrNone );
            }
        }
    }
