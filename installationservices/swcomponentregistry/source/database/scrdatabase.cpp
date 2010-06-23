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
* Implements the SCR Data Layer API which performs all interaction with the underlying database.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#include "scrdatabase.h"
#include "usiflog.h"
// Userinclude headers are used instead of the standard ones as SCR uses a custom version of SQLite
#include "sqlite3.h"
#include "SqliteSecure.h"
#include <stdio.h> 

using namespace Usif;
 
namespace Usif
	{
	 class CDatabaseImplementation : public CBase
	/**
	 	The database implementation class encapsulates the implementation of CDatabase from its interface.
	 */
		{
		friend class CDatabase;
	public:
		static CDatabaseImplementation* NewL(RFile& aDatabaseFile, RFile& aJournalFile);
		~CDatabaseImplementation();	
		/**
			Maps the given SQL database error into a system wide symbian specific error code.
			@leave If the given error code does not map to KErrNone, leaves with a system wide
			       symbian error code.
		 */
		void CheckSqlErrCodeL(TInt aErr);
		
		/**
			@return The extended error codes that provide more detailed information about errors.
		 */
		TInt ErrorCode();
				
	private:
		/**
			@return The English-language text that describes the most recent error.
		 */
		TPtrC8 ErrorMessage();
		
	private:
		CDatabaseImplementation();
		void ConstructorL(RFile& aDatabaseFile, RFile& aJournalFile);
		
	private:
		sqlite3* iDbHandle;		 ///< The database handle, owned by CDatabaseImplementation.		
		};
		
	 
	class CStatementImplementation : public CBase
	/**
		This class encapsulates the implementation details of SQL statement object
		which contains the result sets of queries.
	 */
		{
	public:
		static CStatementImplementation* NewLC(sqlite3_stmt* aStmtHandle);
		~CStatementImplementation();
		inline sqlite3_stmt* Handle() const;
		
	private:
		CStatementImplementation(sqlite3_stmt* aStmtHandle);
		
	private:
		sqlite3_stmt* iStmtHandle; //SQL statement handle
		};	
	
	}// End of namespace Usif

//
// CDatabaseImplementation
// 

CDatabaseImplementation::CDatabaseImplementation()
// Constructor
	{
	// empty
	}

CDatabaseImplementation::~CDatabaseImplementation()
// Destructor
	{
	// Close the db handle
	TInt err = sqlite3_close(iDbHandle);
	if(err != SQLITE_OK)
		{
		TPtrC8 ptrErrMsg(ErrorMessage());
		DEBUG_PRINTF3(_L8("Failed to close the database handle. Error code:%d, Error message:%S"), ErrorCode(), &ptrErrMsg);
		}
	}

CDatabaseImplementation* CDatabaseImplementation::NewL(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	CDatabaseImplementation* self = new(ELeave) CDatabaseImplementation();
	CleanupStack::PushL(self);
	self->ConstructorL(aDatabaseFile, aJournalFile);
	CleanupStack::Pop(self);
	return self;
	}

void CDatabaseImplementation::ConstructorL(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	TSqliteSecure sqliteSecure; // Utility class to open sqlite database via file handles. Implemented by Symbian.
	TInt err = sqliteSecure.Open(aDatabaseFile, aJournalFile, &iDbHandle);
	
	// Check the returned error code
	CheckSqlErrCodeL(err);
	
	// If comes here, means the database file has been opened successfully
	// Now, enable the extended result codes feature of SQLite. In SQLite, this feature is 
	// disabled by default for historical compatibility.
	err = sqlite3_extended_result_codes(iDbHandle, 0);
	if(SQLITE_OK != err)
		{
		TPtrC8 ptrErrMsg(ErrorMessage());
		DEBUG_PRINTF3(_L8("Failed to activate the extended error mechanism. Error code:%d, Error message:%S"), ErrorCode(), &ptrErrMsg);
		}
	}

TPtrC8 CDatabaseImplementation::ErrorMessage()
	{
	const char *errMsg = sqlite3_errmsg(iDbHandle);
	TPtrC8 errPtr((TUint8*)errMsg);
	return errPtr;
	}

TInt CDatabaseImplementation::ErrorCode()
	{
	return sqlite3_errcode(iDbHandle);
	}


