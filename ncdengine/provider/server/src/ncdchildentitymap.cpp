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
* Description:   Implements CNcdChildEntityMap class
*
*/


#include <e32cmn.h>
#include <s32strm.h>
#include <s32mem.h>

#include "ncdchildentitymap.h"
#include "ncdchildentity.h"
#include "ncdnodeidentifier.h"

#include "catalogsdebug.h"


CNcdChildEntityMap* CNcdChildEntityMap::NewL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const RPointerArray<CNcdChildEntity>& aChildArray,
    TInt aChildCount )
    {
    CNcdChildEntityMap* self = 
        CNcdChildEntityMap::NewLC( aNodeIdentifier, aChildArray, aChildCount );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdChildEntityMap* CNcdChildEntityMap::NewLC(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const RPointerArray<CNcdChildEntity>& aChildArray,
    TInt aChildCount )
    {
    CNcdChildEntityMap* self = 
        new( ELeave ) CNcdChildEntityMap( aChildCount );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifier, aChildArray );
    return self;
    }

const CNcdNodeIdentifier& CNcdChildEntityMap::ParentIdentifier() const
    {
    return *iParentIdentifier;
    }
    
const RPointerArray<CNcdChildEntity>& CNcdChildEntityMap::ChildArray() const
    {
    return iChildArray;
    }
    
TInt CNcdChildEntityMap::ChildCount() const
    {
    return iChildCount;
    }

CNcdChildEntityMap::CNcdChildEntityMap( TInt aChildCount )
    : iChildCount( aChildCount )
    {
    }
    
void CNcdChildEntityMap::ConstructL( const CNcdNodeIdentifier& aIdentifier,
    const RPointerArray<CNcdChildEntity>& aChildArray )
    {
    DLTRACEIN((""));
    iParentIdentifier = CNcdNodeIdentifier::NewL( aIdentifier );
    for( TInt i = 0 ; i < aChildArray.Count() ; i++ )
        {
        CNcdChildEntity* child = CNcdChildEntity::NewLC( *aChildArray[i] );
        DLTRACE((_L("Adding child entity, identifier: %S, index: %d"),
             &child->Identifier().NodeId(), child->Index() ));
        iChildArray.AppendL( child );
        CleanupStack::Pop( child );
        }
    }

CNcdChildEntityMap::~CNcdChildEntityMap()
    {
    delete iParentIdentifier;
    iChildArray.ResetAndDestroy();
    }
