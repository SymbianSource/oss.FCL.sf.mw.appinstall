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

#include "sifsecuritycontext.h"
#include <usif/sif/sif.h>
#include <e32def.h>

_LIT(KSifSecurityContextName, "SIF TSecurityContext APIs test");

using namespace Usif;

// This test calls only the GetComponentInfo() API because the logic of passing TSecurityContext to
// a SIF plug-in is exactly the same for all the SIF operations.
namespace
	{
	_LIT(KFileName, "z:\\tusif\\tsif\\data\\test.nonnativepackage.txt");
	_LIT_SECURE_ID(KTestSuiteSecureId, 0x10285BCC);

	void DoTestL()
		{
		RSoftwareInstall sif;
		User::LeaveIfError(sif.Connect());
		CleanupClosePushL(sif);

		CComponentInfo* compInfo = CComponentInfo::NewLC();

		TRequestStatus status;
		sif.GetComponentInfo(KFileName, *compInfo, status);
		User::WaitForRequest(status);

		CleanupStack::PopAndDestroy(2, &sif);
		User::LeaveIfError(status.Int());
		}
	}

CSifSecurityContextTest* CSifSecurityContextTest::NewL()
	{
	CSifSecurityContextTest* self=new(ELeave) CSifSecurityContextTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSifSecurityContextTest::CSifSecurityContextTest()
	{
	SetCapabilityRequired(ECapabilityReadUserData);
	SetSidRequired(KTestSuiteSecureId);
	}

void CSifSecurityContextTest::ConstructL()
	{
	SetNameL(KSifSecurityContextName);
	}

void CSifSecurityContextTest::RunTestL()
	{
	TRAPD(err, DoTestL());
	CheckFailL(err, _L("CSifSecurityContextTest::DoTestL()"));
	}
