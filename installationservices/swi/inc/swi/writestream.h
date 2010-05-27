/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of CWriteStream which is used for streaming data from a descriptor
*
*/


/**
 @file 
 @internalComponent
 @released
*/
#ifndef __WRITESTREAM_H_
#define __WRITESTREAM_H_ 

#include <s32mem.h>

namespace Swi
{
/**
 * A class which acts as a wrapper around the RBufWriteStream class, automatically
 * creating a buffer to write into, and a stream to write into it. The class also provides
 * an acessor to the raw memory of the buffer.
 */
class CWriteStream : public CBase
	{
	public:
		/// Initial buffer size for the stream
		static const TInt KBufferSize;
		
		IMPORT_C static CWriteStream* NewL();
		IMPORT_C static CWriteStream* NewLC();
		
		/// operator so we can use this as an RBufWriteStream
		IMPORT_C operator RBufWriteStream&();
		
		// Access functions
		inline CBufFlat& Buffer();
		inline RBufWriteStream& Stream();
		IMPORT_C TPtr8 Ptr();
		
		// Modifiers
		
		/**
		 * Reallocates stream buffer, clears contents.
		 *
		 * @param aSize New stream buffer size.
		 */
		IMPORT_C void ReAllocL(TInt aSize);
		
		~CWriteStream();
	private:
		CWriteStream();
		void ConstructL();
	
		/// The memory buffer for the stream
		CBufFlat* iBuffer;
		
		/// The stream itself
		RBufWriteStream iWriteStream;
	};

// inline functions from CWriteStream

inline RBufWriteStream& CWriteStream::Stream()
	{
	return iWriteStream;
	}
	
inline CBufFlat& CWriteStream::Buffer()
	{
	return *iBuffer;
	}
	
} // namespace Swi

#endif // #ifndef __WRITESTREAM_H_
