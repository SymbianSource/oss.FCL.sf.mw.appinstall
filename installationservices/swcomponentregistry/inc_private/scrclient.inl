/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Utils functions used by scr session and subsession clients.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRCLIENT_INL
#define SCRCLIENT_INL

#include <usif/scr/scr.h>
#include <scs/cleanuputils.h>
#include <scs/streamingarray.h>
#include <s32mem.h>

namespace Usif
	{
	template <class C>
	inline HBufC8* GetObjectDataLC(const C& aConnection, TInt aFunction, TInt aDataSize)
		{
		HBufC8 *buf = HBufC8::NewLC(aDataSize);
		TPtr8 ptrBuf(buf->Des());
		TIpcArgs argBuf(&ptrBuf);
		User::LeaveIfError(aConnection.SendSyncMessage(aFunction, argBuf));
		return buf; 
		}
	
	template <class T, class C>
	inline T* GetObjectL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs)
		{
		TInt size(0);
		TPckg<TInt> sizePak(size);
		aArgs.Set(aArgNum, &sizePak);
		User::LeaveIfError(aConnection.SendSyncMessage(aSizeFunction, aArgs));
		if(!size)
			{// There is no related object return NULL
			return NULL;
			}
		HBufC8 *buf = GetObjectDataLC(aConnection, aDataFunction, size);
			
		RDesReadStream stream(*buf);
		CleanupClosePushL(stream);
		T *retObject = TTraits<T>::ReadFromStreamL(stream);
		CleanupStack::PopAndDestroy(2, buf);
		return retObject;
		}
	
	template <class T, class C>
	inline TBool GetObjectL(const C& aConnection, T& aObject, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs)
		{
		TInt size(0);
		TPckg<TInt> sizePak(size);
		aArgs.Set(aArgNum, &sizePak);
		User::LeaveIfError(aConnection.SendSyncMessage(aSizeFunction, aArgs));
		if(!size)
			{// There is no related object, return EFalse
			return EFalse;
			}
		HBufC8 *buf = GetObjectDataLC(aConnection, aDataFunction, size);
				
		RDesReadStream stream(*buf);
		CleanupClosePushL(stream);
		aObject.InternalizeL(stream);
		CleanupStack::PopAndDestroy(2, buf);
		return ETrue;
		}
	
	template <class C, class T>
	inline void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RPointerArray<T>& aArray)
		{
		TInt size(0);
		TPckg<TInt> sizePak(size);
		aArgs.Set(aArgNum, &sizePak);
		User::LeaveIfError(aConnection.SendSyncMessage(aSizeFunction, aArgs));
		if(!size)
			{// There is no related data, return.
			return;
			}
		HBufC8 *buf = GetObjectDataLC(aConnection, aDataFunction, size);
			
		RDesReadStream stream(*buf);
		CleanupClosePushL(stream);
		InternalizePointersArrayL(aArray, stream);
		CleanupStack::PopAndDestroy(2, buf);
		}
	
	template <class C, class T>
	inline void GetObjectArrayL(const C& aConnection, TInt aSizeFunction, TInt aDataFunction, TInt aArgNum, TIpcArgs& aArgs, RArray<T>& aArray)
		{
		TInt size(0);
		TPckg<TInt> sizePak(size);
		aArgs.Set(aArgNum, &sizePak);
		User::LeaveIfError(aConnection.SendSyncMessage(aSizeFunction, aArgs));
		if(!size)
			{// There is no related data, return.
			return;
			}
		HBufC8 *buf = GetObjectDataLC(aConnection, aDataFunction, size);
				
		RDesReadStream stream(*buf);
		CleanupClosePushL(stream);
		InternalizeFixedLengthArrayL(aArray, stream);
		CleanupStack::PopAndDestroy(2, buf);
		}
	
	template <class C>
	inline void ExternalizeObjectL(const C* aObject, RBuf8& aBuf)
		{
		const C *localCopyOfObject(0);
		
		if(!aObject)
			{// if object is not supplied, then create an empty one.
			localCopyOfObject = C::NewLC();
			}
		else
			{
			localCopyOfObject = aObject;
			}
			
		// Get the required buffer size for the externalized C object
		TInt bufSize = GetObjectBufferSizeL(*localCopyOfObject);
			
		// Clean the buffer and re-create it with the required buffer size 
		aBuf.Close();
		aBuf.CreateL(bufSize);
			
		// Externalize the filter object into the buffer
		RDesWriteStream wstream(aBuf);
		wstream.PushL();
		wstream << *localCopyOfObject;
		wstream.CommitL();
		wstream.Pop();
		wstream.Release();
			
		if(!aObject)
			{// destroy the filter if created locally
			CleanupStack::PopAndDestroy((C*)localCopyOfObject);
			}
		}// End of ExternalizeObjectL
	
    template <class C>
    inline void ExternalizeRefObjectL(const C& aObject, RBuf8& aBuf)
        {
        // Get the required buffer size for the externalized C object
        TInt bufSize = GetObjectBufferSizeL(aObject);
            
        // Clean the buffer and re-create it with the required buffer size 
        aBuf.Close();
        aBuf.CreateL(bufSize);
            
        // Externalize the filter object into the buffer
        RDesWriteStream wstream(aBuf);
        wstream.PushL();
        wstream << aObject;
        wstream.CommitL();
        wstream.Pop();
        wstream.Release();
            
        }// End of ExternalizeObjectL	
	}

#endif /* SCRCLIENT_INL */
