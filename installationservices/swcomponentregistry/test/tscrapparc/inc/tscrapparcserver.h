/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the test server of the Software Component Registry test harness
*
*/

/**
 @file 
 @internalComponent
 @test
*/
#ifndef TSCRAPPARCSERVER_H_
#define TSCRAPPARCSERVER_H_

#include <test/testexecuteserverbase.h>

_LIT(KScrApparcTestServerName, "tscrapparc");

class CScrApparcTestServer : public CTestServer
/**
    Describes the test server for the SCR-Apparc test harness.
 */
    {
public:
    static CScrApparcTestServer* NewL();
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    ~CScrApparcTestServer();

private:
    TBool IsPerformanceTestStep(const TDesC& aStepName, TPtrC& aStrippedName);
    };
    
#endif /* TSCRAPPARCSERVER_H_ */
