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

const char KTestDbName[] = "C:\\private\\10286A26\\tsqlitesecure_main.db";	//10286A26 is the UID of the calling process - 
																			//tsqlitesecure_main.exe.
																			//tsqlitesecure_main.db resides in the caller's
																			//private data cage
const char KTestDbName2[] = "C:\\sys\\tsqlitesecure_main.db";

void DoWriteTest(sqlite3* aDb)
	{
	TInt err = sqlite3_exec(aDb, "CREATE TABLE A(I INTEGER)", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	err = sqlite3_exec(aDb, "BEGIN TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(aDb, "INSERT INTO A VALUES(1);", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(aDb, "INSERT INTO A VALUES(2);INSERT INTO A VALUES(3)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(aDb, "COMMIT TRANSACTION", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	}

void DoReadTest(sqlite3* aDb)
	{
	sqlite3_stmt* stmt = NULL;
  	const char* tail = 0;

	TInt err = sqlite3_prepare(aDb, "SELECT * FROM A", -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)stmt);
	TEST(!tail || User::StringLength((const TUint8*)tail) == 0);

	TInt cnt = 0;
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		++cnt;
		TInt val = sqlite3_column_int(stmt, 0);
		TEST2(val, cnt);
		}
	TEST2(err, SQLITE_DONE);
	TEST2(cnt, 3);//DoWriteTest does three INSERTs
		
	sqlite3_finalize(stmt);
	}

void DoEncodingTest(sqlite3* aDb)
	{
	sqlite3_stmt* stmt = NULL;
  	const char* tail = 0;

	TInt err = sqlite3_prepare(aDb, "PRAGMA encoding", -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	TEST((unsigned int)stmt);
	TEST(!tail || User::StringLength((const TUint8*)tail) == 0);

	err = sqlite3_step(stmt);
	TEST2(err, SQLITE_ROW);
	
	const unsigned char* enc = sqlite3_column_text(stmt, 0);
	TEST(enc != NULL);

	//The encoding should be UTF8
	TPtrC8 ptr(enc);
	err = ptr.Locate(TChar('8'));
	TEST(err >= 0);

	err = sqlite3_step(stmt);
	TEST2(err, SQLITE_DONE);
		
	sqlite3_finalize(stmt);
	}

void DoSecurityTest()
	{
	//It should not be possible to operate with the database file directly. Only through the passed file handles.		
	sqlite3* db = NULL;
	
	TInt err = sqlite3_open(KTestDbName, &db);
	TEST2(err, SQLITE_CANTOPEN);
	
	sqlite3_close(db);
	}

void DoSecurityTest2()
	{
	//It should not be possible to operate with the database file directly. Only through the passed file handles.		
	sqlite3* db = NULL;
	
	TInt err = sqlite3_open(KTestDbName2, &db);
	TEST2(err, SQLITE_CANTOPEN);
	
	sqlite3_close(db);
	}

void DoTest(RFile& aMainDb, RFile& aJournal, const TDesC& /*aProcessParamter*/)	
	{
	sqlite3* db = NULL;
	
	TInt err = TSqliteSecure::Open(aMainDb, aJournal, &db);
	TEST2(err, SQLITE_OK);
	TEST(db != NULL);

	RDebug::Print(_L("### Simple \"database write\" operations\r\n"));
	DoWriteTest(db);

	RDebug::Print(_L("### Simple \"database read\" operations\r\n"));
	DoReadTest(db);

	RDebug::Print(_L("### Check database encoding\r\n"));
	DoEncodingTest(db);
	
	sqlite3_close(db);

	RDebug::Print(_L("### An attempt to open a database in the caller's private datacage\r\n"));
	DoSecurityTest();

	RDebug::Print(_L("### An attempt to open a database in c:\\sys\\ directory \r\n"));
	DoSecurityTest2();
	}
