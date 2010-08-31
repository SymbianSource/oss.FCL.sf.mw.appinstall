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

#include "scrwritedevicedata.h"

#include <usif/scr/scr.h>
#include <e32def.h>

_LIT(KScrWriteDeviceDataSecName, "SCR WriteDeviceData APIs test");

using namespace Usif;

CScrWriteDeviceDataSecTest* CScrWriteDeviceDataSecTest::NewL()
	{
	CScrWriteDeviceDataSecTest* self=new(ELeave) CScrWriteDeviceDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrWriteDeviceDataSecTest::CScrWriteDeviceDataSecTest()
	{
	SetCapabilityRequired(ECapabilityWriteDeviceData);
	}
	
void CScrWriteDeviceDataSecTest::ConstructL()
	{
	SetNameL(KScrWriteDeviceDataSecName);
	}

void CScrWriteDeviceDataSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	TInt err(0);
	// SetIsComponentDrmProtectedL API	
	TRAP(err, scrSession.SetIsComponentDrmProtectedL(1, ETrue));
	CheckFailL(err, _L("SetIsComponentDrmProtectedL - set as DRM protected"));
	
	TRAP(err, scrSession.SetIsComponentDrmProtectedL(1, EFalse));
	CheckFailL(err, _L("SetIsComponentDrmProtectedL - set as non-DRM protected"));
	
	// SetIsComponentHiddenL API		
	TRAP(err, scrSession.SetIsComponentHiddenL(1, ETrue));
	CheckFailL(err, _L("SetIsComponentHiddenL - set as hidden"));
		
	TRAP(err, scrSession.SetIsComponentHiddenL(1, EFalse));
	CheckFailL(err, _L("SetIsComponentHiddenL - set as visible"));
	
	// SetIsComponentKnownRevokedL API		
	TRAP(err, scrSession.SetIsComponentKnownRevokedL(1, ETrue));
	CheckFailL(err, _L("SetIsComponentKnownRevokedL - set as known-revoked"));
		
	TRAP(err, scrSession.SetIsComponentKnownRevokedL(1, EFalse));
	CheckFailL(err, _L("SetIsComponentKnownRevokedL - set as non-known-revoked"));
	
	// SetIsComponentOriginVerifiedL API		
	TRAP(err, scrSession.SetIsComponentOriginVerifiedL(1, ETrue));
	CheckFailL(err, _L("SetIsComponentOriginVerifiedL - set as origin-verified"));
		
	TRAP(err, scrSession.SetIsComponentOriginVerifiedL(1, EFalse));
	CheckFailL(err, _L("SetIsComponentOriginVerifiedL - set as non-origin-verified"));
	
	CleanupStack::PopAndDestroy(&scrSession);
	}
