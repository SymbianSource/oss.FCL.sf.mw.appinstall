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
* Description:   Implements CNcdRootNode class
*
*/


#include "ncdrootnode.h"
#include "ncdchildentity.h"
#include "catalogsutils.h"

// This is for the contentsourcemap
#include "ncdloadrootnodeoperationimpl.h"


CNcdRootNode* CNcdRootNode::NewL( 
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdRootNode* self = 
        CNcdRootNode::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdRootNode* CNcdRootNode::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    // Notice that the default value for the class id is set in
    // the header constructor definition. No need to set it here.
    CNcdRootNode* self = 
        new( ELeave ) CNcdRootNode( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }

CNcdRootNode::CNcdRootNode( CNcdNodeManager& aNodeManager,
                            NcdNodeClassIds::TNcdNodeClassId aNodeClassId ) 
: CNcdParentOfTransparentNode( aNodeManager, aNodeClassId ) 
    {
    }

void CNcdRootNode::ConstructL( const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdParentOfTransparentNode::ConstructL( aIdentifier );
    iContentSourceMap = CNcdContentSourceMap::NewL();
    }
    
CNcdRootNode::~CNcdRootNode() 
    {
    delete iContentSourceMap;
    }
    
CNcdContentSourceMap& CNcdRootNode::ContentSourceMap() 
    {
    return *iContentSourceMap;
    }
    
void CNcdRootNode::SetContentSourceMap( CNcdContentSourceMap* aMap ) 
    {
    DLTRACEIN((""));
    delete iContentSourceMap;
    iContentSourceMap = aMap;
    }
    
    
void CNcdRootNode::ExternalizeL( RWriteStream& aStream ) 
    {
    DLTRACEIN((""));
    CNcdParentOfTransparentNode::ExternalizeL( aStream );
    iContentSourceMap->ExternalizeL( aStream );
    }
    
void CNcdRootNode::InternalizeL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    CNcdParentOfTransparentNode::InternalizeL( aStream );
    delete iContentSourceMap;
    iContentSourceMap = NULL;
    iContentSourceMap = CNcdContentSourceMap::NewL( aStream );
    }

TInt CNcdRootNode::ServerChildCount() const
    {
    DLTRACEIN(( "this: %X, ChildCount: %d", this, ChildArray().Count() ));
    // root node's child count is always the number of children in the child array
    // because, contrary to regular folders, root node doesn't have an expected child count
    return ChildArray().Count();
    }

const CNcdNodeIdentifier& CNcdRootNode::ChildByServerIndexL( TInt aIndex ) const
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
