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
* Description:   Contains CNcdNodeItem class
*
*/


#ifndef NCD_NODE_ITEM_H
#define NCD_NODE_ITEM_H


#include <e32cmn.h>

#include "ncdnodeimpl.h"


class CNcdNodeItemLink;
class CNcdNodeMetaDataLink;


/**
 *  This class provides item specific functionality for the nodes.
 *
 *  @see CNcdNode
 */
class CNcdNodeItem : public CNcdNode
    {
    
public:

    /**
     * NewL
     *
     * Note, that node link contains the metadata id.
     * Because node link contains metadata id, set meta data function
     * is not provided in this class.
     * @return CNcdNodeItem* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItem* NewL( CNcdNodeManager& aNodeManager,
                               const CNcdNodeIdentifier& aIdentifier );

    /**
     * @see CNcdNodeItem::NewL
     */
    static CNcdNodeItem* NewLC( CNcdNodeManager& aNodeManager,
                                const CNcdNodeIdentifier& aIdentifier );


    /**
     * Destructor
     */
    virtual ~CNcdNodeItem();


protected:

    /**
     * @see CNcdNode::CNcdNode
     *
     */
    CNcdNodeItem( CNcdNodeManager& aNodeManager,
        NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdItemNodeClassId, 
        NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId = NcdNodeClassIds::ENcdItemNodeLinkClassId,
        NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId = NcdNodeClassIds::ENcdItemNodeMetaDataClassId );
    
    /**
     * @see CNcdNode::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );


    /**
     * @see CNcdNode::CreateLinkL
     */
    virtual CNcdNodeLink* CreateLinkL();


    /** 
     * @see CNcdNode::ExternalizeDataForRequestL
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    
    
    
private:
    
    // Prevent these two if they are not implemented
    CNcdNodeItem( const CNcdNodeItem& aObject );
    CNcdNodeItem& operator =( const CNcdNodeItem& aObject );


private: // data
    
    };


#endif // NCD_NODE_ITEM_H
