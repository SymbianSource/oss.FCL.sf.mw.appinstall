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
* For storing data types in Type-Length-Value format.
* @internalComponent
*
*/



#ifndef	TLV_H
#define	TLV_H

#include <ostream>
#ifdef	_MSC_VER


static const int WCharSize = 22;
static int Base = 10;

// Compiler supports 64-bit types, but streams libraries do not.
inline std::basic_ostream<wchar_t>& operator << (std::basic_ostream<wchar_t>& aStream, const __int64& aInt64)
	{
	wchar_t i64 [WCharSize];
	return aStream << ::_i64tow (aInt64, i64, Base);
	}

inline std::basic_ostream<char>& operator << (std::basic_ostream<char>& aStream, const __int64& aInt64)
	{
	char i64 [WCharSize];
	return aStream << ::_i64toa (aInt64, i64, Base);
	}
#endif

#include "symbiantypes.h"
#include "sisfieldtypes.h"
#include "filecontents.h"

using namespace Swi::Sis;

/**
Causes of exceptions raised parsing a TLV
@internalComponent
*/
enum TLVExceptionCause
	{
	ETLVUnexpectedType,
	ETLVUnexpectedValue,
	};

/**
Exceptions raised parsing a TLV
@internalComponent
*/
class TLVException
	{
public:
	TLVExceptionCause	iCause;
	TFieldType		iExpectedType;
	TFieldType		iActualType;
	inline TLVException (TLVExceptionCause aCause, TFieldType aType)
		: iCause (aCause), iExpectedType (aType), iActualType (aType) {}
	inline TLVException (TLVExceptionCause aCause, TFieldType aType1, TFieldType aType2)
		: iCause (aCause), iExpectedType (aType1), iActualType (aType2) {}
	};

/**
Base class for a Type/Length/Value triplet; all SISxxx types inherit from this.
@internalComponent
*/
class TLV
	{
public:
	/** Obtains the L of this TLV */
	inline TFieldLength		Length () const { return iLength; }
	/** Obtains the raw V of ths TLV */
	const char*				BinaryData () const { return iBinary; }
	
	/** Called on exception */
	static void				DisplayError (TLVException*);
	
	/**
	Constructor; reads and checks the T, reads the L, checks and decrements the parent's L.
	The derived class is expected to consume the V.
	@param	aExpectedType		The expected T value for this TLV
	@param	aFileContents		The input stream
	@param	aRemainingLength	The remaining length of the surrounding (parent) TLV
	@param	aPresetType			If set, provides the already read T for this TLV
	*/
	TLV (TFieldType aExpectedType, CFileContents&, TFieldType = EFieldTypeInvalid);

public:
	CFileContents			iFileContents;
private:
	TFieldType				iType;
	TFieldLength			iLength;
	const char*				iBinary;
	static unsigned int		iDisplayIndent;
	};

#endif	/* __TLV_H__ */
