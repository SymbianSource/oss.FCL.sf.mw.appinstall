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

#include "ncd_pp_errorimpl.h"
#include "ncd_cp_query.h"

#include "catalogsdebug.h"

CNcdPreminetProtocolErrorImpl::~CNcdPreminetProtocolErrorImpl()
    {
    delete iMessage;
    delete iId;
    }

TInt CNcdPreminetProtocolErrorImpl::Code() const
    {
    return iCode;
    }

const MNcdConfigurationProtocolQuery* CNcdPreminetProtocolErrorImpl::Message() const
    {
    return iMessage;
    }

const TDesC& CNcdPreminetProtocolErrorImpl::Id() const
{
    return *iId;
}
