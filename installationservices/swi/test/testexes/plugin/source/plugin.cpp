/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements Test ECOM plug-in interface.
*
*/


/**
 @file
 @test
*/
 
#include "plugin.h"

static const TUint KTestPluginImplementationId = 0x102857A3;

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestPluginImplementationId, CTestPlugin::NewL)
	};
	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	
//
//CTestPlugin
//

CTestPlugin::CTestPlugin()
/**
	Constructor.
 */	
 	{
 	
 	}
 	

CTestPlugin::~CTestPlugin()
/**
	Destructor
 */
	{
	}


CTestPlugin* CTestPlugin::NewL()
/**
	Factory method that instantiates a new SWI Observer ECOM plug-in.

	@return A pointer to the new UPS SWI Observer object.
*/
	{
	CTestPlugin *self = new(ELeave) CTestPlugin();
	//CleanupStack::PushL(self);
	//CleanupStack::Pop(self);
	return self;
	}
	
