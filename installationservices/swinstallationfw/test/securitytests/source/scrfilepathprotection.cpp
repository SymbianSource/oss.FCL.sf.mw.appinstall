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

#include "scrfilepathprotection.h"

#include <usif/scr/scr.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KScrPrivatePathSecName, "SCR private path file registration APIs test");

using namespace Usif;

// Throughout the tests we assume that at least one component is present, i.e. that component id 1 is valid
const TComponentId KExistingComponentId = 1;

void CScrPathProtectionSecTest::TestFileApisL(RSoftwareComponentRegistry& aScrSession, const TDesC& aFilename, const TDesC& aLogHeader)
	{
	TBuf<256> debugHeader;
	
	TInt err(0);
	
	TRAP(err, aScrSession.RegisterComponentFileL(KExistingComponentId, aFilename));
	debugHeader.Format(_L("RegisterComponentFileL - %S"), &aLogHeader);
	CheckFailL(err, debugHeader);	
			
	TRAP(err, aScrSession.SetFilePropertyL(KExistingComponentId, aFilename, _L("IntName"), 1));
	debugHeader.Format(_L("SetFilePropertyL - int - %S"), &aLogHeader);
	CheckFailL(err, debugHeader);	
			
	TRAP(err, aScrSession.SetFilePropertyL(KExistingComponentId, aFilename, _L("StringName"), _L8("PropertyValue")));
	debugHeader.Format(_L("SetFilePropertyL - string - %S"), &aLogHeader);
	CheckFailL(err, debugHeader);	
	
	TRAP(err, aScrSession.DeleteFilePropertyL(KExistingComponentId, aFilename, _L("StringName")));
	debugHeader.Format(_L("DeleteFilePropertyL - %S"), &aLogHeader);
	CheckFailL(err, debugHeader);
	
	TRAP(err, aScrSession.UnregisterComponentFileL(KExistingComponentId, aFilename));
	debugHeader.Format(_L("UnregisterComponentFileL - %S"), &aLogHeader);
	CheckFailL(err, debugHeader);		
	}

CScrPrivatePathProtectionSecTest* CScrPrivatePathProtectionSecTest::NewL()
	{
	CScrPrivatePathProtectionSecTest* self=new(ELeave) CScrPrivatePathProtectionSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrPrivatePathProtectionSecTest::CScrPrivatePathProtectionSecTest()
	{
	SetSidRequired(TUid::Uid(0x12345678)); // Require SID which corresponds to private directory path
	}
	
void CScrPrivatePathProtectionSecTest::ConstructL()
	{
	SetNameL(KScrPrivatePathSecName);
	}

void CScrPrivatePathProtectionSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);

	_LIT(KOwnPrivatePathFilename, "c:\\private\\12345678\\file.txt");

	TestFileApisL(scrSession, KOwnPrivatePathFilename, _L("own private path"));	
			
	// This test should always fail, as we are registering a file in a private directory which does not belong to us
	SetExpectPermissionDenied(ETrue); 
	_LIT(KOthersPrivatePathFilename, "c:\\private\\99999999\\file.txt");
	TestFileApisL(scrSession, KOthersPrivatePathFilename, _L("others' private path"));
		
	SetExpectPermissionDenied(EFalse);
	
	// Public and import path should always succeed
	_LIT(KOthersImportPrivatePathFilename, "c:\\private\\99999999\\import\\file.txt");
	TestFileApisL(scrSession, KOthersImportPrivatePathFilename, _L("others' import path"));
	
	_LIT(KPublicFilename, "c:\file.txt");
	TestFileApisL(scrSession, KPublicFilename, _L("public path"));	
	CleanupStack::PopAndDestroy(&scrSession);
	}

//---------------CScrTCBPathProtectionSecTest------------------

_LIT(KScrTCBPathSecName, "SCR TCB path file registration APIs test");

CScrTCBPathProtectionSecTest* CScrTCBPathProtectionSecTest::NewL()
	{
	CScrTCBPathProtectionSecTest* self=new(ELeave) CScrTCBPathProtectionSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrTCBPathProtectionSecTest::CScrTCBPathProtectionSecTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	}
	
void CScrTCBPathProtectionSecTest::ConstructL()
	{
	SetNameL(KScrTCBPathSecName);
	}

void CScrTCBPathProtectionSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);

	_LIT(KTcbPathFilename, "c:\\sys\\file.txt");
	
	TestFileApisL(scrSession, KTcbPathFilename, _L("\\sys path"));
	
	_LIT(KResourcePathFilename, "c:\\resource\\file.txt");
		
	TestFileApisL(scrSession, KResourcePathFilename, _L("\\resource path"));
		
	CleanupStack::PopAndDestroy(&scrSession);
	}

