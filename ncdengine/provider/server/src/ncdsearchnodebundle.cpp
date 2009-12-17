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
* Description:   Implementation of CNcdSearchNodeBundle class.
*
*/


#include "ncdsearchnodebundle.h"
#include "ncdnodeidentifier.h"
#include "ncdchildentity.h"
#include "catalogsutils.h"


CNcdSearchNodeBundle* CNcdSearchNodeBundle::NewL(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {    
    CNcdSearchNodeBundle* self = 
        NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdSearchNodeBundle* CNcdSearchNodeBundle::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdSearchNodeBundle* self =
        new ( ELeave ) CNcdSearchNodeBundle( aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL( aIdentifier );
    return self;
    }


CNcdSearchNodeBundle::CNcdSearchNodeBundle( CNcdNodeManager& aNodeManager, 
                                    NcdNodeClassIds::TNcdNodeClassId aNodeClassId )
: CNcdSearchNodeFolder( aNodeManager, aNodeClassId ) 
    {
    }

CNcdSearchNodeBundle::~CNcdSearchNodeBundle() 
    {
    }
    
void CNcdSearchNodeBundle::ConstructL( const CNcdNodeIdentifier& aIdentifier ) 
    {
    DLTRACEIN((""));
    CNcdSearchNodeFolder::ConstructL( aIdentifier );
    DLTRACEOUT((""));
    }

TInt CNcdSearchNodeBundle::ServerChildCount() const
    {
    DLTRACEIN(( "this: %X, ChildCount: %d", this, ChildArray().Count() ));
    // bundle folders's child count is always the number of children in the child array
    // because, contrary to regular folders, bundle folder doesn't have an expected child count
    return ChildArray().Count();
    }

const CNcdNodeIdentifier& CNcdSearchNodeBundle::ChildByServerIndexL( TInt aIndex ) const
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
