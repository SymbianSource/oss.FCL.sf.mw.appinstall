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
* Description:   Implements CNcdNodeFactory class
*
*/


#include "ncdnodefactory.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdnodemanager.h"
#include "ncdnodedbmanager.h"
#include "ncdnodeimpl.h"
#include "ncdnodeitem.h"
#include "ncdnodefolder.h"
#include "ncdrootnode.h"
#include "ncdsearchrootnode.h"
#include "ncdsearchnodeitem.h"
#include "ncdsearchnodefolder.h"
#include "ncdnodetransparentfolder.h"
#include "ncdbundlefolder.h"
#include "ncdnodesupplier.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeitemmetadata.h"
#include "ncdnodefoldermetadata.h"
#include "catalogsdebug.h"
#include "ncdsearchnodebundle.h"


CNcdNodeFactory* CNcdNodeFactory::NewL( CNcdNodeManager& aNodeManager )
    {
    CNcdNodeFactory* self =   
        CNcdNodeFactory::NewLC( aNodeManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeFactory* CNcdNodeFactory::NewLC( CNcdNodeManager& aNodeManager )
    {
    CNcdNodeFactory* self = 
        new( ELeave ) CNcdNodeFactory( aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }



CNcdNodeFactory::CNcdNodeFactory( CNcdNodeManager& aNodeManager )
: CBase(),
  iNodeManager( aNodeManager )
    {
    }


void CNcdNodeFactory::ConstructL()
    {
    // These values have to be set. 
    }


CNcdNodeFactory::~CNcdNodeFactory()
    {
    DLTRACEIN((""));
    
    // Delete member variables that are owned by this class object here.

    DLTRACEOUT((""));
    }        



// ---------------------------------------------------------------------------
// Functions to check the node types and node purposes
// ---------------------------------------------------------------------------

CNcdNodeFactory::TNcdNodeType CNcdNodeFactory::NodeTypeL( const TDesC8& aNodeData )
    {
    DLTRACEIN((""));
    
    // Check the class ids and return the correct value according to that.

    // Reads the class id from the stream
    if ( aNodeData.Length() == 0 )
        {
        DLINFO(("Node data was empty."));
        DASSERT( EFalse );
        User::Leave( KErrArgument );        
        }
                                    
    DLINFO(("Create stream from db data length: %d", aNodeData.Length()));
    RDesReadStream stream( aNodeData );                                   
    CleanupClosePushL( stream );
 
    // Read the class id from the stream    
    NcdNodeClassIds::TNcdNodeClassId classId =
        static_cast<NcdNodeClassIds::TNcdNodeClassId>( stream.ReadInt32L() );

    CleanupStack::PopAndDestroy( &stream );

    TNcdNodeType type( NodeTypeL( classId ) );

    DLTRACEOUT((""));

    return type;
    }


CNcdNodeFactory::TNcdNodeType CNcdNodeFactory::NodeTypeL( const CNcdNode& aNode )
    {
    DLTRACEIN((""));
    
    TNcdNodeType type( NodeTypeL( aNode.ClassId() ) );

    DLTRACEOUT((""));

    return type;
    }


CNcdNodeFactory::TNcdNodeType CNcdNodeFactory::NodeTypeL( NcdNodeClassIds::TNcdNodeClassId aClassId )
    {        
    // Check the class ids and return the correct value according to that.
    // Use switch case instead of if-else.

    TNcdNodeType type( ENcdNodeItem );

    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdItemNodeClassId:
            DLTRACE(("Item node"));
            type = ENcdNodeItem;
            break;
            
        case NcdNodeClassIds::ENcdSearchItemNodeClassId:
            DLTRACE(("Search item"));
            type = ENcdNodeItem;
            break;
            
        case NcdNodeClassIds::ENcdFolderNodeClassId:
            DLTRACE(("Folder node"));
            type = ENcdNodeFolder;
            break;

        case NcdNodeClassIds::ENcdTransparentFolderNodeClassId:
            DLTRACE(("Transparent folder"));
            type = ENcdNodeFolder;
            break;

        case NcdNodeClassIds::ENcdSearchFolderNodeClassId:
            DLTRACE(("Search folder"));
            type = ENcdNodeFolder;
            break;

        case NcdNodeClassIds::ENcdBundleFolderNodeClassId:
            DLTRACE(("Bundle folder"));
            type = ENcdNodeFolder;
            break;
            
        case NcdNodeClassIds::ENcdRootNodeClassId:
            DLTRACE(("Root node"));
            type = ENcdNodeRoot;
            break;
            
        case NcdNodeClassIds::ENcdSearchRootNodeClassId:
            DLTRACE(("Search root node"));
            type = ENcdNodeRoot;
            break;

        case NcdNodeClassIds::ENcdSupplierNodeClassId:
            DLTRACE(("Supplier node"));
            type = ENcdNodeSupplier;
            break;
        
        case NcdNodeClassIds::ENcdSearchBundleNodeClassId:
            DLTRACE(("Search bundle folder"));
            type = ENcdNodeSearchBundle;
            break;
    
        default:
            DLERROR(("Unknown class id"));
            // For debuggin purpose
            DASSERT( EFalse );
            User::Leave( KErrUnknown );
            break;            
        }    

    return type;
    }


CNcdNodeFactory::TNcdNodePurpose CNcdNodeFactory::NodePurposeL( const CNcdNode& aNode )
    {
    DLTRACEIN((""));
    
    TNcdNodePurpose purpose( NodePurposeL( aNode.ClassId() ) );

    DLTRACEOUT((""));

    return purpose;    
    }


CNcdNodeFactory::TNcdNodePurpose CNcdNodeFactory::NodePurposeL( NcdNodeClassIds::TNcdNodeClassId aClassId )
    {    
    // Check the class ids and return the correct value according to that.
    // Use switch case instead of if-else.

    TNcdNodePurpose purpose( ENcdNormalNode );

    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdItemNodeClassId:
            DLTRACE(("Item node"));
            purpose = ENcdNormalNode;
            break;
            
        case NcdNodeClassIds::ENcdSearchItemNodeClassId:
            DLTRACE(("Search item"));
            purpose = ENcdSearchNode;
            break;
            
        case NcdNodeClassIds::ENcdFolderNodeClassId:
            DLTRACE(("Folder node"));
            purpose = ENcdNormalNode;
            break;

        case NcdNodeClassIds::ENcdTransparentFolderNodeClassId:
            DLTRACE(("Transparent folder"));
            purpose = ENcdTransparentNode;
            break;

        case NcdNodeClassIds::ENcdSearchFolderNodeClassId:
            DLTRACE(("Search folder"));
            purpose = ENcdSearchNode;
            break;

        case NcdNodeClassIds::ENcdBundleFolderNodeClassId:
            DLTRACE(("Bundle folder"));
            purpose = ENcdBundleNode;
            break;
            
        case NcdNodeClassIds::ENcdRootNodeClassId:
            DLTRACE(("Root node"));
            purpose = ENcdNormalNode;
            break;
            
        case NcdNodeClassIds::ENcdSearchRootNodeClassId:
            DLTRACE(("Search root node"));
            purpose = ENcdSearchNode;
            break;

        case NcdNodeClassIds::ENcdSupplierNodeClassId:
            DLTRACE(("Supplier node"));
            purpose = ENcdNormalNode;
            break;
            
        case NcdNodeClassIds::ENcdSearchBundleNodeClassId:
            DLTRACE(("Search bundle folder"));
            purpose = ENcdSearchNode;
            break;
    
        default:
            DLERROR(("Unknown class id"));
            // For debuggin purpose
            DASSERT( EFalse );
            User::Leave( KErrUnknown );
            break;            
        }

    return purpose;
    }



// ---------------------------------------------------------------------------
// Functions to find out the node class id
// ---------------------------------------------------------------------------

NcdNodeClassIds::TNcdNodeClassId CNcdNodeFactory::NodeClassIdL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                                               CNcdNodeFactory::TNcdNodePurpose aNodePurpose )
    {
    
    NcdNodeClassIds::TNcdNodeClassId classId( NcdNodeClassIds::ENcdNullObjectClassId );

    // Check if the node is item, folder or root    
    switch( aNodeType )
        {
        case ENcdNodeItem:
            {
            // The node is item.
            // Check what kind of item.
            switch( aNodePurpose )
                {
                case ENcdNormalNode:
                    DLTRACE(("Normal item node class id"));
                    classId = NcdNodeClassIds::ENcdItemNodeClassId;
                    break;
                    
                case ENcdChildOfTransparentNode:
                    DLTRACE(("Transparent child item class id"));
                    classId = NcdNodeClassIds::ENcdItemNodeClassId;
                    break;

                case ENcdSearchNode:
                    DLTRACE(("Search item class id"));
                    classId = NcdNodeClassIds::ENcdSearchItemNodeClassId;
                    break;
            
                default:
                    DLERROR(("Unknown node item purpose"));
                    // For debuggin purpose
                    DASSERT( EFalse );
                    User::Leave( KErrUnknown );
                    break;            
                }            
            }
            break;

        case ENcdNodeFolder:
            {
            // The node is folder.
            // Check what kind of folder.
            switch( aNodePurpose )
                {
                case ENcdNormalNode:
                    DLTRACE(("Normal folder class id"));
                    classId = NcdNodeClassIds::ENcdFolderNodeClassId;
                    break;

                case ENcdChildOfTransparentNode:
                    DLTRACE(("Transparent child folder class id"));
                    classId = NcdNodeClassIds::ENcdFolderNodeClassId;
                    break;

                case ENcdSearchNode:
                    DLTRACE(("Search folder class id"));
                    classId = NcdNodeClassIds::ENcdSearchFolderNodeClassId;
                    break;

                case ENcdBundleNode:
                    DLTRACE(("Bundle folder class id"));
                    classId = NcdNodeClassIds::ENcdBundleFolderNodeClassId;
                    break;

                case ENcdTransparentNode:
                    DLTRACE(("Transparent folder class id"));
                    classId = NcdNodeClassIds::ENcdTransparentFolderNodeClassId;
                    break;

                default:
                    DLERROR(("Unknown node folder purpose"));
                    // For debuggin purpose
                    DASSERT( EFalse );
                    User::Leave( KErrUnknown );
                    break;            
                }            
            }
            break;

        case ENcdNodeRoot:
            {
            switch( aNodePurpose )
                {
                case ENcdNormalNode:
                    DLTRACE(("Root class id"));
                    classId = NcdNodeClassIds::ENcdRootNodeClassId;
                    break;

                case ENcdSearchNode:
                    DLTRACE(("Search root as normal folder class id"));
                    // Notice that the search root is actually normal search folder.
                    // Should this be changed when the id functions properly?
                    classId = NcdNodeClassIds::ENcdSearchRootNodeClassId;
                    break;
                
                default:
                    DLERROR(("Unknown node root purpose"));
                    // For debuggin purpose
                    DASSERT( EFalse );
                    User::Leave( KErrUnknown );
                    break;                           
                }
            }
            break;

        case ENcdNodeSupplier:
            {
            // The supplier class id will always be the same.
            classId = NcdNodeClassIds::ENcdSupplierNodeClassId;            
            }
        break;
        
        case ENcdNodeSearchBundle:
            {
            // The search bundle class id will always be the same.
            classId = NcdNodeClassIds::ENcdSearchBundleNodeClassId;            
            }
        break;
        
        default:
            DLERROR(("Unknown node type"));
            // For debuggin purpose
            DASSERT( EFalse );
            User::Leave( KErrUnknown );
            break;            
        };
        
    return classId;
    }



