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
* common functionality for containers in SIS files
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "field.h"
#include <vector>

template<class T, CSISFieldRoot::TFieldType FieldType> class CContainer : public CSISField <FieldType>
	{
public:
	typedef std::vector <T*>		TMem;
	typedef  typename TMem::iterator		TMemIter;
	typedef  typename TMem::const_iterator	TMemConIter;
	typedef  typename TMem::size_type		TMemSize;

public:
	/**
	 * Default constructor
	 */
	CContainer ();
	/**
	 * Explicit parameterized constructor
	 */
	explicit CContainer (const bool aRequired);
	/**
	 * Copy Constructor
	 */
	CContainer (const CContainer <T, FieldType>& c);
	/**
	 * Constructor. It also reserves space.
	 */
	CContainer (const CContainer <T, FieldType>& c, const TMemSize aReserve);

	/**
	 * Retrieves the size of the structure.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual CSISFieldRoot::TFieldSize ByteCount (const bool aIsArray) const;
	/**
	 * Sets the size of the structure.
	 * @param size - size of the structure. 
	 */
	virtual void SetByteCount (const CSISFieldRoot::TFieldSize size);
	/**
	 * This function verifies the structure
	 * @param aLanguage - language
	 */ 
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * If object not acceptable, modify it. Assumes, once modified, will not need modifying again
	 */
	virtual void MakeNeat ();
	/**
	 * Dump the entire content in hex format into the stream
	 */
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	/**
	 * Calculates CRC of the content
	 * @param aCRC CRC value of the content
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */ 
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	/**
	 * @return Item count
	 */
	TMemSize size () const
		{
		return iMem.size ();
		}
	/**
	 * Assign new items to the container.
	 * @param aBegin iterator start
	 * @param aEnd iterator end
	 */
	void assign (TMemConIter aBegin, TMemConIter aEnd);
	/**
	 * Clear the items
	 */
	void clear ();

	/**
	 * Iterator functionality
	 * @return iterator pointing to the begining of the item list
	 */
	TMemConIter begin () const
		{
		return iMem.begin ();
		}
	/**
	 * Iterator functionality
	 * @return iterator pointing to the end of the item list
	 */
	TMemConIter end () const
		{
		return iMem.end ();
		}
	/**
	 * Iterator functionality
	 * @return iterator pointing to the begining of the item list
	 */
	TMemIter begin ()
		{
		return iMem.begin ();
		}
	/**
	 * Iterator functionality
	 * @return iterator pointing to the end of the item list
	 */
	TMemIter end ()
		{
		return iMem.end ();
		}

	/**
	 * Retrieve the most recent item from the container and delete it from 
	 * the container list.
	 */
	void Pop ();

protected:
	const T& operator [] (const TMemSize& aIndex) const;
	T& operator [] (const TMemSize& aIndex);
	void Push (T* aData);
	const T& Last () const;
	T& Last ();

protected:
	TMem						iMem;
	CSISFieldRoot::TFieldSize	iSize;
	};

// macros for accessing template member variable and type 
#define ContainerMemSize(aType, aFieldType)  typename CContainer<aType, aFieldType>::TMemSize
#define ContainerMem(aType,aFieldType) CContainer<aType, aFieldType>::iMem
#define ContainerIter(aType) typename CContainer<aType, FieldType>::TMemConIter


