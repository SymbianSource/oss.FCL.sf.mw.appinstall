// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OS_SYMBIAN_PANIC_H
#define OS_SYMBIAN_PANIC_H

#include <e32def.h>

#ifdef _DEBUG
#define 	_SQLITE_ASSERTIONS
#define 	_SQLITE_NOTIFY		//Enable _SQLITE_NOTIFY if you want to get more detailed assert/panic output in _DEBUG mode
#endif

/**
Panic codes - used by the asserts in SQLite OS porting layer and SqliteSecure.cpp.
@internalComponent
*/
enum TSqlitePanicCode
	{
	ESqlitePanicNullOsLayerDataPtr		= 1,
	ESqlitePanicInvalidWAmount 			= 2,
	ESqlitePanicOffset64bit 			= 3,
	ESqlitePanicFhNullPtr				= 8,
	ESqlitePanicFhInvalidType			= 9,
	ESqlitePanicFhInvalidPos			=10,
	ESqlitePanicInvalidOpType			=11,
	ESqlitePanicFhInvalidStr			=12,
	ESqlitePanicFhInvalidData			=13,
	ESqlitePanicInvalidArg				=14,
	ESqlitePanicInvalidRAmount 			=15,
	ESqlitePanicInvalidDrive			=17,
	ESqlitePanicInvalidSectorSize		=18,
	ESqlitePanicInternalError			=19,
	ESqlitePanicLockInvalid				=20,
	ESqlitePanicMutexFailure			=21,
	ESqlitePanicMutexLockCounter 		=31,
	ESqlitePanicMutexOwner 				=32,
	ESqlitePanicMutexNullHandle1		=33,
	ESqlitePanicMutexNullHandle2		=34,
	ESqlitePanicMutexNullPls			=35
	};

/**
Set of useful functions to print diagnostic messages on the console when an error occurs.
@internalComponent
*/
class TSqlitePanicUtil
	{
public:
	static void GetTimeStr(TDes& aWhere);
	static TInt Assert(const TText* aFile, TInt aLine, TInt aPanicCode);
	
private:
	static TPtrC Filename(const TText* aFile);
	
	};

#define __SQLITESTRING(str) _S(str)

/**
This macro should be used when there is a need to panic the SQLite client if "expr" condition is not satisfied.
Works in both debug and release modes.
@internalComponent
*/
#define __SQLITEASSERT_ALWAYS(expr, panicCode)	(void)((expr) || TSqlitePanicUtil::Assert(__SQLITESTRING(__FILE__), __LINE__, panicCode))

//
//       _SQLITE_ASSERTIONS          //
#ifdef _SQLITE_ASSERTIONS

/**
This macro should be used when there is a need to panic the SQLite client if "expr" condition is not satisfied.
Works in only in debug mode. In release mode evaluates to a no-op.
@internalComponent
*/
#define __SQLITEASSERT(expr, panicCode)	(void)((expr) || TSqlitePanicUtil::Assert(__SQLITESTRING(__FILE__), __LINE__, panicCode))

#else

#define __SQLITEASSERT(expr, panicCode) 	void(0)

#endif

//

#endif//OS_SYMBIAN_PANIC_H
