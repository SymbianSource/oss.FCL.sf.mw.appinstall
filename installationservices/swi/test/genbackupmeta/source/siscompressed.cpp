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
* For decompressing SISControllers.
* @internalComponent
*
*/


#include "siscompressed.h"
#include "tlv.h"
#include "filecontents.h"
#include "exceptionhandler.h"

#include <zlib.h>
using Swi::Sis::TFieldLength;

CSISCompressed::CSISCompressed (CFileContents& aSISReader) 
	: iBuffer (0)
	{
	iCompressionAlgorithm = (TCompressionAlgorithm) aSISReader.GetTUint32 ();
	iUncompressedSize = aSISReader.GetTInt64 ();
		
	switch (iCompressionAlgorithm)
		{
		case ECompressDeflate:
			{
			TFieldLength compressedLength = aSISReader.GetRemainingLength();
			iBuffer = new char [iUncompressedSize];
			unsigned char* compressedData = reinterpret_cast<unsigned char*>(aSISReader.GetByteString(compressedLength));
			z_stream compressionState;
			memset (&compressionState, 0, sizeof (z_stream));

			try 
				{
				compressionState.avail_in = compressedLength;
				compressionState.next_in = compressedData;
				compressionState.avail_out = iUncompressedSize;
				compressionState.next_out = (unsigned char *) iBuffer;
				compressionState.data_type = Z_BINARY;
				if (inflateInit (&compressionState) != Z_OK)
					{
					throw TExceptionHandler::EFileCompression;
					}
				if (inflate (&compressionState, Z_FINISH) != Z_STREAM_END)
					{
					throw TExceptionHandler::EFileCompression;
					}
				}
			catch (...)
				{
				inflateEnd (&compressionState);
				delete [] compressedData;
				throw;
				}
			inflateEnd (&compressionState);
			delete [] compressedData;
			break;
			}

		case ECompressNone:
			{
			iBuffer = aSISReader.GetByteString (iUncompressedSize);
			break;
			}

		default:
			{
			throw TExceptionHandler (TExceptionHandler::EUnexpectedValue);
			}
			
		}
	}

const char*	CSISCompressed::DecompressedData () 
	{ 
	return iBuffer; 
	}

TFieldLength CSISCompressed::DecompressedLength () 
	{ 
	return iUncompressedSize; 
	}

CSISCompressed::~CSISCompressed ()
	{
	delete [] const_cast<char*>(iBuffer);
	}

const TFieldType CSISCompressed::iExpectedType = EFieldTypeCompressed;