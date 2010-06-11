/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* generic for integral types found in SIS files.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __RAW_H__
#define __RAW_H__

#include "fieldroot.h"



class CRaw : public CSISFieldRoot

	{
private:
	void Alloc (const TFieldSize& aSize);
	void Dispose ();

public:
	CRaw ();
	CRaw (const CRaw& aInitialiser);

	virtual ~CRaw ();
	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  
	virtual void Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const;
	virtual TFieldSize ByteCount (const bool aInsideArray) const;
	virtual void SetByteCount (const TFieldSize aSize);
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	virtual std::string Name () const;

	const TUint8* Data () const;
	TUint8* Data ();

private:
	 TUint8*	iData;
	 TFieldSize	iSize;
	};


inline CRaw::CRaw () :
		iData (NULL),
		iSize (0)
	{
	}


inline CRaw::~CRaw ()
	{
	Dispose ();
	}


inline const TUint8* CRaw::Data () const
	{
	assert ((iData == NULL) == (iSize == 0));
	return iData;
	}

inline TUint8* CRaw::Data ()
	{
	assert ((iData == NULL) == (iSize == 0));
	return iData;
	}

#endif // __RAW_H__