void CDatabaseImplementation::CheckSqlErrCodeL(TInt aErr)
	{
	TInt symbianErrCode(0);
	
	switch(aErr)
		{
	case SQLITE_OK:
	case SQLITE_DONE:		// sqlite3_step() has finished executing
	case SQLITE_ROW:		// sqlite3_step() has another row ready
		symbianErrCode = KErrNone;
		break;
	case SQLITE_NOTADB:     // File opened that is not a database file
	case SQLITE_NOTFOUND:	// Table or record not found
	case SQLITE_EMPTY:      // Database is empty
	case SQLITE_CANTOPEN:   // Unable to open the database file 
		symbianErrCode = KErrNotFound;
		break;
	case SQLITE_CORRUPT:	// The database disk image is malformed 
	case SQLITE_SCHEMA: 	// The database schema has changed
	case SQLITE_FORMAT:     // Auxiliary database format error 
		symbianErrCode = KErrCorrupt;
		break;
	case SQLITE_NOMEM:		// A malloc() failed in sqlite engine
	case SQLITE_IOERR:      // Some kind of disk I/O error occurred
	case SQLITE_FULL:       // there is no space left on the disk, or the database is too big to hold any more information
		symbianErrCode = KErrNoMemory;
		break;
	case SQLITE_BUSY:		// The database file is locked
	case SQLITE_LOCKED:     // A table in the database is locked
		symbianErrCode = KErrInUse;
		break;
	case SQLITE_TOOBIG:		// Too much data for one row of a table 
		symbianErrCode = KErrOverflow;
		break;
	case SQLITE_INTERRUPT:	// Operation terminated by sqlite_interrupt()
		symbianErrCode = KErrCancel;
		break;
	case SQLITE_MISUSE: 	// The library has been used incorrectly
		symbianErrCode = KErrNotSupported;
		break;
	case SQLITE_CONSTRAINT: // Abort due to constraint violation 
		symbianErrCode = KErrAlreadyExists;
		break;
	case SQLITE_ERROR: 		// SQL error or missing database
	case SQLITE_MISMATCH :  // Data type mismatch
	case SQLITE_RANGE: 		// 2nd parameter to sqlite3_bind out of range
		symbianErrCode = KErrArgument;
		break;
	case SQLITE_INTERNAL:   // An internal logic error in SQLite
		symbianErrCode = KErrUnknown;
		break;
	case SQLITE_PERM:      // Access permission denied
		symbianErrCode = KErrAccessDenied;
		break;
	default:
		symbianErrCode = aErr < 0 ? aErr : KErrGeneral; // The porting layer may return Symbian error codes, which are negative
		}// End of switch
	
	if(KErrNone != symbianErrCode)
		{
		if (aErr > 0) // If the error is negative, it does not come from SQLite
			{
			TPtrC8 ptrErrMsg(ErrorMessage());
			DEBUG_PRINTF3(_L8("The SQL engine error code:%d, The SQL engine error message:%S"), ErrorCode(), &ptrErrMsg);
			}
		// Some functions returns standard error codes.
		// Here ErrorCode is called to get the extended error code, if exists.

		User::Leave(symbianErrCode);
		}
	}


//
// CStatementImplementation
// 

CStatementImplementation::CStatementImplementation(sqlite3_stmt* aStmtHandle)
// Constructor
	:iStmtHandle(aStmtHandle)
	{
	// Make sure that the statement handle is never NULL.
	ASSERT(iStmtHandle != NULL);
	}

CStatementImplementation::~CStatementImplementation()
// Destructor
	{
	TInt err = sqlite3_finalize(iStmtHandle);
	if(SQLITE_OK != err)
		{
		DEBUG_PRINTF2(_L8("Failed to finalize the statement object. Error code:%d"), err);
		}
	}

CStatementImplementation* CStatementImplementation::NewLC(sqlite3_stmt* aStmtHandle)
	{
	CStatementImplementation* self = new(ELeave) CStatementImplementation(aStmtHandle);
	CleanupStack::PushL(self);
	return self;
	}

inline sqlite3_stmt* CStatementImplementation::Handle() const
	{
	return iStmtHandle;
	}

//
// CDatabase
// 

CDatabase::CDatabase()
// Constructor
	{
	// Empty
	}

EXPORT_C CDatabase::~CDatabase()
// Destructor
	{
	delete iDbImpl;
	}

