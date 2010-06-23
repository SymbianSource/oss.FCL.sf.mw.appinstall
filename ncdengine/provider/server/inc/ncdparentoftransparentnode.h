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
* Description:   Contains CNcdParentOfTransparentNode class
*
*/


#ifndef NCDPARENTOFTRANSPARENTNODE_H
#define NCDPARENTOFTRANSPARENTNODE_H

#include "ncdnodefolder.h"

class CNcdNodeManager;
class CNcdNodeIdentifier;


class CNcdParentOfTransparentNode : public CNcdNodeFolder
    {
public:
    /**
     * Destructor.
     */
    virtual ~CNcdParentOfTransparentNode();

public: //New functions

    /**
     * Checks whether a transparent child has been expired.
     *
     * @return ETrue if one of the children is expired/uninitialized or missing.
     */ 
    TBool IsTransparentChildExpiredL() const;

public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

public: // CNcdNode

    /**
     * @see CNcdNodeFolder::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see CNcdNodeFolder::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );
        

protected:

    /**
     * @see CNcdNodeFolder::CNcdNodeFolder
     */
    CNcdParentOfTransparentNode( CNcdNodeManager& aNodeManager,
                                 NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdRootNodeClassId );
    
    /**
     * @see CNcdNodeFolder::ConstructL
     */
    void ConstructL( const CNcdNodeIdentifier& aIdentifier );


    /** 
     * @see CNcdNode::ExternalizeDataForRequestL
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    


    /**
     * This function replaces transparent folder by their children
     * when the child array is sent for the proxy.
     *
     * @see CNcdNodeFolder::ExternalizeChildArrayForRequestL
     */
    virtual void ExternalizeChildArrayForRequestL( RWriteStream& aStream ) const;
    
    /**
     * Checks whether a transparent child has been expired, writes the answer to the message and
     * completes it.
     */ 
    void IsTransparentChildExpiredL( MCatalogsBaseMessage& aMessage ) const;
    
private:

    // Prevent these if not implemented    
    CNcdParentOfTransparentNode( const CNcdParentOfTransparentNode& aObject );
    CNcdParentOfTransparentNode& operator =( const CNcdParentOfTransparentNode& aObject );    
    
    
private: // data

    };

#endif // NCDPARENTOFTRANSPARENTNODE_H