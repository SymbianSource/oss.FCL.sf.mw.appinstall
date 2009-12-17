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



#include "datalocator.h"

DataLocator::DataLocator(Stream& aStream, ifstream& aFile)
	{
	int length = aStream.Length();
	iBuffer = new char[length];
	char* curPointer = iBuffer;

	Frames::Iterator frameIter = aStream.Frame();

	do
		{
		FramePos pos = frameIter->iPos;
		
		int len = (frameIter++)->iDes.Length();
		
		if (len == 0)
			{
			len = frameIter->iPos.Pos()-pos.Pos();
			}

		aFile.seekg(FileOffset(pos).Offset(), ios::beg);
		aFile.read(curPointer, len);
		curPointer += len;
		} while (frameIter->iDes.Type()==FrameDes::Continuation);
	
	// Reset current pointer
	iStrStream = new strstream(iBuffer, length, ios::in);
	}

DataLocator::~DataLocator()
	{
	delete iStrStream;
	delete iBuffer;
	}