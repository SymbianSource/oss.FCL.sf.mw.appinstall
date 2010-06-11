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
*
*/


/**
 @file
*/

#include "swiscaptest.h"

#include "installclientserver.h"

#include <e32def.h>

#include "launcher.h"
#include "writestream.h"

_LIT(KSwisCapTestName, "Install Server capability test");


CSwisCapTest* CSwisCapTest::NewL()
	{
	CSwisCapTest* self=new(ELeave) CSwisCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwisCapTest::CSwisCapTest()
	{
	SetCapabilityRequired(ECapabilityTrustedUI);
//	SetCapabilityRequired(ECapabilityProtServ);
	}
	
void CSwisCapTest::ConstructL()
	{
	SetNameL(KSwisCapTestName);
	}

void CSwisCapTest::RunTestL()
	{
	Swi::RInstallServerSession session;
	CleanupClosePushL(session);
		
	if (KErrNone != CheckFailL(session.Connect(), _L("Connect")))
		{
		// if we get here we didn't connect and expected not to, but cannot carry on with tests
		CleanupStack::PopAndDestroy(&session);
		return;
		}

	TInt err;
	TRequestStatus s;
	TRequestStatus& rs=s;
	
	Swi::CInstallPrefs* installPrefs=Swi::CInstallPrefs::NewLC();
	
	Swi::CWriteStream* writeStream=Swi::CWriteStream::NewL();
	*writeStream << *installPrefs; // This can leave.

	TBuf8<4> arg;
	TPtr8 ptr(0, 0);
	ptr.Set(writeStream->Ptr());
	TIpcArgs argsInstall(&ptr);

	TRAP(err, session.InstallL(argsInstall, rs));
	User::WaitForRequest(rs);
	err+=rs.Int();
	CheckFailL(err, _L("InstallL"));

	// fake package argument
	TIpcArgs argsUninstall(&arg);
	TRAP(err, session.Uninstall(argsUninstall, rs));
	User::WaitForRequest(rs);
	err+=rs.Int();
	CheckFailL(err, _L("UninstallL"));
	
	TRAP(err, session.Cancel());
	CheckFailL(err, _L("Cancel"));

	// fake restore arguments
	TIpcArgs restoreArgs(&arg, &arg);;
	TRAP(err, session.Restore(restoreArgs));
	CheckFailL(err, _L("Restore"));
/*
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();
	
	RFile file;
	User::LeaveIfError(file.Open(fs, _L("z:\\system\\data\\swipolicy.ini"), EFileRead));
	CleanupClosePushL(file);
	
	arg.SetLength(1);
	arg[0] = 'b';
	TIpcArgs restoreFileArgs;
	User::LeaveIfError(file.TransferToServer(restoreFileArgs, 0, 1));
	restoreFileArgs.Set(2, &arg);
	TRAP(err, session.RestoreFile(restoreFileArgs));
	CheckFailL(err, _L("RestoreFile"));

	CleanupStack::PopAndDestroy(2, &fs); // file

	TRAP(err, session.RestoreCommit());
	CheckFailL(err, _L("RestoreCommit"));
*/
	CleanupStack::PopAndDestroy(2, &session);
	}

