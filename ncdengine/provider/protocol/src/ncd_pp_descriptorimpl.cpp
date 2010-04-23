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
* Description:   CNcdPreminetProtocolDescriptorImpl declaration
*
*/


#include "ncd_pp_descriptorimpl.h"
#include "ncdprotocolutils.h"

CNcdPreminetProtocolDescriptorImpl* 
CNcdPreminetProtocolDescriptorImpl::NewL() 
    {
    CNcdPreminetProtocolDescriptorImpl* self =
        new (ELeave) CNcdPreminetProtocolDescriptorImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolDescriptorImpl* 
CNcdPreminetProtocolDescriptorImpl::NewLC() 
    {
    CNcdPreminetProtocolDescriptorImpl* self =
        new (ELeave) CNcdPreminetProtocolDescriptorImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    

void CNcdPreminetProtocolDescriptorImpl::ConstructL() 
    {
    NcdProtocolUtils::AssignEmptyDesL( iType );
    NcdProtocolUtils::AssignEmptyDesL( iName );
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    NcdProtocolUtils::AssignEmptyDesL( iData );
    }

CNcdPreminetProtocolDescriptorImpl::CNcdPreminetProtocolDescriptorImpl() 
: CBase()
    {
    }

/**
 * Destructor
 */
CNcdPreminetProtocolDescriptorImpl::~CNcdPreminetProtocolDescriptorImpl() {
    delete iType;
    delete iName;
    delete iUri;
    delete iData;
}


const TDesC& CNcdPreminetProtocolDescriptorImpl::Type() const
    {
    return *iType;
    }
    
const TDesC& CNcdPreminetProtocolDescriptorImpl::Name() const
    {
    return *iName;
    }
    
const TDesC& CNcdPreminetProtocolDescriptorImpl::Uri() const
    {
    return *iUri;
    }
    
const TDesC8& CNcdPreminetProtocolDescriptorImpl::Data() const
    {
    return *iData;
    }
    
