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
#include <f32file.h>

//

RFs				TheFs;
RTest 			TheTest( _L("tsqlitesecure_main test") );

RFile 			TheMainDb;
RFile 			TheJournal;

TRequestStatus 	TheStatus;
RProcess		TheProcess;

const TInt KPanicCode = 0x1234;

_LIT(KTestDir,  	"C:\\private\\10286A26\\");
_LIT(KMainDbFile, 	"C:\\private\\10286A26\\tsqlitesecure_main.db");
_LIT(KJournalFile,	"C:\\private\\10286A26\\tsqlitesecure_main.db-journal");

//

static void DestroyTestEnv()
	{
	TheFs.Close();
	}

static void DeleteTestFiles()
	{
	TheJournal.Close();
	TheMainDb.Close();
	(void)TheFs.Delete(KJournalFile);
	(void)TheFs.Delete(KMainDbFile);
	}

//
//Test macros and functions
void Check1(TInt aValue, const char* aFile, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TFileName fname;
		fname.Copy(TPtrC8((const TUint8*)aFile));
		TheTest.Printf(_L("*** File \"%S\", Line %d, \"TEST\" expression evaluated to false\r\n"), &fname, aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, const char* aFile, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		TFileName fname;
		fname.Copy(TPtrC8((const TUint8*)aFile));
		TheTest.Printf(_L("*** File \"%S\", Line %d, Expected result: %d, got: %d\r\n"), &fname, aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check1((arg), __FILE__, __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __FILE__, __LINE__)
//

static void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);
	
	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = TheFs.ShareProtected();//In order to call RFile::TransferToProcess(), the file session has to be shared.
	TEST2(err, KErrNone);
	}

//

static void CreateFiles()
	{
	TInt err = TheMainDb.Create(TheFs, KMainDbFile, EFileShareAny | EFileWrite);
	TEST2(err, KErrNone);

	err = TheJournal.Create(TheFs, KJournalFile, EFileShareAny | EFileWrite);
	TEST2(err, KErrNone);
	}

static void OpenFiles()
	{
	TInt err = TheMainDb.Open(TheFs, KMainDbFile, EFileShareAny | EFileWrite);
	TEST2(err, KErrNone);

	err = TheJournal.Open(TheFs, KJournalFile, EFileShareAny | EFileWrite);
	TEST2(err, KErrNone);
	}

static void CloseFiles()
	{
	TheMainDb.Close();
	TheJournal.Close();
	}

static void DeleteFiles()
	{
	TheJournal.Close();
	TInt err = TheFs.Delete(KJournalFile);
	TEST2(err, KErrNone);
	
	TheMainDb.Close();
	err = TheFs.Delete(KMainDbFile);
	TEST2(err, KErrNone);
	}
	
static void CreateProcess(const TDesC& aExeName,const TDesC& aProcessParameter = KNullDesC)
	{
	TInt err = TheProcess.Create(aExeName, aProcessParameter);//aProcessParameter == command line args
	TEST2(err, KErrNone);
	}

static void TransferFileHandles()
	{
	TInt err = TheMainDb.TransferToProcess(TheProcess, 1, 2);
	TEST2(err, KErrNone);
	
	err = TheJournal.TransferToProcess(TheProcess, 3, 4);
    TEST2(err, KErrNone);
	}

static void RunProcess()
	{
	TheProcess.Rendezvous(TheStatus);
	TEST2(TheStatus.Int(), KRequestPending);
	TheProcess.Resume();
	}
	
static TInt WaitProcessToDie()
	{
	User::WaitForRequest(TheStatus);
	TInt rc = TheProcess.ExitReason();
	TheProcess.Close();
	return rc;
	}

//
/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4043
@SYMTestCaseDesc		Simple database operations with a database opened using RFile handles.
						The test creates the main database and journal files, then runs a second
						process and passes the file handles to it. The second process will use the passed
						file handles to open the database and perform database operations like inserting and
						retrieving records. The test also verifies that the second process cannot create a database
						into c:\sys\ directory or in the current process' datacage.
						Tested public API: TSqliteSecure::Open().
@SYMTestPriority		High
@SYMTestActions			Simple database operations with a database opened using RFile handles test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11321, REQ11322, REQ11410, REQ11422
*/
void GeneralOpsTest()
	{
	CreateFiles();
	_LIT(KExeName, "z:\\sys\\bin\\tsqlitesecure_generalops.exe");
	CreateProcess(KExeName);
	TransferFileHandles();
	RunProcess();
	TInt rc = WaitProcessToDie();
	TEST2(rc, KErrNone);
	DeleteFiles();
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4044
@SYMTestCaseDesc		Simple database operations with a database opened using RFile handles.
						The test creates the main database and journal files, then runs a second
						process and passes the file handles to it. The second process will use the passed
						file handles to open the database and perform database operations like inserting and
						retrieving records. The test also verifies that the second process cannot create a database
						into c:\sys\ directory or in the current process' datacage.
						Tested public API: TSqliteSecure::Open16().
@SYMTestPriority		High
@SYMTestActions			Simple database operations with a database opened using RFile handles test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320, REQ11321, REQ11322, REQ11410, REQ11422
*/
void GeneralOpsTest16()
	{
	CreateFiles();
	_LIT(KExeName, "z:\\sys\\bin\\tsqlitesecure_generalops16.exe");
	CreateProcess(KExeName);
	TransferFileHandles();
	RunProcess();
	TInt rc = WaitProcessToDie();
	TEST2(rc, KErrNone);
	DeleteFiles();
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4055
@SYMTestCaseDesc		Journal rollback test using TSqliteSecure. 
						This test call the 2nd process - "tsqlitesecure_rollbacktest.exe" twice.
						Firstly The test creates the main database and journal files. then the 2nd 
						process is called along with the handle passed to it. 
						1) For the first call to the 2nd process the a table and a single record is inserted into the 
						record. An attempt is then made to update the database. However, before the update transaction 
						is complete, the 2nd process is panicked leaving a hot journal
						All the file handles are then closed along with the file server handle. The file handle is 
						then re-opened and is passed to the 2nd process again.
						2) For the second call to the 2nd process the database file is re-opened using TSqliteSecure::Open 
						and then a check is made to make sure that the orginal database is restored back to the database file
@SYMTestPriority		High
@SYMTestActions			Journal rollback test using TSqliteSecure.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11320
*/
void JournalRollbackTest()
	{
	_LIT(KExeName, "z:\\sys\\bin\\tsqlitesecure_rollback.exe");
	
	CreateFiles();
	CreateProcess(KExeName, _L("FailTransaction"));
	TransferFileHandles();
	RunProcess();
	TInt rc = WaitProcessToDie();
	TEST2(rc, KPanicCode);
	
	//Here the RFile and RFs handled are closed and re-opened again
	//this is to make sure that the files are not still locked from the 
	//first call to process 2.
	CloseFiles();
	DestroyTestEnv();
	CreateTestEnv();
	OpenFiles();
	
	CreateProcess(KExeName, _L("CheckRecords"));
	TransferFileHandles();
	RunProcess();
	rc = WaitProcessToDie();
	TEST2(rc, KErrNone);
	DeleteFiles();
	}


void DoTest()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4043 General Operations - 8-bit encoded database"));		
	GeneralOpsTest();

	TheTest.Next( _L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4044 General Operations - 16-bit encoded database"));		
	GeneralOpsTest16();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4055 Journal Rollack Test"));
	JournalRollbackTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
 	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;

	CreateTestEnv();
	DeleteTestFiles();
	DoTest();
	DeleteTestFiles();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	delete tc;

	
	User::Heap().Check();
	return KErrNone;
	}
