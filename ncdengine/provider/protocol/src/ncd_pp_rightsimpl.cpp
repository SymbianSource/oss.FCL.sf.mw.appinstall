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
* Description:   CNcdPreminetProtocolRightsImpl implementation
*
*/


#include "ncd_pp_rightsimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolRightsImpl* 
CNcdPreminetProtocolRightsImpl::NewL() 
    {
    CNcdPreminetProtocolRightsImpl* self =
        new (ELeave) CNcdPreminetProtocolRightsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolRightsImpl* 
CNcdPreminetProtocolRightsImpl::NewLC() 
    {
    CNcdPreminetProtocolRightsImpl* self =
        new (ELeave) CNcdPreminetProtocolRightsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    

void CNcdPreminetProtocolRightsImpl::ConstructL() 
    {
    NcdProtocolUtils::AssignEmptyDesL( iActivationKey );
    NcdProtocolUtils::AssignEmptyDesL( iIssuerUri );
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    NcdProtocolUtils::AssignEmptyDesL( iType );
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iRightsObjectDataBlock );
    NcdProtocolUtils::AssignEmptyDesL( iRightsObjectMime );
    NcdProtocolUtils::AssignEmptyDesL( iRightsObjectData );
    }

CNcdPreminetProtocolRightsImpl::CNcdPreminetProtocolRightsImpl() 
: CBase()
    {
    }

/**
 * Destructor
 */
CNcdPreminetProtocolRightsImpl::~CNcdPreminetProtocolRightsImpl() {
    delete iActivationKey;
    delete iIssuerUri;
    delete iUri;
    delete iType;
    delete iName;    

    delete iRightsObjectDataBlock;    
    delete iRightsObjectMime;    
    delete iRightsObjectData;    
}


const TDesC& CNcdPreminetProtocolRightsImpl::ActivationKey() const
    {
    DASSERT((iActivationKey));
    return *iActivationKey;
    }
    
const TDesC& CNcdPreminetProtocolRightsImpl::IssuerUri() const
    {
    DASSERT((iIssuerUri));
    return *iIssuerUri;
    }
    
const TDesC& CNcdPreminetProtocolRightsImpl::Uri() const
    {
    DASSERT((iUri));
    return *iUri;
    }
    
const TDesC& CNcdPreminetProtocolRightsImpl::Type() const
    {
    DASSERT((iType));
    return *iType;
    }
    
const TDesC& CNcdPreminetProtocolRightsImpl::Name() const
    {
    DASSERT((iName));
    return *iName;
    }

const TDesC& CNcdPreminetProtocolRightsImpl::RightsObjectDataBlock() const
    {
    DLTRACEIN((""));
    DASSERT((iRightsObjectDataBlock));
    return *iRightsObjectDataBlock;
    }
    
const TDesC& CNcdPreminetProtocolRightsImpl::RightsObjectMime() const
    {
    DASSERT((iRightsObjectMime));
    return *iRightsObjectMime;
    }

const TDesC8& CNcdPreminetProtocolRightsImpl::RightsObjectData() const
    {
    DLTRACEIN((""));
    DASSERT((iRightsObjectData));
    return *iRightsObjectData;
    }

