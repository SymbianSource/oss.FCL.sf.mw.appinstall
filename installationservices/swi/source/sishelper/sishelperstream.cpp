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

#include "sishelperstream.h"


namespace Swi
{

const TInt CSisHelperStream::KBufferSize=50;

CSisHelperStream* CSisHelperStream::NewL()
	{
	CSisHelperStream* self=CSisHelperStream::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSisHelperStream* CSisHelperStream::NewLC()
	{
	CSisHelperStream* self=new(ELeave) CSisHelperStream();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
		
CSisHelperStream::~CSisHelperStream()
	{
	if (iWriteStream)
		{
		iWriteStream->Close();
		delete iWriteStream;
		}
	delete iBuffer;
	}

CSisHelperStream::CSisHelperStream()
	{
	}

void CSisHelperStream::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KBufferSize);
	iWriteStream = new RBufWriteStream(*iBuffer);
	}

CSisHelperStream::operator RBufWriteStream&()
	{
	return *iWriteStream;
	}

TPtr8 CSisHelperStream::Ptr()
	{
	return iBuffer->Ptr(0);
	}
	
} // namespace Swi
