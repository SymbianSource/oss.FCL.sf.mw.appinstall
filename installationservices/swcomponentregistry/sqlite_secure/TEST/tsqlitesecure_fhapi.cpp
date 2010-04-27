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
#include <bautils.h>
#include <e32math.h>
#include <hal.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"
#include "SqliteSecure.h"

//

static RTest TheTest(_L("tsqlitesecure_fhapi test"));

_LIT(KTestDir,     "c:\\test\\");

_LIT(KTestDbName,  "c:\\test\\tsqlitesecure_fhapi.db");
_LIT(KTestJournal, "c:\\test\\tsqlitesecure_fhapi.db-journal");

_LIT(KFakeJournal, "c:\\test\\tsqlitesecure_fhapi_aaa.db-journal");

_LIT(KTestDbName2, "c:\\test\\A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789ABCDEFGH.db");
_LIT(KTestJournal2,"c:\\test\\A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789A123456789ABCDEFGH.db-journal");

_LIT(KTestDbName3, "c:\\test\\tsqlitesecure_fhapi.db");
_LIT(KTestJournal3,"c:\\test\\TSqliteSecure_Fhapi.db-journal");

_LIT(KTestDbName4,            "c:\\test\\tsqlitesecure_fhapi_4.db");
const char KTestDbName8_4[] = "c:\\test\\tsqlitesecure_fhapi_4.db";

RFs 	TheFs;
RFile 	TheMainDb;
RFile 	TheJournal;

sqlite3* TheDb = 0;

#ifdef _DEBUG
_LIT(KPanicCategory, "Sqlite");
#endif

//

static void DeleteTestFiles()
	{
	TheJournal.Close();
	TheMainDb.Close();
	
	(void)TheFs.Delete(KTestJournal);
	(void)TheFs.Delete(KTestDbName);
	
	(void)TheFs.Delete(KFakeJournal);
	
	(void)TheFs.Delete(KTestJournal2);
	(void)TheFs.Delete(KTestDbName2);
	
	(void)TheFs.Delete(KTestJournal3);
	(void)TheFs.Delete(KTestDbName3);
	
	(void)TheFs.Delete(KTestDbName4);
	}

//

static void PrintSqliteErrMsg()
	{
	if(TheDb)
		{
		const char* msg = sqlite3_errmsg(TheDb);
		TBuf<200> buf;
		buf.Copy(TPtrC8((const TUint8*)msg));
		TheTest.Printf(_L("*** SQLite error message: \"%S\"\r\n"), &buf);
		}
	}

//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Line %d, \"TEST\" expression evaluated to false\r\n"), aLine);
		DeleteTestFiles();
		PrintSqliteErrMsg();
		TheTest(EFalse, aLine);
		}
	}
	
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Line %d, Expected result: %d, got: %d\r\n"), aLine, aExpected, aValue);
		DeleteTestFiles();
		PrintSqliteErrMsg();
		TheTest(EFalse, aLine);
		}
	}
	
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//

