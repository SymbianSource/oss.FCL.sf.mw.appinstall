/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the class CDataProviderServer.
*
*/


/**
 @file tdataprovider.h
 @internalTechnology
*/


#if (!defined __T_DATA_PROVIDER_H__)
#define __T_DATA_PROVIDER_H__

#include <test/testexecuteserverbase.h>

class CDataProviderServer : public CTestServer
	{
public:
	static CDataProviderServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};

#endif
