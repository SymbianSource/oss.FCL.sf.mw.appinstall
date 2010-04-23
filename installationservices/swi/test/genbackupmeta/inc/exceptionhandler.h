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
* For handling exceptions.
* @internalComponent
*
*/



#ifndef	MAKESISMETAEXCEPTION_H
#define	MAKESISMETAEXCEPTION_H

#include <iostream>

class TExceptionHandler : public std::exception
	{
	public:
		TExceptionHandler (int aErrorCode);
		int GetErrorCode();
		std::string GetErrorString();

	private:
		const TExceptionHandler& operator=(const TExceptionHandler&);

	public:
		
		enum TExceptions
			{
			//CmdParam
			EFileNotPresent=-17,
			EDirNotPresent,

			//SISCompression 
			EUnexpectedValue,

			//FileContents 
			EFileCannotOpenForRead,
			EFileCannotOpenForWrite,
			EFileFormatError,
			EFileErrorWhileReading,
			EFileErrorWhileWriting,
			EFileCompression,

			//MetaData 
			ENoValidFile,

			//CmdParam 
			EInvalidParameters,
			EInvalidFormat,
			EInvalidOptions,
			EMandatoryOptions,
			EInvalidOrder,
			EParameterRepeat,

			//General error 
			ESystemError,

			};

	private:

		std::string iErrorMsg;
		int iErrorCode;

		// cmdparser error strings
		static std::string iFileNotPresent;
		static std::string iDirNotPresent;

		// filecontents error strings
		static std::string iFileCannotOpenForRead;
		static std::string iFileCannotOpenForWrite;
		static std::string iFileFormatError;
		static std::string iFileErrorWhileReading;
		static std::string iFileErrorWhileWriting;

		// siscompressed error strings
		static std::string iFileCompression;

		// metagenerator error strings
		static std::string iNoValidFile;

		// cmdparser error string
		static std::string iInvalidParameters;
		static std::string iInvalidFormat;
		static std::string iInvalidOptions;
		static std::string iMandatoryOptions;
		static std::string iInvalidOrder;
		static std::string iParameterRepeat;

		// general error
		static std::string iSystemError;

	};
	

#endif // MAKESISMETAEXCEPTION_H