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
* Description:   CNcdPreminetProtocolDataEntityContentImpl implementation
*
*/


#include <e32base.h>
#include "ncd_pp_dataentitycontentimpl.h"
#include "ncd_pp_entitydependency.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolDataEntityContentImpl* 
CNcdPreminetProtocolDataEntityContentImpl::NewL() 
    {
    CNcdPreminetProtocolDataEntityContentImpl* self =
        new (ELeave) CNcdPreminetProtocolDataEntityContentImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolDataEntityContentImpl* 
CNcdPreminetProtocolDataEntityContentImpl::NewLC() 
    {
    CNcdPreminetProtocolDataEntityContentImpl* self =
        new (ELeave) CNcdPreminetProtocolDataEntityContentImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdPreminetProtocolDataEntityContentImpl::~CNcdPreminetProtocolDataEntityContentImpl()
    {
    iContentPurposes.ResetAndDestroy();

    iEntityDependencies.ResetAndDestroy();

    delete iTimestamp;
    iTimestamp = 0;
    
    delete iMime;
    iMime = 0;
    
    delete iId;
    iId = 0;
    
    delete iVersion;
    iVersion = 0;
    }
    
CNcdPreminetProtocolDataEntityContentImpl::CNcdPreminetProtocolDataEntityContentImpl()
: CBase(), 
  iSubscriptionType(ENotSubscribable),
  iChildViewable(EFalse), 
  iChildSeparatelyPurchasable(EFalse)
    {
    }
    
void CNcdPreminetProtocolDataEntityContentImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iMime );
    NcdProtocolUtils::AssignEmptyDesL( iVersion );
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const TDesC& CNcdPreminetProtocolDataEntityContentImpl::Timestamp() const
    {
    DASSERT((iTimestamp));
    return *iTimestamp;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TInt CNcdPreminetProtocolDataEntityContentImpl::Size() const
    {
    return iSize;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const TDesC& CNcdPreminetProtocolDataEntityContentImpl::Mime() const 
    {
    DASSERT((iMime));
    return *iMime;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const TDesC& CNcdPreminetProtocolDataEntityContentImpl::Id() const 
    {
    DASSERT((iId));
    return *iId;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const TDesC& CNcdPreminetProtocolDataEntityContentImpl::Version() const 
    {
    DASSERT((iVersion));
    return *iVersion;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TInt CNcdPreminetProtocolDataEntityContentImpl::ValidUntilDelta() const 
    {
    return iValidUntilDelta;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TBool CNcdPreminetProtocolDataEntityContentImpl::ValidUntilAutoUpdate() const 
    {
    return iValidUntilAutoUpdate;
    }
    
/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TInt CNcdPreminetProtocolDataEntityContentImpl::ContentPurposeCount() const 
    {
    return iContentPurposes.Count();
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const TDesC& 
CNcdPreminetProtocolDataEntityContentImpl::ContentPurposeL(TInt aIndex) const 
    {
    DASSERT( aIndex >= 0 && aIndex < iContentPurposes.Count() );
    if ( aIndex < 0 || aIndex >= iContentPurposes.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iContentPurposes[aIndex];
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TInt CNcdPreminetProtocolDataEntityContentImpl::EntityDependencyCount() const 
    {
    return iEntityDependencies.Count();
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
const MNcdPreminetProtocolEntityDependency& 
CNcdPreminetProtocolDataEntityContentImpl::EntityDependencyL(TInt aIndex) const 
    {
    DASSERT( aIndex >= 0 && aIndex < iEntityDependencies.Count() );
    if ( aIndex < 0 || aIndex >= iEntityDependencies.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iEntityDependencies[aIndex];
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TNcdSubscriptionType 
CNcdPreminetProtocolDataEntityContentImpl::SubscriptionType() const
    {
    return iSubscriptionType;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TBool CNcdPreminetProtocolDataEntityContentImpl::ChildViewable() const
    {
    return iChildViewable;
    }

/**
 * @see MNcdPreminetProtocolDataEntityContent
 */
TBool CNcdPreminetProtocolDataEntityContentImpl::ChildSeparatelyPurchasable() const
    {
    return iChildSeparatelyPurchasable;
    }