EXPORT_C CDatabase* CDatabase::NewL(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	CDatabase *self = CDatabase::NewLC(aDatabaseFile, aJournalFile);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDatabase* CDatabase::NewLC(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	CDatabase *self = new(ELeave) CDatabase();
	CleanupStack::PushL(self);
	self->ConstructL(aDatabaseFile, aJournalFile);
	return self;
	}

void CDatabase::ConstructL(RFile& aDatabaseFile, RFile& aJournalFile)
	{
	iDbImpl = CDatabaseImplementation::NewL(aDatabaseFile, aJournalFile);
	}

EXPORT_C CStatement* CDatabase::PrepareStatementLC(const TDesC& aStatementStr)
	{
	// For statements which start with the SELECT key word, this function creates an sql statement  
	// object and returns it.	
	sqlite3_stmt* stmtHandle(0); // Temporary statement handle
	const void* stmtTail(0);	 // Pointer to unused portion of Sql statement.                         
	TInt err = sqlite3_prepare16_v2(iDbImpl->iDbHandle, aStatementStr.Ptr(), aStatementStr.Size(), &stmtHandle, &stmtTail);
	
	// Check the returned error code
	iDbImpl->CheckSqlErrCodeL(err);
	// Since we expect single statement, stmtTail pointer should be NULL or point to zero.
	if(stmtTail && static_cast <const TUint16*> (stmtTail)[0] != 0)
		{
		err = sqlite3_finalize(stmtHandle);
		iDbImpl->CheckSqlErrCodeL(err);
		DEBUG_PRINTF(_L8("There is a problem with the provided SQL statement. It may contain more "
						 "than one statement. Or It may not be terminated with semicolon. "
						 "Or It may contain a space or invalid char after semicolon."));
		User::Leave(KErrArgument);
		}
	// stmtHandle can be NULL for statements like this: ";"
	if(!stmtHandle)
		{
		DEBUG_PRINTF(_L8("The statement handle is NULL."));
		User::Leave(KErrArgument);
		}
	// The statement object which carries handle to the result set of the sql statement
	CStatementImplementation* stmtImpl = CStatementImplementation::NewLC(stmtHandle);
	CStatement* stmtObj = CStatement::NewL(*this, stmtImpl); 
	CleanupStack::Pop(stmtImpl); // Ownership has been passed to CStatement
	CleanupStack::PushL(stmtObj);

	return stmtObj;	
	}

EXPORT_C TInt CDatabase::LastInsertedIdL()
	{
	TInt retVal = (TInt)sqlite3_last_insert_rowid(iDbImpl->iDbHandle); 
	// it is now expected that row ids in scr will require 64-bit storage, so cast the return value to TInt
	if(retVal <= 0)
		{
		User::Leave(KErrNotFound);
		}
	return retVal;
	}

//
// CStatement
//

CStatement::CStatement(const CDatabase& aDb, CStatementImplementation* aStmtImpl)
// Constructor
	:iDb(aDb), iStmtImpl(aStmtImpl)
	{
	// Empty
	}

EXPORT_C CStatement::~CStatement()
// Destructor
	{
	delete iStmtImpl;
	}

EXPORT_C CStatement* CStatement::NewL(const CDatabase& aDb, CStatementImplementation* aStmtImpl)
	{
	CStatement* self = new(ELeave) CStatement(aDb, aStmtImpl);
	return self;
	}

EXPORT_C TBool CStatement::ProcessNextRowL()
	{
	TInt err = sqlite3_step(iStmtImpl->Handle());
	
	switch(err)
		{
	case SQLITE_ROW: // A new row of data is ready for processing.
		return ETrue;
	
	case SQLITE_DONE: // The statement has finished executing successfully.
		return EFalse;
	default:		 
		iDb.iDbImpl->CheckSqlErrCodeL(err);
		}// End of switch
	return EFalse;
	}

EXPORT_C void CStatement::ExecuteStatementL()
	{
	// If the statement doesn't return any result table, it should normally be executed
	// with sqlite3_exec. However, sqlite does not have a 16-bit version of sqlite3_exec.  
	// Therefore, the execution is made with PrepareStatementLC and ProcessNextRowL functions.
	// Now, execute and check if the function has completed successfully by calling ProcessNextRowL.
	// If the function has failed, ProcessNextRowL will leave with one of the system wide error codes.
	while(ProcessNextRowL())
		{
		// nop
		}
	}

EXPORT_C void CStatement::BindIntL(TInt aParameterIndex, TInt aParameterValue)
	{
	TInt err = sqlite3_bind_int(iStmtImpl->Handle(), aParameterIndex, aParameterValue);
	iDb.iDbImpl->CheckSqlErrCodeL(err);
	}

EXPORT_C void CStatement::BindInt64L(TInt aParameterIndex, TInt64 aParameterValue)
	{
	TInt err = sqlite3_bind_int64(iStmtImpl->Handle(), aParameterIndex, aParameterValue);
	iDb.iDbImpl->CheckSqlErrCodeL(err);
	}

template <class A> void VerifyDescriptorLengthL(const A& aDesc, TUint aMaxDescriptorLength)
	{
	if (aDesc.Length() > aMaxDescriptorLength)
		User::Leave(KErrArgument);	
	}
	
EXPORT_C void CStatement::BindStrL(TInt aParameterIndex, const TDesC &aParameterStr)
	{
    const TInt KMaxInputDescriptorLength = 512; 
	VerifyDescriptorLengthL(aParameterStr, KMaxInputDescriptorLength);	
	TInt err = sqlite3_bind_text16(iStmtImpl->Handle(), aParameterIndex, aParameterStr.Ptr(), aParameterStr.Size(), SQLITE_TRANSIENT);
	// The fifth argument has the value SQLITE_TRANSIENT, it means that SQLite makes its own private copy of the data immediately
	iDb.iDbImpl->CheckSqlErrCodeL(err);
	}

EXPORT_C void CStatement::BindBinaryL(TInt aParameterIndex, const TDesC8 &aParameterStr)
	{
    const TInt KMaxInputDescriptorLength = 512;
    BindBinaryL(aParameterIndex, aParameterStr, KMaxInputDescriptorLength);
    }

EXPORT_C void CStatement::BindBinaryL(TInt aParameterIndex, const TDesC8 &aParameterStr, TUint aCustomLength)
    {
    VerifyDescriptorLengthL(aParameterStr, aCustomLength);
    TInt err = sqlite3_bind_blob(iStmtImpl->Handle(), aParameterIndex, reinterpret_cast<const char *>(aParameterStr.Ptr()), aParameterStr.Size(), SQLITE_TRANSIENT);
    iDb.iDbImpl->CheckSqlErrCodeL(err); 
    }

EXPORT_C TPtrC8 CStatement::BinaryColumnL(TInt aColIdx) const
	{
	TInt colType = SQLITE_BLOB;
	ValidateRequestedColumnL(aColIdx, colType);
	
	const TUint8* data = static_cast<const TUint8 *>(sqlite3_column_blob(iStmtImpl->Handle(), aColIdx));
	if(!data)
		iDb.iDbImpl->CheckSqlErrCodeL(iDb.iDbImpl->ErrorCode());

	TInt len = sqlite3_column_bytes(iStmtImpl->Handle(), aColIdx);
	if(!len)
		iDb.iDbImpl->CheckSqlErrCodeL(iDb.iDbImpl->ErrorCode());

	return TPtrC8(data, len);
	}

EXPORT_C TPtrC CStatement::StrColumnL(TInt aColIdx)	const
	{
	TInt colType = SQLITE_TEXT;
	ValidateRequestedColumnL(aColIdx, colType);
	
	// Get the column data from the database
	TUint16* str = (TUint16*)sqlite3_column_text16(iStmtImpl->Handle(), aColIdx);
	if(!str)
		{
		iDb.iDbImpl->CheckSqlErrCodeL(iDb.iDbImpl->ErrorCode());
		}
	// Get the length of the column data
	TInt len = sqlite3_column_bytes16(iStmtImpl->Handle(), aColIdx);
	if(!len)
		{
		iDb.iDbImpl->CheckSqlErrCodeL(iDb.iDbImpl->ErrorCode());
		}
	// Return the value in a pointer descriptor
	// len contains the number of bytes, so divide it by 2 to get the number of chars
	ASSERT(len%2 == 0);
	return TPtrC(str, len/2); 
	}

EXPORT_C TInt64 CStatement::Int64ColumnL(TInt aColIdx) const
	{
	TInt colType = SQLITE_INTEGER;
	ValidateRequestedColumnL(aColIdx, colType);
	return sqlite3_column_int64(iStmtImpl->Handle(), aColIdx);
	}

EXPORT_C TInt CStatement::IntColumnL(TInt aColIdx) const
	{
	TInt colType = SQLITE_INTEGER;
	ValidateRequestedColumnL(aColIdx, colType);
	return sqlite3_column_int(iStmtImpl->Handle(), aColIdx);
	}
 
void CStatement::ValidateRequestedColumnL(TInt aColIdx, TInt& aColumnType) const
// Validate the type and existence of the requested column.
	{
	// Get the number of columns in the result set
	TInt colNum = sqlite3_column_count(iStmtImpl->Handle());
	// Get the type of the column
	TInt colType = sqlite3_column_type(iStmtImpl->Handle(), aColIdx);
	
	if((aColIdx < 0 || aColIdx > colNum-1) ||	// if the column index is invalid
	   (aColumnType != colType && aColumnType != SQLITE_NULL)) // if its type doesn't match with the expected one 
		{// just column range validity is checked in case of SQLITE_NULL.
		DEBUG_PRINTF5(_L("The provided column (Idx=%d, Type=%d) is not valid. Max Column Number=%d. Retrieved Column Type=%d"), aColIdx, aColumnType, colNum, colType);
		User::Leave(KErrArgument);
		}
	aColumnType = colType;
	}

EXPORT_C void CStatement::ResetL()
	{
	TInt err = sqlite3_reset(iStmtImpl->Handle());
	iDb.iDbImpl->CheckSqlErrCodeL(err);
	err = sqlite3_clear_bindings(iStmtImpl->Handle());
	iDb.iDbImpl->CheckSqlErrCodeL(err);
	}

EXPORT_C TBool CStatement::IsFieldNullL(TInt aColIdx) const
	{
	TInt colType = SQLITE_NULL;
	ValidateRequestedColumnL(aColIdx, colType);
	return (SQLITE_NULL == colType) ? ETrue : EFalse;
	}
