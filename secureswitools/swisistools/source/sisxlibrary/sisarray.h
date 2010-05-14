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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISARRAY_H__
#define __SISARRAY_H__

#include "container.h"
#include "element.h"

#include <strstream>

#define SisArrayMemSize(aType) ContainerMemSize(CElement <aType>, CSISFieldRoot::ESISArray)
#define SisArrayMem(aType) ContainerMem(CElement <aType>, CSISFieldRoot::ESISArray)
#define SisArrayIter(aType) ContainerIter(CElement <aType>)

template <class T, CSISFieldRoot::TFieldType FieldType> class CSISArray :
	public CContainer <CElement <T>, CSISFieldRoot::ESISArray>
	{
public:
	typedef CSISUInt32	TMemberCount;

public:
	/**
	 * Default Constructor
	 */
	CSISArray ();
	/**
	 * Parameterised constructor.
	 * @param - Tells if the structure is required or not.
	 */
	CSISArray (const bool aRequired);
	
	/**
	 * Internalize the array content
	 * @param aFile File stream from where data needs to be read.
	 * @param aContainerSize size of the content to be read.
	 * @param aArrayType Type of the array 
	 */
	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * Externalize the array content
	 * @param aFile File stream to which the data needs to be written.
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;
	/**
	 * Retrieves the size of the structure.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual CSISFieldRoot::TFieldSize ByteCount (const bool aInsideArray) const;
	/**
	 * Sets the size of the structure.
	 * @param size - size of the structure. 
	 */
	virtual void SetByteCount (const CSISFieldRoot::TFieldSize size);
	/**
	 * Calculate CRC for this structure.
	 * @param aCRC CRC value of the structure.
	 * @param aIsArrayElement whether the structure is part of an array or not.  
	 */
	virtual void CalculateCrc (TUint16& aCRC, const bool aIsArrayElement) const;
	/**
	 * Class Name
	 */
	virtual std::string Name () const;
	/**
	 * Adds package entry related to this structure.
	 * @param aStream stream into which the package details need to be written.
	 * @param aVerbose If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	
	/**
	 * Operator to access content of this array
	 * @param aIndex index of the item to be retrieved.
	 */
	const T& operator [] (const SisArrayMemSize(T)& aIndex) const;
	/**
	 * Operator to access content of this array
	 * @param aIndex index of the item to be retrieved.
	 */
	T& operator [] (const SisArrayMemSize(T)& aIndex);

	/**
	 * Adds the element into the array
	 * @param aData element to be added into the array
	 */
	void Push (const T& aData);
	/**
	 * Adds an empty element into the array
	 */
	void Push ();
	/**
	 * Retrieves the last element of this array
	 * @return array element
	 */
	const T& Last () const;
	/**
	 * Retrieves the last element of this array
	 * @return array element
	 */
	T& Last ();
	};




template <class T, CSISFieldRoot::TFieldType FieldType> inline
		CSISArray <T, FieldType>::CSISArray ()
	{
	}


