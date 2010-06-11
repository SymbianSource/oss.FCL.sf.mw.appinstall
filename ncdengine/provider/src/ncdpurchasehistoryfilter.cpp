/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of CNcdPurchaseHistoryFilter
*
*/


#include <e32base.h>
#include <s32strm.h>

#include "ncdutils.h"
#include "catalogsutils.h"

EXPORT_C CNcdPurchaseHistoryFilter* CNcdPurchaseHistoryFilter::NewL()
    {
    CNcdPurchaseHistoryFilter* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

EXPORT_C CNcdPurchaseHistoryFilter* CNcdPurchaseHistoryFilter::NewLC()
    {
    CNcdPurchaseHistoryFilter* self = new (ELeave) CNcdPurchaseHistoryFilter;
    CleanupStack::PushL( self );
    RArray< TUid > dummyArray;
    self->ConstructL( dummyArray.Array(), KNullDesC, KNullDesC, 0 );
    return self;
    }

EXPORT_C CNcdPurchaseHistoryFilter* CNcdPurchaseHistoryFilter::NewL(
    TArray< TUid > aClientUids,
    const TDesC& aNamespace,
    const TDesC& aEntityId,
    const TUint aEventId )
    {
    CNcdPurchaseHistoryFilter* self = NewLC(
        aClientUids,
        aNamespace,
        aEntityId,
        aEventId );
    CleanupStack::Pop();
    return self;
    }

EXPORT_C CNcdPurchaseHistoryFilter* CNcdPurchaseHistoryFilter::NewLC(
    TArray< TUid > aClientUids,
    const TDesC& aNamespace,
    const TDesC& aEntityId,
    const TUint aEventId )
    {
    CNcdPurchaseHistoryFilter* self = new (ELeave) CNcdPurchaseHistoryFilter;
    CleanupStack::PushL( self );
    self->ConstructL( aClientUids, aNamespace, aEntityId, aEventId );
    return self;
    }

CNcdPurchaseHistoryFilter::~CNcdPurchaseHistoryFilter()
    {
    iClientUids.Reset();
    delete iNamespace;
    delete iEntityId;
    }

void CNcdPurchaseHistoryFilter::ConstructL(
    TArray< TUid > aClientUids,
    const TDesC& aNamespace,
    const TDesC& aEntityId,
    const TUint aEventId )
    {
    SetClientUids( aClientUids );
    SetNamespaceL( aNamespace );
    SetEntityIdL( aEntityId );
    SetEventId( aEventId );
    }

CNcdPurchaseHistoryFilter::CNcdPurchaseHistoryFilter()
    {
    }

EXPORT_C void CNcdPurchaseHistoryFilter::ExternalizeL( RWriteStream& aStream ) const
    {
    TUint count = iClientUids.Count();
    aStream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ )
        {
        aStream.WriteInt32L( iClientUids[i].iUid );
        }
    ExternalizeDesL( *iNamespace, aStream );
    ExternalizeDesL( *iEntityId, aStream );
    aStream.WriteUint32L( iEventId );
    }

EXPORT_C void CNcdPurchaseHistoryFilter::InternalizeL( RReadStream& aStream )
    {
    TUint count = aStream.ReadUint32L();
    iClientUids.Reset();
    if ( count > 0 )
        {
        for ( TInt i = 0; i < count; i++ )
            {
            User::LeaveIfError(
                iClientUids.Append( TUid::Uid( aStream.ReadInt32L() ) ) );
            }
        }
    InternalizeDesL( iNamespace, aStream );
    InternalizeDesL( iEntityId, aStream );
    iEventId = aStream.ReadUint32L();
    }

const TArray< TUid > CNcdPurchaseHistoryFilter::ClientUids() const
    {
    return iClientUids.Array();
    }

EXPORT_C void CNcdPurchaseHistoryFilter::SetClientUids(
    const TArray< TUid > aClientUids )
    {
    iClientUids.Reset();
    TUint count = aClientUids.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iClientUids.Append( aClientUids[i] );
        }
    }

const TDesC& CNcdPurchaseHistoryFilter::Namespace() const
    {
    return *iNamespace;
    }

EXPORT_C void CNcdPurchaseHistoryFilter::SetNamespaceL( const TDesC& aNamespace )
    {
    AssignDesL( iNamespace, aNamespace );
    }

const TDesC& CNcdPurchaseHistoryFilter::EntityId() const
    {
    return *iEntityId;
    }

EXPORT_C void CNcdPurchaseHistoryFilter::SetEntityIdL( const TDesC& aEntityId )
    {
    AssignDesL( iEntityId, aEntityId );
    }

TUint CNcdPurchaseHistoryFilter::EventId() const
    {
    return iEventId;
    }

EXPORT_C void CNcdPurchaseHistoryFilter::SetEventId( const TUint aEventId )
    {
    iEventId = aEventId;
    }