// ---------------------------------------------------------------------------
// Functions to create node objects
// ---------------------------------------------------------------------------

CNcdNodeItem* CNcdNodeFactory::CreateNodeItemL( const CNcdNodeIdentifier& aParentIdentifier,
                                                const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                                NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    CNcdNodeItem* item( CreateNodeItemLC( aParentIdentifier, 
                                          aMetaDataIdentifier, 
                                          aClassId ) );
    if ( item != NULL )
        {
        CleanupStack::Pop( item );
        }

    DLTRACEOUT((""));

    return item;
    }


CNcdNodeItem* CNcdNodeFactory::CreateNodeItemLC( const CNcdNodeIdentifier& aParentIdentifier,
                                                 const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                                 NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.

    CNcdNodeIdentifier* identifier = 
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentIdentifier, aMetaDataIdentifier );


    // First check if the node can be created as an item.
    CNcdNodeItem* item( CreateNodeItemLC( *identifier, 
                                          aClassId ) );

    CleanupStack::PopAndDestroy( identifier );
        
    DLTRACEOUT((""));
        
    return item;    
    }


CNcdNodeItem* CNcdNodeFactory::CreateNodeItemL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    CNcdNodeItem* item( CreateNodeItemLC( aNodeIdentifier, 
                                          aClassId) );
                                          
    if ( item != NULL )
        {
        CleanupStack::Pop( item );
        }

    DLTRACEOUT((""));

    return item;
    }


