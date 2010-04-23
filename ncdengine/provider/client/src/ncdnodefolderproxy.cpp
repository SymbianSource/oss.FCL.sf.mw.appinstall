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
* Description:   Contains CNcdNodeFolderProxy class implementation
*
*/


#include "ncdnodefolderproxy.h"
#include "ncdnodemetadataproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdloadnodeoperationproxy.h"
#include "ncdnodesearchimpl.h"
#include "catalogsutils.h"
#include "ncdchildentity.h"
#include "ncdproviderdefines.h"
#include "ncdnodeuricontentproxy.h"
#include "ncdcapabilities.h"
#include "ncdnodeseenfolderproxy.h"
#include "ncderrors.h"

CNcdNodeFolderProxy::CNcdNodeFolderProxy( MCatalogsClientServer& aSession,
                                          TInt aHandle,
                                          CNcdNodeManagerProxy& aNodeManager,
                                          CNcdOperationManagerProxy& aOperationManager,
                                          CNcdFavoriteManagerProxy& aFavoriteManager ) 
: CNcdNodeProxy( aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager )
    {
    }


void CNcdNodeFolderProxy::ConstructL()
    {
    CNcdNodeProxy::ConstructL(); 

    // Add the interface information to the list
    MNcdNodeContainer* container( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( container, this, MNcdNodeContainer::KInterfaceUid ) );                    
    }


