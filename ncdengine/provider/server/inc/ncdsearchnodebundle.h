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
* Description:   Declaration of CNcdSearchNodeBundle class.
*
*/


#ifndef C_NCDSEARCHNODEBUNDLE_H
#define C_NCDSEARCHNODEBUNDLE_H

#include "ncdsearchnodefolder.h"

/**
 * Represents a folder of catalog bundle in server side.
 */
class CNcdSearchNodeBundle : public CNcdSearchNodeFolder 
{
public:

    /** 
     * @see CNcdNodeFolder::NewL
     */
    static CNcdSearchNodeBundle* NewL(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier  );
        
    /**
     * @see CNcdNodeBundleFolder::NewL
     */    
    static CNcdSearchNodeBundle* NewLC(
        CNcdNodeManager& aNodeManager,
        const CNcdNodeIdentifier& aIdentifier );

    virtual ~CNcdSearchNodeBundle();


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
    
        
protected:

    /**
     * @see CNcdNodeFolder::CNcdNodeFolder
     */
    CNcdSearchNodeBundle( CNcdNodeManager& aNodeManager,
                      NcdNodeClassIds::TNcdNodeClassId aNodeClassId 
                        = NcdNodeClassIds::ENcdSearchBundleNodeClassId );
                        
    /**
     * @see CNcdNodeFolder::ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );

private:
    
    // Prevent these two if they are not implemented
    CNcdSearchNodeBundle( const CNcdNodeFolder& aObject );
    CNcdSearchNodeBundle& operator =( const CNcdNodeFolder& aObject );
    
};

#endif