CNcdNodeItem* CNcdNodeFactory::CreateNodeItemLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                 NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNodeItem* item( NULL );

    // Create object according to the class type
    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdItemNodeClassId:
            DLINFO(("Item node"));
            item = CNcdNodeItem::NewLC( NodeManager(), aNodeIdentifier );
            break;
            
        case NcdNodeClassIds::ENcdSearchItemNodeClassId:
            DLINFO(("Search item node"));
            item = CNcdSearchNodeItem::NewLC( NodeManager(), aNodeIdentifier );
            break;
            
        default:
            DLERROR(("Unknown node item class id. Return NULL"));
            // Do not insert assert here because CreateNodeL(C) function uses this
            // function among other create functions to create nodes.
            item = NULL;
            break;            
        }

    DLTRACEOUT((""));

    return item;    
    }



CNcdNodeFolder* CNcdNodeFactory::CreateNodeFolderL( const CNcdNodeIdentifier& aParentIdentifier,
                                                    const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    CNcdNodeFolder* folder( CreateNodeFolderLC( aParentIdentifier, 
                                                aMetaDataIdentifier, 
                                                aClassId) );
    if ( folder != NULL ) 
        {
        CleanupStack::Pop( folder );
        }

    DLTRACEOUT((""));

    return folder;
    }


