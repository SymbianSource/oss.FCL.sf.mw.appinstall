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
* Description:   CNcdPreminetProtocolSkin definition
*
*/


#include "ncd_pp_skinimpl.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"

CNcdPreminetProtocolSkin* CNcdPreminetProtocolSkin::NewL()
    {
    CNcdPreminetProtocolSkin* self = new (ELeave) CNcdPreminetProtocolSkin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolSkin* CNcdPreminetProtocolSkin::NewLC()
    {
    CNcdPreminetProtocolSkin* self = new (ELeave) CNcdPreminetProtocolSkin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdPreminetProtocolSkin::CNcdPreminetProtocolSkin()
    {
    }

void CNcdPreminetProtocolSkin::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iTimestamp );
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    }

CNcdPreminetProtocolSkin::~CNcdPreminetProtocolSkin()
    {
    delete iId;
    delete iTimestamp;
    delete iUri;
    }

const TDesC& CNcdPreminetProtocolSkin::Id() const
    {
    DASSERT( iId );
    return *iId;
    }

const TDesC& CNcdPreminetProtocolSkin::Timestamp() const
    {
    DASSERT( iTimestamp );
    return *iTimestamp;
    }

const TDesC& CNcdPreminetProtocolSkin::Uri() const
    {
    DASSERT( iUri );
    return *iUri;
    }