CNcdNodeFolderProxy* CNcdNodeFolderProxy::NewL( MCatalogsClientServer& aSession,
                                                TInt aHandle,
                                                CNcdNodeManagerProxy& aNodeManager,
                                                CNcdOperationManagerProxy& aOperationManager,
                                                CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdNodeFolderProxy* self = 
        CNcdNodeFolderProxy::NewLC(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeFolderProxy* CNcdNodeFolderProxy::NewLC( MCatalogsClientServer& aSession,
                                                 TInt aHandle,
                                                 CNcdNodeManagerProxy& aNodeManager,
                                                 CNcdOperationManagerProxy& aOperationManager,
                                                 CNcdFavoriteManagerProxy& aFavoriteManager )
    {
    CNcdNodeFolderProxy* self = 
        new( ELeave ) CNcdNodeFolderProxy(
            aSession, aHandle, aNodeManager, aOperationManager, aFavoriteManager );  
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeFolderProxy::~CNcdNodeFolderProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeContainer::KInterfaceUid );
    RemoveInterface( MNcdNodeSearch::KInterfaceUid );

    // Delete the objects that are owned by this class and provide functionality
    // through the api.
    // Notice that the api-objects are Released (not deleted) from the UI side.
    // The node owns the data and the reference counter of the node keeps
    // track of the api objects it owns. When the reference counter of the node
    // reaches zero, it means that nobody is using the node or the classes owned
    // by the node. If somebody is using the object owned by the node, the reference
    // counter can not be zero until everything is released.
    // Thus, the node may delete the data here.

    delete iSearch;
    iSearch = NULL;
    
    delete iNodeSeenFolder;
    iNodeSeenFolder = NULL;
    
    iChildren.ResetAndDestroy();
    }




TInt CNcdNodeFolderProxy::ExpectedChildCount() const
    {
    return iExpectedChildCount;
    }


MNcdLoadNodeOperation* CNcdNodeFolderProxy::LoadL(
    MNcdLoadNodeOperationObserver& aObserver ) 
    {
    DLTRACEIN((_L("This ns: %S, id: %S"), &Namespace(), &Id() ));    
    return CNcdNodeProxy::LoadL( aObserver );
    }


TInt CNcdNodeFolderProxy::ChildCount() const
    {
    return iExpectedChildCount;
    }


MNcdNode* CNcdNodeFolderProxy::ChildL( TInt aIndex )
    {
    DLTRACEIN(( _L("This parent: %S, %S"), &Namespace(), &Id(), aIndex ));
    DLINFO(("aIndex = %d, expected childcount= %d, real child count = %d",
        aIndex, iExpectedChildCount, iChildren.Count() ));
        
    if ( aIndex < 0 || aIndex >= iExpectedChildCount )
        {
        // Nothing to be done 
        DLERROR(( "Index error. expected child count: %d Given index: %d", 
                  iExpectedChildCount, aIndex ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );   
        }
        
    // search for a child with given index
    const CNcdNodeIdentifier* child = NULL;
    for( TInt i = 0 ; i < iChildren.Count() ; i++ )
        {
        if ( iChildren[i]->Index() == aIndex )
            {
            child = &iChildren[i]->Identifier();
            }
        else if ( iChildren[i]->Index() > aIndex )
            {
            // no sense in searching further
            break;
            }
        }

    if( child == NULL )
        {
        return NULL;
        }
    
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
    
    
MNcdLoadNodeOperation* CNcdNodeFolderProxy::LoadChildrenL( TInt aIndex, 
                                                           TInt aSize,
                                                           TNcdChildLoadMode aMode,
                                                           MNcdLoadNodeOperationObserver& aObserver )
    {
    DLTRACEIN(("aIndex: %d, aSize: %d, expected child count: %d, load mode: %d",
        aIndex, aSize, iExpectedChildCount, aMode ));

    if ( aSize < 1 
         || aIndex < 0 
         || ( aMode == ELoadMetadata 
              && aSize != KMaxTInt 
              && aIndex + aSize > ChildCount() ) )
        {
        // Nothing to be done 
        DLERROR(( "Argument error. ChildCount: %d Given index: %d, size: %d",
                  ChildCount(), aIndex, aSize ));
        User::Leave( KErrArgument );
        }

    if ( aSize == KMaxTInt )
        {
        // Because aSize is KMaxTInt, reduce it by the aIndex value.
        // This way we will not have possible overload if aIndex is later added
        // to the aSize value. It does not really matter what the aSize value is
        // after this. KMaxTInt is so great value, that in all the cases the server
        // request should give all the required items.
        aSize -= aIndex;
        DLINFO(("aSize was KMaxTInt. Reduced by index: %d", aSize));
        }

    DLTRACE(( _L("Node: %S, %S"), &Namespace(), &Id() ));
    
    CNcdLoadNodeOperationProxy* operation = 
        OperationManager().CreateLoadNodeOperationL( 
            *this, ETrue, aSize, aIndex, 1, aMode );

    if ( !operation )
        {
        DLTRACEOUT(("NULL"));     
        return NULL;
        }

    operation->AddObserverL( this );
    operation->AddObserverL( &aObserver );

    DLTRACEOUT((""));        

    return operation;
    }
    
TNcdContainerType CNcdNodeFolderProxy::ContainerType() const
    {
    if ( IsRemote() ) 
        {
        return ECatalog;
        }
    else 
        {
        return EFolder;
        }
    }

void CNcdNodeFolderProxy::OperationComplete( 
    MNcdLoadNodeOperation& /*aOperation*/, TInt aError )
    {
    DLTRACEIN(( "Error: %d", aError ));
    
    if ( aError == KErrNone || aError == KNcdErrorSomeCatalogsFailedToLoad )
        {
        // update proxy's status from the server
        TRAP_IGNORE( InternalizeL() );
        }
    }

void CNcdNodeFolderProxy::InternalizeL()
    {
    DLTRACEIN((""));

    // First call the parent internalizator. So, all the parent stuff will
    // be initialized before folder specific data.
    CNcdNodeProxy::InternalizeL();

    // Add the search interface because it is availabe for all the folders
    // that have at least one child.
    
    // check that is search supported for this node
    TBool isSearchSupported = IsSearchSupportedL();
        
    DLINFO(( "isSearchSupported: %d, child count: %d", isSearchSupported, 
        ChildCount() ));
    if ( isSearchSupported )
        {
        DLTRACE(("add search interface"));
        if ( iSearch == NULL )
            {
            // Create new search because old one did not exist.
            // Notice that the creation adds the interface to the node interface list
            // automatically.
            iSearch = CNcdNodeSearch::NewL( *this, OperationManager() );    
            }
        }
    else
        {
        DLTRACE(("remove search interface"));        
        delete iSearch;
        iSearch = NULL;
        }
        
    InternalizeNodeSeenFolderL();        
                   
    DLTRACEOUT((""));
    }


void CNcdNodeFolderProxy::InternalizeNodeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // First internalize parent data
    CNcdNodeProxy::InternalizeNodeDataL( aStream );

    // Get the children data here.

    DLTRACE(("Handle children"));

    // Clear the buffer because new childs will be appended
    iChildren.ResetAndDestroy();
    
    TInt childrenCount( aStream.ReadInt32L() );
    DLTRACE(("Children: %d", childrenCount ));

    TInt classObjectType( NcdNodeClassIds::ENcdNullObjectClassId );

    for ( TInt i = 0; i < childrenCount; ++i )
        {
        // This is safe casting because enum is same as TInt
        classObjectType = 
            static_cast<NcdNodeClassIds::TNcdNodeClassId>(aStream.ReadInt32L());
        if ( NcdNodeClassIds::ENcdChildEntityClassId == classObjectType )
            {
            CNcdChildEntity* childEntity = CNcdChildEntity::NewLC( aStream );
            iChildren.AppendL( childEntity );
            CleanupStack::Pop( childEntity );            
            DLINFO((_L("Added child, id: %S, array index: %d, real index: %d"),
            &childEntity->Identifier().NodeId(), i, childEntity->Index() ));
            }
        else
            {
            // For debug purposes
            DLERROR(("Wrong class id"));
            DASSERT( EFalse );

            // Wrong kind of class object info
            User::Leave( KErrCorrupt );
            }
        }
        
    // Show node info here, after the internalization has been done.        
    DLTRACEOUT(( _L("Node: %S, %S, %d"), 
                 &Namespace(), &Id(), NodeIdentifier().ClientUid().iUid ));
    }


void CNcdNodeFolderProxy::InternalizeNodeLinkDataL( RReadStream& aStream )
    {
    DLTRACEIN(( _L("Node: %S, %S"), &Namespace(), &Id() ));
    
    // First internalize parent data
    CNcdNodeProxy::InternalizeNodeLinkDataL( aStream );

    // Then, set the data for this class object
    
    iExpectedChildCount = aStream.ReadInt32L();

    DLTRACEOUT((""));
    }

TBool CNcdNodeFolderProxy::IsSearchSupportedL()
    {
    DLTRACEIN((""));
    // check if this node has search capability set
    return NodeManager().IsCapabilitySupportedL( NodeIdentifier(),
        NcdCapabilities::KSearch );
    }
    

void CNcdNodeFolderProxy::InternalizeNodeSeenFolderL() 
    {
    DLTRACEIN((""));
    
    if ( iNodeSeenFolder )
        {
        iNodeSeenFolder->InternalizeL();
        }
    else 
        {
        // Create the object.  
        // Get the handle at first.
        TInt handle( 0 );
        User::LeaveIfError(
            ClientServerSession().SendSync(
                NcdNodeFunctionIds::ENcdNodeSeenFolderHandle,
                KNullDesC(),
                handle,
                Handle() ) );
        
        DLINFO(( "handle: %d", handle ));
       
        iNodeSeenFolder = CNcdNodeSeenFolderProxy::NewL(
            ClientServerSession(), handle, *this );
        }
    }
                    
                
