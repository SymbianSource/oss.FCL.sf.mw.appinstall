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
//
/** 
* @file BaRsReadImpl.cpp
*
* @internalComponent
* @released
*/
#include <iostream>
#include <cassert>
#include <string>
#include "barsreadimpl.h"
#include "barsc2.h"

#define REINTERPRET_CAST(type,exp) (reinterpret_cast<type>(exp))

/** @internalComponent
An error will be issued at compile time if the class size is not KRsReaderImplSize. */
TResourceReaderImpl::TResourceReaderImpl() :
    iBuffer(NULL),
    iCurrentPtr(NULL)
	{
	//TResourceReaderImpl size. It should be 8 because of the BC reasons.
	//8 is the size of TResourceReader class.
	enum
		{
			KRsReaderImplSize = 8
		};
	assert(sizeof(TResourceReaderImpl) == KRsReaderImplSize);
	}

/** Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

@internalComponent
@param aBuffer Pointer to an 8 bit non-modifiable buffer containing 
or representing resource data.
@param aResourceId The numeric id of the resource to be read.
@post Buffer pointer is initialized.
@post Buffer current position pointer is initialized. */
void TResourceReaderImpl::SetBuffer(const Ptr8* aBuffer)
{
	iBuffer=aBuffer;
	iCurrentPtr= (TUint8*)iBuffer->GetPtr();
}

/** Sets the buffer and current position to NULL.
@internalComponent
@post Buffer pointer is set to NULL.
@post Buffer current position pointer is set to NULL. */
void TResourceReaderImpl::ResetBuffer()
	{
	iBuffer=NULL;
	iCurrentPtr=NULL;
	}

/** Returns the current position within the resource buffer. 

The function makes no assumption about the type of data in the buffer at the 
current position.

@internalComponent
@return A pointer to the current position within the resource buffer. */
const TAny* TResourceReaderImpl::Ptr()
    {
    return(iCurrentPtr);
    }

/** Updates iCurrentPtr with a new value.

@internalComponent
@pre iBuffer is not NULL.
@pre aPtr is not NULL.
@param aPtr The new value of iCurrentPtr.
@post iCurrentPtr is updated.
@leave KErrOff The new iCurrentPtr points beyond the buffer end. */
void TResourceReaderImpl::MovePtrL(const TUint8* aPtr)
    {
	assert(iBuffer != NULL);
	assert(aPtr != NULL);
	if(aPtr > ((TUint8*)(  iBuffer->GetPtr() +  iBuffer->GetLength()  )))
	{
		std::string errMsg= "Failed : Trying to access pointer beyond valid range for registrationFile";
		throw CResourceFileException(errMsg);
	}
		
    iCurrentPtr=aPtr;
    }

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a 16 bit heap buffer containing a copy of this data.

The data is interpreted as:

