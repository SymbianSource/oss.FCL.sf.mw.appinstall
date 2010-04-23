/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility functions for copying and streaming RArrays. 
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __ARRAYUTILS_H_
#define __ARRAYUTILS_H_

#include <e32std.h>
#include <s32strm.h>

namespace Swi
{
	
/////////////////////////////////////////////////////////////////////
// RPointerArray utilities
/////////////////////////////////////////////////////////////////////

template <class T>
void CopyPointerArrayL(RPointerArray<T>& aNew, const RPointerArray<T>& aOld);

template <class T>
void ExternalizePointerArrayL(const RPointerArray<T>& aArray, RWriteStream& aStream);

template <class T>
void InternalizePointerArrayL(RPointerArray<T>& aArray, RReadStream& aStream);

/////////////////////////////////////////////////////////////////////
// RArray utilities
/////////////////////////////////////////////////////////////////////

template <class T>
void CopyArrayL(RArray<T>& aNew, const RArray<T>& aOld);

template <class T>
void ExternalizeArrayL(const RArray<T>& aArray, RWriteStream& aStream);

template <class T>
void InternalizeArrayL(RArray<T>& aArray, RReadStream& aStream);

/////////////////////////////////////////////////////////////////////
// Traits classes - internally required by RPointerArray functions
/////////////////////////////////////////////////////////////////////

template <class T>
class TTraits
	{
public:
	static T* CopyLC(const T& aOther) { return T::NewLC(aOther);}
	static T* ReadFromStreamLC(RReadStream& aStream) { return T::NewLC(aStream); }
	static void WriteToStreamL(const T& aItem, RWriteStream& aStream) { aStream << aItem; }
	};

// Specialisation for HBufs
template <>
class TTraits<HBufC16>
	{
public:
	static HBufC16* CopyLC(const HBufC16& aOther) { return aOther.AllocLC();}
	static HBufC16* ReadFromStreamLC(RReadStream& aStream) { return HBufC16::NewLC(aStream, KMaxTInt); }
	static void WriteToStreamL(const HBufC16& aItem, RWriteStream& aStream) { aStream << aItem; }
	};
		
template <>
class TTraits<HBufC8>
	{
public:
	static HBufC8* CopyLC(const HBufC8& aOther) { return aOther.AllocLC();}
	static HBufC8* ReadFromStreamLC(RReadStream& aStream) { return HBufC8::NewLC(aStream, KMaxTInt); }
	static void WriteToStreamL(const HBufC8& aItem, RWriteStream& aStream) { aStream << aItem; }
	};

#include "arrayutils.inl"
} // namespace Swi
#endif  // __ARRAYUTILS_H_
