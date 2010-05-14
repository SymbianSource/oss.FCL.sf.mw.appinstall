/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* generic for intergral types found in SIS files.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include "fieldroot.h"
#include "exception.h"

#include <strstream>


template<typename T> class CSISBasicType : public CSISFieldRoot

	{
public:
	CSISBasicType (const T aValue = 0);

	virtual void Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  
	virtual void Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const;
	virtual TFieldSize ByteCount (const bool aInsideArray) const;
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	virtual std::string Name () const;

	operator T () const;
	bool operator == (const CSISBasicType<T> aCompare) const;
	CSISBasicType& operator = (const T aInitialiser);
	CSISBasicType& operator += (const T aInitialiser);
	CSISBasicType& operator |= (const T aInitialiser);

	T Value () const;

private:
	T	iData;
	};



template<typename T> inline
		CSISBasicType<T>::CSISBasicType (const T aValue) : 
			iData (aValue)	
	{
	}

template<typename T> inline CSISBasicType<T>::operator T ()	const
	{
	return iData;
	}

template<typename T> inline T CSISBasicType<T>::Value () const
	{
	return iData;
	}

template<typename T> inline CSISBasicType<T>& CSISBasicType<T>::operator = (const T aValue)
	{
	iData = aValue; 
	return *this; 
	}

template<typename T> inline CSISBasicType<T>& CSISBasicType<T>::operator += (const T aValue)
	{
	iData += aValue; 
	return *this; 
	}

template<typename T> inline CSISBasicType<T>& CSISBasicType<T>::operator |= (const T aValue)
	{
	iData |= aValue; 
	return *this; 
	}

template<typename T> inline bool CSISBasicType<T>::operator == (const CSISBasicType<T> aValue) const
	{
	return iData == aValue.iData;
	}

template<typename T> inline void CSISBasicType<T>::Read (TSISStream& aFile, const TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	CSISException::ThrowIf (sizeof (T) > aContainerSize,
							CSISException::EFileFormat,
							"data larger than container");
	aFile.read (reinterpret_cast <TUint8*> (&iData), sizeof (T));
	}

template<typename T> inline void CSISBasicType<T>::Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const
	{
	CSISException::ThrowIf (sizeof (T) > aContainerSize,
							CSISException::EFileFormat,
							"data larger than container");
	aFile.seek (sizeof (T), std::ios_base::cur);
	}

template<typename T> inline void CSISBasicType<T>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	aFile.write (reinterpret_cast <const TUint8*> (&iData), sizeof (T));
	}
	
#ifdef GENERATE_ERRORS
template<> inline void CSISBasicType <TUint64>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	TUint64 value (iData);
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugBigEndian))
		{
		value = ((value & 0xFFFFFFFF00000000) >> 32) | ((value & 0xFFFFFFFF) << 32);
		}
	aFile.write (reinterpret_cast <const TUint8*> (&value), sizeof (TUint64));
	}
	
template<> inline void CSISBasicType <TUint32>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	TUint32 value (iData);
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugBigEndian))
		{
		value = ((value & 0xFFFF0000) >> 16) | ((value & 0xFFFF) << 16);
 		}
	aFile.write (reinterpret_cast <const TUint8*> (&value), sizeof (TUint32));
	}
	
template<> inline void CSISBasicType <TInt32>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	TInt32 value (iData);
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugBigEndian))
		{
		value = ((value & 0xFFFF0000) >> 16) | ((value & 0xFFFF) << 16);
 		}
	aFile.write (reinterpret_cast <const TUint8*> (&value), sizeof (TInt32));
	}
	
template<> inline void CSISBasicType <TUint16>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	TUint16 value (iData);
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugBigEndian))
		{
		value = ((value & 0xFF00) >> 8) + ((value & 0xFF) << 8);
 		}
	aFile.write (reinterpret_cast <const TUint8*> (&value), sizeof (TUint16));
	}
#endif // GENERATE_ERRORS
	
template<typename T> inline CSISFieldRoot::TFieldSize CSISBasicType<T>::ByteCount (const bool aInsideArray) const
	{
	return sizeof (T);
	}

template<typename T> inline void CSISBasicType<T>::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream << static_cast <int> (iData);
	}

template<typename T> inline void CSISBasicType<T>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	CSISFieldRoot::DoTheCrc (aCRC, reinterpret_cast <const TUint8*> (&iData), sizeof (T));
	}


template<typename T> inline std::string CSISBasicType<T>::Name () const
	{
	std::strstream brook;
	brook << sizeof (T) << " byte number";
	return std::string (brook.str (), brook.pcount ());
	}


typedef CSISBasicType<TUint8>	CSISUInt8;
typedef CSISBasicType<TUint16>	CSISUInt16;
typedef CSISBasicType<TUint32>	CSISUInt32;
typedef CSISBasicType<TInt32>	CSISInt32;
typedef CSISBasicType<TUint64>	CSISUInt64;


#endif // __NUMERIC_H__

