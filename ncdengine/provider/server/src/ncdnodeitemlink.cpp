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
* Description:   Implements CNcdNodeItemLink class
*
*/


#include "ncdnodeitemlink.h"
#include "ncd_pp_itemref.h"
#include "catalogsdebug.h"


CNcdNodeItemLink* CNcdNodeItemLink::NewL( CNcdNode& aNode )
    {
    CNcdNodeItemLink* self = 
        CNcdNodeItemLink::NewLC( aNode );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeItemLink* CNcdNodeItemLink::NewLC( CNcdNode& aNode )
    {
    CNcdNodeItemLink* self = new( ELeave ) CNcdNodeItemLink( aNode );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdNodeItemLink::CNcdNodeItemLink( CNcdNode& aNode,
                                    NcdNodeClassIds::TNcdNodeClassId aClassId )
: CNcdNodeLink( aNode, aClassId )

    {
    }

void CNcdNodeItemLink::ConstructL()
    {
    CNcdNodeLink::ConstructL();
    }


CNcdNodeItemLink::~CNcdNodeItemLink()
    {
    }        
    

void CNcdNodeItemLink::InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                                     const CNcdNodeIdentifier& aParentIdentifier,
                                     const CNcdNodeIdentifier& aRequestParentIdentifier,
                                     const TUid& aClientUid )
    {
    DLTRACEIN((""));
    if( aData.Type() != MNcdPreminetProtocolEntityRef::EItemRef )
        {
        // The data should be for the item
        DLERROR(("Wrong type"));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
    
    // Let the parent do its internalizing
    CNcdNodeLink::InternalizeL( 
        aData, aParentIdentifier, aRequestParentIdentifier, aClientUid );
    DLTRACEOUT((""));
    }
    
    
void CNcdNodeItemLink::ExternalizeL( RWriteStream& aStream )
    {
    // Before hanling this object specific data,
    // let the parent handle its data first.
    CNcdNodeLink::ExternalizeL( aStream );
    }
    
void CNcdNodeItemLink::InternalizeL( RReadStream& aStream )
    {
    // Before hanling this object specific data,
    // let the parent handle its data first.
    CNcdNodeLink::InternalizeL( aStream );
    }
    

void CNcdNodeItemLink::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    CNcdNodeLink::ExternalizeDataForRequestL( aStream );
    }
