// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <f32file.h>
#include <e32test.h>
#include "tsqlitesecure_common.h"

const TInt KExitCode1 = -10001;
const TInt KExitCode2 = -10002;

//
//Test macros and functions
void Check1(TInt aValue, const char* aFile, TInt aLine)
	{
	if(!aValue)
		{
		TFileName fname;
		fname.Copy(TPtrC8((const TUint8*)aFile));
		RDebug::Print(_L("*** File \"%S\", Line %d, \"TEST\" expression evaluated to false\r\n"), &fname, aLine);
		User::Exit(KExitCode1);
		}
	}
	
void Check2(TInt aValue, TInt aExpected, const char* aFile, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TFileName fname;
		fname.Copy(TPtrC8((const TUint8*)aFile));
		RDebug::Print(_L("*** File \"%S\", Line %d, Expected result: %d, got: %d\r\n"), &fname, aLine, aExpected, aValue);
		User::Exit(KExitCode2);
		}
	}

//

static void CreateTestEnv()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	TFileName defaultPath;
	err = fs.SessionPath(defaultPath);
	TEST2(err, KErrNone);

	err = fs.MkDir(defaultPath);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

//

static void DoMain()
	{
	TBuf<20> processParameter;
	User::CommandLine(processParameter);
	
	RFile mainDb;	
	TInt err = mainDb.AdoptFromCreator(1, 2);
	TEST2(err, KErrNone);
	
	RFile journal;	
	err = journal.AdoptFromCreator(3, 4);
	TEST2(err, KErrNone);
	
	DoTest(mainDb, journal, processParameter);
	
	journal.Close();
	mainDb.Close();
	}

//

TInt E32Main()
	{
 	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	CreateTestEnv();
	
	__UHEAP_MARK;
	DoMain();
	__UHEAP_MARKEND;

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
