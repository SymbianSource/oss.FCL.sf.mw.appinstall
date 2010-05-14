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
*
*/




/**
 @file
 @test
*/

#include <captestframework/aprcaptestutility.h>

namespace Usif
	{
	class RSoftwareComponentRegistry;
	}

class CScrPathProtectionSecTest : public CDefaultCapabilityTest
	{
protected:
	void TestFileApisL(Usif::RSoftwareComponentRegistry& aScrSession, const TDesC& aFilename, const TDesC& aLogHeader);
	};


class CScrPrivatePathProtectionSecTest : public CScrPathProtectionSecTest
	{
public:
	static CScrPrivatePathProtectionSecTest* NewL();
	
private:
	CScrPrivatePathProtectionSecTest();
	
	void RunTestL();

	void ConstructL();
	};

class CScrTCBPathProtectionSecTest : public CScrPathProtectionSecTest
	{
public:
	static CScrTCBPathProtectionSecTest* NewL();
	
private:
	CScrTCBPathProtectionSecTest();
	
	void RunTestL();

	void ConstructL();
	};

