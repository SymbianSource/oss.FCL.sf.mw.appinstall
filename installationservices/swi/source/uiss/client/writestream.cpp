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
*/

#include <s32mem.h>

#include "writestream.h"

namespace Swi
{
const TInt CWriteStream::KBufferSize=50;

EXPORT_C CWriteStream* CWriteStream::NewL()
	{
	CWriteStream* self=CWriteStream::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteStream* CWriteStream::NewLC()
	{
	CWriteStream* self=new(ELeave) CWriteStream();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
		
CWriteStream::~CWriteStream()
	{
	iWriteStream.Close();
	delete iBuffer;
	}

CWriteStream::CWriteStream()
	{
	}

void CWriteStream::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KBufferSize);
	iWriteStream.Open(*iBuffer); 
	}

EXPORT_C CWriteStream::operator RBufWriteStream&()
	{
	return iWriteStream;
	}

EXPORT_C TPtr8 CWriteStream::Ptr()
	{
	return iBuffer->Ptr(0);
	}

EXPORT_C void CWriteStream::ReAllocL(TInt aSize)
	{
	iWriteStream.Close();
	delete iBuffer;
	iBuffer=NULL;
	iBuffer=CBufFlat::NewL(aSize);
	iWriteStream.Open(*iBuffer);
	}
	
} // namespace Swi
