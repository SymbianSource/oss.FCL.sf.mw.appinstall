// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "os_symbian_mutex.h"
#include "os_symbian_panic.h"
#include <pls.h>

const TUid KSqliteSecUid = {0x102869fd};//See UID3 in the mmp file - it should be the same
_LIT(KMutexName, "SqliteMutex");

/**
Process local storage ("Pls") structure

@internalComponent
*/
struct TPls
	{
	RMutex	iMutex;
	};

/**
This is a callback function used by the Pls() call to get the process local storage initialized.
The function initializes the iMutex data member of the aPls object.
If the mutex is not created yet, an attempt will be made to create the mutex.
If the mutex creation fails, the calling application will be panic'd.
If the mutex is already created, a handle to the mutex will be opened.

@param aPls A pointer to the process local storage.
@return KErrNone The process local storage has been successfully initialized.

@panic Sqlite 33 If the mutex object creation has failed.
@panic Sqlite 34 If the mutex object open operation has failed.
@panic Sqlite 35 If aPls argument is NULL.
*/
static TInt PlsInitialize(TPls* aPls)
	{
	__SQLITEASSERT_ALWAYS(aPls != 0, ESqlitePanicMutexNullPls);
	TInt err = aPls->iMutex.CreateGlobal(KMutexName);
	__SQLITEASSERT_ALWAYS(err == KErrNone || err == KErrAlreadyExists, ESqlitePanicMutexNullHandle1);
	if(err == KErrAlreadyExists)
		{
		err = aPls->iMutex.OpenGlobal(KMutexName);	
		__SQLITEASSERT_ALWAYS(err == KErrNone, ESqlitePanicMutexNullHandle2);
		}
	return KErrNone;
	}

/**
Initializes class' data member with their default values.
*/
TSqlite3SymbianMutex::TSqlite3SymbianMutex() :
	iLockCounter(0),
	iOwnerThreadId(KMaxTUint64)
	{
	}

/**
This function gets a pointer to the process local storage and calls Wait() on the mutex object, stored
in the process local storage.

That gives the calling thread an exclusive access to some SQLITE shared resource(s).
The calling thread becomes a mutex owner.
If the mutex is already locked by another thread, the calling thread will block until 
the other thread unlocks the mutex.
TSqlite3SymbianMutex::Lock() can be called by the mutex owning thread more than once, even if the mutex is already locked.

@panic Sqlite 35 If the Pls() call returns a NULL pointer to the process local storage (process local storage not allocated).
*/
void TSqlite3SymbianMutex::Lock()
	{
	TPls* pls = ::Pls(KSqliteSecUid, &PlsInitialize);
	__SQLITEASSERT_ALWAYS(pls != 0, ESqlitePanicMutexNullPls);
	pls->iMutex.Wait();
	RThread currThread;
	iOwnerThreadId = currThread.Id();
	++iLockCounter;
	}

/**
Unlocks the mutex. If TSqlite3SymbianMutex::Lock() was called more than once by the owning thread, then the number of 
TSqlite3SymbianMutex::Unlock() calls must eventually match the number of TSqlite3SymbianMutex::Lock() calls.
If there are thread(s) blocked on TSqlite3SymbianMutex::Lock(), after the mutex gets unlocked one of the waiting threads
will be able to lock the mutex and get an exclusive access to the protected resource(s).

@panic Sqlite 31 If the lock counter is <= 0.
@panic Sqlite 32 If Unlock() is called by a thread that is not the current mutex owner.
@panic Sqlite 35 If the process local storage is NULL
*/
void TSqlite3SymbianMutex::Unlock()
	{
	__SQLITEASSERT_ALWAYS(iLockCounter > 0, ESqlitePanicMutexLockCounter);
#ifdef _DEBUG
	RThread currThread;	
	__SQLITEASSERT(iOwnerThreadId == currThread.Id(), ESqlitePanicMutexOwner);
#endif
	--iLockCounter;
	TPls* pls = ::Pls(KSqliteSecUid, &PlsInitialize);
	__SQLITEASSERT_ALWAYS(pls != 0, ESqlitePanicMutexNullPls);
	pls->iMutex.Signal();
	}