a byte value defining the number of 16 bit text characters
(the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadHBufCL().

@internalComponent
@pre The same as for ReadTPtrC16L().
@return Pointer to the 16bit heap buffer containing a
copy of the data following the leading byte count at
the current position within the resource buffer. The
pointer can be NULL.
@post iCurrentPtr is updated.
@leave The same as ReadTPtrC16L(). 
@see ReadTPtrC16L() */

Ptr16* TResourceReaderImpl::ReadHBufCL()
	{

		PtrC16* ucode = ReadTPtrC16L();
		if(NULL==ucode)
			return NULL;
		Ptr16* unicode = new Ptr16(ucode->iMaxLength);
		memcpy(unicode->GetPtr(),ucode->iPtr,ucode->iMaxLength*2);
		unicode->UpdateLength(ucode->iMaxLength);
		return unicode;
	}


/** Interprets the data at the current buffer position as leading byte count data 
and constructs an 8 bit non modifiable pointer to represent this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 8 bit text characters or binary data.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC8 returns zero.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadTPtrC().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return 8bit non modifiable pointer representing
the data following the leading byte count at the
current position within the resource buffer.
@post iCurrentPtr is updated.
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
PtrC8* TResourceReaderImpl::ReadTPtrC8L()
	{
	assert(iCurrentPtr != NULL);
	const TUint8* currentPtr=iCurrentPtr;//TUint8 pointer is used, which means that the 
	//resource string length is limited to 255 characters max.
	const TInt strLen=*currentPtr;
	++currentPtr;

	PtrC8* unicode = new PtrC8;

	if(!strLen)
		unicode = NULL;
	else
	{
		unicode ->iMaxLength = strLen;
		unicode ->iPtr = currentPtr;
	}
	MovePtrL(currentPtr+strLen);
	return unicode;
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a 16 bit non modifiable pointer to represent this data.

The data is interpreted as:

a byte value defining the number of 16 bit text characters 
(the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC16 returns zero.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC().

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return Pointer to an 8bit variant flat array.
@post iCurrentPtr is updated.
@leave KErrCorrupt The resource is a unicode string and it is not properly aligned.
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */

PtrC16* TResourceReaderImpl::ReadTPtrC16L()
	{
	assert(iCurrentPtr != NULL);
	const TUint8* currentPtr=iCurrentPtr;//TUint8 pointer is used, which means that the 
	
	//resource string length is limited to 255 characters max.
	const TInt unicodeLength=*currentPtr;
	++currentPtr;
	if (unicodeLength!=0)
		{
		if (REINTERPRET_CAST(TUint,currentPtr)&0x1)
			{
			// The resource compiler puts out a padding byte (arbitrarily 0xab)
			// to ensure the alignment of Unicode strings within each resource.
				if(*currentPtr!=0xab)
				{
					std::string errMsg= "Failed : Improper alignment of Unicode strings (0xab) within each resource.";
					throw CResourceFileException(errMsg);
				}
			++currentPtr;
			}
		}

	if (unicodeLength ==0)
	{
		MovePtrL(currentPtr);
		return NULL;
	}
	else
	{
		PtrC16* unicode = new PtrC16;
		unicode ->iMaxLength = unicodeLength;
		unicode ->iPtr = (TUint16*) currentPtr;

		currentPtr+=unicodeLength*sizeof(TText16);
		MovePtrL(currentPtr);
		return unicode;
	}
}


/** Interprets the data at the current buffer position as a TInt8 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt8.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt8 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt8L()
    {
	assert(iCurrentPtr != NULL);
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt8));
    return(*(TInt8*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TUint8 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint8.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TUint8 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TUint32 TResourceReaderImpl::ReadUint8L()
    {
	assert(iCurrentPtr != NULL);
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TUint8));
    return(*(TUint8*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TInt16 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt16.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt16 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt16L()
    {
	assert(iCurrentPtr != NULL);
    if (((TUint32)iCurrentPtr)%2)
        {
        TInt16 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt16));
    return(*(TInt16*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TInt32 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS, TInt and TInt32 are the same size.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt32 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt32L()
    {
	assert(iCurrentPtr != NULL);
    if (((TUint)iCurrentPtr)%4)
        {
        TInt32 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt32));
	return(*(TInt32*)currentPtr);
    }


/** Interprets the data at the current buffer position as a TUint32 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS a TUint is the same size as a TUint32.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TUint32 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TUint32 TResourceReaderImpl::ReadUint32L()
    {
	assert(iCurrentPtr != NULL);
    if (((TUint32)iCurrentPtr)%4)
        {
        TUint32 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TUint32));
    return(*(TUint32*)currentPtr);
    }

/** Copies a specified length of data from the resource buffer, starting at the 
current position within the buffer, into the location pointed to by a specified 
pointer. No assumption is made about the type of data at being read.

The current position within the resource buffer is updated.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@param aPtr Pointer to the target location for data copied from the resource buffer.
@param  aLength The length of data to be copied from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
void TResourceReaderImpl::ReadL(TAny* aPtr,TInt aLength)
    {
	assert(iCurrentPtr != NULL);
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+aLength);
    memcpy(aPtr,currentPtr,aLength);
    }