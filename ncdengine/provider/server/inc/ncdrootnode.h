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
* Description:   Contains CNcdRootNode class
*
*/


#ifndef NCDROOTNODE_H
#define NCDROOTNODE_H


#include "ncdparentoftransparentnode.h"

class CNcdNodeManager;
class CNcdNodeIdentifier;
class CNcdContentSourceMap;


class CNcdRootNode : public CNcdParentOfTransparentNode
    {
public:
    /**
     * @see CNcdNodeFolder::NewL
     *
     * @return CNcdRootNode* Pointer to the created object 
     * of this class.
     */
    static CNcdRootNode* NewL( CNcdNodeManager& aNodeManager,
                               const CNcdNodeIdentifier& aIdentifier );

    /**
     * @see CNcdRootNode::NewL
     */
    static CNcdRootNode* NewLC( CNcdNodeManager& aNodeManager,
                                const CNcdNodeIdentifier& aIdentifier );
                                  
    /**
     * Destructor.
     */
    virtual ~CNcdRootNode();
    
    CNcdContentSourceMap& ContentSourceMap();
    void SetContentSourceMap( CNcdContentSourceMap* aMap );
    
    
public: // CNcdNode

    /**
     * @see CNcdParentOfTransparentNode::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdParentOfTransparentNode::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );

    
    
public: // CNcdNodeFolder

     /** 
      * @see CNcdNodeFolder::ServerChildCount
      */
    virtual TInt ServerChildCount() const;

    /** 
     * @see CNcdNodeFolder::ChildByServerIndexL
     */
    virtual const CNcdNodeIdentifier& ChildByServerIndexL( TInt aIndex ) const;


protected:

    /**
     * @see CNcdParentOfTransparentNode::CNcdParentOfTransparentNode
     */
    CNcdRootNode( CNcdNodeManager& aNodeManager,
                  NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdRootNodeClassId );
    
    /**
     * @see CNcdParentOfTransparentNode::ConstructL
     */
    void ConstructL( const CNcdNodeIdentifier& aIdentifier );

    
private: // data

    CNcdContentSourceMap* iContentSourceMap;
    };

#endif // NCDROOTNODE_H