CNcdNodeFolder* CNcdNodeFactory::CreateNodeFolderLC( const CNcdNodeIdentifier& aParentIdentifier,
                                                     const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                                     NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.

    CNcdNodeIdentifier* identifier = 
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentIdentifier, aMetaDataIdentifier );


    // First check if the node can be created as an item.
    CNcdNodeFolder* folder( CreateNodeFolderLC( *identifier, 
                                                aClassId ) );

    CleanupStack::PopAndDestroy( identifier );
        
    DLTRACEOUT((""));
        
    return folder;    
    }


CNcdNodeFolder* CNcdNodeFactory::CreateNodeFolderL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNodeFolder* folder( CreateNodeFolderLC( aNodeIdentifier, aClassId ) );
    
    if ( folder != NULL )
        {
        CleanupStack::Pop( folder );
        }
        
    DLTRACEOUT((""));

    return folder;
    }


CNcdNodeFolder* CNcdNodeFactory::CreateNodeFolderLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                     NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNodeFolder* folder( NULL );

    // Create object according to the class type
    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdFolderNodeClassId:
            DLINFO(("Folder node"));
            folder = CNcdNodeFolder::NewLC( NodeManager(), aNodeIdentifier );
            break;

        case NcdNodeClassIds::ENcdTransparentFolderNodeClassId:
            DLINFO(("Transparent folder node"));
            folder = CNcdNodeTransparentFolder::NewLC( NodeManager(), aNodeIdentifier );
            break;

        case NcdNodeClassIds::ENcdSearchFolderNodeClassId:
            DLINFO(("Search folder node"));
            folder = CNcdSearchNodeFolder::NewLC( NodeManager(), aNodeIdentifier );
            break;

        case NcdNodeClassIds::ENcdRootNodeClassId:
            DLINFO(("Root node"));
            folder = CNcdRootNode::NewLC( NodeManager(), aNodeIdentifier );
            break;

        case NcdNodeClassIds::ENcdBundleFolderNodeClassId:
            DLINFO(("Bundle folder"));
            folder = CNcdBundleFolder::NewLC( NodeManager(), aNodeIdentifier );
            break;
            
        case NcdNodeClassIds::ENcdSearchRootNodeClassId:
            DLINFO(("Search root node from db"));
            folder = CNcdSearchRootNode::NewLC( NodeManager(), aNodeIdentifier );
            break;
            
        case NcdNodeClassIds::ENcdSearchBundleNodeClassId:
            DLINFO(("Search bundle folder"));
            folder = CNcdSearchNodeBundle::NewLC( NodeManager(), aNodeIdentifier );
            break;

        default:
            DLERROR(("Unknown folder node class id. Return NULL"));
            // Do not insert assert here because CreateNodeL(C) function uses this
            // function among other create function to create nodes.
            folder = NULL;
            break;            
        }

    DLTRACEOUT((""));

    return folder;    
    }


