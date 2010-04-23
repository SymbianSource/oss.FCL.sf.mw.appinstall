/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the unit test steps for the SCR Data Layer.
*
*/


#include "datalayersteps.h"


CScrTestDataLayer::CScrTestDataLayer(CScrDataLayerTestServer&)
// Constructor.
	:	COomTestStep()
	{
	SetTestStepName(KScrDataLayerStep);
	}

CScrTestDataLayer::~CScrTestDataLayer()
// Destructor.
	{
	}

void CScrTestDataLayer::ImplTestStepPreambleL()
	{
	}

void CScrTestDataLayer::ImplTestStepPostambleL()
	{
	}

static const TUint MAX_SCR_ERROR_MESSAGE_LENGTH = 512;

void CScrTestDataLayer::PrintErrorL(const TDesC& aMsg, TInt aErrNum,...)
	{
	VA_LIST list;
	VA_START(list, aErrNum);
	
	RBuf msgBuf;
	msgBuf.CreateL(MAX_SCR_ERROR_MESSAGE_LENGTH);
	msgBuf.CleanupClosePushL();
	msgBuf.AppendFormatList(aMsg, list);
	
	ERR_PRINTF1(msgBuf);
	SetTestStepResult(EFail);

	CleanupStack::PopAndDestroy(&msgBuf);
	User::Leave(aErrNum);
	}


