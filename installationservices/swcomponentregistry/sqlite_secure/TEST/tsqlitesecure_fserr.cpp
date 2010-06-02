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
// tsqlitesecure_oom.cpp
// 
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
RTest TheTest( _L("tsqlitesecure_fserr.exe") );
RFile TheMainFile, TheJournalFile;

_LIT(KTestDir,  "C:\\test\\");
_LIT(KMainFile, "C:\\test\\tsqlitesecure_fserr.db");
_LIT(KJournalFile, "C:\\test\\tsqlitesecure_fserr.db-journal");

enum TOpenType
	{
	EOpen, //To test TSqliteSecure::Open
	EOpen16 //To test TSqliteSecure::Open16
	};
//
TBool FileExists(const TDesC& aFileName)
	{
	TEntry entry;
	return TheFs.Entry(aFileName, entry) == KErrNone;
	}

static void DeleteTestFiles()
	{
	TheJournalFile.Close();
	TheMainFile.Close();
	TheFs.Delete(KMainFile);
	TheFs.Delete(KJournalFile);
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
	TInt err = TheMainFile.Create(TheFs, KMainFile, EFileRead|EFileWrite|EFileShareAny);
	TEST2(err, KErrNone);

	err = TheJournalFile.Create(TheFs, KJournalFile, EFileRead|EFileWrite|EFileShareAny);
	TEST2(err, KErrNone);
	}

//
//This function checks the record previously inserted still exist in the file
TBool CheckRecord(sqlite3* aDbHandle)
	{
	TBool res = EFalse;
	TInt err(0);
	TInt cnt(0);
	sqlite3_stmt* stmtHandle(0); 
	const char* stmtTail(0);
	const unsigned char* val(0);
	
	err = sqlite3_prepare(aDbHandle, "SELECT Name FROM A WHERE Id=1", -1, &stmtHandle, &stmtTail);
	TEST2(err, SQLITE_OK);
	
	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		++cnt;
		val = sqlite3_column_text(stmtHandle, 0);
		res = Mem::Compare(val, 4, (const TUint8*)"Name", 4);
		}
	
	sqlite3_finalize(stmtHandle);
	TEST2(cnt, 1);
	return res == 0;
	}
