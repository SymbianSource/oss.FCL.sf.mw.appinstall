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
* Description:   CNcdPreminetProtocolSmsDetailImpl implementation
*
*/


#include "ncd_pp_smsdetailsimpl.h"
#include "ncd_pp_purchase.h"
#include "ncdprotocolutils.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"

CNcdPreminetProtocolSmsDetailsImpl* 
CNcdPreminetProtocolSmsDetailsImpl::NewL() 
    {
    CNcdPreminetProtocolSmsDetailsImpl* self =
        new (ELeave) CNcdPreminetProtocolSmsDetailsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolSmsDetailsImpl* 
CNcdPreminetProtocolSmsDetailsImpl::NewLC() 
    {
    CNcdPreminetProtocolSmsDetailsImpl* self =
        new (ELeave) CNcdPreminetProtocolSmsDetailsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolSmsDetailsImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iAddress );
    NcdProtocolUtils::AssignEmptyDesL( iMessage );
    }

CNcdPreminetProtocolSmsDetailsImpl::~CNcdPreminetProtocolSmsDetailsImpl()
    {
    delete iAddress;
    delete iMessage;
    }

const TDesC& 
CNcdPreminetProtocolSmsDetailsImpl::Address() const
    {
    return *iAddress;
    }

const TDesC& 
CNcdPreminetProtocolSmsDetailsImpl::Message() const
    {
    return *iMessage;
    }
    
CNcdPreminetProtocolSmsDetailsImpl*
CNcdPreminetProtocolSmsDetailsImpl::CloneL() const 
    {
    CNcdPreminetProtocolSmsDetailsImpl* clone = NewLC();
    AssignDesL( clone->iAddress, *iAddress );
    AssignDesL( clone->iMessage, *iMessage );
    CleanupStack::Pop( clone );
    return clone;
    }

void CNcdPreminetProtocolSmsDetailsImpl::ExternalizeL(
    RWriteStream& aStream )
    {
    DLTRACEIN((""));
    ExternalizeDesL( *iAddress, aStream );
    ExternalizeDesL( *iMessage, aStream );
    }
    
void CNcdPreminetProtocolSmsDetailsImpl::InternalizeL(
    RReadStream& aStream )
    {
    DLTRACEIN((""));
    InternalizeDesL( iAddress, aStream );
    InternalizeDesL( iMessage, aStream );
    }
