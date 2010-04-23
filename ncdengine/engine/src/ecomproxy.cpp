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
* Description:   ECom implementation table export.
*
*/


#include <e32std.h>
#include <implementationproxy.h>

#include "catalogsengineimpl.h"
#include "catalogsuids.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
        IMPLEMENTATION_PROXY_ENTRY(KCatalogsEngineImplementationUid,  CCatalogsEngineImpl::NewL)
    };

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
