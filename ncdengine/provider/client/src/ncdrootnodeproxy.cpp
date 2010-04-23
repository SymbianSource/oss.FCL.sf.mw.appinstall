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
* Description:   Contains CNcdRootNodeProxy class implementation
*
*/


#include "ncdrootnodeproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncdchildentity.h"
#include "ncderrors.h"


// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdRootNodeProxy* CNcdRootNodeProxy::NewL( MCatalogsClientServer& aSession,
                                            TInt aHandle,
                                            CNcdNodeManagerProxy& aNodeManager,
                                            CNcdOperationManagerProxy& aOperationManager,
                                            CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdRootNodeProxy* self = 
        CNcdRootNodeProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdRootNodeProxy* CNcdRootNodeProxy::NewLC( MCatalogsClientServer& aSession,
                                             TInt aHandle,
                                             CNcdNodeManagerProxy& aNodeManager,
                                             CNcdOperationManagerProxy& aOperationManager,
                                             CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdRootNodeProxy* self = 
        new( ELeave ) CNcdRootNodeProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdRootNodeProxy::CNcdRootNodeProxy( MCatalogsClientServer& aSession,
                                      TInt aHandle,
                                      CNcdNodeManagerProxy& aNodeManager,
                                      CNcdOperationManagerProxy& aOperationManager,
                                      CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdParentOfTransparentNodeProxy(
    aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdRootNodeProxy::ConstructL()
    {
    CNcdParentOfTransparentNodeProxy::ConstructL(); 
    }


CNcdRootNodeProxy::~CNcdRootNodeProxy()
    {
    }


TInt CNcdRootNodeProxy::ChildCount() const
    {
    return iChildren.Count();
    }


MNcdNode* CNcdRootNodeProxy::ChildL( TInt aIndex )
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

MNcdNode::TState CNcdRootNodeProxy::State() const
    {
    DLTRACEIN((""));

    // Check if the link handle has been set, which means that also
    // link data has been internalized. Root node does not contain
    // metadata so it does not need to be checked.
    // The child count is also checked because in some situations when root loading
    // is cancelled the link handle may be set, but the children are not inserted to 
    // the root child list. So, the root should have children to be in initialized mode.
    if ( LinkHandleSet() && ChildCount() > 0 )
        {
        DLTRACE(("State was initialized"));
        // If state was initialized we have to check if the state
        // has acutally expired already 
        TTime now;
        now.HomeTime();

        DLTRACE(("now time: %d", now.Int64() ));
        DLTRACE(("expired time: %d", ExpiredTime().Int64() ));

        // We can just compare the times here. Server side
        // inserts the maximum value for the expired time if the
        // protocol has set never expire value for the validity delta.
        TBool isExpired = ( now > ExpiredTime() );
        TInt err = KErrNone;
        
        if ( !isExpired ) 
            {            
            TRAP( err, isExpired = IsTransparentChildExpiredL() );
            }
            
        if ( isExpired || err != KErrNone )
            {
            DLTRACEOUT(("Expired"));
            return MNcdNode::EStateExpired;
            }
            
        DLTRACEOUT(("Initialized"));
        return MNcdNode::EStateInitialized;
        }
     else
        {
        // Node has not been initialized.
        DLTRACEOUT(("Not intialized"));
        return MNcdNode::EStateNotInitialized;
        }

    }


MNcdLoadNodeOperation* CNcdRootNodeProxy::LoadL( MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    
    CNcdLoadNodeOperationProxy* operation( NULL );

    operation =
        OperationManager().
            CreateLoadRootNodeOperationL( *this );

    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );

    DLTRACEOUT((""));

    return operation;
    }


MNcdLoadNodeOperation* CNcdRootNodeProxy::LoadChildrenL( TInt /*aIndex*/, 
                                                         TInt /*aSize*/,
                                                         TNcdChildLoadMode aMode,
                                                         MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN(("Root load mode: %d", aMode));

    // Root is a special case when children are loaded.
    // Because root operation always load its children same time normal LoadL is called
    // for the root, the children of the root are always up to date.
    // The only exception is, if the schemes are added to the root list separately.
    // Because of the possible schemes, this function is overloaded and the 
    // staring index is set to be zero and the size has to be the whole child array size which
    // includes the possible scheme.
    
    CNcdLoadNodeOperationProxy* operation = 
        OperationManager().CreateLoadNodeOperationL( *this, ETrue,
                                                     ServerChildCount(),
                                                     0,
                                                     1,
                                                     aMode );

    if( operation == NULL )
        {
        DLTRACEOUT(("NULL"));     
        return NULL;
        }

    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );

    DLTRACEOUT((""));        

    return operation;
    }


RCatalogsArray<MNcdOperation> CNcdRootNodeProxy::OperationsL() const
    {
    DLTRACEIN((""));
    RCatalogsArray<MNcdOperation> operations;
    CleanupClosePushL( operations );

    // Operations will be get differently for root node, because root
    // node does not have metadata.
    
    // Get the original array and insert its content to catalogs array.
    // Also, increase the reference counter for the items.
    const RPointerArray<MNcdOperation>& origArray = OperationManager().Operations();
    const MNcdNode* thisNode( this );
    MNcdOperation* oper( NULL );
    MNcdNode* node( NULL );

    for ( TInt i = 0; i < origArray.Count(); ++i )
        {
        oper = origArray[ i ];

        // Notice that node ref count is increased. So, release it when done.
        node = oper->Node();
        CleanupReleasePushL( *node );

        if( node == thisNode )
            {
            operations.AppendL( oper );
            oper->AddRef();
            }

        CleanupStack::PopAndDestroy( node );
        }

    CleanupStack::Pop( &operations );
    
    DLTRACEOUT(( "" ));
    return operations;
    }


void CNcdRootNodeProxy::OperationComplete( MNcdLoadNodeOperation& /*aOperation*/,
                                           TInt aError )
    {
    DLTRACEIN((""));
    // Should all the error code checks be removed and always internalize root node?
    if( aError == KErrNone || aError == KNcdErrorSomeCatalogsFailedToLoad || 
        aError == KErrCancel )
        {
        // Because operation went ok. Update the node data.
        TRAP_IGNORE( InternalizeL() );
        }
    DLTRACEOUT((""));
    }

