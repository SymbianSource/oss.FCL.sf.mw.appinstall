/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_REVOCATION_SERVER_H
#define T_REVOCATION_SERVER_H
#include <test/testexecuteserverbase.h>

/**
 *	@file tRevocationServer.h
 *
 *	TRevocationServer is the base class for all revocation tests
 */

class CTRevocationServer : public CTestServer
	{
public:
	static CTRevocationServer*	NewL();
	virtual CTestStep*				CreateTestStep(const TDesC& aStepName);
	RFs& Fs()	{return iFs;};

private:
	RFs								iFs;
	};

#endif	/* T_REVOCATION_SERVER_H */

