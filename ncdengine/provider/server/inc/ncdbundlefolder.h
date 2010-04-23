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
* Description:   Declaration of CNcdBundleFolder class.
*
*/


#ifndef C_NCDBUNDLEFOLDER_H
#define C_NCDBUNDLEFOLDER_H

#include "ncdparentoftransparentnode.h"

/**
 * Represents a folder of catalog bundle in server side.
 */
class CNcdBundleFolder : public CNcdParentOfTransparentNode 
{
public:

    /** 
     * @see CNcdNodeFolder::NewL
     */
    static CNcdBundleFolder* NewL(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier  );
        
    /**
     * @see CNcdNodeBundleFolder::NewL
     */    
    static CNcdBundleFolder* NewLC(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier );


    /**
     * Destructor
     */
    virtual ~CNcdBundleFolder();


public: // CNcdNodeFolder

     /** 
      * @see CNcdNodeFolder::ServerChildCount
      */
    virtual TInt ServerChildCount() const;

    /** 
     * @see CNcdNodeFolder::ChildByServerIndexL
     */
    virtual const CNcdNodeIdentifier& ChildByServerIndexL( TInt aIndex ) const;
    

public:

    const TDesC& ViewType() const;
    
    void SetViewTypeL( const TDesC& aViewType );

    
    
public: // CNcdNode

    /**
     * @see CNcdParentOfTransparentNode::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );

    /**
     * @see CNcdParentOfTransparentNode::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );

    /**
     * @see CNcdNode::ExternalizeDataForRequestL
     */    
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    


protected:

    /**
     * @see CNcdNodeFolder::CNcdNodeFolder
     */
    CNcdBundleFolder( CNcdNodeManager& aNodeManager,
                      NcdNodeClassIds::TNcdNodeClassId aNodeClassId 
                        = NcdNodeClassIds::ENcdBundleFolderNodeClassId );

    void ConstructL( const CNcdNodeIdentifier& aIdentifier );


private:
    
    // Prevent these two if they are not implemented
    CNcdBundleFolder( const CNcdNodeFolder& aObject );
    CNcdBundleFolder& operator =( const CNcdNodeFolder& aObject );
    
private:

    HBufC* iViewType;    
};

#endif
