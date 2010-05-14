/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ECOM proxy table for AppMngr2 GS (General Settigns)
*                Installation Files plug-in
*
*/


#include "appmngr2gsinstfilesplugin.h"  // CAppMngr2GsInstFilesPlugin
#include <ecom/implementationproxy.h>   // TImplementationProxy

// ---------------------------------------------------------------------------
// Local array that maps the ECom implementation UIDs to factory functions
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( KAppMngr2InstFilesUidValue, CAppMngr2GsInstFilesPlugin::NewL )
	};

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

