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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include "siswrite.h"
#include "utility_interface.h"
#include <zlib.h>

bool CompressData (	const TUint8* aInputData, const CSISFieldRoot::TFieldSize aInputSize,
					TUint8*& aOutputData, CSISFieldRoot::TFieldSize& aOutputSize,
					const bool aCanAbandon)
	{
	assert (aOutputData == NULL);
	assert (aInputData != NULL);
	assert (! IsBadReadPtr (aInputData, aInputSize));
	CSISFieldRoot::TFieldSize inputSize = aInputSize;
	inputSize = inputSize + (inputSize / 64) + 16;
		// see zlib library documentation for the reasons for this calculation
	TUint8* buffer = new TUint8 [inputSize];
	try
		{
		z_stream compressionState;
		memset (&compressionState, 0, sizeof (z_stream));
		compressionState.avail_out = inputSize;
		compressionState.data_type = Z_BINARY;
		compressionState.avail_in = aInputSize;
		compressionState.next_out = buffer;
		compressionState.next_in = const_cast <Bytef*> (aInputData);
		try 
			{
			CSISException::ThrowIf (deflateInit (&compressionState, Z_DEFAULT_COMPRESSION) != Z_OK,
									CSISException::ECompress,
									"deflation progress");
			CSISException::ThrowIf (deflate (&compressionState, Z_FINISH) != Z_STREAM_END,
									CSISException::ECompress,
									"deflation end");
			}
		catch (...)
			{
			deflateEnd (&compressionState);
			throw;
			}
		deflateEnd (&compressionState);
		aOutputData = buffer;
		aOutputSize = static_cast <CSISFieldRoot::TFieldSize> (compressionState.total_out);
		}
	catch (...) 
		{
		delete [] buffer;
		aOutputData = NULL;
		aOutputSize = 0;
		throw;
		}
	if ((aOutputSize < aInputSize) || ! aCanAbandon)
		{
		return true;
		}
	delete [] buffer;
	aOutputData = NULL;
	aOutputSize = 0;
	return false;
	}




void ExpandStream (const TUint8* aInputData, const CSISFieldRoot::TFieldSize aInputSize, TSISStream& stream)
	{
	assert (aInputData != NULL);
	assert (! IsBadReadPtr (aInputData, aInputSize));
	z_stream compressionState;
	memset (&compressionState, 0, sizeof (z_stream));
	const uLong arbitraryBufferSize (262144);
	TSISStream::pos_type start = stream.tell ();
	TUint8* buffer = new TUint8 [arbitraryBufferSize];
	try 
		{
		compressionState.avail_in = aInputSize;
		compressionState.next_in = const_cast <Bytef*> (aInputData);
		compressionState.avail_out = arbitraryBufferSize;
		compressionState.next_out = buffer;
		compressionState.data_type = Z_BINARY;
		CSISException::ThrowIf (inflateInit (&compressionState) != Z_OK,
								CSISException::ECompress,
								"inflation initialisation");
		int status;
		while ((status = inflate (&compressionState, Z_NO_FLUSH)) == Z_OK)
			{
			stream.write (buffer, arbitraryBufferSize - compressionState.avail_out);
			compressionState.avail_out = arbitraryBufferSize;
			compressionState.next_out = buffer;
			}
		CSISException::ThrowIf (status != Z_STREAM_END,
								CSISException::ECompress,
								"inflation");
		status = inflate (&compressionState, Z_FINISH);
		CSISException::ThrowIf (status != Z_STREAM_END,
								CSISException::ECompress,
								"inflation");
		stream.write (buffer, arbitraryBufferSize - compressionState.avail_out);
		}
	catch (...)
		{
		inflateEnd (&compressionState);
		delete [] buffer;
		stream.seek (start);
		throw;
		}
	inflateEnd (&compressionState);
	delete [] buffer;
	}
