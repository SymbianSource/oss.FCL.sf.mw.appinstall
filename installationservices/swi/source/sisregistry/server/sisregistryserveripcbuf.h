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
* Library to add s32strm support for IPC (ie. stream via multiple IPC read/writes instead of
* copying to a buffer and streaming to/from there.
*
*/


/**
 @file
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYSERVERIPCBUF_H__
#define __SISREGISTRYSERVERIPCBUF_H__

#include <e32base.h>
#include <s32buf.h>

_LIT(KIpcBufPanic, "IpcBuf");

class RIpcBuf : public MStreamBuf
	{
public:
	void Open(const RMessage2& aMessage, TInt aMessageSlot);

protected:
	
	TInt DoReadL(TAny *aPtr, TInt aMaxLength);
	void DoWriteL(const TAny* aPtr,TInt aLength);

private:
	enum TIpcBufPanic
		{
		EIpcBufSeekUnknownLocation,
		EIpcBufSeekBadOffset
		};

private:
	TInt iReadPos;
	TInt iWritePos;
	RMessage2 iMessage;
	TInt iMessageSlot;
	};



#endif /* __SISREGISTRYSERVERIPCBUF_H__ */

