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


#include "ncd_pp_itemrefimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolItemRefImpl* 
CNcdPreminetProtocolItemRefImpl::NewL() 
    {
    CNcdPreminetProtocolItemRefImpl* self =
        new (ELeave) CNcdPreminetProtocolItemRefImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolItemRefImpl* 
CNcdPreminetProtocolItemRefImpl::NewLC() 
    {
    CNcdPreminetProtocolItemRefImpl* self =
        new (ELeave) CNcdPreminetProtocolItemRefImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CNcdPreminetProtocolItemRefImpl::ConstructL()
    {
    iDescription = ENormal;
    NcdProtocolUtils::AssignEmptyDesL( iParentId );
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iNamespace );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iRemoteUri );
    NcdProtocolUtils::AssignEmptyDesL( iServerUri );
    NcdProtocolUtils::AssignEmptyDesL( iParentNamespace );
    }


MNcdPreminetProtocolEntityRef::TType CNcdPreminetProtocolItemRefImpl::Type() const
    {
    return EItemRef;
    }

MNcdPreminetProtocolEntityRef::TDescription CNcdPreminetProtocolItemRefImpl::Description() const
    {
    return iDescription;
    }


CNcdPreminetProtocolItemRefImpl::~CNcdPreminetProtocolItemRefImpl()
    {
    delete iId;
    delete iParentId;
    delete iNamespace;
    delete iTimestamp;
    delete iRemoteUri;
    iQueries.Close();
    delete iServerUri;
    delete iParentNamespace;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::ParentId() const
    {
    return *iParentId;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::Timestamp() const
    {
    return *iTimestamp;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::Namespace() const
    {
    return *iNamespace;
    }

const RArray<TInt>& CNcdPreminetProtocolItemRefImpl::Queries() const
    {
    return iQueries;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::ServerUri() const
    {
    return *iServerUri;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::RemoteUri() const
    {
    return *iRemoteUri;
    }

const TDesC& CNcdPreminetProtocolItemRefImpl::ParentNamespace() const
    {
    return *iParentNamespace;
    }

TInt CNcdPreminetProtocolItemRefImpl::ValidUntilDelta() const 
    {
    return iValidUntilDelta;
    }
    
TBool CNcdPreminetProtocolItemRefImpl::ValidUntilAutoUpdate() const 
    {
    return iValidUntilAutoUpdate;
    }    
