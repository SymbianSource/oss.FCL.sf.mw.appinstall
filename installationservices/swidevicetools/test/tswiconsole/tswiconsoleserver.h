/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file provides class declaration of CTSwiConsoleServer
*
*/


#ifndef __TSWICONSOLE_SERVER_H__
#define __TSWICONSOLE_SERVER_H__

#include <test/testexecuteserverbase.h>

class CTSwiConsoleServer : public CTestServer
	{
public:
	static CTSwiConsoleServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};

#endif	// __TSWICONSOLE_SERVER_H__ 