void CScrTestDataLayer::ImplTestStepL()
	{
	INFO_PRINTF1(_L("SCR Data Layer Unit Tests"));
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0001: Open database file"));

	_LIT(KDbFilePath, "c:\\private\\1028634C\\scr.db");
	_LIT(KJournalFilePath, "c:\\private\\1028634C\\scr.db-journal");
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile databaseFile;
	User::LeaveIfError(databaseFile.Open(fs, KDbFilePath, EFileShareAny|EFileWrite));
	CleanupClosePushL(databaseFile);
	RFile journalFile;
	User::LeaveIfError(journalFile.Replace(fs, KJournalFilePath, EFileShareAny|EFileWrite));
	CleanupClosePushL(journalFile);
	CDatabase *db = CDatabase::NewL(databaseFile, journalFile);
	CleanupStack::PushL(db);
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0001.1: Negative test in LastInsertedIdL"));
	TRAPD(err, db->LastInsertedIdL());
	if (err != KErrNotFound)
		PrintErrorL(_L("Unexpected error for LastInsertedIdL when nothing was inserted"), err);
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0002: Insert records"));
	_LIT(KScrTestInsert, "INSERT INTO Components(SoftwareTypeId,InstalledDrivesRefCount,Version,InstallTime) VALUES(?,?,?,?);");
	CStatement* stmt = db->PrepareStatementLC(KScrTestInsert());
	// Insert first record
	const TInt KSoftTypeIdRow1 = 1;
	_LIT(KInstalledDrivesRow1, "123");
	_LIT(KVersionRow1, "1.1.1");
	InsertRecordL(*stmt, KSoftTypeIdRow1, KInstalledDrivesRow1(), KVersionRow1());
	// Insert second record
	const TInt KSoftTypeIdRow2 = 2;
	_LIT(KInstalledDrivesRow2, "456");
	_LIT(KVersionRow2, "1.1.2");
	InsertRecordL(*stmt, KSoftTypeIdRow2, KInstalledDrivesRow2(), KVersionRow2());
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0003: Verify inserted records"));
	_LIT(KScrTestSelect, "SELECT SoftwareTypeId,InstalledDrivesRefCount,Version FROM Components;");
	stmt = db->PrepareStatementLC(KScrTestSelect());
	if(!stmt->ProcessNextRowL())
		PrintErrorL(_L("Result row set contains unexpected number of records (0)!"), KErrGeneral);
	VerifyRecordL(*stmt, KSoftTypeIdRow1, KInstalledDrivesRow1, KVersionRow1);
	if(!stmt->ProcessNextRowL())
		PrintErrorL(_L("Result row set contains unexpected number of records (1)!"), KErrGeneral);
	VerifyRecordL(*stmt, KSoftTypeIdRow2, KInstalledDrivesRow2(), KVersionRow2());
	if(stmt->ProcessNextRowL())
		PrintErrorL(_L("Result row set contains more records than expected ones (2)!"), KErrGeneral);
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0004: Update a record"));
	_LIT(KScrTestUpdate, "UPDATE Components SET Version=? WHERE InstalledDrivesRefCount=?;");
	stmt = db->PrepareStatementLC(KScrTestUpdate());
	_LIT(KNewVersionRow2, "6.7.9");
	stmt->BindStrL(1, KNewVersionRow2());
	stmt->BindStrL(2, KInstalledDrivesRow2());
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0005: Verify updated record"));
	_LIT(KScrTestSelectCond, "SELECT SoftwareTypeId,InstalledDrivesRefCount,Version FROM Components WHERE InstalledDrivesRefCount=?;");
	stmt = db->PrepareStatementLC(KScrTestSelectCond());
	stmt->BindStrL(1, KInstalledDrivesRow2());
	if(!stmt->ProcessNextRowL())
		PrintErrorL(_L("Updated record could not be found!"), KErrGeneral);
	VerifyRecordL(*stmt, KSoftTypeIdRow2, KInstalledDrivesRow2(), KNewVersionRow2());
	CleanupStack::PopAndDestroy(stmt);
		
	INFO_PRINTF1(_L(" SEC_SCR_DataLayer_0006: Delete a record"));
	_LIT(KScrTestDelete, "DELETE FROM Components WHERE InstalledDrivesRefCount=?;");
	stmt = db->PrepareStatementLC(KScrTestDelete());
	stmt->BindStrL(1, KInstalledDrivesRow1());
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0007: Verify deleted record"));
	_LIT(KScrTestSelectVerify, "SELECT * FROM Components WHERE InstalledDrivesRefCount=?;");
	stmt = db->PrepareStatementLC(KScrTestSelectVerify());
	stmt->BindStrL(1, KInstalledDrivesRow1());
	if(stmt->ProcessNextRowL())
		PrintErrorL(_L("Deleted record has been retrieved!"), KErrGeneral);
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0008: Begin a transaction"));
	_LIT(KScrTestBegin, "BEGIN TRANSACTION SCRUnitTest;");
	stmt = db->PrepareStatementLC(KScrTestBegin());
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0009: Insert a record"));
	stmt = db->PrepareStatementLC(KScrTestInsert());
	const TInt KSoftTypeIdRow3 = 1;
	_LIT(KInstalledDrivesRow3, "789");
	_LIT(KVersionRow3, "1.1.3");
	InsertRecordL(*stmt, KSoftTypeIdRow3, KInstalledDrivesRow3(), KVersionRow3());
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0010: Insert second record"));
	stmt = db->PrepareStatementLC(KScrTestInsert());
	const TInt KSoftTypeIdRow4 = 3;
	_LIT(KInstalledDrivesRow4, "001");
	_LIT(KVersionRow4, "0.0.7");
	InsertRecordL(*stmt, KSoftTypeIdRow4, KInstalledDrivesRow4(), KVersionRow4());
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0011: Rollback the transaction"));
	_LIT(KScrTestRollback, "ROLLBACK TRANSACTION SCRUnitTest;");
	stmt = db->PrepareStatementLC(KScrTestRollback());
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0012: Verify rollback"));
	stmt = db->PrepareStatementLC(KScrTestSelectVerify());
	stmt->BindStrL(1, KInstalledDrivesRow3());
	if(stmt->ProcessNextRowL())
		PrintErrorL(_L("First record inserted  has been retrieved! It should have been rolled back!"), KErrGeneral);
	stmt->ResetL();
	stmt->BindStrL(1, KInstalledDrivesRow4());
	if(stmt->ProcessNextRowL())
		PrintErrorL(_L("Second record inserted  has been retrieved! It should have been rolled back!"), KErrGeneral);
	CleanupStack::PopAndDestroy(stmt);
	
	ExecuteBadStatementL(*db, 13, _L("INSERT INTO Components;"));
	ExecuteBadStatementL(*db, 14, _L(";"));
	ExecuteBadStatementL(*db, 15, _L("SELECT * FROM Components;SELECT * FROM Files;"));
		
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0016: Bind to a column which does not exist"));
	stmt = db->PrepareStatementLC(KScrTestInsert());
	TRAP(err, stmt->BindIntL(5, 1)); // There is no 5th column
	if(err != KErrArgument)
		PrintErrorL(_L("Unexpected error returned with binding to a non-existing column!"), err);
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0017: Retrieve the value of a column which does not exist"));
	stmt = db->PrepareStatementLC(KScrTestSelect());
	if(!stmt->ProcessNextRowL())
			PrintErrorL(_L("The database is empty!"), KErrGeneral);
	TRAP(err,TPtrC ptrColX(stmt->StrColumnL(5))); // There is no 5th column
	if(err != KErrArgument)
		PrintErrorL(_L("Unexpected error returned with retrieving a non-existing column!"), err);
	
	TRAP(err,TPtrC ptrColX(stmt->StrColumnL(-1))); // Check negative value
	if(err != KErrArgument)
		PrintErrorL(_L("Unexpected error returned with retrieving a negative column!"), err);	
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0018: Retrieve the value of a column whose type doesn't match"));
	TRAP(err,TPtrC ptrCol0(stmt->StrColumnL(0))); // Column0 is integer
	if(err != KErrArgument)
		PrintErrorL(_L("Unexpected error returned with retrieving the type mismatched column!"), err);
	CleanupStack::PopAndDestroy(stmt);
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0019: Insert a record - attempt injecting raw SQL in parameters"));
	stmt = db->PrepareStatementLC(KScrTestInsert());
	const TInt KSoftTypeIdRowInject1 = 1;
	_LIT(KRowInject1, "a?b");
	_LIT(KVersionRowInject1, "???");
	InsertRecordL(*stmt, KSoftTypeIdRowInject1, KRowInject1(), KVersionRowInject1());
	
	_LIT(KRowInject2, "';<>!?\\/.,~#:");
	_LIT(KRowInject3, "\n\r\t\b");	
	InsertRecordL(*stmt, KSoftTypeIdRowInject1, KRowInject2(), KRowInject3());
	
	CleanupStack::PopAndDestroy(stmt);	
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0020: Query - attempt injecting raw SQL"));
	_LIT(KScrInjectTestQuery, "SELECT * FROM Components WHERE ComponentId=?;");
	stmt = db->PrepareStatementLC(KScrInjectTestQuery());
	stmt->BindStrL(1, KRowInject1());
	stmt->ProcessNextRowL();
	CleanupStack::PopAndDestroy(stmt);
	
	stmt = db->PrepareStatementLC(KScrInjectTestQuery());
	stmt->BindStrL(1, KRowInject2());
	stmt->ProcessNextRowL();
	CleanupStack::PopAndDestroy(stmt);
	
	stmt = db->PrepareStatementLC(KScrInjectTestQuery());
	stmt->BindStrL(1, KRowInject3());
	stmt->ProcessNextRowL();		
	CleanupStack::PopAndDestroy(stmt);	
	
	_LIT8(K8BitInstalledDrives, "testInstalledDrives");
	_LIT8(K8BitVersion, "testVersion");
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0021: Test 8-bit field support"));
	stmt = db->PrepareStatementLC(KScrTestInsert());
	
	stmt->BindIntL(1, 1);
	stmt->BindBinaryL(2, K8BitInstalledDrives());	
	stmt->BindBinaryL(3, K8BitVersion());
	stmt->BindIntL(4, 0); // install time
	stmt->ExecuteStatementL();
	CleanupStack::PopAndDestroy(stmt);
	// Try and do a select by 8-bit values, followed by their retrieval
	_LIT(KScr8BitTestQuery, "SELECT InstalledDrivesRefCount, ComponentId FROM Components WHERE Version=?;");
	stmt = db->PrepareStatementLC(KScr8BitTestQuery());
	stmt->BindBinaryL(1, K8BitVersion());
	if(!stmt->ProcessNextRowL())
		PrintErrorL(_L("8-bit value SELECT did not match!"), KErrGeneral);
	TPtrC8 ptrCol(stmt->BinaryColumnL(0));
	if (ptrCol != K8BitInstalledDrives)
		PrintErrorL(_L("8-bit values did not match!"), KErrGeneral);
	TRAP(err, stmt->BinaryColumnL(1));
	if (err != KErrArgument)
		PrintErrorL(_L("Expected error on non-binary columnb did not match!"), KErrGeneral);

	CleanupStack::PopAndDestroy(stmt);			
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0022: Try binding huge parameters"));	
	stmt = db->PrepareStatementLC(KScrInjectTestQuery());
  	static TUint charsCount = 1024;
  	_LIT(KTestPrefix, "TestValuePrefix");
  	
  	// Test large localizable property with negative values
  	RBuf propertyValue;
  	propertyValue.CreateL(charsCount * 2 + KTestPrefix().Length());
  	CleanupClosePushL(propertyValue);
  	propertyValue.Copy(KTestPrefix);
  	const TChar testChar(0xFFFD);
  	propertyValue.AppendFill(testChar, charsCount);
	TRAP(err, stmt->BindStrL(2, propertyValue));
	if (err != KErrArgument)
		PrintErrorL(_L("Expected error wasn't returned on large parameter!"), KErrGeneral);
	CleanupStack::PopAndDestroy(&propertyValue);
		
	CleanupStack::PopAndDestroy(4, &databaseFile); // databaseFile, journalFile, db, stmt
	
	INFO_PRINTF1(_L("SEC_SCR_DataLayer_0023: Open and work on a non-database file"));
	CDatabase* nondb(0); 

	_LIT(KScrTestNonDbFile, "c:\\private\\1028634C\\nonscr.db");
	_LIT(KScrTestNonDbFileJournal, "c:\\private\\1028634C\\nonscr.db-journal");
	User::LeaveIfError(databaseFile.Open(fs, KScrTestNonDbFile, EFileShareAny|EFileRead));
	CleanupClosePushL(databaseFile);
	User::LeaveIfError(journalFile.Replace(fs, KScrTestNonDbFileJournal, EFileShareAny|EFileWrite));
	CleanupClosePushL(journalFile);
	nondb = CDatabase::NewL(databaseFile, journalFile);
	CleanupStack::PushL(nondb);
  	stmt = NULL;
  	TRAP(err, stmt = nondb->PrepareStatementLC(KScrTestSelect()));
  	if (stmt) 
  		CleanupStack::PopAndDestroy(stmt);
   	
  	if(err == KErrNoMemory)
  		User::Leave(err);
  	else if(err != KErrNotFound)
  		PrintErrorL(_L("Unexpected error returned with non-database file!"), err);			

	CleanupStack::PopAndDestroy(4, &fs); // fs, databaseFile, journalFile, nondb
	}

