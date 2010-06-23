/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Library to add s32strm support for IPC (ie. stream via multiple IPC read/writes instead of
* copying to a buffer and streaming to/from there.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef _IPCUTIL_H_
#define _IPCUTIL_H_

#include <scs/ipcstream.h>
#include <scs/streamingarray.h>


template <class T>
TInt GetObjectSizeL(const T* aObject)
    {
    TInt size(0);
   	if(aObject)
        {
        RNullWriteStream nullstream;
        CleanupClosePushL(nullstream);
        nullstream << *aObject;
        nullstream.CommitL();
        size = nullstream.BytesWritten();
        CleanupStack::PopAndDestroy(&nullstream);
        return size;
        }
   	return -1;
    }

template <class T>
void WriteObjectDataL(const RMessage2& aMessage, TInt aParam, const T* aObject)
    {
    if(!aObject)
        {
        User::Leave(KErrAbort);
        }
    RIpcWriteStream ipcstream;
    ipcstream.Open(aMessage, aParam);
    CleanupClosePushL(ipcstream);
    ipcstream << *aObject;
    CleanupStack::PopAndDestroy(&ipcstream); // Data is committed in Close method
    }

#endif /* _IPCUTIL_H_ */
