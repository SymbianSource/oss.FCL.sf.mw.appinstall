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


#include "ncd_pp_folderrefimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolFolderRefImpl* 
CNcdPreminetProtocolFolderRefImpl::NewL() 
    {
    CNcdPreminetProtocolFolderRefImpl* self =
        new (ELeave) CNcdPreminetProtocolFolderRefImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolFolderRefImpl* 
CNcdPreminetProtocolFolderRefImpl::NewLC() 
    {
    CNcdPreminetProtocolFolderRefImpl* self =
        new (ELeave) CNcdPreminetProtocolFolderRefImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
void CNcdPreminetProtocolFolderRefImpl::ConstructL()
    {
    iDescription = ENormal;
    NcdProtocolUtils::AssignEmptyDesL( iParentId );
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iNamespace );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iRemoteUri );
    NcdProtocolUtils::AssignEmptyDesL( iServerUri );
    NcdProtocolUtils::AssignEmptyDesL( iParentNamespace );
    iCount = KValueNotSet;
    iAuthorizedCount = KValueNotSet;
    }

CNcdPreminetProtocolFolderRefImpl::~CNcdPreminetProtocolFolderRefImpl()
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

MNcdPreminetProtocolEntityRef::TType CNcdPreminetProtocolFolderRefImpl::Type() const
    {
    return EFolderRef;
    }

MNcdPreminetProtocolEntityRef::TDescription CNcdPreminetProtocolFolderRefImpl::Description() const
    {
    return iDescription;
    }

TInt CNcdPreminetProtocolFolderRefImpl::Count() const
    {
    return iCount;
    }

TInt CNcdPreminetProtocolFolderRefImpl::AuthorizedCount() const
    {
    return iAuthorizedCount;
    }



const TDesC& CNcdPreminetProtocolFolderRefImpl::ParentId() const
    {
    return *iParentId;
    }

const TDesC& CNcdPreminetProtocolFolderRefImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdPreminetProtocolFolderRefImpl::Timestamp() const
    {
    return *iTimestamp;
    }

const TDesC& CNcdPreminetProtocolFolderRefImpl::Namespace() const
    {
    return *iNamespace;
    }

const RArray<TInt>& CNcdPreminetProtocolFolderRefImpl::Queries() const
    {
    return iQueries;
    }

const TDesC& CNcdPreminetProtocolFolderRefImpl::ServerUri() const
    {
    return *iServerUri;
    }


const TDesC& CNcdPreminetProtocolFolderRefImpl::RemoteUri() const
    {
    return *iRemoteUri;
    }


const TDesC& CNcdPreminetProtocolFolderRefImpl::ParentNamespace() const
    {
    return *iParentNamespace;
    }

TInt CNcdPreminetProtocolFolderRefImpl::ValidUntilDelta() const 
    {
    return iValidUntilDelta;
    }
    
TBool CNcdPreminetProtocolFolderRefImpl::ValidUntilAutoUpdate() const 
    {
    return iValidUntilAutoUpdate;
    }    