CNcdNodeSupplier* CNcdNodeFactory::CreateNodeSupplierL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNodeSupplier* node( CreateNodeSupplierLC( aNodeIdentifier, aClassId ) );
    
    if ( node != NULL )
        {
        CleanupStack::Pop( node );
        }
        
    DLTRACEOUT((""));

    return node;
    }


CNcdNodeSupplier* CNcdNodeFactory::CreateNodeSupplierLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                                         NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNodeSupplier* node( NULL );

    // Create object according to the class type
    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdSupplierNodeClassId:
            DLINFO(("Supplier node"));
            node = CNcdNodeSupplier::NewLC( NodeManager(), aNodeIdentifier );
            break;

        default:
            DLERROR(("Unknown supplier node class id. Return NULL"));
            // Do not insert assert here because CreateNodeL(C) function uses this
            // function among other create function to create nodes.
            node = NULL;
            break;            
        }

    DLTRACEOUT((""));

    return node;    
    }


CNcdNode* CNcdNodeFactory::CreateNodeL( const CNcdNodeIdentifier& aParentIdentifier,
                                        const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNode* node( CreateNodeLC( aParentIdentifier, aMetaDataIdentifier, aClassId ) );
    
    if ( node != NULL )
        {
        CleanupStack::Pop( node );
        }
        
    DLTRACEOUT((""));

    return node;
    }


CNcdNode* CNcdNodeFactory::CreateNodeLC( const CNcdNodeIdentifier& aParentIdentifier,
                                         const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                         NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.

    CNcdNodeIdentifier* identifier = 
        NcdNodeIdentifierEditor::CreateNodeIdentifierLC( aParentIdentifier, aMetaDataIdentifier );


    // First check if the node can be created as an item.
    CNcdNode* node( CreateNodeL( *identifier, aClassId ) );

    CleanupStack::PopAndDestroy( identifier );
    
    CleanupStack::PushL( node );
        
    DLTRACEOUT((""));
        
    return node;    
    }


CNcdNode* CNcdNodeFactory::CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.
    CNcdNode* node( CreateNodeLC( aNodeIdentifier, aClassId ) );
    
    if ( node != NULL )
        {
        CleanupStack::Pop( node );
        }
        
    DLTRACEOUT((""));

    return node;
    }


CNcdNode* CNcdNodeFactory::CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                         NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the node, 
    // but does not internalize its data from anywhere.

    // First check if the node can be created as an item.
    CNcdNode* node( CreateNodeItemLC( aNodeIdentifier, 
                                      aClassId ) );

        
    if ( node == NULL )
        {
        // Node was not an item. So, create a folder.
        node = CreateNodeFolderLC( aNodeIdentifier, 
                                   aClassId );
        }

    if ( node == NULL )
        {
        // Node was none of above. So, try to create node supplier.
        node = CreateNodeSupplierLC( aNodeIdentifier, aClassId );
        }

    DLINFO(("Node should have been created"));
    if ( node == NULL )
        {
        DLERROR(("Null node"))
        DASSERT( EFalse );
        User::Leave( KErrNotFound );
        }

    DLTRACEOUT((""));
        
    return node;    
    }



CNcdNode* CNcdNodeFactory::CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                        RReadStream& aStream ) const
    {
    DLTRACEIN((""));
    
    // Creates the node 
    // and internalizes its data from given stream.
    CNcdNode* node( CreateNodeLC( aNodeIdentifier, aStream ) );
    
    if ( node != NULL )
        {
        CleanupStack::Pop( node );
        }
        
    DLTRACEOUT((""));

    return node;    
    }
    
