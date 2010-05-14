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
* Description:   Implementation of CNcdNodeSupplier class
*
*/


#include "ncdnodesupplier.h"
#include "ncdnodesupplierlink.h"

CNcdNodeSupplier* CNcdNodeSupplier::NewL(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdNodeSupplier* self = 
        CNcdNodeSupplier::NewLC( aNodeManager,
                                 aIdentifier );
    CleanupStack::Pop( self );
    return self;    
    }

CNcdNodeSupplier* CNcdNodeSupplier::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdNodeSupplier* self = 
        new ( ELeave ) CNcdNodeSupplier( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;    
    }
    
CNcdNodeSupplier::CNcdNodeSupplier( CNcdNodeManager& aNodeManager ) :
    CNcdNode(
        aNodeManager,
        NcdNodeClassIds::ENcdSupplierNodeClassId,
        NcdNodeClassIds::ENcdSupplierNodeLinkClassId,
        NcdNodeClassIds::ENcdSupplierNodeMetaDataClassId ) 
    {
    }
    
CNcdNodeLink* CNcdNodeSupplier::CreateLinkL() 
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
        return CNcdNodeSupplierLink::NewL( *this );        
        }
    }    
