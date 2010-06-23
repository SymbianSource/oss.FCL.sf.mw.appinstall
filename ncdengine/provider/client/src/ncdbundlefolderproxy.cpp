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
* Description:   Contains CNcdBundleFolderProxy class implementation
*
*/


#include "ncdbundlefolderproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdloadnodeoperationproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdchildentity.h"
#include "ncdnodemanagerproxy.h"
#include "ncdchildloadmode.h"
#include "ncdcapabilities.h"

CNcdBundleFolderProxy* CNcdBundleFolderProxy::NewL(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    DLTRACEIN((""));
    CNcdBundleFolderProxy* self = NewLC(
        aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdBundleFolderProxy* CNcdBundleFolderProxy::NewLC(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager ) 
    {
    DLTRACEIN((""));
    CNcdBundleFolderProxy* self = new ( ELeave ) CNcdBundleFolderProxy(
        aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdBundleFolderProxy::CNcdBundleFolderProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdFavoriteManagerProxy& aFavoriteManager )
: CNcdParentOfTransparentNodeProxy( aSession, 
                                    aHandle, 
                                    aNodeManager,
                                    aOperationManager,
                                    aFavoriteManager ) 
    {
    }
    
CNcdBundleFolderProxy::~CNcdBundleFolderProxy()  
    {
    delete iViewType;
    }

MNcdNode::TState CNcdBundleFolderProxy::State() const
    {
    DLTRACEIN((_L("Node namespace=%S, id=%S"), &Namespace(), &Id() ));

    // Check if the link handle has been set, which means that also
    // link data has been internalized. Also, check if the metadata 
    // exists, which means that metadata has also been internalized.
    if ( LinkHandleSet() 
         && Metadata() != NULL )
        {
        DLINFO(("State was initialized"));
        // If state was initialized we have to check if the state
        // has acutally expired already 
        TTime now;
        now.HomeTime();

        DLINFO(("now time: %Ld", now.Int64() ));
        DLINFO(("expired time: %Ld", ExpiredTime().Int64() ));


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
        DLTRACEOUT(("Not initialized"));
        return MNcdNode::EStateNotInitialized;
        }
    }

TInt CNcdBundleFolderProxy::ChildCount() const
    {
    return iChildren.Count();
    }


MNcdNode* CNcdBundleFolderProxy::ChildL( TInt aIndex )
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

MNcdLoadNodeOperation* CNcdBundleFolderProxy::LoadChildrenL(
    TInt aIndex, 
    TInt aSize,
    TNcdChildLoadMode aMode,
    MNcdLoadNodeOperationObserver& aObserver ) 
    {
    DLTRACEIN((""));
    
    if( aSize < 1 || aIndex < 0 || ( aMode == ELoadMetadata && aIndex + aSize > ChildCount() ) )
    {
        // Nothing to be done 
        DLERROR(( "Argument error. ChildCount: %d Given index: %d, size: %d",
                  ChildCount(), aIndex, aSize ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
    }

    DLTRACE(( _L("Node: %S, %S"), &Namespace(), &Id() ));
    
    if( aMode == ELoadMetadata )
        {
        // Child metadata can be loaded in a regular load op.
        // Notice, because this node can be parent of transparent nodes
        // we have to make sure that the indexing is correct. (Transparent folders
        // may have been replaced by their children, and then the indexing will not
        // match the server side.)
        // Note that start index is always 0 for bundles
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
    else
        {
        // Everything else needs to be loaded via root load op
        return LoadL( aObserver );
        }
    }


MNcdLoadNodeOperation* CNcdBundleFolderProxy::LoadL(
    MNcdLoadNodeOperationObserver& aObserver ) 
    {    
    DLTRACEIN((""));

    CNcdLoadNodeOperationProxy* operation( NULL );

    // bundle's are loaded with load bundle op
    operation =
        OperationManager().CreateLoadBundleNodeOperationL( *this );

    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );

    return operation;
    }
    
void CNcdBundleFolderProxy::ConstructL() 
    {
    DLTRACEIN((("this: %x"), this ));
    AssignDesL( iViewType, KNullDesC() );
    CNcdParentOfTransparentNodeProxy::ConstructL();
    
    // Add the interface information to the list
    MNcdBundle* bundle( this );
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL(
            bundle, this, MNcdBundle::KInterfaceUid ) );
    }

const TDesC& CNcdBundleFolderProxy::ViewType() const 
    {
    DLTRACEIN((_L("this: %x, viewtype: %S"), this, iViewType));
    return *iViewType;
    }



void CNcdBundleFolderProxy::InternalizeL()
    {
    DLTRACEIN((("this: %x"), this));

    // First call the parent internalizator. So, all the parent stuff will
    // be initialized before folder specific data.
    CNcdParentOfTransparentNodeProxy::InternalizeL();
    
    DLTRACEOUT((""));
    }



void CNcdBundleFolderProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // First internalize parent data
    CNcdParentOfTransparentNodeProxy::InternalizeNodeDataL( aStream );

    InternalizeDesL( iViewType, aStream );

    DLINFO((_L("this: %x, viewtype: %S"), this, iViewType));

    DLTRACEOUT((""));
    }

void CNcdBundleFolderProxy::InternalizeNodeLinkDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // First internalize parent data
    CNcdParentOfTransparentNodeProxy::InternalizeNodeLinkDataL( aStream );

    DLTRACEOUT((""));
    }

TBool CNcdBundleFolderProxy::IsSearchSupportedL()
    {
    DLTRACEIN((""));
    // If one of the bundles children has search capability set, then this
    // bundle supports searching.
    // NOTE: Search will only be conducted for children with search capability set.
    for( TInt i = 0 ; i < iChildren.Count() ; i++ )
        {
        if( NodeManager().IsCapabilitySupportedL( iChildren[i]->Identifier(),
            NcdCapabilities::KSearch ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
