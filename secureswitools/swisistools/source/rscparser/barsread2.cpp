// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Resource reader
// 
/** 
* @file barsread2.cpp
*
* @internalComponent
* @released
*/
#include <iostream>
#include <cassert>
#include "barsc2.h"
#include "barsread2.h"
#include "barsreadimpl.h"

/** It creates the implementation in place - iImpl array,
and sets the default.
*/
RResourceReader::RResourceReader() :
	iRscBuffer(NULL)
	{
	new (iImpl) TResourceReaderImpl;
	}

/** The method calls RResourceReader::Close() method to release 
allocated by the instance resources.
*/
RResourceReader::~RResourceReader()
	{
	Close();
	}

/** 
Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8L(), 
start at the beginning of this buffer.
@param aRscFile A pointer to the CResourceFile object, used as a resource data supplier.
@param aResourceId Numeric id of the resource to be read.
*/
void RResourceReader::OpenL(CResourceFile* aRscFile, TInt aResourceId)
{
	if (aRscFile)
	{
		Close();
		iRscBuffer = aRscFile->AllocReadL(aResourceId);
		Impl()->SetBuffer(iRscBuffer);
	}
	else
	{
		std::string errMsg="Failed : Invalid Argument : CResourceFile* ";
		throw CResourceFileException(errMsg);
	}
}

/** 
Destroys the buffer containing the resource data.

Open() method should be called if you want to set
the buffer and current position again.

If a one or more copies of the same RResourceReader object exist - they share the same 
resource data buffer. So destroying the RResourceReader object you will destroy the
shared resource data buffer.

@post Buffer pointer is set to NULL.
@post Buffer current position pointer is set to NULL. 
*/
void RResourceReader::Close()
	{
	delete iRscBuffer;
	iRscBuffer = NULL;
	Impl()->ResetBuffer();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs an 8 bit non modifiable pointer to represent this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 8 bit text characters or binary data.

If the value of the leading byte is zero, calling Length() on the returned 
PtrC8 returns zero.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadTPtrCL().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@pre Open() is called to initialize RResourceReader data members.
@return 8bit non modifiable pointer representing
the data following the leading byte count at the
current position within the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
PtrC8* RResourceReader::ReadTPtrC8L()
	{
	return Impl()->ReadTPtrC8L();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a 16 bit non modifiable pointer to represent this data.

The data is interpreted as:

a byte value defining the number of 16 bit text characters
(the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, calling Length() on the returned 
PtrC16 returns zero.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrCL().

@pre Open() is called to initialize RResourceReader data members.
@return 16 bit non modifiable pointer representing
the data following the leading byte count at the
current position within the resource buffer.
@post Current buffer position is updated.
@leave KErrCorrupt The new buffer position is beyond the buffer end. */

PtrC16* RResourceReader::ReadTPtrC16L()
	{
	return Impl()->ReadTPtrC16L();
	}


/** Interprets the data at the current buffer position as a TInt8 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt8.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt8 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
TInt RResourceReader::ReadInt8L()
    {
	return Impl()->ReadInt8L();
    }

/** Interprets the data at the current buffer position as a TUint8 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint8.

@pre Open() is called to initialize RResourceReader data members.
@return The TUint8 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
TUint32 RResourceReader::ReadUint8L()
    {
	return Impl()->ReadUint8L();
    }

/** Interprets the data at the current buffer position as a TInt16 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt16.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt16 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
TInt RResourceReader::ReadInt16L()
    {
	return Impl()->ReadInt16L();
    }


/** Interprets the data at the current buffer position as a TInt32 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS, TInt and TInt32 are the same size.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt32 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
TInt RResourceReader::ReadInt32L()
    {
	return Impl()->ReadInt32L();
    }

/** Interprets the data at the current buffer position as a TUint32 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS a TUint is the same size as a TUint32.

@pre Open() is called to initialize RResourceReader data members.
@return The TUint32 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
TUint32 RResourceReader::ReadUint32L()
    {
	return Impl()->ReadUint32L();
    }

/** Copies a specified length of data from the resource buffer, starting at the 
current position within the buffer, into the location pointed to by a specified 
pointer. No assumption is made about the type of data at being read.

The current position within the resource buffer is updated.

@pre Open() is called to initialize RResourceReader data members.
@param aPtr Pointer to the target location for data copied from the resource buffer.
@param aLength The length of data to be copied from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
void RResourceReader::ReadL(TAny* aPtr,TInt aLength)
    {
	Impl()->ReadL(aPtr,aLength);
    }

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a build independent heap buffer containing a copy of this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data. This resource data is interpreted as either 
8 bit or 16 bit, depending on the build.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8 bit variant ReadHBufC8L().

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to the heap buffer containing a copy of
the data following the leading byte count at the
current position within the resource buffer. The
pointer can be NULL.
@post Current buffer position is updated.
@leave KErrCorrupt The resulting position lies beyond the end of the resource buffer. */
Ptr16* RResourceReader::ReadHBufCL()
    {
	return Impl()->ReadHBufCL();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a non modifiable pointer to represent this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data. This resource data is interpreted as either 
8 bit or 16 bit, depending on the build.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC returns zero.

The current position within the resource buffer is updated.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8 bit variant ReadTPtrC8L().

@pre Open() is called to initialize RResourceReader data members.
@return Non modifiable pointer representing the
data following the leading byte count of the element
at the specified position within the array.
@post Current buffer position is updated.
@leave KErrCorrupt The resulting position lies beyond the end of the resource buffer. */
PtrC16* RResourceReader::ReadTPtrCL()
    {
	return ReadTPtrC16L();
	}

/** @internalComponent
@return Non-const pointer to the implementation object. */
TResourceReaderImpl* RResourceReader::Impl()
	{
	return reinterpret_cast <TResourceReaderImpl*> (iImpl);
	}

/** @internalComponent
@return Const pointer to the implementation object. */
const TResourceReaderImpl* RResourceReader::Impl() const
	{
	return reinterpret_cast <const TResourceReaderImpl*> (iImpl);
	}