CNcdNode* CNcdNodeFactory::CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                         RReadStream& aStream ) const
    {
    DLTRACEIN((""));
    
    // Creates the node 
    // and internalizes its data from given stream.
    CNcdNode* node( NULL );
    
    // The first four bits (TInt32) should describe the class type of the data.
    // This data type is used to create the right class and the stream data
    // after that will be used to initialize the class object.    
    NcdNodeClassIds::TNcdNodeClassId classId =
        static_cast<NcdNodeClassIds::TNcdNodeClassId>( aStream.ReadInt32L() );
 
    DLINFO(("classId: %d", classId ));
     
    // Creates the corresponding node
    node = CreateNodeLC( aNodeIdentifier, classId );   

    // Internalizes the node from the stream
    node->InternalizeL( aStream );

    DLTRACEOUT((""));

    return node;
    }
    
    
CNcdNode* CNcdNodeFactory::CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier,
                                        const TDesC8& aData ) const
    {
    DLTRACEIN((""));

    CNcdNode* node( NULL );
    
    // Reads the class id from the stream
    if ( aData.Length() == 0 )
        {
        DLINFO(("Node data was empty."));
        DASSERT( EFalse );
        User::Leave( KErrArgument );        
        }
                                    
    DLINFO(("Create stream from data length: %d", aData.Length()));
    RDesReadStream stream( aData );                                   
    CleanupClosePushL( stream );
 
    node = CreateNodeL( aNodeIdentifier, stream );

    CleanupStack::PopAndDestroy( &stream );
    
    DLTRACEOUT((""));

    return node;    
    }
  
    
CNcdNode* CNcdNodeFactory::CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier, 
                                         const TDesC8& aData ) const
    {
    DLTRACEIN((""));
    
    // Creates the node 
    // and internalizes its data from given data.
    CNcdNode* node( CreateNodeL( aNodeIdentifier, aData ) );
    
    if ( node != NULL )
        {
        CleanupStack::PushL( node );
        }
        
    DLTRACEOUT((""));

    return node;
    }
    
    
void CNcdNodeFactory::InternalizeNodeL( CNcdNode& aNode, const TDesC8& aData ) 
    {
    DLTRACEIN((""));
    
    // Reads the class id from the stream
    if ( aData.Length() == 0 )
        {
        DLINFO(("Node data was empty."));
        DASSERT( EFalse );
        User::Leave( KErrArgument );        
        }

    DLINFO(("Create stream from data length: %d", aData.Length()));
    RDesReadStream stream( aData );                                   
    CleanupClosePushL( stream );

    // The first four bits (TInt32) should describe the class type of the data.
    // This data type is used to create the right class and the stream data
    // after that will be used to initialize the class object.    
    NcdNodeClassIds::TNcdNodeClassId classId =
        static_cast<NcdNodeClassIds::TNcdNodeClassId>( stream.ReadInt32L() );
    if ( aNode.ClassId() != classId ) 
        {
        User::Leave( KErrArgument );
        DASSERT( EFalse );
        }
        
    aNode.InternalizeL( stream );    
    CleanupStack::PopAndDestroy( &stream );
    }


// ---------------------------------------------------------------------------
// Functions to get metadata type info
// ---------------------------------------------------------------------------


CNcdNodeFactory::TNcdNodeType CNcdNodeFactory::MetaDataTypeL( const CNcdNodeMetaData& aNode )
    {
    DLTRACEIN((""));
    
    TNcdNodeType type( MetaDataTypeL( aNode.ClassId() ) );

    DLTRACEOUT((""));

    return type;
    }


CNcdNodeFactory::TNcdNodeType CNcdNodeFactory::MetaDataTypeL( NcdNodeClassIds::TNcdNodeClassId aClassId )
    {
    DLTRACEIN((""));
    
    // Check the class ids and return the correct value according to that.
    // Use switch case instead of if-else.

    TNcdNodeType type( ENcdNodeItem );

    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdItemNodeMetaDataClassId:
            DLINFO(("Item metadata"));
            type = ENcdNodeItem;
            break;

        case NcdNodeClassIds::ENcdFolderNodeMetaDataClassId:
            DLINFO(("Folder metadata"));
            type = ENcdNodeFolder;
            break;

        case NcdNodeClassIds::ENcdSupplierNodeMetaDataClassId:
            DLINFO(("Folder metadata"));
            type = ENcdNodeSupplier;
            break;
               
        default:
            DLERROR(("Unknown class id"));
            // For debuggin purpose
            DASSERT( EFalse );
            User::Leave( KErrUnknown );
            break;            
        }

    DLTRACEOUT((""));

    return type;
    }



