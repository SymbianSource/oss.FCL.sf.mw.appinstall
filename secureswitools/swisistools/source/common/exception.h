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
* CException - Generic exception handler.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "toolsconf.h"
#include <string>

namespace ExceptionCodes
{
	enum TExceptionCode
			{
			EUnintializedCode = -1,
			ENone,
			EEnvNotSpecified,

			// options exception
			EInvalidArgument=10,
			EMandatoryOption,
			EMutuallyExclusive,
			EFileNotPresent,
			EFileExists,
			
			// xml exceptions
			EWarning=20,
			EError,
			EFatalError,
			EParseError,
			EResetError,
			
			// database exceptions
			ELibraryLoadError=30,
			ESqlArgumentError,
			ESqlNotFoundError,
			ESqlCorrupt,

			// general exceptions
			EConversionError=40,
			};
}

class CException
	{
	public:
		
		/**
			constructs the exception handler object through setting the message and the 
			corresponding code is uninitialized.
		 */
		DllExport CException(const std::string& aMessage);
		
		/**
			constructs the exception handler object through setting the code and the 
			corresponding message is empty.
		 */
		DllExport CException(int aCode);
		
		/**
			constructs the exception handler object through setting the message and the 
			corresponding code.
		 */
		DllExport CException(const std::string& aMessage, int aCode);

		/**
			constructs the exception handler object through setting the message and the 
			corresponding code.
		 */
		DllExport CException(const char* aMessage, int aCode);
		
		/**
			Retrieve the message corresponding to the exception handler.
		 */
		DllExport const std::string& GetMessageA() const;

		/**
			Retrieve the code corresponding to the exception handler.
		 */
		DllExport int GetCode() const;

	protected:
		std::string iMessage;
		int iCode;
	
	protected:
		static std::string EmptyMessage;

	};

#endif // EXCEPTION_H
