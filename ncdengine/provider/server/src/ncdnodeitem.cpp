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
* Description:   Implements CNcdNodeItem class
*
*/


#include "ncdnodeitem.h"
#include "ncdnodeitemlink.h"
#include "ncdnodeitemmetadata.h"
#include "ncdnodemanager.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodeclassids.h"


CNcdNodeItem::CNcdNodeItem( CNcdNodeManager& aNodeManager,
    NcdNodeClassIds::TNcdNodeClassId aNodeClassId, 
    NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId,
    NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId )
: CNcdNode( aNodeManager,
            aNodeClassId,
            aAcceptedLinkClassId,
            aAcceptedMetaDataClassId )
    {
    }

void CNcdNodeItem::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNode::ConstructL( aIdentifier );
    }


CNcdNodeItem::~CNcdNodeItem()
    {
    }
        

CNcdNodeItem* CNcdNodeItem::NewL( CNcdNodeManager& aNodeManager,
                                  const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeItem* self = 
        CNcdNodeItem::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeItem* CNcdNodeItem::NewLC( CNcdNodeManager& aNodeManager,
                                   const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeItem* self = 
        new( ELeave ) CNcdNodeItem( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }
                                  

CNcdNodeLink* CNcdNodeItem::CreateLinkL()
    {
    DLTRACEIN((""));
    CNcdNodeLink* link = NodeLink();
        
    if ( link )
        {
        DLTRACEOUT(("Link already exists"));
        // The link was already created
        return link;
        }
    else
        {
        DLTRACEOUT(("Creating a new link"));
        // Link was not already created.
        // So, create new.
        return CNcdNodeItemLink::NewL( *this );        
        }
    }


void CNcdNodeItem::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    CNcdNode::ExternalizeDataForRequestL( aStream );
    }


