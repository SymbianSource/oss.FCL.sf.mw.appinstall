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
// The secure SQlite API.
// 
//

/**
 @file
*/

#include <stdlib.h>
#include "SqliteSecure.h"
#include "os_symbian_panic.h"

_LIT(KJournalSuffix, "-journal");
_LIT(KSecureDbNameFmt, "|%08X%08X%S");
const TInt KSecureDbAddLen = 2 * 8 + 2; //The formatted secure database name is KSecureDbAddLen characters longer than
										//the original database name: 2 * 8 characters for the main and journal file
										//addresses + a character for the "|" symbol + a character for the "\x0".

//The function will construct the "File Handle" database file name using the KSecureDbNameFmt format.
static TInt ConstructSecureDbName(RFile& aMainDb, RFile& aJournal, TFileName& aSecureDbName)
	{
	if(aMainDb.SubSessionHandle() == 0 || aJournal.SubSessionHandle() == 0)
		{
		return KErrBadHandle;	
		}
	TFileName dbName;
	TInt err = aMainDb.FullName(dbName);
	if(err != KErrNone)
		{
		return err;	
		}

	if(dbName.Length() > (KMaxFileName - KSecureDbAddLen))
		{
		return KErrArgument;	
		}

	err = aJournal.FullName(aSecureDbName);//aSecureDbName contains the journal file name now
	if(err != KErrNone)
		{
		return err;	
		}
	
	dbName.Append(KJournalSuffix);
	if(dbName.Compare(aSecureDbName) != 0) //The database name + "-journal" suffix must be exactly the same as the journal name
		{
		return KErrArgument;
		}

	dbName.SetLength(dbName.Length() - KJournalSuffix().Length());
	aSecureDbName.Format(KSecureDbNameFmt, &aMainDb, &aJournal, &dbName);
	aSecureDbName.Append(TChar(0));
	
	return KErrNone;
	}

//aFileName argument is expected to point to UTF16 encoded, zero terminated string.
//The function converts aFileName to UTF8 encoded file name, and stores the UTF8 encoded file name
//to the place pointed by aFileNameDestBuf argument.
//If the UTF8 conversion of the file name failed because the file name is too long or NULL, 
//the function returns EFalse. 
//Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
//aFileNameDestBuf max length must be at least KMaxFileName characters.
//
//aFileNameDestBuf will hold UTF8, non-zero-terminated string
static TBool ConvertFromUnicode(const TDesC& aFileName, TDes8& aFileNameDestBuf)
	{
	char* dest = reinterpret_cast <char*> (const_cast <TUint8*> (aFileNameDestBuf.Ptr()));
	const wchar_t* src = reinterpret_cast <const wchar_t*> (aFileName.Ptr());
	TInt len = wcstombs(dest, src, KMaxFileName);
	//Check the file name length. If it is longer than KMaxFileName characters, then the file name is not valid.
	if(len > 0 && len <= KMaxFileName)
		{
		aFileNameDestBuf.SetLength(len);
		return ETrue;
		}
	return EFalse;
	}

/**
Tells SQLite to use a 16-bit encoded SQLite database, which is already opened and main db and journal file handles
are provided in the aMainDb and aJournal parameters. 
This function should be used if the database is located an a different process' datacage and the current process has no
platsec permissions to create or open a file there. 

@param aMainDb  Main database file handle. aMainDb should be initialized (created or opened) before the call;
@param aJournal Journal file handle. aJournal should be initialized (created or opened) before the call;
@param aHandle	Output parameter. Will be initialized with the database handle if the call completes successfully;

@panic Sqlite 8 In _DEBUG if aHandle is NULL.

@return KErrNone 		The call has completed successfully,
		KErrBadHandle	Either the main db or the journal file handle is 0,
		KErrArgument	The database file name is too long (over KMaxFileName-18 characters),
		KErrArgument	The journal file name is not the database name + the "-journal" suffix,
		KErrArgument	The journal file name is the database name + the "-journal" suffix, but the case is not the same,
					 	Note that the function may leave also with some other database specific or OS specific
					 	error codes (positive or negative).
		
@see KMaxFileName

@internalComponent
*/
EXPORT_C TInt TSqliteSecure::Open16(RFile& aMainDb, RFile& aJournal, sqlite3 **aHandle)
	{
	__SQLITEASSERT(aHandle != NULL, ESqlitePanicFhNullPtr);
	
	*aHandle = NULL;
	
	TFileName secureDbName;
	
	TInt err = ::ConstructSecureDbName(aMainDb, aJournal, secureDbName);
	if(err != KErrNone)
		{
		return err;	//Negative Symbian OS error code
		}
		
	return sqlite3_open16(secureDbName.Ptr(), aHandle); //Zero or positive SQLite error code
 	}

/**
Tells SQLite to use a 8-bit encoded SQLite database, which is already opened and main db and journal file handles
are provided in the aMainDb and aJournal parameters. 
This function should be used if the database is located an a different process' datacage and the current process has no
platsec permissions to create or open a file there. 

@param aMainDb  Main database file handle. aMainDb should be initialized (created or opened) before the call;
@param aJournal Journal file handle. aJournal should be initialized (created or opened) before the call;
@param aHandle	Output parameter. Will be initialized with the database handle if the call completes successfully;

@panic Sqlite 8 In _DEBUG if aHandle is NULL.

@return KErrNone 		The call has completed successfully,
		KErrBadHandle	Either the main db or the journal file handle is 0,
		KErrArgument	The database file name is too long (over KMaxFileName-18 characters),
		KErrArgument	The journal file name is not the database name + the "-journal" suffix,
		KErrArgument	The journal file name is the database name + the "-journal" suffix, but the case is not the same,
					 	Note that the function may leave also with some other database specific or OS specific
					 	error codes (positive or negative).
		
@see KMaxFileName

@internalComponent
*/
EXPORT_C TInt TSqliteSecure::Open(RFile& aMainDb, RFile& aJournal, sqlite3 **aHandle)
	{
	__SQLITEASSERT(aHandle != NULL, ESqlitePanicFhNullPtr);
	
	*aHandle = NULL;
	
	TFileName secureDbName;
	
	TInt err = ::ConstructSecureDbName(aMainDb, aJournal, secureDbName);
	if(err != KErrNone)
		{
		return err;	//Negative Symbian OS error code
		}

	TBuf8<KMaxFileName + 1> fname;
	if(!::ConvertFromUnicode(secureDbName, fname))
		{
		return KErrArgument;	
		}
	fname.Append(TChar(0));		
	
	return sqlite3_open(reinterpret_cast <const char*> (fname.Ptr()), aHandle); //Zero or positive SQLite error code
 	}
