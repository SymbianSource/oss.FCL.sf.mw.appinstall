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
* Description:   Implements CNcdSearchNodeItem class
*
*/


#include "ncdsearchnodeitem.h"
#include "ncdnodeitemlink.h"
#include "ncdnodeitemmetadata.h"
#include "ncdnodemanager.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodeclassids.h"


CNcdSearchNodeItem::CNcdSearchNodeItem( CNcdNodeManager& aNodeManager )
: CNcdNodeItem( aNodeManager,
            NcdNodeClassIds::ENcdSearchItemNodeClassId,
            NcdNodeClassIds::ENcdItemNodeLinkClassId,
            NcdNodeClassIds::ENcdItemNodeMetaDataClassId )
    {
    }

void CNcdSearchNodeItem::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeItem::ConstructL( aIdentifier );
    }


CNcdSearchNodeItem::~CNcdSearchNodeItem()
    {
    }
        

CNcdSearchNodeItem* CNcdSearchNodeItem::NewL( CNcdNodeManager& aNodeManager,
                                              const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdSearchNodeItem* self = 
        CNcdSearchNodeItem::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSearchNodeItem* CNcdSearchNodeItem::NewLC( CNcdNodeManager& aNodeManager,
                                               const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdSearchNodeItem* self = 
        new( ELeave ) CNcdSearchNodeItem( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }
