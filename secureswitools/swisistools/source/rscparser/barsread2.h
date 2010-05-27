// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
/** 
* @file barsread2.h
*
* @internalComponent
* @released
*/
#ifndef __BARSREAD2_H__
#define __BARSREAD2_H__

#include "commontypes.h"

//Forward declarations
class TResourceReaderImpl;
class CResourceFile;

/** Interprets resource data read from a resource file.

To use an instance of this class, pass the CResourceFile object containing the resource 
data with aResourceId ID to it, by calling OpenL() or OpenLC(). Close the reader
calling Close() after finishing with the resource.

The current position within the buffer is always maintained and any request 
for data is always supplied from the current position. The current position 
is always updated .

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The buffer used by the source class instance will be shared with the 
destination class instance. However source and destination instances will have their own 
current position pointers, both pointed initially to the same buffer position.

@publishedAll
@released
@see CResourceFile */
class RResourceReader
	{
public:
	 RResourceReader();
	 ~RResourceReader();
	 void	 OpenL(CResourceFile* aRscFile, TInt aResourceId);
	 void	 Close();

    // Read counted strings into allocated buffer
     Ptr16*	 ReadHBufCL();

	 // Build pointer from a counted string
     PtrC16* ReadTPtrCL();
     PtrC8*  ReadTPtrC8L();
     PtrC16* ReadTPtrC16L();

     TInt	 ReadInt8L();
     TUint32 ReadUint8L();
     TInt	 ReadInt16L();
	 TInt	 ReadInt32L();
     TUint32 ReadUint32L();

     void	 ReadL(TAny* aPtr,TInt aLength);

private:
	TResourceReaderImpl* Impl();
	const TResourceReaderImpl* Impl() const;
private:
	//Size of the RResourceReader implementation.
	//It should be 12 because of the BC reasons.
	//That's the size of the implementation class,
	//which implements TResourceReader functionality too.
	enum
		{
		KRsReaderSize = 8
		};
	TUint8 iImpl[KRsReaderSize];
	Ptr8* iRscBuffer;
	};

#endif//__BARSREAD2_H__