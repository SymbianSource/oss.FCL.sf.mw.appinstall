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
* Description:   Implements CNcdSearchRootNode class
*
*/


#include "ncdsearchrootnode.h"
#include "ncdchildentity.h"
#include "catalogsutils.h"

CNcdSearchRootNode* CNcdSearchRootNode::NewL( 
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdSearchRootNode* self = 
        CNcdSearchRootNode::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSearchRootNode* CNcdSearchRootNode::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    // Notice that the default value for the class id is set in
    // the header constructor definition. No need to set it here.
    CNcdSearchRootNode* self = 
        new( ELeave ) CNcdSearchRootNode( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }

CNcdSearchRootNode::CNcdSearchRootNode( CNcdNodeManager& aNodeManager,
                            NcdNodeClassIds::TNcdNodeClassId aNodeClassId ) 
: CNcdSearchNodeFolder( aNodeManager, aNodeClassId ) 
    {
    }

void CNcdSearchRootNode::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdSearchNodeFolder::ConstructL( aIdentifier );
    }
    
CNcdSearchRootNode::~CNcdSearchRootNode() 
    {
    }
    
TInt CNcdSearchRootNode::ServerChildCount() const
    {
    DLTRACEIN(( "this: %X, ChildCount: %d", this, ChildArray().Count() ));
    // search root node's child count is always the number of children in the child array
    // because, contrary to regular folders, search root node doesn't have an expected child count
    return ChildArray().Count();
    }

const CNcdNodeIdentifier& CNcdSearchRootNode::ChildByServerIndexL( TInt aIndex ) const
    {
    DLTRACEIN((""));
    
    if ( aIndex < 0 || aIndex >= ChildArray().Count() )
        {
        // For debugging purposes
        DLERROR(("Wrong child index"));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
        
    return ChildArray()[aIndex]->Identifier();
    }