template <class T, CSISFieldRoot::TFieldType FieldType> inline
	CContainer <T, FieldType>::CContainer () :
		iSize (0)
	{
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
	CContainer <T, FieldType>::CContainer (const bool aRequired) :
		iSize (0), 
		CSISField <FieldType> (aRequired)
	{
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
	CContainer <T, FieldType>::CContainer (const CContainer <T, FieldType>& c) :
		CSISField <FieldType> (c),
		iMem (c.iMem),
		iSize (c.iSize)
	{
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
	CContainer <T, FieldType>::CContainer (const CContainer <T, FieldType>& c, const TMemSize aReserve) :
		CSISField <FieldType> (c),
		iMem (aReserve),
		iSize (0)
	{
	}


template <class T, CSISFieldRoot::TFieldType FieldType> 
		void CContainer <T, FieldType>::clear ()
	{
	iMem.clear ();
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CContainer <T, FieldType>::assign (TMemConIter aBegin, TMemConIter aEnd)
	{
	iMem.assign (aBegin, aEnd);
	}


template <class T, CSISFieldRoot::TFieldType FieldType> 
		void CContainer <T, FieldType>::Verify (const TUint32 aLanguages) const
	{
	for (TMemConIter iterMemb = iMem.begin (); iterMemb != iMem.end (); iterMemb++)
		{
		assert (*iterMemb != NULL);
		(*iterMemb) -> Verify (aLanguages);
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CContainer <T, FieldType>::MakeNeat ()
	{
	for (TMemIter iterMemb = iMem.begin (); iterMemb != iMem.end (); iterMemb++)
		{
		assert (*iterMemb != NULL);
		(*iterMemb) -> MakeNeat ();
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CContainer <T, FieldType>::Dump (std::ostream& aStream, const int aLevel) const
	{
	for (TMemSize index = 0; index < size (); index++)
		{
		if (index > 0) 
			{
			aStream << std::string (aLevel, ' ');
			}
		if (iMem [index] -> Name ().empty ()) 
			{
			aStream << "  ";
			}
		else
			{
			aStream << iMem [index] -> Name ();
			if (FieldType == CSISFieldRoot::ESISArray) 
				{
				aStream << " [" << index << "]";
				}
			aStream << iMem [index] -> NoteIfOptional ();
			aStream << std::endl << std::string (aLevel + 2, ' ');
			}
		iMem [index] -> Dump (aStream, aLevel + 2);
		if (index + 1 < size ()) 
			{
			aStream << std::endl;
			}
		}
	}


template <class T, CSISFieldRoot::TFieldType FieldType>
		CSISFieldRoot::TFieldSize CContainer <T, FieldType>::ByteCount (const bool aIsArray) const
	{
	if (WasteOfSpace ()) 
		{
		return 0;
		}
	CSISFieldRoot::TFieldSize size = 0;
	for (TMemConIter iterMemb = iMem.begin (); iterMemb != iMem.end (); iterMemb++)
		{
		assert (*iterMemb != NULL);
		size += (*iterMemb) -> ByteCountWithHeader (aIsArray);
		}
	return size; 
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CContainer <T, FieldType>::SetByteCount (const CSISFieldRoot::TFieldSize aSize)
	{
	iSize = aSize;
	}


template <class T, CSISFieldRoot::TFieldType FieldType>
		void CContainer <T, FieldType>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{ 
	if(!WasteOfSpace())
		{
		for (TMemConIter iterMemb = iMem.begin (); iterMemb != iMem.end (); iterMemb++)
			{
			assert (*iterMemb != NULL);
			(*iterMemb) -> CalculateCrc (aCRC, aIsArrayElement);
			}
		CSISFieldRoot::PaddingCrc(aCRC, ByteCount(false));
		}
	}

#ifdef GENERATE_ERRORS
template <class T, CSISFieldRoot::TFieldType FieldType>
		void CContainer <T, FieldType>::CreateDefects ()
	{ 
	for (TMemConIter iterMemb = iMem.begin (); iterMemb != iMem.end (); iterMemb++)
		{
		assert (*iterMemb != NULL);
		(*iterMemb) -> CreateDefects ();
		}
	}
#endif // GENERATE_ERRORS


template <class T, CSISFieldRoot::TFieldType FieldType> 
		const T& CContainer <T, FieldType>::operator [] (const TMemSize& aIndex) const
	{
	assert (aIndex < size ());
	assert (iMem [aIndex] != NULL);
	return (*iMem [aIndex]);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> 
		T& CContainer <T, FieldType>::operator [] (const TMemSize& aIndex)
	{
	assert (aIndex < size ());
	assert (iMem [aIndex] != NULL);
	return (*iMem [aIndex]);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CContainer <T, FieldType>::Push (T* aData)
	{
	iMem.push_back (aData);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		const T& CContainer <T, FieldType>::Last () const
	{
	assert (size () > 0);
	assert (iMem [iMem.size () - 1] != NULL);
	return * (iMem [iMem.size () - 1]);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		T& CContainer <T, FieldType>::Last ()
	{
	assert (size () > 0);
	assert (iMem [iMem.size () - 1] != NULL);
	return * (iMem [iMem.size () - 1]);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CContainer <T, FieldType>::Pop ()
	{
	assert (size () > 0);
	assert (iMem [iMem.size () - 1] != NULL);
	T* ptr = iMem [iMem.size () - 1];
	iMem.pop_back ();
	delete ptr;
	}

#endif // __SISXTYPE_H__

