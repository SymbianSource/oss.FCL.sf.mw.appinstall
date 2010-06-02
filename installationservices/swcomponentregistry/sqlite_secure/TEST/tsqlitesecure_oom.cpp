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

// System include files
#include <f32file.h>
#include <e32test.h>
#include "sqlite3.h"
#include "SqliteSecure.h"

TInt TheProcessHandleCount = 0;
TInt TheThreadHandleCount = 0;
TInt TheAllocatedCellsCountB = 0;

RFs TheFs;
RTest TheTest( _L("tsqlitesecure_oom.exe") );
RFile TheMainFile, TheJournalFile;

_LIT(KTestDir,  "C:\\test\\");
_LIT(KMainFile, "C:\\test\\tsqlitesecure_oom.db");
_LIT(KJournalFile, "C:\\test\\tsqlitesecure_oom.db-journal");
const char* KTestDb = "C:\\test\\tsqlitesecure_oom2.db";
_LIT(KTestDb2,        "C:\\test\\tsqlitesecure_oom2.db");

#ifdef _DEBUG
const TInt KBurstRate = 100;
#endif

enum TOpenType
	{
	EOpenSecure,
	EOpenSecure16,
	EOpen
	};
//
	
static void DeleteTestFiles()
	{
	TheJournalFile.Close();
	TheMainFile.Close();
	TheFs.Delete(KMainFile);
	TheFs.Delete(KJournalFile);
	TheFs.Delete(KTestDb2);
	}

//
//Test macros and functions
LOCAL_C void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Line %d, \"TEST\" expression evaluated to false\r\n"), aLine);
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Line %d, Expected result: %d, got: %d\r\n"), aLine, aExpected, aValue);
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

//

static void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	// make the paths (it will be used for test files)
	err = TheFs.MkDir( KTestDir );
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

static void DestroyTestEnv()
	{
	TheFs.Close();
	}

static void	CreateTestFiles()
	{
	TInt err = TheMainFile.Create(TheFs, KMainFile, EFileRead|EFileWrite);
	TEST2(err, KErrNone);

	err = TheJournalFile.Create(TheFs, KJournalFile, EFileRead|EFileWrite);
	TEST2(err, KErrNone);
	}

//

void MarkHandles()
	{
	RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
	}

void MarkAllocatedCells()
	{
	TheAllocatedCellsCountB = User::CountAllocCells();
	}

void CheckAllocatedCells()
	{
	TInt allocatedCellsCountE = User::CountAllocCells();
	TEST2(allocatedCellsCountE, TheAllocatedCellsCountB);
	}

void CheckHandles()
	{
#if defined __WINSCW__ && defined _DEBUG
	static TBool firstCalled = ETrue;
#endif
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	//This is needed due to the global variable RMutex object allocated using the Process localised 
	//storage (PLS) for emulator builds. In this case the mutex object is allocated when its first used 
	//and not freed until the process is closed.Therefore the count is offset to makes sure that resource 
	//handle check later on does not fail due to this problem 
#if defined __WINSCW__ && defined _DEBUG
	if (firstCalled)
		{
		endProcessHandleCount--;
		firstCalled = EFalse;
		}
#endif
	
	TEST2(TheProcessHandleCount, endProcessHandleCount);
	TEST2(TheThreadHandleCount, endThreadHandleCount);
	}

//
//TSqliteSecure - OOM test.
//Either TSqliteSecure::Open() or TSqliteSecure::Open16() function is tested in OOM simulation loop.
//The expectation is that if the iteration fails with SQLITE_NOMEM, no memory leak will occur
void DoOpenDatabaseOOMTest(TOpenType aOpenType)
	{
	sqlite3* dbHandle = 0; 
	TInt err = SQLITE_NOMEM;
	TInt failingAllocationNo = 0;
	
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == SQLITE_NOMEM)
		{
		MarkHandles();
		MarkAllocatedCells();

		++failingAllocationNo;
		TheTest.Printf(_L(" %d"), failingAllocationNo);
		
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);
		
		switch(aOpenType)
			{
			case EOpenSecure:
				err = TSqliteSecure::Open(TheMainFile, TheJournalFile, &dbHandle);
				break;
			case EOpenSecure16:
				err = TSqliteSecure::Open16(TheMainFile, TheJournalFile, &dbHandle);
				break;
			case EOpen:
				err = sqlite3_open(KTestDb, &dbHandle);
				break;
			default:
				TEST(0);
				break;
			}

		(void)sqlite3_close(dbHandle);
	
		if(err != SQLITE_NOMEM)
			{
			TEST2(err, KErrNone);
			}
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4024
@SYMTestCaseDesc		TSqliteSecure::Open16() OOM test
						This test executes the TSqliteSecure::Open16() in a OOM simulation loop. 
						In the event of a SQLITE_NOMEM error, it checks that no memory or  
						resource leak will occur				
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open16() function OOM test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/

void Open16DatabaseOOMTest()
	{
	DoOpenDatabaseOOMTest(EOpenSecure16);
	}


/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4025
@SYMTestCaseDesc		TSqliteSecure::Open() OOM test
						This test executes the TSqliteSecure::Open() in a OOM simulation loop. 
						In the event of a SQLITE_NOMEM error, it checks that no memory or 
						resource leak will occur
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open() OOM test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/
void OpenDatabaseOOMTest()
	{
	DoOpenDatabaseOOMTest(EOpenSecure);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4056
@SYMTestCaseDesc		Test for the fix of DEF132008 SQLite Secure 3 API, os_symbian_secure_mt.cpp, DoOpenFile(), KErrNoMemory
						This test executes the sqlite3_open() in a OOM simulation loop. 
						In the event of a SQLITE_NOMEM error, it checks that no memory leak will occur. 
						Before the fix, the implementation of "database open" API inside the OS porting layer
						would make an attempt to create the database. If the attempt fails, whatever is the error, the
						OS porting layer would try to open the database. If the "create database" operation failed with
						KErrNoMemory, that error would have been hidden by the "database open" call.
@SYMTestPriority		High
@SYMTestActions			Test for the fix of DEF132008 SQLite Secure 3 API, os_symbian_secure_mt.cpp, DoOpenFile(), KErrNoMemory
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF121506
*/
void DEF132008()
	{
	(void)TheFs.Delete(KTestDb2);
	DoOpenDatabaseOOMTest(EOpen);
	(void)TheFs.Delete(KTestDb2);
	}

//

LOCAL_C void DoTests()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4024 OOM Test for TSqliteSecure::Open16()"));
	Open16DatabaseOOMTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4025 OOM Test for TSqliteSecure::Open()"));
	OpenDatabaseOOMTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4056 SQLite Secure 3 API, os_symbian_secure_mt.cpp, DoOpenFile(), KErrNoMemory"));
	DEF132008();
	}
//

TInt E32Main()
	{
	TheTest.Title();
	
 	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;
	
	CreateTestEnv();
	DeleteTestFiles();
	CreateTestFiles();
	
	DoTests();
	
	DeleteTestFiles();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;


	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
