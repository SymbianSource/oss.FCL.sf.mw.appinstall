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


#include <e32test.h>
#include <f32file.h>

static RTest test(_L("setsubst"));

GLDEF_C TInt E32Main()
/**
	setsubst drive-letter: subst-path
 */
    {
	CTrapCleanup* cleanup = CTrapCleanup::New();
	test(cleanup != 0);
	
	__UHEAP_MARK;

	test.Title();
	test.Start(_L(" @SYMTestCaseID API-SEC-SETSUBST-0001 Starting tests..."));

	// read in the command line args
	const TInt KCmdLineBufLen = 64;
	test(User::CommandLineLength() <= KCmdLineBufLen);

	TBuf<KCmdLineBufLen> cmdLine;
	User::CommandLine(cmdLine);
	TLex l(cmdLine);

	// extract the drive number
	TPtrC token = l.NextToken();
	test(token.Length() == 2);
	test(TChar(token[0]).IsAlpha());
	test(token[1] == ':');
	TInt drvNum;
	TChar drvCh = token[0];
	test(RFs::CharToDrive(drvCh, drvNum) == KErrNone);

	// extract the replacement path
	token.Set(l.NextToken());
	TInt dirLen = token.Length();
	test(	dirLen == 0
		||	(	dirLen >= 3
			&&	TChar(token[0]).IsAlpha()
			&&	token[1] == ':'
			&&	token[2] == '\\' ) );
	
	// do the substitution
	RFs fs;
	test(fs.Connect() == KErrNone);

	test(fs.SetSubst(token, drvNum) == KErrNone);

	fs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	delete cleanup;
	
	return KErrNone;
	}