template <class T, CSISFieldRoot::TFieldType FieldType> inline
		CSISArray <T, FieldType>::CSISArray (const bool aRequired) :
			CContainer <CElement <T>, CSISFieldRoot::ESISArray> (aRequired)
	{
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSISArray <T, FieldType>::Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	SetPreHeaderPos (aFile.tell ());

	CSISHeader header (aArrayType);
	if (! IsDataThere (aFile, header, aArrayType))
		{
		if (! Required () && (header.DataSize () == 0))
			{
			return;
			}
		throw CSISException (CSISException::EFileFormat, "Array expected");
		}
	TSISStream::pos_type pos = aFile.tell ();

	SetPostHeaderPos (pos);

	TSISStream::pos_type done = aFile.tell () + static_cast <TSISStream::pos_type> (AlignedSize (header.DataSize ()));
	CSISFieldRoot::TFieldType type;
	aFile >> type;
	CSISException::ThrowIf (type != FieldType, CSISException::EFileFormat, "unexpected element type");
#ifdef _DEBUG
	pos = aFile.tell ();
	assert (done >= pos);
#endif
	while (done > aFile.tell ())
		{
		Push (T ());
		Last ().Read (aFile, header.DataSize (), FieldType);
		ReadFiller (aFile);
		}
#ifdef _DEBUG
	TSISStream::pos_type here = aFile.tell ();
	assert (done == here);
#endif
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSISArray <T, FieldType>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	if (! WasteOfSpace ())
		{
#ifdef _DEBUG
		TSISStream::pos_type before = aFile.tell ();
#endif
#ifdef GENERATE_ERRORS
		bool arrLenBug; 
		if (arrLenBug = CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugArrayCount))
			{
			CSISHeader (CSISFieldRoot::ESISArray, rand ()).Write (aFile, aIsArrayElement);
			}
		else
#endif // GENERATE_ERRORS
			{
			CSISHeader (CSISFieldRoot::ESISArray, ByteCount (true)).Write (aFile, aIsArrayElement);
			}
#ifdef _DEBUG
		TSISStream::pos_type pos = aFile.tell ();
		TSISStream::pos_type expected = pos + static_cast <TSISStream::pos_type> (AlignedSize (ByteCount (true)));
#endif
		aFile << FieldType;
		for (SisArrayIter(T) iterMemb = SisArrayMem(T).begin (); iterMemb != SisArrayMem(T).end (); iterMemb++)
			{
			(**iterMemb) -> Write (aFile, true);
			}
		TSISStream::pos_type beforeFiller = aFile.tell();
		WriteFiller (aFile);
#ifdef _DEBUG
		TSISStream::pos_type actual = aFile.tell ();

#ifdef GENERATE_ERRORS
		if (! IsAnyBugSet ())
			{
			assert (expected == actual);
			}
		else 
			{
			if (expected != actual)
				{
				aFile.seek (before);
				// If we wanted to corrupt the array length, then we don't much care here.
				// if the length we expected is wrong
				if (!arrLenBug)
					{
					CSISHeader (CSISFieldRoot::ESISArray, beforeFiller - pos).Write (aFile, aIsArrayElement);
					}
				aFile.seek (actual);
				}
			}

#else
		assert (expected == actual);
#endif //GENERATE_ERRORS
#endif // DEBUG
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		CSISFieldRoot::TFieldSize CSISArray<T, FieldType>::ByteCount (const bool aInsideArray) const
	{
	if (WasteOfSpace ())
		{
		return 0;
		}
	return	CSISUInt32 (FieldType).ByteCount (aInsideArray) +
			CContainer <CElement <T>, CSISFieldRoot::ESISArray>::ByteCount (true);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> 
		void CSISArray <T, FieldType>::SetByteCount (const CSISFieldRoot::TFieldSize aSize)
	{
	CContainer <CElement <T>, CSISFieldRoot::ESISArray>::SetByteCount (aSize - CSISFieldRoot::SizeOfFieldType ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSISArray <T, FieldType>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	if(!WasteOfSpace())
		{	
		CSISHeader (CSISFieldRoot::ESISArray, ByteCount (false)).CalculateCrc (aCRC, aIsArrayElement); 
		CSISUInt32 (FieldType).CalculateCrc (aCRC, false);
		CContainer <CElement <T>, CSISFieldRoot::ESISArray>::CalculateCrc (aCRC, true);
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CSISArray<T, FieldType>::Push (const T& aData)
	{
	T* ptr = new T (aData);
	try 
		{
		CContainer <CElement <T>, CSISFieldRoot::ESISArray>::Push (new CElement <T> (ptr));
		} 
	catch (...)
		{
		delete ptr;
		throw;
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CSISArray<T, FieldType>::Push ()
	{
	T* ptr = new T ();
	try 
		{
		CContainer <CElement <T>, CSISFieldRoot::ESISArray>::Push (new CElement <T> (ptr));
		} 
	catch (...)
		{
		delete ptr;
		throw;
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		const T& CSISArray<T, FieldType>::Last () const
	{
	return * (CContainer <CElement <T>, CSISFieldRoot::ESISArray>::Last ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		T& CSISArray<T, FieldType>::Last ()
	{
	return * (CContainer <CElement <T>, CSISFieldRoot::ESISArray>::Last ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		std::string CSISArray<T, FieldType>::Name () const
	{
	std::strstream brook;
	brook << "Array of " << size ();
	return std::string (brook.str (), brook.pcount ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		const T& CSISArray<T, FieldType>::operator [] (const SisArrayMemSize(T)& aIndex) const
	{
	return * (CContainer <CElement <T>, CSISFieldRoot::ESISArray>::operator [] (aIndex));
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		T& CSISArray<T, FieldType>::operator [] (const SisArrayMemSize(T)& aIndex)
	{
	return * (CContainer <CElement <T>, CSISFieldRoot::ESISArray>::operator [] (aIndex));
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSISArray <T, FieldType>::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	for (SisArrayIter(T) iterMemb = SisArrayMem(T).begin (); iterMemb != SisArrayMem(T).end (); iterMemb++)
		{
		(**iterMemb)->AddPackageEntry(aStream, aVerbose);
		}
	}

#endif // __SISARRAY_H__

