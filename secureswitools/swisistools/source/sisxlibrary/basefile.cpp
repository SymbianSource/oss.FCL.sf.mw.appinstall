/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "basefile.h"
#include "exception.h"
#include "utils.h"
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
	// malloc suite used because realloc required

TSISStream::size_type TSISStream::iChunk = 2048;
	// A magic value which should be related to the probable page size



TSISStream::TSISStream () :
		iBuffer (NULL),
		iSize (0),
		iOffset (0),
		iDataLength (0)
	{
	}


TSISStream::~TSISStream ()
	{
	if (iBuffer != NULL)
		{
		free (iBuffer);
		}
	}



void TSISStream::reserve (const size_type aSize)
	{
	if (iOffset + aSize > iSize)
		{
		size_type bloat = ((aSize / iChunk) + 1) * iChunk;
		iBuffer = reinterpret_cast <TUint8*> (realloc (iBuffer, iSize + bloat));
		CSISException::ThrowIf (iBuffer == NULL, CSISException::EMemory, "cannot increase stream buffer size");
		iSize += bloat;
		}
	assert (! IsBadWritePtr (iBuffer, iSize));
	}


void TSISStream::read (TUint8* aBuffer, const size_type aSize)
	{
	CSISException::ThrowIf (iOffset + aSize > iSize,
							CSISException::EMemory, 
							"attempt to read beyond the end of buffer");
	CSISException::ThrowIf (iOffset + aSize > iDataLength,
							CSISException::EMemory, 
							"attempt to read beyond the max write pointer");
	memcpy (aBuffer, &iBuffer [iOffset], aSize);
	iOffset += aSize;
	}
	
	 
void TSISStream::write (const TUint8* aBuffer, const size_type aSize)
	{
	reserve (aSize);
	memcpy (&iBuffer [iOffset], aBuffer, aSize);
	iOffset += aSize;
	if (iOffset > iDataLength)
		{
		iDataLength = iOffset;
		}
	}

void TSISStream::seek (const TSISStream::pos_type aPos, const std::ios_base::seekdir aRel)
	{
	TSISStream::pos_type to;
	switch (aRel)
		{
	case std::ios_base::beg :
		to = aPos;
		break;
	case std::ios_base::cur :
		to = iOffset + aPos;
		break;
	case std::ios_base::end :
		to = iDataLength + aPos;
		break;
	default :
		assert (false);
		return;
		}
	if (to >= iSize)
		{
		reserve (to - iSize);
		}
	iOffset = to;
	}


bool TSISStream::import (HANDLE aFile, TUint64* filesize)
	{
	try {
		DWORD dwHigh = 0;
		DWORD dwLow = ::GetFileSize(aFile,&dwHigh);
		TInt64 size = (dwHigh << 32) | dwLow;
		reserve (size);
		if (filesize)
			{
			*filesize = size;
			}
		size_t obtained = 0;
		::ReadFile(aFile,iBuffer,size,(DWORD*)&obtained,0);
		iDataLength = obtained;
		return (size == obtained);
		} 
	catch (...)
		{
		return false;
		}
	}


bool TSISStream::exportfile (HANDLE aFile)
	{
	try
		{
		size_t written = 0;
		::WriteFile(aFile,iBuffer,length(),(DWORD*)&written,0);
		return (written == length ());
		}
	catch (...)
		{
		return false;
		}
	}

void TSISStream::reset ()
	{
	if (iBuffer != NULL)
		{
		free (iBuffer);
		iBuffer = NULL;
		}
	iSize = 0;
	iOffset = 0;
	iDataLength = 0;
	}

