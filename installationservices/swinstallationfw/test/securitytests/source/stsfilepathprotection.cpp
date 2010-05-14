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

#include "stsfilepathprotection.h"

#include <usif/sts/sts.h>
#include <scs/cleanuputils.h>
#include <e32def.h>
#include <f32file.h>

_LIT(KStsPrivatePathSecName, "STS private path file registration APIs test");

using namespace Usif;

CStsPrivatePathProtectionSecTest* CStsPrivatePathProtectionSecTest::NewL()
	{
	CStsPrivatePathProtectionSecTest* self=new(ELeave) CStsPrivatePathProtectionSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CStsPrivatePathProtectionSecTest::CStsPrivatePathProtectionSecTest()
	{
	SetSidRequired(TUid::Uid(0x10285DDF)); //this ID must match the one used in the file path below
	}
	
void CStsPrivatePathProtectionSecTest::ConstructL()
	{
	SetNameL(KStsPrivatePathSecName);
	}

void CStsPrivatePathProtectionSecTest::RunTestL()
	{
	RStsSession stsSession;
	TInt32 transactionId(stsSession.CreateTransactionL());
	CleanupClosePushL(stsSession);
	TInt err(0);	
	TUint newFileMode(EFileShareExclusive | EFileWrite);
	
	_LIT(KOwnPrivatePathFilename, 	"c:\\private\\10285DDF\\file.txt");

	TRAP(err, stsSession.RegisterNewL(KOwnPrivatePathFilename));
	CheckFailL(err, _L("RegisterNewL - own private path c:\\private\\10285DDF"));
	
	RFile dummyFileHandle;
	CleanupClosePushL(dummyFileHandle);

	TRAP(err, stsSession.CreateNewL(KOwnPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateNewL - own private path c:\\private\\10285DDF"));
	
	TRAP(err, stsSession.RemoveL(KOwnPrivatePathFilename));
	CheckFailL(err, _L("RemoveL - own private path c:\\private\\10285DDF"));

	TRAP(err, stsSession.RegisterTemporaryL(KOwnPrivatePathFilename));
	CheckFailL(err, _L("RegisterTemporaryL - own private path c:\\private\\10285DDF"));

	TRAP(err, stsSession.CreateTemporaryL(KOwnPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateTemporaryL - own private path c:\\private\\10285DDF"));

	TRAP(err, stsSession.OverwriteL(KOwnPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("OverwriteL - own private path c:\\private\\10285DDF"));
	
	//---------------------------------------------------------------------------------	
	
	// This test should always pass, as we are registering a file in an import directory which can be accessed regardless of the SID
	SetExpectPermissionDenied(EFalse); 
	_LIT(KOthersImportPrivatePathFilename, "c:\\private\\99999999\\import\\file.txt");
	

	TRAP(err, stsSession.RegisterNewL(KOthersImportPrivatePathFilename));
	CheckFailL(err, _L("RegisterNewL - others import path c:\\private\\99999999\\import"));
	
	TRAP(err, stsSession.CreateNewL(KOthersImportPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateNewL - others import path c:\\private\\99999999\\import"));
	
	TRAP(err, stsSession.RemoveL(KOthersImportPrivatePathFilename));
	CheckFailL(err, _L("RemoveL - others import path c:\\private\\99999999\\import"));

	TRAP(err, stsSession.RegisterTemporaryL(KOthersImportPrivatePathFilename));
	CheckFailL(err, _L("RegisterTemporaryL - others import path c:\\private\\99999999\\import"));

	TRAP(err, stsSession.CreateTemporaryL(KOthersImportPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateTemporaryL - others import path c:\\private\\99999999\\import"));

	TRAP(err, stsSession.OverwriteL(KOthersImportPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("OverwriteL - others import path c:\\private\\99999999\\import"));
	
	//---------------------------------------------------------------------------------	
	
	// This test should always fail, as we are registering a file in a private directory which does not belong to us
	SetExpectPermissionDenied(ETrue); 
	_LIT(KOthersPrivatePathFilename, "c:\\private\\99999999\\file.txt");

	TRAP(err, stsSession.RegisterNewL(KOthersPrivatePathFilename));
	CheckFailL(err, _L("RegisterNewL - others private path c:\\private\\99999999"));
	
	TRAP(err, stsSession.CreateNewL(KOthersPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateNewL - others private path c:\\private\\99999999"));
	
	TRAP(err, stsSession.RemoveL(KOthersPrivatePathFilename));
	CheckFailL(err, _L("RemoveL - others private path c:\\private\\99999999"));

	TRAP(err, stsSession.RegisterTemporaryL(KOthersPrivatePathFilename));
	CheckFailL(err, _L("RegisterTemporaryL - others private path c:\\private\\99999999"));

	TRAP(err, stsSession.CreateTemporaryL(KOthersPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateTemporaryL - others private path c:\\private\\99999999"));

	TRAP(err, stsSession.OverwriteL(KOthersPrivatePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("OverwriteL - others private path c:\\private\\99999999"));
	
	CleanupStack::PopAndDestroy(&dummyFileHandle);
	CleanupStack::PopAndDestroy(&stsSession);
	}

//---------------CStsTCBPathProtectionSecTest------------------

_LIT(KStsTCBPathSecName, "STS TCB path file registration APIs test");

CStsTCBPathProtectionSecTest* CStsTCBPathProtectionSecTest::NewL()
	{
	CStsTCBPathProtectionSecTest* self=new(ELeave) CStsTCBPathProtectionSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CStsTCBPathProtectionSecTest::CStsTCBPathProtectionSecTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	}
	
void CStsTCBPathProtectionSecTest::ConstructL()
	{
	SetNameL(KStsTCBPathSecName);
	}

void CStsTCBPathProtectionSecTest::RunTestL()
	{
	RStsSession stsSession;
	TInt32 transactionId(stsSession.CreateTransactionL());
	CleanupClosePushL(stsSession);
	TInt err(0);	
	
	_LIT(KTcbPathFilename, "c:\\sys\\file.txt");

	TRAP(err, stsSession.RegisterNewL(KTcbPathFilename));
	CheckFailL(err, _L("RegisterNewL - \\sys path"));
	
	TUint newFileMode(EFileShareExclusive | EFileWrite);
	RFile dummyFileHandle;
	CleanupClosePushL(dummyFileHandle);

	TRAP(err, stsSession.CreateNewL(KTcbPathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateNewL - \\sys path"));
	
	TRAP(err, stsSession.RemoveL(KTcbPathFilename));
	CheckFailL(err, _L("RemoveL - \\sys path"));

	TRAP(err, stsSession.RegisterTemporaryL(KTcbPathFilename));
	CheckFailL(err, _L("RegisterTemporaryL - \\sys path"));

	TRAP(err, stsSession.CreateTemporaryL(KTcbPathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateTemporaryL - \\sys path"));

	TRAP(err, stsSession.OverwriteL(KTcbPathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("OverwriteL - \\sys path"));


	_LIT(KResourcePathFilename, "c:\\resource\\file.txt");

	TRAP(err, stsSession.RegisterNewL(KResourcePathFilename));
	CheckFailL(err, _L("RegisterNewL - \\resource path"));
	
	TRAP(err, stsSession.CreateNewL(KResourcePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateNewL - \\resource path"));

	TRAP(err, stsSession.RemoveL(KResourcePathFilename));
	CheckFailL(err, _L("RemoveL - \\resource path"));

	TRAP(err, stsSession.RegisterTemporaryL(KResourcePathFilename));
	CheckFailL(err, _L("RegisterTemporaryL - \\resource path"));

	TRAP(err, stsSession.CreateTemporaryL(KResourcePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("CreateTemporaryL - \\resource path"));

	TRAP(err, stsSession.OverwriteL(KResourcePathFilename, dummyFileHandle, newFileMode));
	dummyFileHandle.Close();
	CheckFailL(err, _L("OverwriteL - \\resource path"));
		
	CleanupStack::PopAndDestroy(&dummyFileHandle);
	CleanupStack::PopAndDestroy(&stsSession);
	}

 
 //---------------CStsPublicPathSecTest------------------
 
 _LIT(KStsPublicPathSecName, "STS public path file registration APIs test");
 
 CStsPublicPathSecTest* CStsPublicPathSecTest::NewL()
 	{
 	CStsPublicPathSecTest* self=new(ELeave) CStsPublicPathSecTest();
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop(self);
 	return self;
 	}
 
 CStsPublicPathSecTest::CStsPublicPathSecTest()
 	{
 	}
 	
 void CStsPublicPathSecTest::ConstructL()
 	{
 	SetNameL(KStsPublicPathSecName);
 	}
 
 void CStsPublicPathSecTest::RunTestL()
 	{
 	RStsSession stsSession;
 	TInt32 transactionId(stsSession.CreateTransactionL());
 	CleanupClosePushL(stsSession);
 	TInt err(0);	

	TUint newFileMode(EFileShareExclusive | EFileWrite);

 	//this test must always pass
 	SetExpectPermissionDenied(EFalse); 
 	_LIT(KPublicPathFilename, "c:\\publicdir\\file.txt");
 
 	TRAP(err, stsSession.RegisterNewL(KPublicPathFilename));
 	CheckFailL(err, _L("RegisterNewL - \\publicdir path"));
 	
 	RFile dummyFileHandle;
 	CleanupClosePushL(dummyFileHandle);
 
 	TRAP(err, stsSession.CreateNewL(KPublicPathFilename, dummyFileHandle, newFileMode));
 	dummyFileHandle.Close();
 	CheckFailL(err, _L("CreateNewL - \\publicdir path"));
 	
 	TRAP(err, stsSession.RemoveL(KPublicPathFilename));
 	CheckFailL(err, _L("RemoveL - \\publicdir path"));
 
 	TRAP(err, stsSession.RegisterTemporaryL(KPublicPathFilename));
 	CheckFailL(err, _L("RegisterTemporaryL - \\publicdir path"));
 
 	TRAP(err, stsSession.CreateTemporaryL(KPublicPathFilename, dummyFileHandle, newFileMode));
 	dummyFileHandle.Close();
 	CheckFailL(err, _L("CreateTemporaryL - \\publicdir path"));
 
 	TRAP(err, stsSession.OverwriteL(KPublicPathFilename, dummyFileHandle, newFileMode));
 	dummyFileHandle.Close();
 	CheckFailL(err, _L("OverwriteL - \\publicdir path"));
 
 	CleanupStack::PopAndDestroy(&dummyFileHandle);
 	CleanupStack::PopAndDestroy(&stsSession);
 	}