static void CreateTestDir()
    {
	TInt err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	err = TheFs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

static void CreateDatabase(const TDesC& aMainDbName, const TDesC& aJournalName)
	{
	(void)TheFs.Delete(KTestDbName);

	TInt err = TheMainDb.Replace(TheFs, aMainDbName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TheJournal.Replace(TheFs, aJournalName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);

	err = sqlite3_exec(TheDb, "CREATE TABLE Tbl(I INTEGER, I2 NOT NULL)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	sqlite3_close(TheDb);
	TheDb = 0;
	TheJournal.Close();
	TheMainDb.Close();
	}

static void OpenDatabase(const TDesC& aMainDbName, const TDesC& aJournalName)
	{
	TInt err = TheMainDb.Open(TheFs, aMainDbName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TheJournal.Open(TheFs, aJournalName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, SQLITE_OK);
	}

static void CloseDatabase()
	{
	sqlite3_close(TheDb);
	TheDb = 0;
	TheJournal.Close();
	TheMainDb.Close();
	}

static void DeleteDatabase(const TDesC& aMainDbName, const TDesC& aJournalName)
	{
	TInt err = TheFs.Delete(aJournalName);
	TEST2(err, KErrNone);
	err = TheFs.Delete(aMainDbName);
	TEST2(err, KErrNone);
	}
	
static void SimpleDbOperations()
	{
	TInt err = sqlite3_exec(TheDb, "CREATE TABLE A(I INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(TheDb, "BEGIN TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(1);", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(2);INSERT INTO A VALUES(3)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "COMMIT TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	}

static void SimpleDbOperations2()
	{
	TInt err = sqlite3_exec(TheDb, "BEGIN TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(4);", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(5)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "COMMIT TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	}

//

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4045
@SYMTestCaseDesc		TSqliteSecure::Open() - negative tests
						The test verifies that TSqliteSecure::Open() will detect and report the following problems:
							- non-initialized RFile handles;
							- the journal name is not the database name + "-journal" suffix;
							- the journal name is the database name + "-journal" suffix, but the case is not the same;
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open() - negative tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11321, REQ11322
*/
void NegativeOpenTests()
	{
	//Passing invalid file handles
	TInt err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrBadHandle);

	//Passing invalid journal file handle
	err = TheMainDb.Replace(TheFs, KTestDbName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrBadHandle);

	TheMainDb.Close();
	(void)TheFs.Delete(KTestDbName);
	
	//Passing invalid main database file handle
	err = TheJournal.Replace(TheFs, KTestJournal, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrBadHandle);
	
	TheJournal.Close();
	(void)TheFs.Delete(KTestJournal);
	
	//The journal file name is not the database file name + "-journal"
	err = TheMainDb.Replace(TheFs, KTestDbName, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TheJournal.Replace(TheFs, KFakeJournal, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrArgument);
	
	TheJournal.Close();
	TheMainDb.Close();
	(void)TheFs.Delete(KFakeJournal);
	(void)TheFs.Delete(KTestDbName);

	//The journal file name is the database file name + "-journal", but the case is not the same
	err = TheMainDb.Replace(TheFs, KTestDbName3, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TheJournal.Replace(TheFs, KTestJournal3, EFileRead | EFileWrite);
	TEST2(err, KErrNone);

	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrArgument);
	
	TheJournal.Close();
	TheMainDb.Close();
	
	(void)TheFs.Delete(KTestJournal3);
	(void)TheFs.Delete(KTestDbName3);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4046
@SYMTestCaseDesc		TSqliteSecure::Open() - very long database name
						The test verifies that TSqliteSecure::Open() is able to work with very long database file names
						(the max possible one that can be created using RFile::Replace()).
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open() - very long database name
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11321
*/
void LongDatabaseNameTest()
	{
	//Passing a long database file name. The max possible one without causing RFile::Replace() to fail.
	TInt err = TheMainDb.Replace(TheFs, KTestDbName2, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TheJournal.Replace(TheFs, KTestJournal2, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	
	err = TSqliteSecure::Open(TheMainDb, TheJournal, &TheDb);
	TEST2(err, KErrNone);

	SimpleDbOperations();
	
	sqlite3_close(TheDb);
	TheDb = 0;
	
	TheJournal.Close();
	TheMainDb.Close();
	
	(void)TheFs.Delete(KTestJournal2);
	(void)TheFs.Delete(KTestDbName2);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4047
@SYMTestCaseDesc		TSqliteSecure::Open() - database names test
						The test verifies that TSqliteSecure::Open() opens without problems databases with names
						containing "-journal" or "-mj" strings. A set of simple database operations is also executed.
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open() - database names test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11321
*/
void DbNamesTest()
	{
	//Database file name with "-journal" string in the name
	_LIT(KDbName1,      "c:\\test\\aaa-journalA.db");
	_LIT(KJournalName1, "c:\\test\\aaa-journalA.db-journal");
	CreateDatabase(KDbName1, KJournalName1);
	OpenDatabase(KDbName1, KJournalName1);
	SimpleDbOperations();
	CloseDatabase();
	DeleteDatabase(KDbName1, KJournalName1);

	//Database file name with "-mj" string in the name
	_LIT(KDbName2,      "c:\\test\\aaa-mj12345678bcd.db");
	_LIT(KJournalName2, "c:\\test\\aaa-mj12345678bcd.db-journal");
	CreateDatabase(KDbName2, KJournalName2);
	OpenDatabase(KDbName2, KJournalName2);
	SimpleDbOperations();
	CloseDatabase();
	DeleteDatabase(KDbName2, KJournalName2);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4048
@SYMTestCaseDesc		Invalid database names test
						The test verifies that the OS porting layer can detect zero main database handle or
						non-numeric main database file handle.
@SYMTestPriority		High
@SYMTestActions			Invalid database names test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11321, REQ11322
*/
void InvDbNamesTest()
	{
	sqlite3* db = 0;
	
	//"File Handle" database file name with zero main db file handle
	const char KDbName1[] = "|0000000012345678c:\\test\\a.db.db";
	TInt err = sqlite3_open(KDbName1, &db);
	TEST2(err, SQLITE_CANTOPEN);
	sqlite3_close(db);
	db = 0;

	//"File Handle" database file name, invalid main db file handle
	const char KDbName2[] = "|tyereweq12345678c:\\test\\a.db.db";
	err = sqlite3_open(KDbName2, &db);
	TEST2(err, SQLITE_CANTOPEN);
	sqlite3_close(db);
	db = 0;
	}

//
//  Panic tests //

#ifdef _DEBUG	

//Panic thread function. 
//It will cast aData parameter to a TFunctor pointer and call it.
//The expectation is that the called function will panic and kill the panic thread.
static TInt ThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TFunctor* obj = reinterpret_cast<TFunctor*> (aData);
	TEST(obj != NULL);
	(*obj)();//call the panic function
	
	delete tc;
	
	return KErrNone;		
	}

//Panic test.
//PanicTest function will create a new thread - panic thread, giving it a pointer to the function which has to
//be executed and the expectation is that the function will panic and kill the panic thread.
//PanicTest function will check the panic thread exit code, exit category and the panic code.
void DoPanicTest(TFunctor& aFunctor, TExitType aExpectedExitType, const TDesC& aExpectedCategory, TInt aExpectedPanicCode)
	{
	RThread thread;
	_LIT(KThreadName,"SqlDbPanicThread");
	TEST2(thread.Create(KThreadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&aFunctor, EOwnerThread), KErrNone);
	
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling

	TEST2(thread.ExitType(), aExpectedExitType);
	TEST(thread.ExitCategory() == aExpectedCategory);
	TEST2(thread.ExitReason(), aExpectedPanicCode);
	
	CLOSE_AND_WAIT(thread);
	}

//Panic when using a very short "file handle" database name
class TSqlite_ShortDbName : public TFunctor
	{
private:		
	virtual void operator()()
		{
		sqlite3* db = 0;
		const char KDbName[] = "|123";
		(void)sqlite3_open(KDbName, &db);
		}
	};
static TSqlite_ShortDbName TheSqlite_ShortDbName;

//Panic when passing a NULL sqlite3** parameter to TSqliteSecure::Open()
class TSqlite_NullSqlitePrm : public TFunctor
	{
private:		
	virtual void operator()()
		{
		(void)TSqliteSecure::Open(TheMainDb, TheJournal, 0);
		}
	};
static TSqlite_NullSqlitePrm TheSqlite_NullSqlitePrm;

//Panic when passing a NULL sqlite3** parameter to TSqliteSecure::Open16()
class TSqlite_NullSqlitePrm16 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		(void)TSqliteSecure::Open16(TheMainDb, TheJournal, 0);
		}
	};
static TSqlite_NullSqlitePrm16 TheSqlite_NullSqlitePrm16;

#endif//_DEBUG
//  Panic tests //
//

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4049
@SYMTestCaseDesc		TSqliteSecure::Open(), TSqliteSecure::Open16() - Panic test
						The test verifies that the Open() methods panic in _DEBUG mode if:
							- the formatted "file handle" database name is too short;
							- the sqlite3** parameter is NULL;
@SYMTestPriority		High
@SYMTestActions			TSqliteSecure::Open(), TSqliteSecure::Open16() - Panic test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11321, REQ11322
*/
void PanicTest()
	{
#ifdef _DEBUG	
	TheTest.Printf(_L("Short \"File Handle\" database file name - panic test\r\n"));
	DoPanicTest(TheSqlite_ShortDbName, EExitPanic, KPanicCategory, 10);

	TheTest.Printf(_L("Null sqlite3** parameter to TSqliteSecure::Open() - panic test\r\n"));
	DoPanicTest(TheSqlite_NullSqlitePrm, EExitPanic, KPanicCategory, 8);

	TheTest.Printf(_L("Null sqlite3** parameter to TSqliteSecure::Open16() - panic test\r\n"));
	DoPanicTest(TheSqlite_NullSqlitePrm16, EExitPanic, KPanicCategory, 8);
#endif//_DEBUG	
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4050
@SYMTestCaseDesc		Master journal test
						The test verifies that if an operation is executed on a "file handle" database and that operation
						requires master journal file to be created, then the operation will fail and no master journal 
						file is created.
@SYMTestPriority		High
@SYMTestActions			Master journal test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11319
*/
void MasterJournalTest()
	{
	//Open a "normal" database
	sqlite3* db = 0;
	TInt err = sqlite3_open(KTestDbName8_4, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != 0);
	
	err = sqlite3_exec(db, "create table A(I INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	//Open a "File Handle" database
	CreateDatabase(KTestDbName, KTestJournal);
	OpenDatabase(KTestDbName, KTestJournal);
	err = sqlite3_exec(TheDb, "create table A(I INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	//Attach
	err = sqlite3_exec(TheDb, "ATTACH 'c:\\test\\tsqlitesecure_fhapi_4.db' AS db2", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	//Perform set of operations that, where the final COMMIT operation requres a master journal to be created
	err = sqlite3_exec(TheDb, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_exec(TheDb, "INSERT INTO A VALUES(1)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_exec(TheDb, "INSERT INTO db2.A VALUES(1)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_exec(TheDb, "COMMIT", 0, 0, 0);
	TEST2(err, SQLITE_CANTOPEN);//The master journal file cannot be opened - expected result

	//Detach
	err = sqlite3_exec(TheDb, "DETACH db2", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	CloseDatabase();
	DeleteDatabase(KTestDbName, KTestJournal);
	sqlite3_close(db);
	(void)TheFs.Delete(KTestDbName4);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4051
@SYMTestCaseDesc		Multi-open database test
						The test verifies that a "file handle" database can be closed and reopened multiple times.
@SYMTestPriority		High
@SYMTestActions			Multi-open database test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11323
*/
void MultiDbOpenTest()
	{
	CreateDatabase(KTestDbName, KTestJournal);
	
	OpenDatabase(KTestDbName, KTestJournal);
	SimpleDbOperations();
	CloseDatabase();
	
	OpenDatabase(KTestDbName, KTestJournal);
	SimpleDbOperations2();
	CloseDatabase();

	DeleteDatabase(KTestDbName, KTestJournal);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4052
@SYMTestCaseDesc		Delete journal test
						The test verifies that at the end of transaction the journal file of a "file handle" database
						won't be deleted. The journal file size is set to 0.
@SYMTestPriority		High
@SYMTestActions			Delete journal test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11324
*/
void ZeroJournalSizeTest()
	{
	CreateDatabase(KTestDbName, KTestJournal);
	
	OpenDatabase(KTestDbName, KTestJournal);
	SimpleDbOperations();
	
	//The SimpleDbOperations() has just completed a transaction.
	//In a non-exclusive mode the journal file is deleted at the end of each transaction.
	//For "file handle"	databases the journal file size is truncated to 0, instead file to be deleted
	TEntry entry;
	TInt err = TheFs.Entry(KTestJournal, entry);
	TEST2(err, KErrNone);
	TEST2(entry.iSize, 0);
	
	CloseDatabase();
	DeleteDatabase(KTestDbName, KTestJournal);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4053
@SYMTestCaseDesc		Temporary files location test
						The test verifies that the temporary database files are not created on the disk.
@SYMTestPriority		High
@SYMTestActions			Temporary files location test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11375
*/
void TempFilesLocationTest()
	{
	//Get the number of the files in the folder, where the "file handle" database will be created
	TFindFile findFile(TheFs);
	CDir* fileNameCol = NULL;
	TInt fileCount1 = 0;
	TInt err = findFile.FindWildByPath(_L("*.*"), &KTestDir, fileNameCol);
	if(err == KErrNone)
		{
		fileCount1 = fileNameCol->Count();
		}
	delete fileNameCol;
	
	CreateDatabase(KTestDbName, KTestJournal);
	OpenDatabase(KTestDbName, KTestJournal);
	
	//If the SQlite is compiled with "TEMP_STORE=1" option and "PRAGMA TEMP_STORE=FILE" is executed,
	//then all temporary files will be created on the disk. "CREATE TEMP TABLE" statement will create a temporary
	//file located in the private datacage of the current process.
	//But SQLite is compiled with the "TEMP_STORE=3" option, in which case all teporary files (except the master
	//and statement journals) are created in memory.
	err = sqlite3_exec(TheDb, "PRAGMA TEMP_STORE=FILE", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "CREATE TEMP TABLE B(I INTEGER); INSERT INTO B VALUES(1)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	//There are two possible places, where the temporary database files might be created: the application datacage or
	//the database directory. Check both.
	
	//Check now the application's datacage for temporary files
	TFileName privatePath;
	err = TheFs.PrivatePath(privatePath);
	TEST2(err, KErrNone);
	//If there are files in the test's private datacage, that's bad. The test should not have created files there.
	fileNameCol = NULL;
	err = findFile.FindWildByPath(_L("*.*"), &privatePath, fileNameCol);
	TEST2(err, KErrNotFound);
	delete fileNameCol;
	
	//Check now the test folder for new (temporary) files.
	fileNameCol = NULL;
	err = findFile.FindWildByPath(_L("*.*"), &KTestDir, fileNameCol);
	TEST2(err, KErrNone);
	TInt fileCount2 = fileNameCol->Count();
	delete fileNameCol;
	TEST(fileCount2 <= (fileCount1 + 2));//The file count at the end should be less or equal the file count at the 
										 //beginning + 2 files(the main db + journal)
	
	CloseDatabase();
	DeleteDatabase(KTestDbName, KTestJournal);
	}

/**
@SYMTestCaseID			PDS-SQLITE3SEC-UT-4054
@SYMTestCaseDesc		Temporary statement journal files test
						The test verifies that when a statement journal file is created for a "file handle" database,
						that file is created in the application's private datacage.
@SYMTestPriority		High
@SYMTestActions			Temporary statement journal files test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ11375
*/
void TempFilesLocationTest2()
	{
	CreateDatabase(KTestDbName, KTestJournal);
	OpenDatabase(KTestDbName, KTestJournal);
	
	TInt err = sqlite3_exec(TheDb, "INSERT INTO Tbl VALUES(1, 1);INSERT INTO Tbl VALUES(2, 2)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	//Active statement
	sqlite3_stmt* stmt = 0;
	err = sqlite3_prepare(TheDb, "SELECT * FROM Tbl", -1, &stmt, 0);
	TEST2(err, SQLITE_OK);
	err = sqlite3_step(stmt);
	TEST2(err, SQLITE_ROW);
	
	//A transaction that will hit a "NOT NULL" constraint at the middle
	err = sqlite3_exec(TheDb, "BEGIN TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "UPDATE Tbl SET I2=11 WHERE I=1", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(TheDb, "UPDATE Tbl SET I2=NULL WHERE I=2", 0, 0, 0);
	TEST2(err, SQLITE_CONSTRAINT);

	//Check now the application's datacage for temporary files. There will be one temporary file.
	TFileName privatePath;
	err = TheFs.PrivatePath(privatePath);
	TEST2(err, KErrNone);
	CDir* fileNameCol = NULL;
	TFindFile findFile(TheFs);
	err = findFile.FindWildByPath(_L("*.*"), &privatePath, fileNameCol);
	TEST2(err, KErrNone);
	TEST2(fileNameCol->Count(), 1);
	for(TInt i=0;i<fileNameCol->Count();++i)
		{
		const TEntry& entry = (*fileNameCol)[i];
		TheTest.Printf(_L("### Dir: \"%S\". Temporary file name: \"%S\"\r\n"), &privatePath, &entry.iName);
		}
	delete fileNameCol;
	
	err = sqlite3_finalize(stmt);
	TEST2(err, SQLITE_OK);	
	
	err = sqlite3_exec(TheDb, "ROLLBACK TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);	
	
	CloseDatabase();
	DeleteDatabase(KTestDbName, KTestJournal);
	}

static void DoTests()
	{
	const char* ver = sqlite3_libversion();
	TBuf<20> buf;
	buf.Copy(TPtrC8((const TUint8*)ver));
	TheTest.Printf(_L("*** SQLite library version: \"%S\"\r\n"), &buf);
	
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4045 TSqliteSecure::Open() negative tests"));
	NegativeOpenTests();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4046 TSqliteSecure::Open() - long database name test"));
	LongDatabaseNameTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4047 TSqliteSecure::Open() - database names test"));
	DbNamesTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4048 Invalid \"file handle\" database names test"));
	InvDbNamesTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4049 Panic test"));
	PanicTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4050 Master journal test"));
	MasterJournalTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4051 Multi-\"Database Open\" operation test"));
	MultiDbOpenTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4052 Zero journal size test"));
	ZeroJournalSizeTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4053 Temp files location test"));
	TempFilesLocationTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-SQLITE3SEC-UT-4054 Temp files location test-2"));
	TempFilesLocationTest2();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);

	CreateTestDir();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();

	TheFs.Close();

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
