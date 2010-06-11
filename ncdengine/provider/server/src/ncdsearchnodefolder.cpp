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
* Description:   Implements CNcdSearchNodeFolder class
*
*/


#include "ncdsearchnodefolder.h"
#include "ncdnodefolderlink.h"
#include "ncdnodefoldermetadata.h"
#include "ncdnodemanager.h"
#include "ncdnodeclassids.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "catalogsbasemessage.h"
#include "catalogsutils.h"
#include "ncdutils.h"

#include "catalogsdebug.h"

CNcdSearchNodeFolder* CNcdSearchNodeFolder::NewL( CNcdNodeManager& aNodeManager,
                                                  const CNcdNodeIdentifier& aIdentifier,
                                                  TBool aIsTransparent )
    {
    CNcdSearchNodeFolder* self = 
        CNcdSearchNodeFolder::NewLC( aNodeManager, aIdentifier, aIsTransparent );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSearchNodeFolder* CNcdSearchNodeFolder::NewLC( CNcdNodeManager& aNodeManager,
                                                   const CNcdNodeIdentifier& aIdentifier,
                                                   TBool aIsTransparent )
    {
    CNcdSearchNodeFolder* self = 
        new( ELeave ) CNcdSearchNodeFolder( aNodeManager,
            NcdNodeClassIds::ENcdSearchFolderNodeClassId, aIsTransparent );
    CleanupClosePushL( *self );
    self->ConstructL( aIdentifier );
    return self;        
    }

CNcdSearchNodeFolder::CNcdSearchNodeFolder( CNcdNodeManager& aNodeManager,
    NcdNodeClassIds::TNcdNodeClassId aNodeClassId, TBool aIsTransparent )
: CNcdParentOfTransparentNode( aNodeManager,
            aNodeClassId ), iIsTransparent( aIsTransparent )
    {
    }

void CNcdSearchNodeFolder::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    DLTRACEIN(("this: %X", this ));

    CNcdParentOfTransparentNode::ConstructL( aIdentifier );
    iSearchFilter = CNcdSearchFilter::NewL();
    iOriginIdentifier = CNcdNodeIdentifier::NewL();
    DLTRACEOUT((""));
    }


CNcdSearchNodeFolder::~CNcdSearchNodeFolder()
    {
    DLTRACEIN(("this: %X", this));
    delete iSearchFilter;
    delete iOriginIdentifier;
    DLTRACEOUT((""));
    }

void CNcdSearchNodeFolder::SetSearchFilterL( const CNcdSearchFilter& aFilter )
    {
    DLTRACEIN((""));
    
    DeletePtr( iSearchFilter );
    iSearchFilter = CNcdSearchFilter::NewL( aFilter );
    
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    const MDesCArray& keywords = aFilter.Keywords();
    DLINFO(("Search filter: "));
    for ( TInt i = 0; i < keywords.MdcaCount(); i++ ) 
        {
        DLINFO((_L("%S"), &keywords.MdcaPoint( i ) ));
        }
    #endif
    }
    
void CNcdSearchNodeFolder::SetOriginIdentifierL(
    const CNcdNodeIdentifier& aOriginIdentifier )
    {
    DLTRACEIN((""))
    CNcdNodeIdentifier* originIdentifier = NULL;
    originIdentifier = CNcdNodeIdentifier::NewL( aOriginIdentifier );
    delete iOriginIdentifier;
    iOriginIdentifier = originIdentifier;
    }

const CNcdNodeIdentifier& CNcdSearchNodeFolder::OriginIdentifierL() const
    {
    DLTRACEIN((""));
    if( iOriginIdentifier->ContainsEmptyFields() )
        {
        DLTRACE((_L("Origin identifier not present") ));
        // own origin identifier is empty, ask from parent
        // NOTE: this will leave if parent folder can't be acquired, this is ok
        // because some folder in the parent hierarchy must always have an origin
        // identifier.
        if( ClassId() == NcdNodeClassIds::ENcdSearchRootNodeClassId )
            {
            DLERROR((_L("Already at root and no origin found! Leave!") ));
            User::Leave( KErrNotFound );
            }
        DLTRACE((_L("Asking Origin identifier from parent") ));
        return NodeManager().SearchFolderL( NodeLinkL().ParentIdentifier() )
            .OriginIdentifierL();
        }
    else
        {
        DLTRACE((_L("Origin identifier present, id: %S"), &iOriginIdentifier->NodeId() ));
        return *iOriginIdentifier;
        }
    }

void CNcdSearchNodeFolder::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN(("this: %X", this ));

    CNcdParentOfTransparentNode::ExternalizeL( aStream );
    
    iSearchFilter->ExternalizeL( aStream );
    iOriginIdentifier->ExternalizeL( aStream );
    aStream.WriteInt32L( iIsTransparent );
    DLTRACEOUT((""));
    }

void CNcdSearchNodeFolder::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN(("this: %X", this));

    CNcdParentOfTransparentNode::InternalizeL( aStream );
    
    iSearchFilter->InternalizeL( aStream );
    iOriginIdentifier->InternalizeL( aStream );
    iIsTransparent = aStream.ReadInt32L();
    DLTRACEOUT((""));
    }

void CNcdSearchNodeFolder::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN(("this: %X", this));

    CNcdParentOfTransparentNode::ExternalizeDataForRequestL( aStream );

    iSearchFilter->ExternalizeL( aStream );
        
    DLTRACEOUT((""));
    }
    
TBool CNcdSearchNodeFolder::IsTransparent() const
    {
    DLTRACEIN((""));
    return iIsTransparent;
    }
    
void CNcdSearchNodeFolder::SetTransparent( TBool aIsTransparent )
    {
    DLTRACEIN((""));
    iIsTransparent = aIsTransparent;
    }
    
