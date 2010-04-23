/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common utility functions.
* @internalComponent
*
*/



#ifndef	UTILS_H
#define	UTILS_H

#include <iostream>

class Utils
	{
	public:
		static std::string GetCurrentDirectory();
		static int HexToInt(const std::string& aHexString);
		static std::string& StringToLower(std::string& aString);
		static bool FileExists(const std::string& aFile);
		static void TrimSpaces(std::string& aString);
	};

#endif // UTILS_H