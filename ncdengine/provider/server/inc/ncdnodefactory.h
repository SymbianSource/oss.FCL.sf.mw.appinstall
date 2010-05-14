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
* Description:   Contains CNcdNodeFactory class
*
*/


#ifndef NCD_NODE_FACTORY_H
#define NCD_NODE_FACTORY_H


// For streams
#include <s32mem.h>

// For CBase
#include <e32base.h>

#include "ncdnodeclassids.h"

class CNcdNodeIdentifier;
class CNcdNodeManager;
class CNcdNodeDbManager;
class CNcdNode;
class CNcdNodeItem;
class CNcdNodeFolder;
class CNcdNodeSupplier;
class CNcdNodeMetaData;
class CNcdNodeItemMetaData;
class CNcdNodeFolderMetaData;

/**
 *  This class object should be used to create node and metadata
 *  objects instead of directly creating them in different places
 *  of code.
 *  Also, functions to check the object types are provided here.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeFactory : public CBase
    {

public:

    /**
     * TNcdNodeType is used to inform the general type of the
     * node. Folder may contain other folders or items.
     */
    enum TNcdNodeType
        {
        /**
         * The node is some kind of item node.
         */ 
        ENcdNodeItem,

        /**
         * The node is some kind of folder node.
         */ 
        ENcdNodeFolder,
        
        /**
         * The node some kind of root
         */
        ENcdNodeRoot,
        
        /**
         * This node is not really a full node but a wrapper that will
         * contain an actual node when the type of the actual node is known
         * and the actual node can be created.
         */
        ENcdNodeSupplier,
        
        /**
         * The node is a search bundle folder (special type needed because
         * bundle searching needs special handling).
         */
        ENcdNodeSearchBundle
        };


    /**
     * Defines the purpose of the node. This enumeration can be used
     * to inform if node should be normal node or if it should have
     * some special meaning or functionality.
     */
    enum TNcdNodePurpose
        {
        /**
         * This means that normal nodes should be created.
         */
        ENcdNormalNode,

        /**
         * This means that the ref data is gotten as a result of the search.
         */
        ENcdSearchNode,

        /**
         * This means that the ref data was gotten from the configuration server
         * for the bundle node.
         */
        ENcdBundleNode,

        /**
         * This means that the configuration server has given data for the 
         * transparent node.
         */
        ENcdTransparentNode,

        /**
         * This means that the given data is for the child of the transparent parent
         * folder. This type should be used only for the nodes that are directly children
         * of the transparent folder. The grand children of the transparent folder should
         * use ENormalNode type.
         *
         * @note Because the parser gives the child data before the parent data, this type
         * can be used to check if the parent that will be created should be transparent folder
         * instead of for example normal 
         */
        ENcdChildOfTransparentNode
        
        };


    /**
     * NewL
     *
     * @param aNodeManager
     * @return CNcdNodeFactory* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFactory* NewL( CNcdNodeManager& aNodeManager );

    /**
     * NewLC
     *
     * @param aNodeManager
     * @return CNcdNodeFactory* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeFactory* NewLC( CNcdNodeManager& aNodeManager );


    /**
     * Destructor
     *
     */
    virtual ~CNcdNodeFactory();


public: // Functions to check the node types and node purposes

    /** 
     *
     */
    static TNcdNodeType NodeTypeL( const TDesC8& aNodeData );

    /**
     *
     */
    static TNcdNodeType NodeTypeL( const CNcdNode& aNode );

    /**
     *
     */
    static TNcdNodeType NodeTypeL( NcdNodeClassIds::TNcdNodeClassId aClassId );


    /**
     * @note Warning! Does not identify ENcdChildOfTransparentNode but returns
     * ENcdNormalNode instead.
     */
    static TNcdNodePurpose NodePurposeL( const CNcdNode& aNode );

    /**
     * @see CNcdNodeFactory::NodePurposeL
     */
    static TNcdNodePurpose NodePurposeL( NcdNodeClassIds::TNcdNodeClassId aClassId );    
    

public: // Functions to find out the node class id

    /**
     * This function gives the class id of a node that has given properties.
     * The class id can be used to create the node of the right type by
     * using the creation function of this class.
     *
     * @param aNodeType
     * @param aNodePurpose
     * @return NcdNodeClassIds::TNcdNodeClassId Class of the node that has
     * the given properties.
     */
    static NcdNodeClassIds::TNcdNodeClassId NodeClassIdL( CNcdNodeFactory::TNcdNodeType aNodeType,
                                                         CNcdNodeFactory::TNcdNodePurpose aNodePurpose );


