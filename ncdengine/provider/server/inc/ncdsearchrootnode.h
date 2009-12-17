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
* Description:   Contains CNcdSearchRootNode class
*
*/


#ifndef NCDSEARCHROOTNODE_H
#define NCDSEARCHROOTNODE_H

#include "ncdsearchnodefolder.h"

class CNcdNodeManager;
class CNcdNodeIdentifier;


class CNcdSearchRootNode : public CNcdSearchNodeFolder
    {
public:
    /**
     * NewL
     *
     * @return CNcdSearchRootNode* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchRootNode* NewL( CNcdNodeManager& aNodeManager,
                               const CNcdNodeIdentifier& aIdentifier );

    /**
     * NewLC
     *
     * @return CNcdNodeFolder* Pointer to the created object 
     * of this class.
     */
    static CNcdSearchRootNode* NewLC( CNcdNodeManager& aNodeManager,
                                const CNcdNodeIdentifier& aIdentifier );
                                  
    /**
     * Destructor.
     */
    virtual ~CNcdSearchRootNode();
    
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
     * Constructor
     *
     */
    CNcdSearchRootNode( CNcdNodeManager& aNodeManager,
                  NcdNodeClassIds::TNcdNodeClassId aNodeClassId = NcdNodeClassIds::ENcdSearchRootNodeClassId );
    
    void ConstructL( const CNcdNodeIdentifier& aIdentifier );

    };

#endif // NCDSEARCHROOTNODE_H