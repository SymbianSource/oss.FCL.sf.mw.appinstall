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
* @file barsreadimpl.h
*
* @internalComponent
* @released
*/
#ifndef __BARSREADIMPL_H__
#define __BARSREADIMPL_H__

#include "commontypes.h"

/** Interprets resource data read from a resource file.

To use an instance of this class, pass the buffer containing the resource 
data to it by calling SetBuffer().

The buffer containing the resource data is created by RResourceFile::AllocReadL()/CResourceFile::AllocReadL() 
which reads the specified resource into it.

The current position within the buffer is always maintained and any request 
for data is always supplied from the current position. The current position 
is always updated .

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The buffer used by the source class instance will be shared with the 
destination class instance. However source and destination instances will have their own 
current position pointers, both pointed initially to the same buffer position.

@internalComponent
@see TResourceReaderImpl::SetBuffer()
@see CResourceFile::AllocReadL() */
class TResourceReaderImpl
    {
public:
	TResourceReaderImpl();

	void	SetBuffer(const Ptr8* aBuffer);
	void	ResetBuffer();
    const	TAny* Ptr();

    // Read counted strings into allocated buffer
	Ptr16*	ReadHBufCL();

    // Build pointer from a counted string
    PtrC8*  ReadTPtrC8L();
	PtrC16* ReadTPtrC16L();

    TInt	ReadInt8L();
    TUint32 ReadUint8L();
    TInt	ReadInt16L();
	TInt	ReadInt32L();
    TUint32 ReadUint32L();

    void	ReadL(TAny* aPtr,TInt aLength);

private:
    void	MovePtrL(const TUint8* aPtr);
private:
		
	const Ptr8*		iBuffer;
    const TUint8*	iCurrentPtr;
    };

#endif//__BARSREADIMPL_H__