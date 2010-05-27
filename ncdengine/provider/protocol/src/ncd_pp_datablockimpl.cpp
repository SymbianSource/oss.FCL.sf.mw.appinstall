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


#include "ncdprotocolutils.h"
#include "ncd_pp_datablockimpl.h"
#include "catalogsdebug.h"

CNcdPreminetProtocolDataBlockImpl* 
CNcdPreminetProtocolDataBlockImpl::NewL() 
    {
    CNcdPreminetProtocolDataBlockImpl* self =
        new (ELeave) CNcdPreminetProtocolDataBlockImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolDataBlockImpl* 
CNcdPreminetProtocolDataBlockImpl::NewLC() 
    {
    CNcdPreminetProtocolDataBlockImpl* self =
        new (ELeave) CNcdPreminetProtocolDataBlockImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CNcdPreminetProtocolDataBlockImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iId );
    NcdProtocolUtils::AssignEmptyDesL( iNameSpace );
    }

CNcdPreminetProtocolDataBlockImpl::~CNcdPreminetProtocolDataBlockImpl()
    {
    delete iId;
    iId = 0;
    
    delete iNameSpace;
    iNameSpace = 0;
    
    delete iContent;
    iContent = 0;
    }

const TDesC& CNcdPreminetProtocolDataBlockImpl::Id() const
    {
    return *iId;
    }

const TDesC& CNcdPreminetProtocolDataBlockImpl::NameSpace() const
    {
    return *iNameSpace;
    }

const TDesC8& CNcdPreminetProtocolDataBlockImpl::Content() const
    {
    return *iContent;
    }