void CScrTestDataLayer::InsertRecordL(CStatement& aStmt, TInt aCol1, const TDesC& aCol2, const TDesC& aCol3)
	{
	aStmt.BindIntL(1, aCol1);
	aStmt.BindStrL(2, aCol2);
	aStmt.BindStrL(3, aCol3);
	aStmt.BindIntL(4, aCol1); // install time
	aStmt.ExecuteStatementL();
	aStmt.ResetL();
	}

void CScrTestDataLayer::VerifyRecordL(CStatement& aStmt, TInt aCol1, const TDesC& aCol2, const TDesC& aCol3)
	{
	TInt intCol1 = aStmt.IntColumnL(0);
	TInt64 int64Col1 = aStmt.Int64ColumnL(0); // Due to coverage concerns
	if((intCol1 != aCol1) || (int64Col1 != aCol1))
		{
		ERR_PRINTF1(_L("The integer column does not match with the returned one."));
		SetTestStepResult(EFail);
		User::Leave(KErrGeneral);
		}
	TPtrC ptrCol2(aStmt.StrColumnL(1));
	TPtrC ptrCol3(aStmt.StrColumnL(2));
	if(ptrCol2.Compare(aCol2) || ptrCol3.Compare(aCol3))
		{
		ERR_PRINTF1(_L("The string column does not match with the returned one."));
		SetTestStepResult(EFail);
		User::Leave(KErrGeneral);	
		}
	}

void CScrTestDataLayer::ExecuteBadStatementL(CDatabase &aDb, TInt aTestNum, const TDesC& aStatement)
	{
	INFO_PRINTF3(_L("SEC_SCR_DataLayer_00%d: Run a badly constructed statement (%S)"), aTestNum, &aStatement);
	CStatement	*stmt = NULL;
	TRAPD(err, stmt = aDb.PrepareStatementLC(aStatement); 
		  if(stmt) 
			  CleanupStack::PopAndDestroy(stmt););// end of TRAPD
	if(err != KErrArgument)
			PrintErrorL(_L("The badly constructed statement did not cause the expected err"), err);
	}
