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


#include <e32test.h>
#include <e32uid.h>
#include <f32file.h>
#include <e32math.h>
#include <e32des16.h>
#include "sqlite3.h"


static RTest TheTest(_L("tsqlitesecure_defect"));
static RFs	TheFs;

static sqlite3* TheDb = NULL;

const char* KTestDir = "c:\\test\\";
const char* KTestDb  = "c:\\test\\tsqlitesecure_defect.db";

static void DeleteFile(const char* aFileName)
	{
	TFileName fname;
	fname.Copy(TPtrC8((const TUint8*)aFileName));
	(void)TheFs.Delete(fname);
	}

/**
 * Creates the database file and the directory that the test file will be stored.
 */
static void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);
	
	err = TheFs.ShareAuto();
	TheTest(err == KErrNone);

    TFileName testDir;
    testDir.Copy(TPtrC8((const TUint8*)KTestDir));
	err = TheFs.MkDir(testDir);
	TheTest(err == KErrNone || err == KErrAlreadyExists);
	
	TFileName fname;
	fname.Copy(TPtrC8((const TUint8*)KTestDb));
	(void)TheFs.Delete(fname);
	}
/**
 * Closes the database and erases the database file, but not the directory.
 */
static void DestroyTestEnv()
	{
	if(TheDb)
		{
		(void)sqlite3_close(TheDb);
		TheDb = 0;
		}
	if(TheFs.Handle() != KNullHandle)
		{
		DeleteFile(KTestDb);
		}
	TheFs.Close();
	}


//
//Test macros and functions

static void PrintErrMsg()
	{
	TBuf<256> buf;
	if(TheDb)
		{
		const char* msg = sqlite3_errmsg(TheDb);	
		buf.Copy(TPtrC8((const TUint8*)msg));
		TheTest.Printf(_L("*** Db1 err msg: \"%S\"\r\n"), &buf);
		}
	}

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Line %d, \"TEST\" expression evaluated to false\r\n"), aLine);
		PrintErrMsg();
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Line %d, Expected result: %d, got: %d\r\n"), aLine, aExpected, aValue);
		PrintErrMsg();
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//
//         Sqlite3 DLL OOM test         //
//

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4011
@SYMTestCaseDesc		Sqlite OOM test 
						Precondition: none
						A standard OOM test checks the sqlite3 dll for memory leaks documented 
						on the raised defect, to check if the applied fix is working. Before 
						the fix the test was failing with PANIC USER:84 on the second iteration 
						loop on DEF121506()
@SYMTestPriority		Medium
@SYMTestActions			Sqlite OOM test - 
						Opens the database file.						
						Calls sqlite3_prepare16_v2()
						Closes database
						Checks memory leaks
						Repeats the above indefinitely until SQLITE_OK
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF121506
*/
void DEF121506()
	{
	TheTest.Printf(_L("Iteration: \r\n"));
	for (TInt it = 1;  ; ++it)
		{
		TheTest.Printf(_L("%d "), it);
		TInt c1 = User::CountAllocCells();
 		__UHEAP_SETFAIL(RHeap::EDeterministic, it);
		
 		TInt err = sqlite3_open(KTestDb,&TheDb);

		if(err == SQLITE_OK)
			{
	 		sqlite3_stmt* stmt = 0; 
	 		const void* tail = 0;	
			err = sqlite3_prepare16_v2(TheDb, 
						L"CREATE TABLE Sample(Id INTEGER PRIMARY KEY NOT NULL, Name TEXT NOT NULL UNIQUE COLLATE NOCASE);", 
						-1, &stmt, &tail);	
			(void)sqlite3_finalize(stmt);
			}
 		
		(void)sqlite3_close(TheDb);
		TheDb = NULL;
		__UHEAP_RESET;
		
 		TInt c2 = User::CountAllocCells();
 		if (c1 != c2)
			{	
			TheTest.Printf(_L("\r\n*** OOM Test failed\r\n"));
			TEST(EFalse);
			} 		
 		else if (err == SQLITE_OK) 
 			{
 			TheTest.Printf(_L("\r\n*** OOM Test passed\r\n"));
 			break; 			
 			}
 		TEST2(err, SQLITE_NOMEM);
 		}	
	}


void DoTest()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4011 DEF121506 test"));
	DEF121506();
	}

//

TInt E32Main()
	{
	TheTest.Title();
	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;
	
	CreateTestEnv();
	DoTest();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;

	TheTest.End();	
	TheTest.Close();
	delete tc;
	User::Heap().Check();
	return KErrNone;
	}
