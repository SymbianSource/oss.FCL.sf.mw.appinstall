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
* Description:   Implementation of CNcdNodeSupplierLink class
*
*/


#include "ncdnodesupplierlink.h"
#include "catalogsdebug.h"


CNcdNodeSupplierLink* CNcdNodeSupplierLink::NewL( CNcdNode& aNode )
    {
    CNcdNodeSupplierLink* self = 
        CNcdNodeSupplierLink::NewLC( aNode );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeSupplierLink* CNcdNodeSupplierLink::NewLC( CNcdNode& aNode )
    {
    CNcdNodeSupplierLink* self = new( ELeave ) CNcdNodeSupplierLink( aNode );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeSupplierLink::CNcdNodeSupplierLink(
    CNcdNode& aNode,
    NcdNodeClassIds::TNcdNodeClassId aClassId )
    : CNcdNodeLink( aNode, aClassId )
    {
    }


CNcdNodeSupplierLink::~CNcdNodeSupplierLink()
    {
    }        
    

void CNcdNodeSupplierLink::InternalizeL(
    const MNcdPreminetProtocolEntityRef& /*aData*/,
    const CNcdNodeIdentifier& /*aParentIdentifier*/,
    const CNcdNodeIdentifier& /*aRequestParentIdentifier*/ )
    {
    // This should never be internalized from the protocol data.
    // The temporary node should contain the data that has been
    // received from the protocol response. The supplier is only
    // wrapper until the correct node can be created.
    
    DASSERT( EFalse );
    }
