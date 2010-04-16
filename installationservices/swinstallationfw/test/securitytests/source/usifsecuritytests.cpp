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
#include "scrcomponentspecific.h"
#include "scrreaduserdata.h"
#include "scrallfiles.h"
#include "scrwritedevicedata.h"
#include "scrnonrestricted.h"
#include "scrfilepathprotection.h"
#include "stsfilepathprotection.h"
#include "stsrollbackalltest.h"
#include "scrpluginmanagement.h"
#include "scrapparccomponentspecific.h"

#include <s32file.h>

// Factory function
MCapabilityTestFactory* CapabilityTestFactoryL()
	{
	CDefaultCapabilityTestFactory* factory=new(ELeave) CDefaultCapabilityTestFactory();

	factory->AddTestL(CScrComponentSpecificSecTest::NewL());
	factory->AddTestL(CScrReadUserDataSecTest::NewL());
	factory->AddTestL(CScrNonRestrictedSecTest::NewL());
	factory->AddTestL(CScrPrivatePathProtectionSecTest::NewL());
	factory->AddTestL(CScrTCBPathProtectionSecTest::NewL());
	factory->AddTestL(CScrAllFilesSecTest::NewL());
	factory->AddTestL(CScrWriteDeviceDataSecTest::NewL());
	factory->AddTestL(CStsTCBPathProtectionSecTest::NewL());
	factory->AddTestL(CStsPrivatePathProtectionSecTest::NewL());
	factory->AddTestL(CStsPublicPathSecTest::NewL());
	factory->AddTestL(CStsRollbackAllSecTest::NewL());
	factory->AddTestL(CScrPluginManagementSecTest::NewL());
	factory->AddTestL(CScrApparcComponentSpecificSecTest::NewL());

	return factory;
	}

EXPORT_C MCapabilityTestFactory* CapabilityTestFactory()
	{
	MCapabilityTestFactory* factory=NULL;
	TRAP_IGNORE(factory = CapabilityTestFactoryL());
	return factory;
	}


