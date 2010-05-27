/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ECom interface implementation
*
*/


#include "cswinstadapter.h"             // CSWInstAdapter
#include "swinstadapteruids.hrh"        // KSWInstAdapterImplUid
#include <ecom/implementationproxy.h>   // TImplementationProxy

// ----------------------------------------------------------------------------
// ImplementationTable
// Ecom plug-in implementation table
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY( KSWInstAdapterImplUid, CSWInstAdapter::NewL )
#else   // __EABI__
     { { KSWInstAdapterImplUid }, CSWInstAdapter::NewL }
#endif  // __EABI__
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Instance of implementation proxy
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End Of File
