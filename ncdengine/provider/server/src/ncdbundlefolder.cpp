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
* Description:   Implementation of CNcdBundleFolder class.
*
*/


#include "ncdbundlefolder.h"
#include "ncdnodeidentifier.h"
#include "ncdchildentity.h"
#include "catalogsutils.h"


CNcdBundleFolder* CNcdBundleFolder::NewL(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {    
    CNcdBundleFolder* self = 
        NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdBundleFolder* CNcdBundleFolder::NewLC(
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier ) 
    {
    CNcdBundleFolder* self =
        new ( ELeave ) CNcdBundleFolder( aNodeManager );
    CleanupStack::PushL( self );
    self->ConstructL( aIdentifier );
    return self;
    }


CNcdBundleFolder::CNcdBundleFolder( CNcdNodeManager& aNodeManager, 
                                    NcdNodeClassIds::TNcdNodeClassId aNodeClassId )
: CNcdParentOfTransparentNode( aNodeManager, aNodeClassId ) 
    {
    }

CNcdBundleFolder::~CNcdBundleFolder() 
    {
    delete iViewType;
    }

void CNcdBundleFolder::ConstructL( const CNcdNodeIdentifier& aIdentifier ) 
    {
    DLTRACEIN((""));
    CNcdParentOfTransparentNode::ConstructL( aIdentifier );
    AssignDesL( iViewType, KNullDesC() );
    DLTRACEOUT((""));
    }


TInt CNcdBundleFolder::ServerChildCount() const
    {
    DLTRACEIN(( "this: %X, ChildCount: %d", this, ChildArray().Count() ));
    // bundle folders's child count is always the number of children in the child array
    // because, contrary to regular folders, bundle folder doesn't have an expected child count
    return ChildArray().Count();
    }

const CNcdNodeIdentifier& CNcdBundleFolder::ChildByServerIndexL( TInt aIndex ) const
    {
    DLTRACEIN((""));    
    // bundle folders's child count is always the number of children in the child array
    // because, contrary to regular folders, bundle folder doesn't have an expected child count
    if ( aIndex < 0 || aIndex >= ChildArray().Count() )
        {
        // For debugging purposes
        DLERROR(("Wrong child index: %d, count: %d", aIndex, ChildArray().Count() ));
        DASSERT( EFalse );
        User::Leave( KErrArgument );
        }
    return ChildArray()[aIndex]->Identifier();
    }


const TDesC& CNcdBundleFolder::ViewType() const 
    {
    DLTRACEIN((""));
    return *iViewType;
    }

void CNcdBundleFolder::SetViewTypeL( const TDesC& aViewType ) 
    {
    DLTRACEIN((""));
    AssignDesL( iViewType, aViewType );
    DLTRACEOUT((""));
    }


void CNcdBundleFolder::ExternalizeL( RWriteStream& aStream ) 
    {
    CNcdParentOfTransparentNode::ExternalizeL( aStream );
    ExternalizeDesL( *iViewType, aStream );
    }

void CNcdBundleFolder::InternalizeL( RReadStream& aStream )
    {
    CNcdParentOfTransparentNode::InternalizeL( aStream );
    InternalizeDesL( iViewType, aStream );
    }
    
void CNcdBundleFolder::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    CNcdParentOfTransparentNode::ExternalizeDataForRequestL( aStream );
    ExternalizeDesL( *iViewType, aStream );
    }
