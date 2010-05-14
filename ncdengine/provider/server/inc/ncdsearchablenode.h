/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdSearchableNode
*
*/


#ifndef C_NCDSEARCHABLENODE_H
#define C_NCDSEARCHABLENODE_H

#include "ncdnodeimpl.h"
#include "ncdnodeidentifier.h"

class CNcdSearchableNode : public CNcdNode
    {
public:
    
    CNcdSearchableNode( 
        CNcdNodeManager& aNodeManager ) 
    : CNcdNode( aNodeManager, 
        NcdNodeClassIds::ENcdNullObjectClassId, 
        NcdNodeClassIds::ENcdNullObjectClassId, 
        NcdNodeClassIds::ENcdNullObjectClassId ),
      iSearchId( NULL )
        {
        }
        
    void SetIdentifier( const CNcdNodeIdentifier& aSearchId )
        {
        iSearchId = &aSearchId;
        }
    
    virtual const CNcdNodeIdentifier& Identifier() const 
        {
        return *iSearchId;
        }

protected:

    virtual CNcdNodeLink* CreateLinkL() 
        {
        return NULL;
        }
private:

    const CNcdNodeIdentifier* iSearchId;    
    };

#endif // C_NCDSEARCHABLENODE_H