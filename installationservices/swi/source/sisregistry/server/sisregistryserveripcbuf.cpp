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
*/
#include "sisregistryserveripcbuf.h"


void RIpcBuf::Open(const RMessage2& aMessage, TInt aMessageSlot)
	{
	iMessage = aMessage;
	iMessageSlot = aMessageSlot;
	iReadPos = 0;
	iWritePos = 0;
	}



TInt RIpcBuf::DoReadL(TAny *aPtr, TInt aMaxLength)
	{
	TPtr8 ptr((TUint8*)aPtr, aMaxLength);
	iMessage.ReadL(iMessageSlot, ptr, iReadPos);
	TInt len = ptr.Length();
	iReadPos += len;
	return len;
	}

void RIpcBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	TPtr8 ptr((TUint8*)aPtr, aLength, aLength);
	iMessage.WriteL(iMessageSlot, ptr, iWritePos);
	iWritePos += aLength;
	}


