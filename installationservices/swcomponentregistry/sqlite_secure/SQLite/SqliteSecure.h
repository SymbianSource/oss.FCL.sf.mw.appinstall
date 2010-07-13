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

#ifndef SQLITESECURE_H
#define SQLITESECURE_H

#include <f32file.h>
#include "sqlite3.h"

// Forward declarations
class RFile;

/**
SQLite utility class, that provides public exported methods allowing a database to be opened using 
main database and journal RFile handles to already created or opened database files.
These methods may be practically useful in the case where the database is located in a different process datacage
and the SQLite process does not have platsec permissions to create or open files there.

An example, how a database file can be opened by one process, then the file handle passed to another process and
used there for working with the database:

Proces 1 (the process that can create or open the database files).

@code

_LIT(KProcess2Name, "z:\\sys\\bin\\process2.exe");
RProcess process2;
TInt err = process2.Create(KProcess2Name, KNullDesC);
//check the err

//Create the database files
_LIT(KMainDbFile, 	"C:\\private\\<process1 uid>\\example.db");
RFile mainDb;
err = mainDb.Create(<RFs object>, KMainDbFile, EFileRead | EFileWrite);
//check the err

_LIT(KJournalFile,	"C:\\private\\<process1 uid>\\example.db-journal");
RFile journal;
err = journal.Create(<RFs object>, KJournalFile, EFileRead | EFileWrite);
//check the err

//Transfer to process2 the main db file handle
err = mainDb.TransferToProcess(process2, 1, 2);
//check the err
	
//Transfer to process2 the journal file handle
err = journal.TransferToProcess(process2, 3, 4);
//check the err

//Run process2
....
//Wait process2 to finish
....

//Cleanup
journal.Close()
mainDb.Close();

@endcode

Proces 2 (the process that operates with the already created/opened database files).

@code

//Adopt the main db handle
RFile mainDb;	
TInt err = mainDb.AdoptFromCreator(1, 2);
//check the err
	
//Adopt the journal handle
RFile journal;	
err = journal.AdoptFromCreator(3, 4);
//check the err

//Open the database	
sqlite3* db = NULL;
err = TSqliteSecure::Open(mainDb, journal, &db);
//check the err
//use the database
.....

//Cleanup	
journal.Close();
mainDb.Close();

@endcode

@internalComponent
*/
class TSqliteSecure
	{
public:
	IMPORT_C static TInt Open16(RFile& aMainDb, RFile& aJournal, sqlite3** aHandle);
	IMPORT_C static TInt Open(RFile& aMainDb, RFile& aJournal, sqlite3** aHandle);

	};

#endif	//SQLITESECURE_H
