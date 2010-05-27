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
* Description:   Implements CNcdChildEntity class
*
*/


#include <e32cmn.h>
#include <s32strm.h>
#include <s32mem.h>

#include "ncdchildentity.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"


CNcdChildEntity* CNcdChildEntity::NewL( 
    TInt aIndex,
    const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    CNcdChildEntity* self = 
        CNcdChildEntity::NewLC( aIndex, aNodeIdentifier, aTransparent, aNodeType );
    CleanupStack::Pop( self );
    return self;        
    }
    
CNcdChildEntity* CNcdChildEntity::NewLC( 
    TInt aIndex,
    const CNcdNodeIdentifier& aNodeIdentifier,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    CNcdChildEntity* self = 
        new( ELeave ) CNcdChildEntity( aIndex, aTransparent, aNodeType );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeIdentifier );
    return self;
    }

CNcdChildEntity* CNcdChildEntity::NewLC( RReadStream& aReadStream )
    {
    CNcdChildEntity* self = 
        new( ELeave ) CNcdChildEntity();
    CleanupStack::PushL( self );
    self->InternalizeL( aReadStream );
    return self;        
    }

CNcdChildEntity* CNcdChildEntity::NewL( RReadStream& aReadStream )
    {
    CNcdChildEntity* self = NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdChildEntity* CNcdChildEntity::NewL( const CNcdChildEntity& aChildEntity )
    {
    CNcdChildEntity* self = 
        CNcdChildEntity::NewLC( aChildEntity );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdChildEntity* CNcdChildEntity::NewLC( const CNcdChildEntity& aChildEntity )
    {
    CNcdChildEntity* self = 
        new( ELeave ) CNcdChildEntity( aChildEntity.Index(),
            aChildEntity.IsTransparent(), aChildEntity.NodeType() );
    CleanupStack::PushL( self );
    self->ConstructL( aChildEntity.Identifier() );
    return self;
    }

const CNcdNodeIdentifier& CNcdChildEntity::Identifier() const
    {
    return *iIdentifier;
    }
    
void CNcdChildEntity::SetIdentifierL( const CNcdNodeIdentifier& aIdentifier )
    {
    CNcdNodeIdentifier* temp = CNcdNodeIdentifier::NewL( aIdentifier );
    delete iIdentifier;
    iIdentifier = temp;
    }
    
TInt CNcdChildEntity::Index() const
    {
    return iIndex;
    }

void CNcdChildEntity::InternalizeL( RReadStream& aReadStream )
    {
    iIndex = aReadStream.ReadInt32L();
    delete iIdentifier;
    iIdentifier = NULL;
    iIdentifier = CNcdNodeIdentifier::NewL( aReadStream );
    iTransparent = aReadStream.ReadInt8L();
    iNodeType =
        static_cast<CNcdNodeFactory::TNcdNodeType>( aReadStream.ReadInt8L() );
    }

void CNcdChildEntity::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteInt32L( iIndex );
    iIdentifier->ExternalizeL( aWriteStream );
    aWriteStream.WriteInt8L( iTransparent );
    aWriteStream.WriteInt8L( iNodeType );
    }
        

TBool CNcdChildEntity::Equals( const CNcdChildEntity& aObject ) const
    {
    // iTransparent is ignored in equality checks
    return iIdentifier->Equals( aObject.Identifier() ) &&
        iIndex == aObject.Index();
    }
    
TBool CNcdChildEntity::IsTransparent() const
    {
    return iTransparent;
    }
    
void CNcdChildEntity::SetTransparent( TBool aTransparent )
    {
    iTransparent = aTransparent;
    }
    
CNcdNodeFactory::TNcdNodeType CNcdChildEntity::NodeType() const
    {
    return iNodeType;
    }
    
void CNcdChildEntity::SetNodeType( CNcdNodeFactory::TNcdNodeType aNodeType )
    {
    iNodeType = aNodeType;
    }

CNcdChildEntity::CNcdChildEntity( 
    TInt aIndex,
    TBool aTransparent,
    CNcdNodeFactory::TNcdNodeType aNodeType ) 
    : iIndex( aIndex ),
      iTransparent( aTransparent ),
      iNodeType( aNodeType )
    {
    }
    
CNcdChildEntity::CNcdChildEntity()
    {
    }

void CNcdChildEntity::ConstructL( const CNcdNodeIdentifier& aIdentifier )
    {
    iIdentifier = CNcdNodeIdentifier::NewL( aIdentifier );
    }

CNcdChildEntity::~CNcdChildEntity()
    {
    delete iIdentifier;
    }
