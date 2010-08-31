/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Util - Common utility functions.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef UTIL_H
#define UTIL_H

#include "toolsconf.h"
#include <string>
#include "symbiantypes.h"

namespace Util
	{
	DllExport std::string wstring2string (const std::wstring& aWide);
	std::wstring string2wstring (const std::string& aNarrow);
	std::wstring string2wstring (const char* aNarrow);
	DllExport int WideCharToInteger(const wchar_t* aWideChar);
	DllExport const std::wstring IntegerToWideString(int aInt);
	TInt64 WideCharToInt64(const wchar_t* aWideChar);
	std::string Base64Encode( const std::string& aData );
	std::string Base64Decode( const std::string& aEncodedData );
	TUint32 Crc32(const void* aPtr, TInt aLength);
	};


#endif // UTIL_H
