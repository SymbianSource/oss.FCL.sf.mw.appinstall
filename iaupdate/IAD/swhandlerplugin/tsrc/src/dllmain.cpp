/*
* Copyright (c) 2010- Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <digia/eunit/ceunittestsuite.h>
#include <ut_swhandlerplugin.h>

/**
 * UT_CSwHandlerPlugin.dll test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
	{
	// In this example, a root suite is needed for holding the two suites
	//with test cases. If there was only one suite with tests, that suite
	//instance could be returned directly. (see the other examples)
	CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("Sw Handler Plugin Unit Tests"));

	// Note that NewLC leaves the object in the cleanupstack.
	rootSuite->AddL( UT_CSwHandlerPlugin::NewLC() );
	CleanupStack::Pop();


	CleanupStack::Pop( rootSuite );

	return rootSuite;
	}
