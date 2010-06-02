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

#include <s32file.h>
#include "os_symbian_mutex.h"
#include "os_symbian_panic.h"

/**
Initializes class' data members with their default values.
Creates the mutex and if the operation fails calls User::Exit().
*/
TSqlite3SymbianMutex::TSqlite3SymbianMutex() :
	iLockCounter(0),
	iOwnerThreadId(KMaxTUint64)
	{
	TInt err = iMutex.CreateLocal();
	if(err != KErrNone)
		{
		RDebug::Print(_L("===SQLITE OS porting layer, mutex creation has failed with err=%d.\r\n"), err);
		User::Exit(err);	
		}
	}

/**
Gives the calling thread an exclusive access to some SQLITE shared resource(s).
The calling thread becomes a mutex owner.
If the mutex is already locked by another thread, the calling thread will block until 
the other thread unlocks the mutex.
TSqlite3SymbianMutex::Lock() can be called by the mutex owning thread more than once, even if the mutex is already locked.
*/
void TSqlite3SymbianMutex::Lock()
	{
	iMutex.Wait();
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
*/
void TSqlite3SymbianMutex::Unlock()
	{
	__SQLITEASSERT_ALWAYS(iLockCounter > 0, ESqlitePanicMutexLockCounter);
#ifdef _DEBUG
	RThread currThread;	
	__SQLITEASSERT(iOwnerThreadId == currThread.Id(), ESqlitePanicMutexOwner);
#endif
	--iLockCounter;
	iMutex.Signal();
	}
