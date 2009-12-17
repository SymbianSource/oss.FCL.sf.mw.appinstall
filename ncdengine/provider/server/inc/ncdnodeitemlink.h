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
* Description:   Contains CNcdNodeItemLink class
*
*/


#ifndef NCD_NODE_ITEM_LINK_H
#define NCD_NODE_ITEM_LINK_H


#include "ncdnodelink.h"
#include "ncdnodeclassids.h"

class CNcdNode;


/**
 *  CNcdNodeItemLink ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeItemLink : public CNcdNodeLink
    {

public:

    /**
     * NewL
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeItemLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemLink* NewL( CNcdNode& aNode );

    /**
     * NewLC
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeItemLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeItemLink* NewLC( CNcdNode& aNode );


    /**
     * Destructor
     */
    virtual ~CNcdNodeItemLink();


public: // CNcdNodeLink

    /**
     * @see CNcdNodeLink::InternalizeL
     */
    virtual void InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                               const CNcdNodeIdentifier& aParentIdentifier,
                               const CNcdNodeIdentifier& aRequestParentIdentifier,
                               const TUid& aClientUid );


    /**
     * @see CNcdNodeLink::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNodeLink::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * Constructor
     */
    CNcdNodeItemLink( CNcdNode& aNode,
                      NcdNodeClassIds::TNcdNodeClassId aClassId = NcdNodeClassIds::ENcdItemNodeLinkClassId );

    /**
     * ConstructL
     */
    virtual void ConstructL();

    /**
     * This function adds possible item link data to the stream after
     * the parent class data.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;
    
    
private:

    // Prevent these two if they are not implemented
    CNcdNodeItemLink( const CNcdNodeItemLink& aObject );
    CNcdNodeItemLink& operator =( const CNcdNodeItemLink& aObject );


private: // data

    };


#endif // NCD_NODE_ITEM_LINK_H
