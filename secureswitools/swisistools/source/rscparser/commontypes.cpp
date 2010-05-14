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
//
/** 
* @file commontypes.cpp
*
* @internalComponent
* @released
*/
#include "commontypes.h"
#include "barsc2.h"
#include <cassert>

/** 
 Implementation of Class Ptr8
*/

Ptr8::Ptr8(const TUint32& aSize)
		:iLength(0), iMaxLength(aSize)		
{
	iPtr= new TUint8[aSize];
}

Ptr8::~Ptr8()
{
	delete iPtr;
	iPtr=0;
}


TUint8 Ptr8::operator[](TInt aIndex) const
{
	assert(aIndex>=0);
	return(iPtr[aIndex]);

}

TUint8* Ptr8::GetPtr() const 
{
	return iPtr;
}

void Ptr8::SetPtr(TUint8* aPtr)
{
	iPtr=aPtr;
}

void Ptr8::ForceLength(const TUint32& aLength)
{
		iLength = aLength;
}

void Ptr8::SetLength(const TUint32& aLength)
{
	if ((iLength+aLength) <= iMaxLength)
	{
		iLength = aLength;
		return 0;
	}
	else
	{
		return 1;	
	}
}
	
TBool Ptr8::UpdateLength(const TUint32& aLength) 
{
	// Current length should not increase the 
	// Max length.
	if ((iLength+aLength) <= iMaxLength)
	{
		iLength+= aLength;
		return 0;
	}
	else
	{
		return 1;	
	}
}

TUint32 Ptr8::GetLength() const 
{
	return iLength;
}

TUint32 Ptr8::GetMaxLength() const 
{
	return iMaxLength;
}

void Ptr8::Append(TUint8* aBuffer, TInt aLen)
{
	if ((iLength+aLen) <= iMaxLength)
	{
		memcpy(iPtr+iLength,aBuffer, aLen);
		iLength+= aLen;
	}
	else
	{
		std::string errMsg= "Failed : Cannot Append Beyond Maximum Length";
		throw CResourceFileException(errMsg);	
	}
}

/** 
 Implementation of Class Ptr16
*/

Ptr16::Ptr16(const TUint32& aSize)
		:iLength(0), iMaxLength(aSize)		
{
	iPtr= new TUint16[aSize];
}

Ptr16::~Ptr16()
{
	delete iPtr;
	iPtr=0;
}

TUint16* Ptr16::GetPtr() const 
{
	return iPtr;
}

void Ptr16::SetPtr(TUint16* aPtr)
{
	iPtr=aPtr;
}

TUint16 Ptr16::operator[](TInt aIndex) const
{
	assert(aIndex>=0);
	return(iPtr[aIndex]);
}

TBool Ptr16::UpdateLength(const TUint32& aLength) 
{
	// Current length should not increase the 
	// Max length.
	if ((iLength+aLength) <= iMaxLength)
	{
		iLength+= aLength;
		return 0;
	}
	else
	{
		return 1;	
	}
}

TUint32 Ptr16::GetLength() const 
{
	return iLength;
}

/**
Implementation of Sructure PtrC8
*/
PtrC8::PtrC8():
	iMaxLength(0)
{	
	iPtr = new TUint8;
}

/**
Implementation of Sructure PtrC8
*/
PtrC8::~PtrC8()
{	
	 delete (TUint8*) iPtr;
	 iPtr = NULL;
}

/**
Implementation of Sructure PtrC16
*/
PtrC16::PtrC16():
	iMaxLength(0)
{	
	iPtr = new TUint16;
}

/**
Implementation of Sructure PtrC16
*/
PtrC16::~PtrC16()
{	
	delete iPtr;
	iPtr = NULL;
}


/**
 Implementation of Sructure TUid
*/
	
sTUid::sTUid():
	iUid1(0), iUid2(0), iUid3(0)
	{
	}
