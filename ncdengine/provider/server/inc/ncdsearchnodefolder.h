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
* Description:   Contains CNcdSearchNodeFolder class
*
*/


#ifndef NCD_SEARCH_NODE_FOLDER_H
#define NCD_SEARCH_NODE_FOLDER_H


#include <e32cmn.h>

#include "ncdparentoftransparentnode.h"
//#include "ncdsearchnode.h"

class CNcdSearchFilter;

/**
 * This class provides search foler specific functionality for the nodes.
 */
class CNcdSearchNodeFolder : public CNcdParentOfTransparentNode
    {
    
public:

    /**
     * NewL
     *
     * Note, that node link contains the metadata id.
     * Because node link contains metadata id, set meta data function
     * is not provided in this class.
     * @return CNcdSearchNodeFolder* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchNodeFolder* NewL( CNcdNodeManager& aNodeManager,
                                       const CNcdNodeIdentifier& aIdentifier,
                                       TBool aIsTransparent = EFalse );

    /**
     * @see CNcdSearchNodeFolder::NewLC
     */
    static CNcdSearchNodeFolder* NewLC( CNcdNodeManager& aNodeManager,
                                        const CNcdNodeIdentifier& aIdentifier,
                                        TBool aIsTransparent = EFalse );


    /**
     * Destructor
     */
    virtual ~CNcdSearchNodeFolder();
    
    
    /**
     * Setter for search filter
     *
     * @param 
     */
    void SetSearchFilterL( const CNcdSearchFilter& aFilter );
    
    /**
     * Setter for origin identifier.
     */
    void SetOriginIdentifierL( const CNcdNodeIdentifier& aOriginIdentifier );
    
    /**
     * Getter for origin identifier.
     */
    const CNcdNodeIdentifier& OriginIdentifierL() const;
    
    /**
     * ETrue if transparent.
     */
    TBool IsTransparent() const;
    
    /**
     * Setter for transparent flag.
     */
    void SetTransparent( TBool aIsTransparent );
    
public: // CNcdNode

    /**
     * @see CNcdNode::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNode::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * @see CNcdNodeFolder::CNcdNodeFolder
     *
     */
    CNcdSearchNodeFolder( CNcdNodeManager& aNodeManager,
        NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdSearchFolderNodeClassId,
        TBool aIsTransparent = EFalse );
    
    /**
     * @see CNcdNodeFolder::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );
                             
    /** 
     * @see CNcdNode::ExternalizeDataForRequestL
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;


private:
    
    // Prevent these two if they are not implemented
    CNcdSearchNodeFolder( const CNcdSearchNodeFolder& aObject );
    CNcdSearchNodeFolder& operator =( const CNcdSearchNodeFolder& aObject );


private: // data
    
    /**
     * Search filter needs to be stored so that it can be used later when 
     * the children of this folder are loaded.
     */
    CNcdSearchFilter* iSearchFilter;
    
    /**
     * This identifies the origin of this search folder, i.e. the original
     * node that the search was started from (search node ids are different
     * because search nodes are added under search root node).
     *
     * This identifier can be used e.g. to get the correct access point for
     * this node.
     */
    CNcdNodeIdentifier* iOriginIdentifier;
    
    
    /**
     * ETrue if transparent.
     */
    TBool iIsTransparent;
    
    };


#endif // NCD_SEARCH_NODE_FOLDER_H
