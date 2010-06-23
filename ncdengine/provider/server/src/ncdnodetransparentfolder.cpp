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
* Description:   Implements CNcdNodeTransparentFolder class
*
*/


#include "ncdnodetransparentfolder.h"
#include "ncdnodemanager.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"


CNcdNodeTransparentFolder::CNcdNodeTransparentFolder( 
    CNcdNodeManager& aNodeManager,
    NcdNodeClassIds::TNcdNodeClassId aNodeClassId, 
    NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId,
    NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId )
: CNcdNodeFolder( aNodeManager,
                  aNodeClassId,
                  aAcceptedLinkClassId,
                  aAcceptedMetaDataClassId )
    {
    }

void CNcdNodeTransparentFolder::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(("this: %X", this ));

    CNcdNodeFolder::ConstructL( aIdentifier );
        
    DLTRACEOUT((""));
    }


CNcdNodeTransparentFolder::~CNcdNodeTransparentFolder()
    {
    DLTRACEIN(("this: %X", this));
    DLTRACEOUT((""));
    }


CNcdNodeTransparentFolder* CNcdNodeTransparentFolder::NewL( 
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeTransparentFolder* self = 
        CNcdNodeTransparentFolder::NewLC( aNodeManager, aIdentifier );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdNodeTransparentFolder* CNcdNodeTransparentFolder::NewLC( 
    CNcdNodeManager& aNodeManager,
    const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeTransparentFolder* self = 
        new( ELeave ) CNcdNodeTransparentFolder( aNodeManager );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }


void CNcdNodeTransparentFolder::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // First use the parent to externalize the general data
    CNcdNodeFolder::ExternalizeL( aStream );

    DLTRACEOUT((""));
    }
    
void CNcdNodeTransparentFolder::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // First use the parent to internalize the general data
    CNcdNodeFolder::InternalizeL( aStream );
            
    DLTRACEOUT((""));
    }

TBool CNcdNodeTransparentFolder::HasExpiredOrMissingChildrenL()
    {
    DLTRACEIN((""));
    TBool isChildExpired = EFalse;
    for( TInt i = 0 ; i < ServerChildCountL() ; i++ )
        {
        CNcdNode* child = NULL;
        TRAPD( err, child = &NodeManager().NodeL( ChildByServerIndexL(i) ) );
        if( err == KErrNotFound || child->State() != MNcdNode::EStateInitialized )
            {
            DLTRACE(("At least one child was expired/uninitialized/not found."));
            isChildExpired = ETrue;
            break;
            }
        else if( err != KErrNone && err != KErrNotFound )
            {
            DLTRACE(("Error: %d", err));
            User::Leave( err );
            }
        }
    return isChildExpired;
    }
                                  
void CNcdNodeTransparentFolder::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    CNcdNodeFolder::ExternalizeDataForRequestL( aStream );
        
    DLTRACEOUT((""));
    }

