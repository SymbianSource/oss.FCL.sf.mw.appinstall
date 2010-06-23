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
* Description:  
*
*/


#include "ncdprotocolutils.h"
#include "ncd_pp_expiredcacheddataimpl.h"
#include "catalogsdebug.h"

void CNcdPreminetProtocolExpiredEntityImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iEntityId );
    }

CNcdPreminetProtocolExpiredEntityImpl::~CNcdPreminetProtocolExpiredEntityImpl()
    {
    delete iEntityId;
    }

const TDesC& CNcdPreminetProtocolExpiredEntityImpl::EntityId() const
    {
    return *iEntityId;   
    }

TBool CNcdPreminetProtocolExpiredEntityImpl::Recursive() const
    {
    return iRecursive;
    }

TBool CNcdPreminetProtocolExpiredEntityImpl::ForceUpdate() const
    {
    return iForceUpdate;
    }

void CNcdPreminetProtocolClearOldEntitiesImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iStartTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iEndTimestamp );
    }

CNcdPreminetProtocolClearOldEntitiesImpl::~CNcdPreminetProtocolClearOldEntitiesImpl()
    {
    delete iStartTimestamp;
    delete iEndTimestamp;
    }

const TDesC& CNcdPreminetProtocolClearOldEntitiesImpl::StartTimestamp() const
    {
    return *iStartTimestamp;
    }

const TDesC& CNcdPreminetProtocolClearOldEntitiesImpl::EndTimestamp() const
    {
    return *iEndTimestamp;
    }

void CNcdPreminetProtocolExpiredCachedDataImpl::ConstructL()
    {
    }

CNcdPreminetProtocolExpiredCachedDataImpl::~CNcdPreminetProtocolExpiredCachedDataImpl()
    {
    iExpiredEntityCounts.ResetAndDestroy();
    delete iClearOldEntities;
    iClearOldEntities = 0;
    }

TInt CNcdPreminetProtocolExpiredCachedDataImpl::ExpiredEntityCount() const
    {
    return iExpiredEntityCounts.Count();
    }

const MNcdPreminetProtocolExpiredEntity& CNcdPreminetProtocolExpiredCachedDataImpl::ExpiredEntityL( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iExpiredEntityCounts.Count() );
    if ( aIndex < 0 || aIndex >= iExpiredEntityCounts.Count() )
        {
        User::Leave( KErrArgument );
        }

    return *( iExpiredEntityCounts[aIndex] );
    }

const MNcdPreminetProtocolClearOldEntities& CNcdPreminetProtocolExpiredCachedDataImpl::ClearOldEntities() const
    {
    return *iClearOldEntities;
    }
