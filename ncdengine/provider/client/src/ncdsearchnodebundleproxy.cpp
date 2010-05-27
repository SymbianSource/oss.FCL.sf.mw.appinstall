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
* Description:   Contains CNcdSearchNodeFolder class implementation
*
*/


#include "ncdsearchnodebundleproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdloadnodeoperationproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdchildentity.h"
#include "ncdnodemanagerproxy.h"
#include "ncdchildloadmode.h"
#include "ncdcapabilities.h"

CNcdSearchNodeBundleProxy* CNcdSearchNodeBundleProxy::NewL(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    DLTRACEIN((""));
    CNcdSearchNodeBundleProxy* self = NewLC(
        aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSearchNodeBundleProxy* CNcdSearchNodeBundleProxy::NewLC(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    DLTRACEIN((""));
    CNcdSearchNodeBundleProxy* self = new ( ELeave ) CNcdSearchNodeBundleProxy(
        aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }

CNcdSearchNodeBundleProxy::CNcdSearchNodeBundleProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager )
: CNcdSearchNodeFolderProxy( aSession, aHandle, aNodeManager,
                       aOperationManager, aFavoriteManager ) 
    {
    }
    
CNcdSearchNodeBundleProxy::~CNcdSearchNodeBundleProxy()  
    {
    }

TInt CNcdSearchNodeBundleProxy::ChildCount() const
    {
    return iChildren.Count();
    }


MNcdNode* CNcdSearchNodeBundleProxy::ChildL( TInt aIndex )
    {
    DLTRACEIN(( _L("This parent: %S, %S"), &Namespace(), &Id() ));

           
    if ( aIndex < 0 || aIndex >= iChildren.Count() )
        {
        // Nothing to be done 
        DLERROR(( "Index error. child count: %d Given index: %d", 
                  iChildren.Count(), aIndex ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
        
    const CNcdNodeIdentifier* child = &iChildren[aIndex]->Identifier();
        
    MNcdNode* node( NULL );
    
    TRAPD( err, node = &NodeManager().NodeL( *child ) );
    
    if ( err == KErrNotFound ) 
        {
        return NULL;
        }
        
    User::LeaveIfError( err );
    
    // Increase the reference counter by one
    node->AddRef();
    
    DLTRACEOUT((""));

    return node;
    }
    

MNcdLoadNodeOperation* CNcdSearchNodeBundleProxy::LoadChildrenL( TInt aIndex, 
                                                           TInt aSize,
                                                           TNcdChildLoadMode aMode,
                                                           MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN((("this: %X"), this));
    DASSERT( iSearchFilter );
        
    if( aSize < 1 || aIndex < 0 || ( aMode == ELoadMetadata && aIndex + aSize > ChildCount() ))
        {
        // Nothing to be done 
        DLERROR(( "Argument error. ChildCount: %d Given index: %d, size: %d",
                  ChildCount(), aIndex, aSize ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }

    DLTRACE(( _L("Node: %S, %S"), &Namespace(), &Id() ));

    // Search bundle may contain transparent stuff, so use server child count for loading
    // to get correct indexing on server side.
    return CNcdSearchNodeFolderProxy::LoadChildrenL( 
        0,
        ServerChildCount(),
        aMode,
        aObserver );
    }
