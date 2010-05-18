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
#include "sqlite3.h"
#include "SqliteSecure.h"
#include "tsqlitesecure_common.h"
//

const TInt KLargeDataBufLen = 131072; //128KB of Data
char TheLargeData[KLargeDataBufLen] = {0};

const TInt KPanicCode = 0x1234;

_LIT(KPanicCategory, "TransFail");

//

static sqlite3* OpenDatabase(RFile& aMainDb, RFile& aJournal)
	{
	sqlite3* db = 0;
	TInt err = TSqliteSecure::Open(aMainDb, aJournal, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != 0);
	return db;
	}
	
//Open the database passed from "tsqlitesecure_main.exe"
//Create a table and insert a single record in the database
//Close the database.
static void InsertRecord(RFile& aMainDb, RFile& aJournal)
	{
	sqlite3* db = OpenDatabase(aMainDb, aJournal);

	TInt err = sqlite3_exec(db, "CREATE TABLE Tbl(I INTEGER, T TEXT)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(db, "INSERT INTO Tbl VALUES(1,'Original_Text')", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	sqlite3_close(db);
	}

//Begin a transaction and execute a UPDATE statement.
//The statement is not finalized.
//The transaction is not committed.
static void UpdateRecord(sqlite3* aDb)
	{
	TEST(aDb != 0);
	
	TInt err = sqlite3_exec(aDb, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	sqlite3_stmt* stmtHandle = NULL;
	err = sqlite3_prepare(aDb, "UPDATE Tbl SET T=:Prm1 WHERE I=1", -1, &stmtHandle, NULL);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_bind_text(stmtHandle, 1, TheLargeData, -1, SQLITE_STATIC);
	TEST2(err, SQLITE_OK);
	
	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		}
	TEST2(err, SQLITE_DONE);
	}

//This function checks if the text column of the record matches the aText parameter value
void CheckRecord(sqlite3* aDb, const char* aText)
	{
	TEST(aDb != 0);
	TEST(aText != 0);
	
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = sqlite3_prepare(aDb, "SELECT * FROM Tbl", -1, &stmtHandle, NULL);
	TEST2(err, SQLITE_OK);
	
	TInt cnt(0);
	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		TInt intVal = sqlite3_column_int(stmtHandle, 0);
		TEST2(intVal, 1);
		const unsigned char* textVal = sqlite3_column_text(stmtHandle, 1);
		TPtrC8 colVal((const TUint8*)textVal);
		TEST2(colVal.Compare(TPtrC8((const TUint8*)aText)), 0);
		cnt++;
		}
	TEST2(err, SQLITE_DONE);
	
	//Check that only 1 record exists in the table
	TEST2(cnt, 1);
	sqlite3_finalize(stmtHandle);
	}

//This function firstly opens a database and creates a table with
//a single record. Next a 128KB buffer was filled with data and an
//attempt is made to update the text column of the existing record 
//with the buffer. However before the transaction is complete the 
//process is panicked.
void FailTransaction(RFile& aMainDb, RFile& aJournal)
	{
	//Open the database, create the table and insert one record. Close the database.
	InsertRecord(aMainDb, aJournal);
	
	//Fill the large buffer for updating later on
	Mem::Fill(TheLargeData, KLargeDataBufLen, TChar('z'));
	TheLargeData[KLargeDataBufLen - 1] = 0;

	//Reopen the database, begin a transaction and update the record without committing the transaction.
	sqlite3* db = OpenDatabase(aMainDb, aJournal);	
	UpdateRecord(db);
	CheckRecord(db, TheLargeData);
	
	//Before the transaction is committed the process is panicked
	User::SetJustInTime(EFalse);
	User::Panic(KPanicCategory, KPanicCode);
	}

//This function re-opens the previously created database. Its then checks
//if the original record is restored to the database due to the failed
//transaction from FailTransaction().
void CheckRecords(RFile& aMainDb, RFile& aJournal)
	{
	sqlite3* db = OpenDatabase(aMainDb, aJournal);
	
	CheckRecord(db, "Original_Text");
	
	sqlite3_close(db);
	}

void DoTest(RFile& aMainDb, RFile& aJournal, const TDesC& aProcessParameter)	
	{
	if(aProcessParameter.Compare(_L("FailTransaction")) == 0)
		{
		RDebug::Print(_L("\r\n"));
		FailTransaction(aMainDb, aJournal);
		}
	else if(aProcessParameter.Compare(_L("CheckRecords")) == 0)
		{
		User::SetJustInTime(ETrue);
		RDebug::Print(_L("\r\n"));
		CheckRecords(aMainDb, aJournal);
		}
	else
		{
		RDebug::Print(_L("This test requires either the 'CrashProcess' or 'CheckRecords' parameter\r\n"));
		TEST(EFalse);
		}
	}
