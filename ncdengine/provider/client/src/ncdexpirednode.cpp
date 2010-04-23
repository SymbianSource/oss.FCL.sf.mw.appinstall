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
* Description:  
*
*/


#include <s32strm.h>

#include "ncdexpirednode.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdutils.h"

// ======== MEMBER FUNCTIONS ========

CNcdExpiredNode* CNcdExpiredNode::NewL( RReadStream& aReadStream )
    {
    CNcdExpiredNode* self = CNcdExpiredNode::NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;
    }

CNcdExpiredNode* CNcdExpiredNode::NewLC( RReadStream& aReadStream )
    {
    CNcdExpiredNode* self = new ( ELeave ) CNcdExpiredNode( EFalse );
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdExpiredNode* CNcdExpiredNode::NewL( const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aForceUpdate )
    {
    CNcdExpiredNode* self = CNcdExpiredNode::NewLC( aNodeIdentifier, aForceUpdate );
    CleanupStack::Pop( self );
    return self;
    }

CNcdExpiredNode* CNcdExpiredNode::NewLC( const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aForceUpdate )
    {
    CNcdExpiredNode* self = new ( ELeave ) CNcdExpiredNode( aForceUpdate );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifier );
    return self;
    }

CNcdExpiredNode* CNcdExpiredNode::NewL( const CNcdExpiredNode& aExpiredNode )
    {
    CNcdExpiredNode* self = CNcdExpiredNode::NewLC( aExpiredNode );
    CleanupStack::Pop( self );
    return self;
    }

CNcdExpiredNode* CNcdExpiredNode::NewLC( const CNcdExpiredNode& aExpiredNode )
    {
    CNcdExpiredNode* self = new ( ELeave ) CNcdExpiredNode(
        aExpiredNode.ForceUpdate() );
    CleanupStack::PushL( self );
    self->ConstructL( aExpiredNode.NodeIdentifier() );
    return self;
    }

CNcdExpiredNode::~CNcdExpiredNode()
    {
    delete iNodeIdentifier;
    iNodeIdentifier = 0;
    }


void CNcdExpiredNode::InternalizeL( RReadStream& aReadStream )
    {
    delete iNodeIdentifier;
    iNodeIdentifier = NULL;
    iNodeIdentifier = CNcdNodeIdentifier::NewL( aReadStream );
    iForceUpdate = aReadStream.ReadInt32L();
    }
    
void CNcdExpiredNode::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    iNodeIdentifier->ExternalizeL( aWriteStream );
    aWriteStream.WriteInt32L( iForceUpdate );
    }

const CNcdNodeIdentifier& CNcdExpiredNode::NodeIdentifier() const
    {
    return *iNodeIdentifier;
    }
    
TBool CNcdExpiredNode::ForceUpdate() const
    {
    return iForceUpdate;
    }

CNcdExpiredNode::CNcdExpiredNode( TBool aForceUpdate )
    : CBase(), iForceUpdate( aForceUpdate )
    {    
    }
    
void CNcdExpiredNode::ConstructL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    iNodeIdentifier = CNcdNodeIdentifier::NewL( aNodeIdentifier );
    }