// ---------------------------------------------------------------------------
// Functions to find out the metadata class id
// ---------------------------------------------------------------------------

NcdNodeClassIds::TNcdNodeClassId CNcdNodeFactory::MetaDataClassId( CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    DLTRACEIN((""));
    
    NcdNodeClassIds::TNcdNodeClassId classId( NcdNodeClassIds::ENcdNullObjectClassId );

    // Check if the node is item, folder or root    
    switch( aNodeType )
        {
        case ENcdNodeItem:
            DLINFO(("Item metadata"));
            classId = NcdNodeClassIds::ENcdItemNodeMetaDataClassId;
            break;

        case ENcdNodeFolder:
            DLINFO(("Folder metadata"));
            classId = NcdNodeClassIds::ENcdFolderNodeMetaDataClassId;
            break;

        case ENcdNodeSupplier:
            DLINFO(("Supplier metadata"));
            classId = NcdNodeClassIds::ENcdSupplierNodeMetaDataClassId;
            break;
        
        default:
            DLERROR(("Unknown node type"));
            // For debuggin purpose
            DASSERT( EFalse );
            _LIT( KCategory, "Unknown node type" );
            User::Panic( KCategory, KErrUnknown );
            break;            
        };

    DLTRACEOUT((""));
        
    return classId;
    }



// ---------------------------------------------------------------------------
// Functions to create metadata objects
// ---------------------------------------------------------------------------

CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier,
                                                    TNcdNodeType aMetaType ) const
    {
    DLTRACEIN((""));
    
    CNcdNodeMetaData* metaData( CreateMetaDataLC( aIdentifier, aMetaType ) );
    
    if ( metaData != NULL )
        {
        CleanupStack::Pop( metaData );
        }
    
    DLTRACEOUT((""));
    
    return metaData;    
    }

CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                                     TNcdNodeType aMetaType ) const
    {
    DLTRACEIN((""));
    
    CNcdNodeMetaData* metaData( NULL );

    switch( aMetaType )
        {
        case ENcdNodeItem:
            metaData = 
                CreateMetaDataLC( aIdentifier, 
                                  NcdNodeClassIds::ENcdItemNodeMetaDataClassId );
            break;

        case ENcdNodeFolder:
            metaData = 
                CreateMetaDataLC( aIdentifier, 
                                  NcdNodeClassIds::ENcdFolderNodeMetaDataClassId );
            break;

        default:
            DLERROR(("Unknown metadata class id."));
            DASSERT( EFalse );
            User::Leave( KErrArgument );
            break;                    
        }

    DASSERT( metaData );
        
    DLTRACEOUT((""));    
    return metaData;    
    }


CNcdNodeItemMetaData* CNcdNodeFactory::CreateItemMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                                            NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    CNcdNodeItemMetaData* item( CreateItemMetaDataLC( aIdentifier, aClassId ) );
                                          
    if ( item != NULL )
        {
        CleanupStack::Pop( item );
        }

    DLTRACEOUT((""));

    return item;
    }


CNcdNodeItemMetaData* CNcdNodeFactory::CreateItemMetaDataLC( const CNcdNodeIdentifier& aIdentifier, 
                                                             NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata, 
    // but does not internalize its data from anywhere.
    CNcdNodeItemMetaData* item( NULL );

    // Create object according to the class type
    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdItemNodeMetaDataClassId:
            DLINFO(("Item meta"));
            item = CNcdNodeItemMetaData::NewLC( aIdentifier, NodeManager() );
            break;
                        
        default:
            DLERROR(("Unknown metadata item class id. Return NULL"));
            // Do not insert assert here because CreateMetaDataL(C) function uses this
            // function and CreateMetadataFolderL(C) function to create nodes.
            item = NULL;
            break;            
        }

    DLTRACEOUT((""));

    return item;    
    }


CNcdNodeFolderMetaData* CNcdNodeFactory::CreateFolderMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                                                NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata, 
    // but does not internalize its data from anywhere.
    CNcdNodeFolderMetaData* folder( CreateFolderMetaDataLC( aIdentifier, aClassId ) );
    
    if ( folder != NULL )
        {
        CleanupStack::Pop( folder );
        }
        
    DLTRACEOUT((""));

    return folder;
    }


