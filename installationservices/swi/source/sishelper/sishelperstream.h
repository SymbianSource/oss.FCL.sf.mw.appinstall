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




/**
 @file
 @internalComponent
*/

#ifndef __SISHELPERSTREAM_H_
#define __SISHELPERSTREAM_H_ 

class RBufWriteStream;

namespace Swi
{

class CSisHelperStream : public CBase
	{
	public:
		static const TInt KBufferSize;
		
		static CSisHelperStream* NewL();
		static CSisHelperStream* NewLC();
		
		operator RBufWriteStream&();
		
		inline CBufFlat* Buffer();
		inline RBufWriteStream* Stream();
		TPtr8 Ptr();
		
		~CSisHelperStream();
	private:
		CSisHelperStream();
		void ConstructL();
	
		CBufFlat* iBuffer;
		RBufWriteStream* iWriteStream;
	};

inline RBufWriteStream* CSisHelperStream::Stream()
	{
	return iWriteStream;
	}
	
inline CBufFlat* CSisHelperStream::Buffer()
	{
	return iBuffer;
	}
	
} // namespace Swi

#endif // #ifndef __SISUISTREAM_H_
