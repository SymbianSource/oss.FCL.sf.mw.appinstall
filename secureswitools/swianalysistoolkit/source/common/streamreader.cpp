/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "streamreader.h"
#include "options.h"
#include "ucmp.h"
#include "utf8.h"

StreamReader::StreamReader(istream& aFile)
			:iFileStream(&aFile),
			 iBuf(NULL),
			 iCurrentPos(NULL),
			 iLength(0),
			 iStreamOwnership(false)
	{
	}


StreamReader::StreamReader(const char* aRegFile)
			:iBuf (0) ,
			 iCurrentPos (0),
			 iStreamOwnership(true)
	{
	iFileStream = new ifstream(aRegFile, ios::binary );
	}

bool StreamReader::Initialise()
	{
	// get length of file:
	iFileStream->seekg (0, ios::end);
	iLength = iFileStream->tellg();
	
	//checks if file is empty
	if(!iLength)
		{
		return true;
		}

	iFileStream->seekg (0, ios::beg);
	// allocate memory:
	iBuf = new char [iLength];
	iCurrentPos = iBuf;
	// read data as a block:
	iFileStream->read (iCurrentPos,iLength);

	return false;
	}


unsigned int StreamReader::ReadInt32()
	{
	int i = 0;
	if ((iLength - (iCurrentPos - iBuf)) < 4)
		{
		throw EInvalidFile;
		}
	unsigned int result = (iCurrentPos[i] & 0x000000FF) + ((iCurrentPos[i+1] << 8) & 0x0000FF00) + 
		((iCurrentPos[i+2] << 16) & 0x00FF0000) + ((iCurrentPos[i+3] << 24) & 0xFF000000) ;
	
	iCurrentPos += 4;
	return result;
	}

unsigned short int StreamReader::ReadInt16()
	{
	int i = 0;
	if ((iLength - (iCurrentPos - iBuf)) < 2)
		{
		throw EInvalidFile;
		}
	unsigned short int result = (iCurrentPos[i] & 0x00FF) 
		+ ((iCurrentPos[i+1] << 8) & 0xFF00);

	iCurrentPos += 2;
	
	return result;
	}

unsigned char StreamReader::ReadInt8()
	{
	int i = 0;
	if ((iLength - (iCurrentPos - iBuf)) < 1)
		{
		throw EInvalidFile;
		}
	unsigned char result = iCurrentPos[i];
	iCurrentPos += 1;

	return result;
	}
	

unsigned char StreamReader::Peek()
	{
	unsigned char result = ReadInt8();
	iCurrentPos -= 1;
	return result;
	}


char* StreamReader::ReadDescriptor() 
	{
	bool isWide = false;
	int len = GetDescriptorInfo(isWide);
	
	// allocate a buffer big enough to hold this stuff and
	// a null at the end
	char *result = NULL;
	
	TInt32 bytesRead = 0;

	if(isWide) 
		{
		wchar_t* buffer = new wchar_t[len + 1];
		TMemoryUnicodeSink unicodeTarget(buffer);

		TUnicodeExpander converter;

		converter.Expand(unicodeTarget, (const TUint8*)iCurrentPos, len, KMaxTInt, NULL, &bytesRead);

		buffer[len] = 0;

		// UTF16toUTF8Conversion allocates memory
		// This memory will be freed by the caller of this function
		result = UTF16toUTF8Convert(buffer, len);

		delete []buffer;
		} 
	else 
		{
		// This memory will be freed by the caller of this function
		result = new char[len + 2];
		memcpy(result, iCurrentPos, len);

		bytesRead = len;
		result[len] = 0;
		}

	iCurrentPos += bytesRead;
	return result;
	}


int StreamReader::GetDescriptorInfo(bool& aIsWide, const bool &aConstReadPtr) 
	{
	int len;
	unsigned char cardinality = Peek();
	if ((cardinality & 0x01) == 0)
		{
		// 8 bit cardinality
		len = ReadInt8() & 0xFF;
		// shift of the cardinality bit
		len >>= 1;
		
		if(aConstReadPtr)
			{
			iCurrentPos -= 1;
			}
		}
	else if ((cardinality & 0x02) == 0)
		{
		// 16 bit cardinality
		len = ReadInt16() & 0xFFFF;
		// shift off the cardinality bits
		len >>= 2;

		if(aConstReadPtr)
			{
			iCurrentPos -= 2;
			}
		}
	else if ((cardinality & 0x04) == 0)	
		{
		// 32 bit cardinality
		len = ReadInt32();
		// shift off the cardinality bits
		len >>= 3;

		if(aConstReadPtr)
			{
			iCurrentPos -= 4;
			}
		}
	else
		{
		// WTF. Unknown cardinality
		throw EInvalidFile;
		}
	
	// read the wideness of the characters
	aIsWide = ((len & 0x01) == 0);
	// shift off the wideness bit
	len >>= 1;
	
	// check if we have enough length left in the buffer
	if ((iLength - (iCurrentPos - iBuf)) < len)
		{
		throw EInvalidFile;
		}
	
	return len;
	}


char* StreamReader::UTF16toUTF8Convert(wchar_t* aSource, int aSrcLength)
{
	int targetLength = 2 * aSrcLength;
	int totalSize = targetLength + 1;
	// This memory will be freed by the caller of this function
	char *ptrUtf8 = new char[targetLength + 1];
	
	UTF16* sourceStart = reinterpret_cast<UTF16*>(aSource);
	UTF16* sourceEnd = sourceStart + aSrcLength; 
	UTF8* targetStart = reinterpret_cast<UTF8*>(ptrUtf8);
	UTF8* targetEnd;
	
	ConversionResult result = ok;
	do
		{
		targetEnd = targetStart + targetLength;
		result = ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd);

		if(targetExhausted != result)
			{
			break;
			}

		// Memory reallocation

		int endOffset = (char *)targetStart - ptrUtf8;

		char *temp = new char[totalSize + targetLength];
		
		memcpy(temp, ptrUtf8, totalSize);
		delete[] ptrUtf8;

		ptrUtf8 = temp;

		// New source pointer
		targetStart = reinterpret_cast<UTF8*>(ptrUtf8) + endOffset;
		totalSize += targetLength;
		}while(true);

	// NULL terminating the UTF8 string
	*targetStart = 0;

	return ptrUtf8;
}


StreamReader::~StreamReader()
	{
	if(iStreamOwnership) 
		{
		ifstream* str = static_cast<ifstream *>(iFileStream);
		str->close();
		delete iFileStream;
		}

	delete [] iBuf;
	}

