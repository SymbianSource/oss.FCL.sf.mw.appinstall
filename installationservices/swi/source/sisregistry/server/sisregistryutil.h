/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistryUtil a utility class used by variety classes 
* implemented as static functions
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYUTIL_H__
#define __SISREGISTRYUTIL_H__

#include <f32file.h>
#include <s32mem.h>
#include "hashcontainer.h"
#include "arrayutils.h"
#include "sisregistryclientserver.h"

namespace Swi
{

static TBool operator==(const CHashContainer& a, const CHashContainer& b);

class SisRegistryUtil
	{
public:
	static TBool FileExistsL(RFs& aFs, const TDesC& aFullName);	
 	static HBufC* BuildUninstallLogFileNameLC(TUid aUid, TInt aIndex, TChar aDriveLetter);
 	static HBufC* BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController);
 	static HBufC* BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController, TInt aDrive);
	static HBufC* BuildStubFilesPathLC(TInt aDrive);
	static void EnsureDirExistsL(RFs& aFs, const TDesC& aPath);
	static void CreateFileWithAttributesL(RFs& aFs, const TDesC& aPath, const TUint aAttributesMask = 0);
	static TInt DeleteFile(RFs& aFs, const TDesC& aPath);
	static TBool RomBasedPackageL(const TUid& aUid); // To be removed: Use CSisRegistrySession::RomBasedPackageL instead
	static TBool CheckIfMatchingStubPUidL(const TUid& aUid, RFs& aFileServer, const TFileName& aStubFileName);
	static HBufC* BuildLogFileNameLC();
	static TUid IdentifyUidFromSisFileL(RFs& aFs, TDesC& aFilename);

	// IPC helper methods
	template<class T> 
	static void SendDataL(const RMessage2& aMessage, const T& aProvider, TInt aIpcIndx);
	template<class T> 
	static void SendDataArrayL(const RMessage2& aMessage, const RArray<T> aProvider, TInt aIpcIndx);
	template<class T> 
	static void SendDataPointerArrayL(const RMessage2& aMessage, const RPointerArray<T> aProvider, TInt aIpcIndx);

	static inline TDriveNumber SystemRomDrive()
		{
		return EDriveZ;
		}

private:
	static void ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3);
	};

inline TBool operator==(const CHashContainer& a, const CHashContainer& b)
	{
	return (a.Algorithm() == b.Algorithm()) && (a.Data() == b.Data());
	}	

// Templated function definitions must appear in the header file

template<class T> 
void SisRegistryUtil::SendDataPointerArrayL(const RMessage2& aMessage, const RPointerArray<T> aProvider, TInt aIpcIndx)
	// templated version for a RPointerArray of T objects 
	// aMessage - the message
	// aProvider - the data to be serialised 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	// externalise the pointer array		
	ExternalizePointerArrayL(aProvider, stream);
	stream.CommitL();
	
	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
		
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer	
	}

template<class T> 
void SisRegistryUtil::SendDataArrayL(const RMessage2& aMessage, const RArray<T> aProvider, TInt aIpcIndx)
	// templated version for a an array of objects
	// aMessage - the message
	// aProvider - the data to be serialised 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
		
	// externalise the array of objects		
	ExternalizeArrayL(aProvider, stream);
	stream.CommitL();

	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    	
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
		
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer		
	}	

template<class T> 
void SisRegistryUtil::SendDataL(const RMessage2& aMessage, const T& aProvider, TInt aIpcIndx)
	// templated version for a single object 
	// aMessage - the message
	// aProvider - the data to be serialised - object 
	// aIpcIndx - the index of the IPC argument where the data will be serialised
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);	
	
	// externalise the object		
	aProvider.ExternalizeL(stream);
	stream.CommitL();

	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
    TPtr8 pbuffer(buffer->Des());
    	
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer	
	}

} //namespace
#endif //__SISREGISTRYUTIL_H__
