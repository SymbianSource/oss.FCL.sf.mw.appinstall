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
* Description:   CNcdPreminetProtocolIcon definition
*
*/


#include "ncd_pp_iconimpl.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"

CNcdPreminetProtocolIcon* CNcdPreminetProtocolIcon::NewL()
    {
    CNcdPreminetProtocolIcon* self = new (ELeave) CNcdPreminetProtocolIcon;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolIcon* CNcdPreminetProtocolIcon::NewLC()
    {
    CNcdPreminetProtocolIcon* self = new (ELeave) CNcdPreminetProtocolIcon;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNcdPreminetProtocolIcon::CNcdPreminetProtocolIcon()
    {
    }

void CNcdPreminetProtocolIcon::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iDataBlock );
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    NcdProtocolUtils::AssignEmptyDesL( iData );
    }

CNcdPreminetProtocolIcon::~CNcdPreminetProtocolIcon()
    {
    delete iId;
    delete iDataBlock;
    delete iUri;
    delete iData;
    }

const TDesC& CNcdPreminetProtocolIcon::Id() const
    {
    DASSERT( iId );
    return *iId;
    }

const TDesC& CNcdPreminetProtocolIcon::DataBlock() const
    {
    DASSERT( iDataBlock );
    return *iDataBlock;
    }

const TDesC& CNcdPreminetProtocolIcon::Uri() const
    {
    DASSERT( iUri );
    return *iUri;
    }

const TDesC8& CNcdPreminetProtocolIcon::Data() const
    {
    //DASSERT( iData );
    return *iData;
    }

