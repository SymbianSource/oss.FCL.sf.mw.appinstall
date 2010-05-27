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
* Description:   Contains CNcdSearchRootNodeProxy class implementation
*
*/


#include "ncdsearchrootnodeproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncdchildentity.h"
#include "ncdchildloadmode.h"
#include "ncderrors.h"

// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdSearchRootNodeProxy* CNcdSearchRootNodeProxy::NewL( MCatalogsClientServer& aSession,
                                            TInt aHandle,
                                            CNcdNodeManagerProxy& aNodeManager,
                                            CNcdOperationManagerProxy& aOperationManager,
                                            CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchRootNodeProxy* self = 
        CNcdSearchRootNodeProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSearchRootNodeProxy* CNcdSearchRootNodeProxy::NewLC( MCatalogsClientServer& aSession,
                                             TInt aHandle,
                                             CNcdNodeManagerProxy& aNodeManager,
                                             CNcdOperationManagerProxy& aOperationManager,
                                             CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdSearchRootNodeProxy* self = 
        new( ELeave ) CNcdSearchRootNodeProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdSearchRootNodeProxy::CNcdSearchRootNodeProxy( MCatalogsClientServer& aSession,
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdSearchNodeFolderProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdSearchRootNodeProxy::ConstructL()
    {
    CNcdNodeFolderProxy::ConstructL(); 
    }

CNcdSearchRootNodeProxy::~CNcdSearchRootNodeProxy()
    {
    }


MNcdNode::TState CNcdSearchRootNodeProxy::State() const
    {
    DLTRACEIN((""));
    // Search root is a local node that can't be loaded and
    // therefore is always initialized.
    DLTRACEOUT(("Initialized"));
    return MNcdNode::EStateInitialized;
    }


MNcdLoadNodeOperation* CNcdSearchRootNodeProxy::LoadL(
    MNcdLoadNodeOperationObserver& /*aObserver*/ )
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    // return null because search root can't be loaded.
    return NULL;
    }

TInt CNcdSearchRootNodeProxy::ChildCount() const
    {
    return iChildren.Count();
    }


MNcdNode* CNcdSearchRootNodeProxy::ChildL( TInt aIndex )
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
    
    node = &NodeManager().NodeL( *child );
    
    // Increase the reference counter by one
    node->AddRef();
    
    DLTRACEOUT((""));

    return node;
    }

MNcdLoadNodeOperation* CNcdSearchRootNodeProxy::LoadChildrenL( 
        TInt aIndex, TInt aSize, TNcdChildLoadMode aMode,
        MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    if( aSize < 1 || aIndex < 0 || ( aMode == ELoadMetadata && aIndex + aSize > ChildCount() ))
        {
        // Nothing to be done 
        DLERROR(( "Argument error. ChildCount: %d Given index: %d, size: %d",
                  ChildCount(), aIndex, aSize ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
    if( aMode == ELoadMetadata )
        {
        // Child metadata can be loaded in a regular load op
        return CNcdSearchNodeFolderProxy::LoadChildrenL( aIndex,
            aSize,
            aMode,
            aObserver );
        }
    else
        {
        
        return NULL;
        }
    }

void CNcdSearchRootNodeProxy::OperationComplete( 
    MNcdLoadNodeOperation& /*aOperation*/, TInt aError )
    {
    DLTRACEIN(( "Error: %d", aError ));
    
    if ( aError == KErrNone || aError == KNcdErrorSomeCatalogsFailedToLoad ) 
        {
        // update proxy's status from the server
        TRAP_IGNORE( InternalizeL() );
        }
    }

