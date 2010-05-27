/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef	__SYMBIAN_TYPES_H__
#define	__SYMBIAN_TYPES_H__

/**
@file
@internalComponent
*/

// This file allows a PC-based program to be built in isolation from the emulator run-time

// The basic types are included

#ifdef __TOOLS2_WINDOWS__
	#ifndef __int64
		# define __int64 long long
	#endif
#endif // __TOOLS2_WINDOWS__

#ifdef	_WIN32
typedef						__int64	TInt64;
typedef unsigned			__int64 TUint64;
#endif

#ifdef __TOOLS2_LINUX__
#include <stdint.h>
typedef unsigned long long				TUint64;
typedef int64_t							TInt64;
#endif

typedef			  long		int		TInt32;
typedef	unsigned  long		int		TUint32;
typedef			  short		int		TInt16;
typedef	signed				char	TInt8;
typedef	unsigned  short		int		TUint16;
typedef	unsigned			char	TUint8;
typedef						bool	TBool;

typedef TUint16						TCRC;
typedef						int		TInt;
typedef unsigned			int		TUint;
typedef void						TAny;
typedef unsigned short		int		TText16;
typedef unsigned			char	TText8;

#endif	/* __SYMBIAN_TYPES_H__ */

#ifdef UNICODE
#define STRING std::wstring
#else
#define STRING std::string
#endif // _UNICODE

#define ETrue  true;
#define EFalse false;
