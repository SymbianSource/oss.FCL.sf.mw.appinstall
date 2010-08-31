/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "sisstring.h"
#include "utility.h"
#include "exception.h"
#include "utils.h"
#include "utf8_wrapper.h"

// we cannot write or read a string directly from a file, because we don't know what format the
// MS implementation of STL will use for the std::string. So we need to go via an intemediate buffer.
// But strings can be variable size, including very big indeed, so that buffer will have to be
// allocated as need be. Buffer allocation and deletion is expensive. It is sensible to allocate
// a common buffer, and only change its size when necessary. To reduce the frequency of file size
// changes, the actual allocation oversteps the mark, so the next time something requests a slightly
// bigger string, no allocation is required (usually). These variables could be private static members of our
// string class, but that would mean some minor change to this code would require the entire
// product to be recompiled.

static TUint8* gBuffer = NULL;
static unsigned long gBufferLength = 0;
static const unsigned long gChunk = 512;



void VerifyBufferSize (const unsigned long aSize)
	{
	if (aSize == 0)
		{
		return;
		}
	if (aSize > gBufferLength)
		{
		delete [] gBuffer;
		gBufferLength = aSize + gChunk;
		gBuffer = new TUint8 [gBufferLength];
		}
	}


void CSISString::ExitInstance ()
	{
	delete [] gBuffer;
	gBuffer = NULL;
	gBufferLength = 0;
	}


void CSISString::Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	CSISHeader header (CSISFieldRoot::ESISString);
	if (IsDataThere (aFile, header, aArrayType))
		{
		if (header.DataSize () == 0)
			{
			iData.erase ();
			}
		else
			{
				unsigned int dataSize = header.DataSize();
				VerifyBufferSize (dataSize);
				dataSize = AlignedSize(dataSize);
				aFile.read (gBuffer, dataSize);

				if(sizeof(WCHAR) != 2)
					{
					int targetLength = dataSize*sizeof(wchar_t);
					wchar_t *ptrUCS4 = new wchar_t [dataSize];
					UTF16* sourceStart = reinterpret_cast<UTF16*>(gBuffer);
					UTF16* sourceEnd = sourceStart + dataSize;
					UCS4* targetStart = reinterpret_cast<UCS4*>(ptrUCS4);
					UCS4* targetEnd = targetStart + dataSize*2;
					ConvertUTF16toUCS4(&sourceStart, sourceEnd, &targetStart, targetEnd);
					iData.assign ((ptrUCS4), static_cast <std::wstring::size_type> (dataSize / 2));
					}
				else
					{
					iData.assign (	reinterpret_cast <wchar_t*> (gBuffer), 
  							static_cast <std::wstring::size_type> (header.DataSize () / sizeof (wchar_t)));
					}
			}

		}
	}


void CSISString::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	if (! WasteOfSpace ())
		{
#ifdef GENERATE_ERRORS
		if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInsaneString))
			{
			CSISHeader (CSISFieldRoot::ESISString, rand ()).Write (aFile, aIsArrayElement);
			}
		else
#endif // GENERATE_ERRORS
			{
			CSISHeader (CSISFieldRoot::ESISString, ByteCount (aIsArrayElement)).Write (aFile, aIsArrayElement);
			}
		
		// SIS file format only supports UCS-2 format , so need conversion here if wchar is 4 bytes (Linux OS)
		if(sizeof(WCHAR) != 2)
			{
			int targetLength = iData.size()*2;
			UTF16 *ptrUtf16 = new UTF16[targetLength + 1];
			UCS4* sourceStart = reinterpret_cast<UCS4*>(const_cast<wchar_t*>(iData.c_str()));
			UCS4* sourceEnd = sourceStart + (iData.size()*sizeof(WCHAR));
			UTF16* targetStart = reinterpret_cast<UTF16*>(ptrUtf16);
			UTF16* targetEnd = targetStart + targetLength;
			ConvertUCS4toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd);
			int endOffset = (UTF16*)targetStart - ptrUtf16;
			targetStart = reinterpret_cast<UTF16*>(ptrUtf16) + endOffset;
			*targetStart = 0;
			aFile.write (reinterpret_cast <const TUint8*> (ptrUtf16),targetLength);
			}
		else
			{
			aFile.write (reinterpret_cast <const TUint8*> (iData.c_str ()), ByteCount (aIsArrayElement));
			}
		WriteFiller (aFile);
		}
	}


void CSISString::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream << "\"" << wstring2string (iData) << "\"";
	}


void CSISString::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	if(!WasteOfSpace())
		{
		CSISHeader (CSISFieldRoot::ESISString, ByteCount (false)).CalculateCrc (aCRC, aIsArrayElement);
		CSISFieldRoot::DoPaddedCrc(aCRC, reinterpret_cast <const TUint8*> (iData.c_str ()), ByteCount (aIsArrayElement));
		}
	}


CSISFieldRoot::TFieldSize CSISString::ByteCount (const bool aInsideArray) const
	{
	if(sizeof(wchar_t) != 2)
		return static_cast <CSISFieldRoot::TFieldSize> (iData.size () * sizeof (wchar_t)/2);
	else
		return static_cast <CSISFieldRoot::TFieldSize> (iData.size () * sizeof (wchar_t));
	}


bool CSISString::WasteOfSpace () const
	{
	return ! Required () && iData.empty ();
	}

