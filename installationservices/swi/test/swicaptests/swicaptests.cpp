/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

// tests
#include "swilaunchercaptest.h"
#include "swiocspcaptest.h"
#include "swiscaptest.h"
#include "sishelpercaptest.h"
#include "registrycaptest.h"
#include "uisscaptest.h"

#include <captestframework/aprcaptestutility.h>

#include <s32file.h>

// Factory function
MCapabilityTestFactory* CapabilityTestFactoryL()
	{
	CDefaultCapabilityTestFactory* factory=new(ELeave) CDefaultCapabilityTestFactory();
	
	// Add tests. This is where you would add a new test
	factory->AddTestL(CSwiLauncherCapTest::NewL());
	factory->AddTestL(CSwiOcspCapTest::NewL());
	factory->AddTestL(CSwisCapTest::NewL());
	//factory->AddTestL(CSisHelperCapTest::NewL());

	factory->AddTestL(CReadUserDataRegistryCapTest::NewL());
	factory->AddTestL(CPrivateRegistryCapTest::NewL());
	factory->AddTestL(CPublicRegistryCapTest::NewL());
	factory->AddTestL(CDaemonRegistryCapTest::NewL());
	factory->AddTestL(CRevocationRegistryCapTest::NewL());
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	factory->AddTestL(CSifServerRegistryCapTest::NewL());
	factory->AddTestL(CSwiLauncherInstallServerUtilCapTest::NewL());
	factory->AddTestL(CSwiLauncherSisRegistryServerUtilCapTest::NewL());
	factory->AddTestL(CSisRegistryTCBCapTest::NewL());
	factory->AddTestL(CSisLauncherSwiSidTest::NewL());
#endif
	return factory;
	}
	
EXPORT_C MCapabilityTestFactory* CapabilityTestFactory()
	{
	MCapabilityTestFactory* factory=NULL;
	TRAP_IGNORE(factory = CapabilityTestFactoryL());
	return factory;
	}


