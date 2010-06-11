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


#include "exceptionhandler.h"
using std::string;

TExceptionHandler::TExceptionHandler (int aErrorCode) : iErrorCode(aErrorCode) 
	{
	
	switch(aErrorCode)
		{
		
		case EFileNotPresent:
			iErrorMsg = iFileNotPresent;
			break;

		case EDirNotPresent:
			iErrorMsg = iDirNotPresent;
			break;

		case EFileCannotOpenForRead:
			iErrorMsg = iFileCannotOpenForRead;
			break;

		case EFileCannotOpenForWrite:
			iErrorMsg = iFileCannotOpenForWrite;
			break;

		case EFileFormatError:
			iErrorMsg = iFileFormatError;
			break;

		case EFileErrorWhileReading:
			iErrorMsg = iFileErrorWhileReading;
			break;

		case EFileErrorWhileWriting:
			iErrorMsg = iFileErrorWhileWriting;
			break;

		case EFileCompression:
			iErrorMsg = iFileCompression;
			break;
		
		case ENoValidFile:
			iErrorMsg = iNoValidFile;
			break;

		case EInvalidParameters:
			iErrorMsg = iInvalidParameters;
			break;

		case EInvalidFormat:
			iErrorMsg = iInvalidFormat;
			break;

		case EInvalidOptions: 
			iErrorMsg = iInvalidOptions;
			break;

		case EMandatoryOptions:
			iErrorMsg = iMandatoryOptions;
			break;
		
		case EInvalidOrder:
			iErrorMsg = iInvalidOrder;
			break;

		case EParameterRepeat:
			iErrorMsg = iParameterRepeat;
			break;
		
		case ESystemError: 
			iErrorMsg = iSystemError;
			break;

		default:
			iErrorMsg = iSystemError;
			break;

		};
	}

int TExceptionHandler::GetErrorCode() 
	{ 
	return iErrorCode; 
	}

string TExceptionHandler::GetErrorString() 
	{ 
	return iErrorMsg; 
	}

// cmdparser error strings
string TExceptionHandler::iFileNotPresent		= "File specified is not present.";
string TExceptionHandler::iDirNotPresent		= "Directory specified is not present.";

// filecontents error strings
string TExceptionHandler::iFileCannotOpenForRead	= "Cannot open file for read.";
string TExceptionHandler::iFileCannotOpenForWrite	= "Cannot open file for write.";
string TExceptionHandler::iFileFormatError			= "File format incorrect.";
string TExceptionHandler::iFileErrorWhileReading	= "Error while reading file.";
string TExceptionHandler::iFileErrorWhileWriting	= "Error while writing file.";

// siscompressed error strings
string TExceptionHandler::iFileCompression	= "Error in compression.";

// metagenerator error strings
string TExceptionHandler::iNoValidFile		= "No valid SIS file available for parsing.";

// cmdparser error string
string TExceptionHandler::iInvalidParameters= "Invalid parameters specified.";
string TExceptionHandler::iInvalidFormat	= "Invalid format specified.";
string TExceptionHandler::iInvalidOptions	= "Invalid options specified.";
string TExceptionHandler::iMandatoryOptions	= "Mandatory options not specified.";
string TExceptionHandler::iInvalidOrder		= "Ordering of options incorrect.";
string TExceptionHandler::iParameterRepeat	= "Parameter specified more than once.";

// general error
string TExceptionHandler::iSystemError		= "System Error.";