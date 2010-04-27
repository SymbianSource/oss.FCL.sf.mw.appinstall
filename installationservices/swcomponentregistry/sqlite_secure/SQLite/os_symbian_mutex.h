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

#ifndef __OS_SYMBIAN_MUTEX_H__
#define __OS_SYMBIAN_MUTEX_H__

#include <e32std.h>

/**
TSqlite3SymbianMutex class manages one RMutex instance needed for the SQLITE per-thread synchronisation.
Only one global TSqlite3SymbianMutex object shall be defined per process that uses the SQLITE 'C' API. 
For the Emulator builds a proprietary WSD solution is used which is why TSqlite3SymbianMutex only 
contains an RMutex data member for ARMV5 builds.

Mutex creation rules:

	Target hardware builds
		The mutex creation code is located inside TSqlite3SymbianMutex's constructor. 
		If the mutex creation fails, User::Exit() is called with the error code as an argument.

	Emulator builds
		The mutex creation code is located inside TSqlite3SymbianMutex::Lock(). 
		If the mutex creation fails, the calling application will be panic'd.
		
@internalComponent
*/
NONSHARABLE_CLASS(TSqlite3SymbianMutex)
	{
public:	
	TSqlite3SymbianMutex();
	void Lock();
	void Unlock();
	inline TBool IsLocked(TInt aThisThreadOnly) const;
		
private:
	TInt		iLockCounter;
	TThreadId	iOwnerThreadId;
#ifndef __WINSCW__
	RMutex		iMutex;//For Emulator builds a proprietary WSD solution is used.
#endif
	
	};

/**
Returns true if the mutex is already locked.

@param aThisThreadOnly If the parameter is non-zero, then the check is - 
                       "the mutex is already locked and the calling thread id and the owning thread id are the same".
@return True if the mutex is locked, false otherwise
*/
inline TBool TSqlite3SymbianMutex::IsLocked(TInt aThisThreadOnly) const
	{
	RThread currThread;
	return iLockCounter > 0 && (aThisThreadOnly == 0 || iOwnerThreadId == currThread.Id());
	}

#endif//__OS_SYMBIAN_MUTEX_H__