//
//Performs file I/O failures simulation tests using the open functions for the TSqliteSecure class.
//The test is performed on an empty database file.
void DoEmptyFileIoErrTest(TOpenType aOpenType)
	{
	sqlite3* dbHandle = 0; 
	TInt err = -1;
	for(TInt cnt=1;err!=KErrNone;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
			{
			(void)TheFs.SetErrorCondition(fsError, cnt);
			if (aOpenType == EOpen)
				{
				err = TSqliteSecure::Open(TheMainFile, TheJournalFile, &dbHandle);
				}
			else
				{
				err = TSqliteSecure::Open16(TheMainFile, TheJournalFile, &dbHandle);
				}
			(void)TheFs.SetErrorCondition(KErrNone);
			(void)sqlite3_close(dbHandle);
			dbHandle = 0;
			}
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	sqlite3_close(dbHandle);
	TEST2(err, KErrNone);
	TEST(FileExists(KMainFile));
	TEST(FileExists(KJournalFile));
	TheTest.Printf(_L("\r\n"));
	}

//Performs file I/O failures simulation tests using the open functions for the TSqliteSecure class.
//The test is performed a database file containing a single table and record.
void DoFileIoErrTest(TOpenType aOpenType)
	{
	DeleteTestFiles();
	
	sqlite3* dbHandle = 0;
	
	//Create a single table and insert one record
	//This makes sure the database file is not empty
	TInt err = sqlite3_open16(KMainFile().Ptr(), &dbHandle);
	TEST2(err, SQLITE_OK);
	err = sqlite3_exec(dbHandle, "CREATE TABLE A(Id INTEGER,Name TEXT)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_exec(dbHandle, "INSERT INTO A(Id,Name) VALUES(1,'Name')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_close(dbHandle);
	dbHandle = 0;
	TEST2(err, SQLITE_OK);
	
	err = KErrNotFound;
	for(TInt cnt=1;err!=KErrNone;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		for (TInt fsError=KErrNotFound;fsError>=KErrDied;--fsError)
			{
			
			//Need to re-open the RFiles error because it needs to closed 
			//after each call to sqlite3_close() below
			err = TheMainFile.Open(TheFs, KMainFile, EFileRead|EFileWrite);
			TEST2(err, KErrNone);
			err = TheJournalFile.Replace(TheFs, KJournalFile, EFileRead|EFileWrite);
			TEST2(err, KErrNone);
				
			(void)TheFs.SetErrorCondition(fsError, cnt);
			if (aOpenType == EOpen)
				{
				err = TSqliteSecure::Open(TheMainFile, TheJournalFile, &dbHandle);
				}
			else
				{
				err = TSqliteSecure::Open16(TheMainFile, TheJournalFile, &dbHandle);
				}
			(void)TheFs.SetErrorCondition(KErrNone);
				
			if(err != KErrNone)
				{
				sqlite3_close(dbHandle);//close the database to recover from the last error
				dbHandle = 0;
				//These RFiles handles have to be closed here so the same file can be 
				//re-opened before CheckRecord()
				TheMainFile.Close();
				TheJournalFile.Close();
				
				//Re-open the database for checking
				TEST2(sqlite3_open16(KMainFile().Ptr(),&dbHandle), SQLITE_OK);
				//check the database content is still the same as before the "open" call
				TEST(CheckRecord(dbHandle));
				sqlite3_close(dbHandle);
				dbHandle = 0;
				}
			else
				{
				//check the database content is still the same as before the operation, without closing the database
				TEST(CheckRecord(dbHandle));
				sqlite3_close(dbHandle);
				dbHandle = 0;
				TheMainFile.Close();
				TheJournalFile.Close();
				}
			}
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	TEST2(err, KErrNone);
	//check the database content is the same as before the operation, after reopening the database.
	err = sqlite3_open16(KMainFile().Ptr(), &dbHandle);
	TEST2(err, SQLITE_OK);
	TEST(CheckRecord(dbHandle));
	sqlite3_close(dbHandle);
	DeleteTestFiles();
	CreateTestFiles();
	TheTest.Printf(_L("\r\n"));
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4020
@SYMTestCaseDesc		I/O Error test using TSqliteSecure::Open() on an Empty file
						The test attempts to open the empty database file using TSqliteSecure::Open() 
						while simulating file I/O failures. At the end of the test the database content is 
						tested and is expected to be the same, when the test succeeds the test verifies that 
						the database file does exist.
@SYMTestPriority		High
@SYMTestActions			I/O Error test using TSqliteSecure::Open() on an Empty file
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/

void OpenEmptyDbTest()
	{
	DoEmptyFileIoErrTest(EOpen);
	}


/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4021
@SYMTestCaseDesc		I/O Error test using TSqliteSecure::Open16() on an Empty file
						The test attempts to open the empty database file using TSqliteSecure::Open16() 
						while simulating file I/O failures. At the end of the test the database content is 
						tested and is expected to be the same, when the test succeeds the test verifies that 
						the database file does exist.
@SYMTestPriority		High
@SYMTestActions			I/O Error test using TSqliteSecure::Open16() on an Empty file
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/
void OpenEmptyDb16Test()
	{
	DoEmptyFileIoErrTest(EOpen16);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4022
@SYMTestCaseDesc		I/O Error test using TSqliteSecure::Open()
						The test creates a test database with one table, inserts one record.
						Then the test attempts to open the database while simulating file I/O failures.
						At the end of the test the database content is tested and is expected to be the same
						as it was before the test. TSqliteSecure::Open() is used in the test.				
@SYMTestPriority		High
@SYMTestActions			I/O Error test using TSqliteSecure::Open()
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/

void OpenDbTest()
	{
	DoFileIoErrTest(EOpen);
	}


/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4023
@SYMTestCaseDesc		I/O Error test using TSqliteSecure::Open16()
						The test creates a test database with one table, inserts one record.
						Then the test attempts to open the database while simulating file I/O failures.
						At the end of the test the database content is tested and is expected to be the same
						as it was before the test. TSqliteSecure::Open16() is used in the test.	
@SYMTestPriority		High
@SYMTestActions			I/O Error test using TSqliteSecure::Open16()
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11322
*/
void OpenDb16Test()
	{
	DoFileIoErrTest(EOpen16);
	}

//

LOCAL_C void DoTests()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4020 I/O Error test using TSqliteSecure::Open() on an Empty file"));
	OpenEmptyDbTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4021 I/O Error test using TSqliteSecure::Open16() on an Empty file"));
	OpenEmptyDb16Test();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4022 I/O Error test using TSqliteSecure::Open()"));
	OpenDbTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4023 I/O Error test using TSqliteSecure::Open16()"));
	OpenDb16Test();
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
