/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* For decompressing SISControllers.
* @internalComponent
*
*/



#ifndef	SISCOMPRESSED_H
#define	SISCOMPRESSED_H

#include "symbiantypes.h"
#include "sisfieldtypes.h"
using Swi::Sis::TFieldLength;

class CFileContents;

class CSISCompressed
	{
public:
	CSISCompressed (CFileContents& aSISReader);
	~CSISCompressed ();
	// Memory cleanup ownership is of the caller.
	const char*	DecompressedData ();
	Swi::Sis::TFieldLength DecompressedLength ();

public:
	static const Swi::Sis::TFieldType iExpectedType;

private:
	const char*	iBuffer;
	Swi::Sis::TCompressionAlgorithm	iCompressionAlgorithm;
	Swi::Sis::TFieldLength			iUncompressedSize;

	};

#endif	/* __SISCOMPRESSED_H_ */
