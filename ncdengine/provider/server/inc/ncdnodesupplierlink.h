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
* Description:   Contains CNcdNodeSupplierLink class
*
*/


#ifndef NCDNODESUPPLIERLINK_H
#define NCDNODESUPPLIERLINK_H


#include "ncdnodelink.h"
#include "ncdnodeclassids.h"

class CNcdNodeSupplierLink : public CNcdNodeLink
{
public:
    
    /**
     * NewL
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeItemLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSupplierLink* NewL( CNcdNode& aNode );

    /**
     * NewLC
     *
     * @param aNode The node that owns this link.
     * @return CNcdNodeItemLink* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSupplierLink* NewLC( CNcdNode& aNode );


    /**
     * Destructor
     */
    virtual ~CNcdNodeSupplierLink();
    

public: // CNcdNodeLink

    /**
     * @see CNcdNodeLink::InternalizeL
     */
    virtual void InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                               const CNcdNodeIdentifier& aParentIdentifier,
                               const CNcdNodeIdentifier& aRequestParentIdentifier );
                               
protected:

    /**
     * Constructor.
     */
    CNcdNodeSupplierLink(
        CNcdNode& aNode,
        NcdNodeClassIds::TNcdNodeClassId aClassId = NcdNodeClassIds::ENcdSupplierNodeLinkClassId );
                      
private:

    // Prevent these two if they are not implemented
    CNcdNodeSupplierLink( const CNcdNodeSupplierLink& aObject );
    CNcdNodeSupplierLink& operator =( const CNcdNodeSupplierLink& aObject );
                                                          
};

#endif // NCDNODESUPPLIERLINK_H