public: // Functions to create node objects

    /**
     *
     */
    CNcdNodeItem* CreateNodeItemL( const CNcdNodeIdentifier& aParentIdentifier,
                                   const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                   NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeItemL
     */
    CNcdNodeItem* CreateNodeItemLC( const CNcdNodeIdentifier& aParentIdentifier,
                                    const CNcdNodeIdentifier& aMetaDataIdentifier,
                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     *
     */
    CNcdNodeItem* CreateNodeItemL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                   NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeItemL
     */
    CNcdNodeItem* CreateNodeItemLC( const CNcdNodeIdentifier& aNodeIdentifier,
                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const;
                                    

    /**
     *
     */
    CNcdNodeFolder* CreateNodeFolderL( const CNcdNodeIdentifier& aParentIdentifier,
                                       const CNcdNodeIdentifier& aMetaDataIdentifier, 
                                       NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeFolderL
     */
    CNcdNodeFolder* CreateNodeFolderLC( const CNcdNodeIdentifier& aParentIdentifier,
                                        const CNcdNodeIdentifier& aMetaDataIdentifier,
                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     *
     */
    CNcdNodeFolder* CreateNodeFolderL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                       NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeFolderL
     */
    CNcdNodeFolder* CreateNodeFolderLC( const CNcdNodeIdentifier& aNodeIdentifier,
                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     *
     */
    CNcdNodeSupplier* CreateNodeSupplierL( const CNcdNodeIdentifier& aNodeIdentifier, 
                                           NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeSupplierL
     */
    CNcdNodeSupplier* CreateNodeSupplierLC( const CNcdNodeIdentifier& aNodeIdentifier,
                                            NcdNodeClassIds::TNcdNodeClassId aClassId ) const;


    /**
     *
     */
    CNcdNode* CreateNodeL( const CNcdNodeIdentifier& aParentIdentifier,
                           const CNcdNodeIdentifier& aMetaDataIdentifier, 
                           NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeL
     */
    CNcdNode* CreateNodeLC( const CNcdNodeIdentifier& aParentIdentifier,
                            const CNcdNodeIdentifier& aMetaDataIdentifier,
                            NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     *
     */
    CNcdNode* CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier, 
                           NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeL
     */
    CNcdNode* CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier,
                            NcdNodeClassIds::TNcdNodeClassId aClassId ) const;


    /**
     *
     */
    CNcdNode* CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier,
                           RReadStream& aStream ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeL
     */
    CNcdNode* CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier,
                            RReadStream& aStream ) const;    


    /**
     *
     */
    CNcdNode* CreateNodeL( const CNcdNodeIdentifier& aNodeIdentifier,
                           const TDesC8& aData ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeL
     */
    CNcdNode* CreateNodeLC( const CNcdNodeIdentifier& aNodeIdentifier,
                            const TDesC8& aData ) const;    

    /** 
     * Internalizes the given node from the given data.
     *
     * @param aNode The node.
     * @param aData The data.
     */
    void InternalizeNodeL( CNcdNode& aNode, const TDesC8& aData );

public: // Functions to get metadata type info

    /**
     *
     */
    static TNcdNodeType MetaDataTypeL( const CNcdNodeMetaData& aMetaData );

    /**
     *
     */
    static TNcdNodeType MetaDataTypeL( NcdNodeClassIds::TNcdNodeClassId aClassId );


public: // Functions to find out the metadata class id

    /**
     * This function gives the class id of a metadata that has given properties.
     * The class id can be used to create the node of the right type by
     * using the creation function of this class.
     *
     * @param aMetaType
     * @return NcdNodeClassIds::TNcdNodeClassId Class of the node that has
     * the given properties.
     */
    static NcdNodeClassIds::TNcdNodeClassId MetaDataClassId( CNcdNodeFactory::TNcdNodeType aMetaType );


public: // Functions to create metadata objects

    /**
     *
     */
     CNcdNodeMetaData* CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier,
                                        TNcdNodeType aMetaType ) const;

    /**
     * @see CNcdNodeFactory::CreateMetaDataL
     */
    CNcdNodeMetaData* CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                        TNcdNodeType aMetaType ) const;


    /**
     *
     */
    CNcdNodeItemMetaData* CreateItemMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                               NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateItemMetaDataItemL
     */
    CNcdNodeItemMetaData* CreateItemMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                                NcdNodeClassIds::TNcdNodeClassId aClassId ) const;
                                    

    /**
     *
     */
    CNcdNodeFolderMetaData* CreateFolderMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                                   NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateNodeFolderL
     */
    CNcdNodeFolderMetaData* CreateFolderMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                                    NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     *
     */
    CNcdNodeMetaData* CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier, 
                                       NcdNodeClassIds::TNcdNodeClassId aClassId ) const;

    /**
     * @see CNcdNodeFactory::CreateMetaData
     */
    CNcdNodeMetaData* CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                        NcdNodeClassIds::TNcdNodeClassId aClassId ) const;


    /**
     *
     */
    CNcdNodeMetaData* CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier,
                                       RReadStream& aStream ) const;

    /**
     * @see CNcdNodeFactory::CreateMetaDataL
     */
    CNcdNodeMetaData* CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                        RReadStream& aStream ) const;    


    /**
     *
     */
    CNcdNodeMetaData* CreateMetaDataL( const CNcdNodeIdentifier& aIdentifier,
                                       const TDesC8& aData ) const;

    /**
     * @see CNcdNodeFactory::CreateMetaDataL
     */
    CNcdNodeMetaData* CreateMetaDataLC( const CNcdNodeIdentifier& aIdentifier,
                                        const TDesC8& aData ) const;    


protected:

    /**
     * Constructor
     *
     * @param aNodeManager     
     */
    CNcdNodeFactory( CNcdNodeManager& aNodeManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * @return CNcdNodeManager& Manager that owns this cleaner
     */
    CNcdNodeManager& NodeManager() const;


private:

    // Prevent these two if they are not implemented
    CNcdNodeFactory( const CNcdNodeFactory& aObject );
    CNcdNodeFactory& operator =( const CNcdNodeFactory& aObject );


private: // data
    
    // NodeManager that owns this factory
    CNcdNodeManager& iNodeManager;
    
    };
    
#endif // NCD_NODE_FACTORY_H    
