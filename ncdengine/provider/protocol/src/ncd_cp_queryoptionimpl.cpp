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


#include "ncd_cp_queryoptionimpl.h"
#include "ncdprotocolutils.h"
#include "ncdstring.h"

void CNcdConfigurationProtocolQueryOptionImpl::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iValue );
    iName = CNcdString::NewL();
    }

CNcdConfigurationProtocolQueryOptionImpl::~CNcdConfigurationProtocolQueryOptionImpl()
    {
    delete iValue;
    delete iName;    
    }

const TDesC& CNcdConfigurationProtocolQueryOptionImpl::Value() const
    {
    return *iValue;
    }

const CNcdString& CNcdConfigurationProtocolQueryOptionImpl::Name() const
    {
    return *iName;
    }
