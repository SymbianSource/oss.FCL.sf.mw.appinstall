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
* For parsing contents of a SIS file.
* @internalComponent
*
*/


#include "filecontents.h"
#include "exceptionhandler.h"

#include <fstream>
#include <iostream>

using Swi::Sis::TFieldLength;
using Swi::Sis::TFieldType;

CFileContents::CFileContents(const CFileContents& aContents) :
	iBase (aContents.iCurrent),
	iCurrent (aContents.iCurrent),
	iRemainingLength (aContents.iRemainingLength),
	iOwnBase (false)
	{}

CFileContents::CFileContents (const char* aBuffer, TFieldLength aBufferLength) :
	iBase (aBuffer),
	iCurrent (aBuffer),
	iRemainingLength (aBufferLength),
	iOwnBase (false)
	{}

CFileContents::CFileContents (const char* aFileName) :
	iBase (0),
	iOwnBase (true)
	{
	std::ifstream stream (aFileName, std::ios::in|std::ios::binary);
	if (stream.fail ())
		{
		stream.close ();
		throw TExceptionHandler(TExceptionHandler::EFileCannotOpenForRead);
		}

	stream.seekg (0, std::ios::end);
	std::streampos currentPos = stream.tellg ();
	stream.seekg (0, std::ios::beg);
	if (stream.fail ())
		{
		stream.close ();
		throw TExceptionHandler(TExceptionHandler::EFileErrorWhileReading);
		}

	iRemainingLength = currentPos;

	char* buffer = new char [iRemainingLength];

	stream.read (buffer,iRemainingLength);
	stream.close();

	if (stream.fail ())
		{
		delete[] buffer;
		throw TExceptionHandler(TExceptionHandler::EFileErrorWhileReading);
		}
	iCurrent = iBase = buffer;
	}

CFileContents::~CFileContents ()
	{
	if (iOwnBase)
		{
		delete[] const_cast<char*>(iBase);
		}
	}

void CFileContents::SetRemainingLength (TFieldLength aRemainingLength)
	{
	if (aRemainingLength > iRemainingLength)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength = aRemainingLength;
	}

Swi::Sis::TFieldLength CFileContents::GetRemainingLength () const 
	{ 
	return iRemainingLength; 
	}

void CFileContents::Advance (TFieldLength aOffset)
	{
	if (aOffset > iRemainingLength)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= aOffset;
	iCurrent += aOffset;
	}

TFieldLength CFileContents::AlignedLength ()
	{
	TFieldLength result = iRemainingLength + (iCurrent - iBase);
	int unroundedBytes = result & 0x3;
	int rounding = (4 - unroundedBytes) & 0x3;
	result += rounding;
	return result;
	}

TInt64 CFileContents::GetTInt64 ()
	{
	TInt64 result;
	if (iRemainingLength < sizeof(result))
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= sizeof(result);
	result = *reinterpret_cast<const TInt64*>(iCurrent);
	iCurrent += sizeof(result);

	return result;
	}

TUint32 CFileContents::GetTUint32 ()
	{
	TUint32 result;
	if (iRemainingLength < sizeof(result))
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= sizeof(result);
	result = *reinterpret_cast<const TUint32*>(iCurrent);
	iCurrent += sizeof(result);

	return result;
	}

TUint16 CFileContents::GetTUint16 ()
	{
	TUint16 result;
	if (iRemainingLength < sizeof(result))
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= sizeof(result);
	result = *reinterpret_cast<const TUint16*>(iCurrent);
	iCurrent += sizeof(result);

	return result;
	}

TUint8 CFileContents::GetTUint8 ()
	{
	TUint8 result;
	if (iRemainingLength < sizeof(result))
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= sizeof(result);
	result = *reinterpret_cast<const TUint8*>(iCurrent);
	iCurrent += sizeof(result);

	return result;
	}

TFieldLength CFileContents::GetLength ()
	{
	TFieldLength result;
	TUint32 firstFourBytes = GetTUint32 ();

	if (firstFourBytes & 0x80000000)
		{
		firstFourBytes &= ~0x80000000;

		TUint32 secondFourBytes = GetTUint32 ();

		result = firstFourBytes;
		result <<= 32;
		result |= secondFourBytes;
		}
	else
		{
		result = firstFourBytes;
		}
	return result;
	}

TFieldType CFileContents::GetType ()
	{
	return static_cast<TFieldType>(GetTUint32 ());
	}

TFieldType CFileContents::PeekType ()
	{
	TFieldLength remainingLength = iRemainingLength;
	const char* current = iCurrent;

	TFieldType result = GetType ();

	iCurrent = current;
	iRemainingLength = remainingLength;

	return result;
	}

char* CFileContents::GetByteString (TFieldLength aLength)
	{
	if (aLength > iRemainingLength)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}

	char* result = new char [aLength+1];

	iRemainingLength -= aLength;
	memcpy (result, iCurrent, aLength);
	iCurrent += aLength;
	result[aLength] = '\0';
	
	return result;
	}

const wchar_t*	CFileContents::GetUnicodeString (TFieldLength aLength)
	{
	if (aLength > iRemainingLength)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}

	if (aLength & 1)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}

	wchar_t* result = new wchar_t [aLength/2+1];

	iRemainingLength -= aLength;
	memcpy (result, iCurrent, aLength);
	iCurrent += aLength;
	result [aLength/2] = L'\0';

	return result;
	}

void CFileContents::CopyToFile (const char* aFileName, TFieldLength aLength)
	{
	if (aLength > iRemainingLength)
		{
		throw TExceptionHandler(TExceptionHandler::EFileFormatError);
		}
	iRemainingLength -= aLength;

	std::ofstream* stream = new std::ofstream (aFileName, std::ios::out|std::ios::binary);
	if (stream->fail ())
		{
		stream->close();
		delete stream;
		throw TExceptionHandler(TExceptionHandler::EFileCannotOpenForWrite);
		}

	stream->write (iCurrent, aLength);
	stream->close ();
	if (stream->fail ())
		{
		delete stream;
		throw TExceptionHandler(TExceptionHandler::EFileErrorWhileWriting);
		}
	delete stream;
	}

const char*	CFileContents::GetCurrentAddress () const 
	{ 
	return iCurrent; 
	}
