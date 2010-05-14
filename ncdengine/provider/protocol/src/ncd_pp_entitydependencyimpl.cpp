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
* Description:   CNcdPreminetProtocolEntityDependencyImpl implementation
*
*/


#include <e32base.h>
#include "ncd_pp_entitydependencyimpl.h"
#include "ncd_pp_download.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolEntityDependencyImpl* 
CNcdPreminetProtocolEntityDependencyImpl::NewL() 
    {
    CNcdPreminetProtocolEntityDependencyImpl* self =
        new (ELeave) CNcdPreminetProtocolEntityDependencyImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolEntityDependencyImpl* 
CNcdPreminetProtocolEntityDependencyImpl::NewLC() 
    {
    CNcdPreminetProtocolEntityDependencyImpl* self =
        new (ELeave) CNcdPreminetProtocolEntityDependencyImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdPreminetProtocolEntityDependencyImpl::~CNcdPreminetProtocolEntityDependencyImpl()
    {
    delete iName;
    delete iTimestamp;
    delete iContentId;
    delete iContentVersion;
    delete iEntityId;
    delete iEntityTimestamp;
    delete iDownloadDetails;
    }
    
CNcdPreminetProtocolEntityDependencyImpl::CNcdPreminetProtocolEntityDependencyImpl()
: CBase(), iType(EDependency)
    {
    }
    
void CNcdPreminetProtocolEntityDependencyImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iContentId );
    NcdProtocolUtils::AssignEmptyDesL( iContentVersion );
    NcdProtocolUtils::AssignEmptyDesL( iEntityId );
    NcdProtocolUtils::AssignEmptyDesL( iEntityTimestamp );
    }

/**
 * Returns the last modified date for this entity.
 * @return Last modified time, or KNullDesC if never modified.
 */
const TDesC& 
CNcdPreminetProtocolEntityDependencyImpl::Name() const 
    {
    DASSERT((iName));
    return *iName;
    }

/**
 * Returns the size of this content.
 * @return Type
 */
TNcdDependencyType 
CNcdPreminetProtocolEntityDependencyImpl::Type() const
    {
    return iType;
    }

/**
 * Returns the ID of this entity.
 * @return Id or KNullDesC
 */
const TDesC& 
CNcdPreminetProtocolEntityDependencyImpl::ContentId() const
    {
    DASSERT((iContentId));
    return *iContentId;
    }

/**
 * Retuns the version of this entity.
 * @return Version or KNullDesC
 */
const TDesC& 
CNcdPreminetProtocolEntityDependencyImpl::ContentVersion() const
    {
    DASSERT((iContentVersion));
    return *iContentVersion;
    }

/**
 * Returns the ID of this entity.
 * @return Id or KNullDesC
 */
const TDesC& 
CNcdPreminetProtocolEntityDependencyImpl::EntityId() const
    {
    DASSERT((iEntityId));
    return *iEntityId;
    }

/**
 * Retuns the version of this entity.
 * @return Version or KNullDesC
 */
const TDesC& 
CNcdPreminetProtocolEntityDependencyImpl::EntityTimestamp() const
    {
    DASSERT((iEntityTimestamp));
    return *iEntityTimestamp;
    }

/**
 * Returns the ID of this entity.
 * @return Id or KNullDesC
 */
const MNcdPreminetProtocolDownload* 
CNcdPreminetProtocolEntityDependencyImpl::DownloadDetails() const
    {
    return iDownloadDetails;
    }
