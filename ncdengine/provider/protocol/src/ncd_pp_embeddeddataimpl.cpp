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


#include "ncd_pp_dataentity.h"
#include "ncd_pp_embeddeddataimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolEmbeddedDataImpl* 
CNcdPreminetProtocolEmbeddedDataImpl::NewL() 
    {
    CNcdPreminetProtocolEmbeddedDataImpl* self =
        new (ELeave) CNcdPreminetProtocolEmbeddedDataImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolEmbeddedDataImpl* 
CNcdPreminetProtocolEmbeddedDataImpl::NewLC() 
    {
    CNcdPreminetProtocolEmbeddedDataImpl* self =
        new (ELeave) CNcdPreminetProtocolEmbeddedDataImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
 
 void CNcdPreminetProtocolEmbeddedDataImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iParentId );
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iNameSpace );
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iTimeStamp );
    }

CNcdPreminetProtocolEmbeddedDataImpl::~CNcdPreminetProtocolEmbeddedDataImpl()
    {
    delete iId;
    delete iParentId;
    delete iNameSpace;
    delete iName;
    delete iTimeStamp;
    iDataEntities.ResetAndDestroy();
    }

const TDesC& CNcdPreminetProtocolEmbeddedDataImpl::ParentId() const
    {
    return *iParentId;
    }

const TDesC& CNcdPreminetProtocolEmbeddedDataImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdPreminetProtocolEmbeddedDataImpl::TimeStamp() const
    {
    return *iTimeStamp;
    }

const TDesC& CNcdPreminetProtocolEmbeddedDataImpl::NameSpace() const
    {
    return *iNameSpace;
    }

const TDesC& CNcdPreminetProtocolEmbeddedDataImpl::Name() const
    {
    return *iName;
    }

TInt CNcdPreminetProtocolEmbeddedDataImpl::DataEntityCount() const
    {
    return iDataEntities.Count();
    }
    
const MNcdPreminetProtocolDataEntity&
    CNcdPreminetProtocolEmbeddedDataImpl::DataEntityL(TInt aIndex) const
    {
    DASSERT( aIndex >= 0 && aIndex < iDataEntities.Count() );
    if ( aIndex < 0 || aIndex >= iDataEntities.Count() ) 
        {
        User::Leave(KErrArgument);
        }
    return *iDataEntities[aIndex];
    }