CNcdNodeFolderMetaData* CNcdNodeFactory::CreateFolderMetaDataLC( const CNcdNodeIdentifier& aIdentifier, 
                                                                 NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));

    // Creates the metadata, 
    // but does not internalize its data from anywhere.
    CNcdNodeFolderMetaData* folder( NULL );

    // Create object according to the class type
    switch( aClassId )
        {
        case NcdNodeClassIds::ENcdFolderNodeMetaDataClassId:
            DLINFO(("Folder meta"));
            folder = CNcdNodeFolderMetaData::NewLC( aIdentifier, NodeManager() );
            break;

        default:
            DLERROR(("Unknown folder metadata class id. Return NULL"));
            // Do not insert assert here because CreateMetaDataL(C) function uses this
            // function and CreateFolderMetaDataL(C) function to create nodes.
            folder = NULL;
            break;            
        }

    DLTRACEOUT((""));

    return folder;    
    }


CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata, 
    // but does not internalize its data from anywhere.
    CNcdNodeMetaData* metadata( CreateMetaDataLC( aIdentifier, aClassId ) );
    
    if ( metadata != NULL )
        {
        CleanupStack::Pop( metadata );
        }
        
    DLTRACEOUT((""));

    return metadata;
    }


CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier, 
                                                     NcdNodeClassIds::TNcdNodeClassId aClassId ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata, 
    // but does not internalize its data from anywhere.

    // First check if the metadata can be created as an item.
    CNcdNodeMetaData* metadata( CreateItemMetaDataLC( aIdentifier, 
                                                      aClassId ) );

        
    if ( metadata == NULL )
        {
        // Node was not an item. So, create a folder.
        metadata = CreateFolderMetaDataLC( aIdentifier, 
                                           aClassId );
        }

    DLTRACEOUT((""));
        
    return metadata;    
    }



CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                                    RReadStream& aStream ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata 
    // and internalizes its data from given stream.
    CNcdNodeMetaData* metadata( CreateMetaDataLC( aIdentifier, aStream ) );
    
    if ( metadata != NULL )
        {
        CleanupStack::Pop( metadata );
        }
        
    DLTRACEOUT((""));

    return metadata;    
    }
    
CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier, 
                                                     RReadStream& aStream ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata 
    // and internalizes its data from given stream.
    CNcdNodeMetaData* metadata( NULL );
    
    // The first four bits (TInt32) should describe the class type of the data.
    // This data type is used to create the right class and the stream data
    // after that will be used to initialize the class object.    
    NcdNodeClassIds::TNcdNodeClassId classId =
        static_cast<NcdNodeClassIds::TNcdNodeClassId>( aStream.ReadInt32L() );
 
    DLINFO(("classId: %d", classId ));
     
    // Creates the corresponding node
    metadata = CreateMetaDataLC( aIdentifier, classId );   

    // Internalizes the metadata from the stream
    metadata->InternalizeL( aStream );

    DLTRACEOUT((""));

    return metadata;
    }
    
    
CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier,
                                                    const TDesC8& aData ) const
    {
    DLTRACEIN((""));

    CNcdNodeMetaData* metadata( NULL );
    
    // Reads the class id from the stream
    DASSERT( aData.Length() );
                                    
    DLINFO(("Create stream from data length: %d", aData.Length()));
    RDesReadStream stream( aData );                                   
    CleanupClosePushL( stream );
 
    metadata = CreateMetaDataL( aIdentifier, stream );

    CleanupStack::PopAndDestroy( &stream );
    
    DLTRACEOUT((""));

    return metadata;    
    }
  
    
CNcdNodeMetaData* CNcdNodeFactory::CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier, 
                                                     const TDesC8& aData ) const
    {
    DLTRACEIN((""));
    
    // Creates the metadata 
    // and internalizes its data from given data.
    CNcdNodeMetaData* metadata( CreateMetaDataL( aIdentifier, aData ) );
    
    if ( metadata != NULL )
        {
        CleanupStack::PushL( metadata );
        }
        
    DLTRACEOUT((""));

    return metadata;
    }



// ---------------------------------------------------------------------------
// Protected functions
// ---------------------------------------------------------------------------

CNcdNodeManager& CNcdNodeFactory::NodeManager() const
    {
    return iNodeManager;
    }

