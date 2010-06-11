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
* Description:   Contains NcdNodeIdentifierEditor namespace
*
*/


#ifndef NCD_NODE_IDENTIFIER_EDITOR_H
#define NCD_NODE_IDENTIFIER_EDITOR_H


#include <e32std.h>

class CNcdNodeIdentifier;


/**
 *  This class object should be used to create node and metadata
 *  identifiers instead of directly creating them in different places
 *  of code.
 */
namespace NcdNodeIdentifierEditor
    {
    
    // Functions to create correct node identifiers

    /**
     * @param aParentIdentifier
     * @param aMetaDataIdentifier
     * @return CNcdNodeIdentifier* The identifier that has been created by using the
     * parent id and the metadata id. Ownership is transferred.
     */
    CNcdNodeIdentifier* CreateNodeIdentifierLC( const CNcdNodeIdentifier& aParentIdentifier,
                                                const CNcdNodeIdentifier& aMetaDataIdentifier );

    void MarkMetaDataPartL( TLex& aMetaId );
    
    /**
     * Gets parent id and namespace from child's id.
     * If there is no parent the values will be set to KNullDesC.
     *
     * @param aChildId Child's id.
     * @param aParentId Will contain parent's id upon return.
     * @param aParentId Will contain parent's namespace upon return.
     */
    void GetParentIdAndNsL( const TDesC& aChildId, TPtrC& aParentId, TPtrC& aParentNs );
    
    /**
     * @see CNcdNodeIdentifierEditor::CreateMetaIdentifierLC
     */
    CNcdNodeIdentifier* CreateMetaDataIdentifierL( const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * @param aNodeIdentifier The identifier that identifies the node. (Contains parent info in id).
     * @return CNcdNodeIdentifier* Identifier of the metadata of the given node.
     */
    CNcdNodeIdentifier* CreateMetaDataIdentifierLC( const CNcdNodeIdentifier& aNodeIdentifier );
    
    /**
     * 
     */
    TBool DoesMetaDataIdentifierMatchL( const CNcdNodeIdentifier& aNodeIdentifier,
        const TDesC& aMetaId, const TDesC& aMetaNameSpace, const TUid& aMetaUid );

    TBool DoesMetaDataIdentifierMatchL( const CNcdNodeIdentifier& aMetaDataIdentifier,
        const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * All the root identifiers have the same namespace but the client id
     * is used also as the id.
     *
     * @param aUid Uid of the client whose root identifier is asked.
     * @return CNcdNodeIdentifier* Identifier of the root node of a given client.
     */
    CNcdNodeIdentifier* CreateRootIdentifierForClientLC( const TUid& aUid );

    /**
     * All the search root identifiers have the same namespace but the client id
     * is used also as the id.
     *
     * @param aUid Uid of the client whose search root identifier is asked.
     * @return CNcdNodeIdentifier* Identifier of the search root node of a given client.
     */
    CNcdNodeIdentifier* CreateSearchRootIdentifierForClientLC( const TUid& aUid );

    /**
     * This function creates the identifier for the temporary node. Temporary nodes are not child
     * of any other node. So, they will be left hanging themselves.
     *
     * @param aMetaDataIdentifier Identifies the metadata that this temporary node will contain.
     * @return CNcdNodeIdentifier* Identifies the temporary node.
     */ 
    CNcdNodeIdentifier* CreateTemporaryNodeIdentifierLC( const CNcdNodeIdentifier& aMetaDataIdentifier );    


    // Functions that can provide information that is included to the identifiers

    /**
     * This function returns the depth of the node in the node hierarchy.
     * 
     * @note Namespaces are omitted in the node id. So, only the metadata id parts are
     * counted for the depth. Depth is zero if only one metadata id is included to the node id.
     *
     * @param aNodeIdentifier Identifier of the node. The node id contains also
     * parent names. So, the depth can be calculated from id descriptor.
     * @return TInt The depth of the node. The root level is zero.
     */
    TInt NodeDepthL( const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * This function checks if the given identifier identifies
     * some root. The namespace information of the identifier
     * is used to check this. So, this function can be used
     * for both metadata and nodes. There may be multiple 
     * namespaces that identify different roots. For example, 
     * normal node root or search root etc.
     * 
     * @param aIdentifier Identifier that is used to check
     * if it identifies root some root. 
     * @return TBool ETrue if identifier identifies root. Else
     * EFalse.
     */
    TBool IdentifiesSomeRoot( const CNcdNodeIdentifier& aIdentifier );

    /**
     * This function check if the given identifier identifies
     * some temporary node, which does not belong under some root
     * node. In other words, the node just hangs by itself.
     *
     * @param aIdentifier The identifier of the node that is checked.
     * @return TBool ETrue if the node is temporary node. Else EFalse.
     */
    TBool IdentifiesTemporaryNodeL( const CNcdNodeIdentifier& aIdentifier );

    /**
     * @param aParentNodeIdentifier A supposed parent
     * @param aChildNodeIdentifier A supposed child or grand child etc. of parent
     * @return TBool ETrue if the child is the child or grand child etc. of the 
     * given parent. Else EFalse.
     */
    TBool ParentOf( const CNcdNodeIdentifier& aParentNodeIdentifier,
                    const CNcdNodeIdentifier& aChildNodeIdentifier );    


    /**
     * This function parses the child identifier and creates the parent information
     * from the child identifier.
     * @param aChildNodeIdentifier A supposed child or grand child etc. of parent
     * @return CNodeIdentifier* The identifier of the parent of the node that is identified
     * by the aChildNodeIdentifier.
     * @exception Leaves with KErrArgument if aChildNodeIdentifier contains empty fields
     * or if no parent can be deducted from the id (e.g. root node's identifier).
     */    
    CNcdNodeIdentifier* ParentOfLC( const CNcdNodeIdentifier& aChildNodeIdentifier );    

    /**
     * This function checks if the given identifier identifies
     * some search node.
     *
     * @param aIdentifier The identifier of the node that is checked.
     * @return TBool ETrue if the node is a search node. Else EFalse.
     */
    TBool IdentifiesSearchNodeL( const CNcdNodeIdentifier& aIdentifier );

    }

#endif // NCD_NODE_IDENTIFIER_EDITOR_H
