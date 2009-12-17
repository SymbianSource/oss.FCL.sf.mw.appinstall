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
* For parsing contents of a SIS file.
* @internalComponent
*
*/



#ifndef	FILE_CONTENTS_H
#define	FILE_CONTENTS_H

#include "symbiantypes.h"
#include "sisfieldtypes.h"
#include <iostream>

/**
Operations on the contents of a SIS file or part thereof with checking of TLV scope
@see tlv.h
@internalComponent
*/
class CFileContents
	{
public:
	CFileContents (const char* aFilename);
	CFileContents (const char* aBuffer, Swi::Sis::TFieldLength aBufferLength);
	CFileContents (const CFileContents& aContents);
	~CFileContents ();

	/**
	Constrains the length
	*/
	void SetRemainingLength (Swi::Sis::TFieldLength aRemainingLength);

	/**
	Gets the remaining length
	*/
	Swi::Sis::TFieldLength GetRemainingLength () const;

	/**
	Reads a TLV Type field
	*/
	Swi::Sis::TFieldType GetType ();
	/**
	Reads a TLV Type field without advancing the read pointer
	*/
	Swi::Sis::TFieldType PeekType ();
	/**
	Reads a TLV Length field which may be 4 or 8 octets long
	*/
	Swi::Sis::TFieldLength GetLength ();
	/**
	Reads an arbitrary byte string and returns the ownership
	@param aLength The number of bytes to be read
	*/
	char* GetByteString (Swi::Sis::TFieldLength aLength);
	/**
	Reads an arbitrary Unicode string and returns the ownership
	@param aLength The number of bytes to be read
	*/
	const wchar_t*	GetUnicodeString (Swi::Sis::TFieldLength aLengthInBytes);
	/**
	Reads the next 8 octets of data
	*/
	TInt64 GetTInt64 ();
	/**
	Reads the next 4 octets of data
	*/
	TUint32 GetTUint32 ();
	/**
	Reads the next 2 octets of data
	*/
	TUint16 GetTUint16 ();
	/**
	Reads the next octet of data
	*/
	TUint8 GetTUint8 ();
	/**
	Copies aLength octets to a new file
	*/
	void CopyToFile (const char* aFileName, Swi::Sis::TFieldLength aLength);

	/**
	Advances the notional read pointer as if aOffset bytes had been read
	*/
	void Advance (Swi::Sis::TFieldLength aOffset);

	/**
	Computes the length of this instance, rounded up to a quad-octet boundary
	*/
	Swi::Sis::TFieldLength AlignedLength ();

	const char*	GetCurrentAddress () const;

private:
	const char*	iBase;
	const char*	iCurrent;
	Swi::Sis::TFieldLength	iRemainingLength;
	bool iOwnBase;		// Whether the memory should be deleted on close

	};

#endif	/* __FILE_CONTENTS_H__ */
