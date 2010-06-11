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
* Defines the test server of the Software Component Registry data layer unit test
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCRDATALAYERSERVER_H
#define TSCRDATALAYERSERVER_H

#include <test/testexecuteserverbase.h>

_LIT(KScrDataLayerTestServerName, "tscrdatalayer");

class CScrDataLayerTestServer : public CTestServer
/**
	Describes the test server for the SCR data layer unit test harness.
 */
	{
public:
	static CScrDataLayerTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CScrDataLayerTestServer();
	};

#endif /* TSCRSERVER_H */
