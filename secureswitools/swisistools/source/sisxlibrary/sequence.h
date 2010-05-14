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
* zero or more of a specific SIS structure
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include "container.h"
#include "element.h"

#define SequenceMemSize(aType, aFieldType) ContainerMemSize(CElement <aType>, aFieldType)
#define SequenceMem(aType, aFieldType) ContainerMem(CElement <aType>, aFieldType)
#define SequenceIter(aType) ContainerIter(CElement <aType>)

template <class T, CSISFieldRoot::TFieldType FieldType> class CSequence : public CContainer <CElement <T>, FieldType> 
	// zero or more occurences of a SISField of the given type
	{
public:
	CSequence ();

	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	virtual void Skip (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize) const;
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;
	virtual CSISFieldRoot::TFieldSize ByteCount (const bool aIsArray) const;
	virtual CSISFieldRoot::TFieldSize ByteCountWithHeader (const bool aInsideArray) const;
	virtual void SkipOldWriteNew (TSISStream& aFile) const;

	const  T& operator [] (const SequenceMemSize(T, FieldType) &aIndex) const;
	T& operator [] (const SequenceMemSize(T, FieldType) &aIndex);

	void Push (const T& aData);
	const T& Last () const;
	T& Last ();

private:
	TUint32		iLastRead;
	};



template <class T, CSISFieldRoot::TFieldType FieldType> inline
		CSequence <T, FieldType>::CSequence () :
			iLastRead (0),
			CContainer <CElement <T>, FieldType> (false)
	{
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSequence <T, FieldType>::Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	assert (	(aArrayType == CSISFieldRoot::ESISUndefined) || 
				(aArrayType == CSISFieldRoot::ESISUnknown));	// sequences in arrays don't work
	TSISStream::pos_type pos = aFile.tell ();
	SetPreHeaderPos (pos);
	SetPostHeaderPos (pos);
	CSISHeader header (FieldType);
	CSISFieldRoot::TFieldSize available = aContainerSize;
	iLastRead = 0;
	while ((available > 0) && IsDataThere (aFile, header, aArrayType, true))
		{
		T* item = new T ();
		try {
			item -> Read (aFile, header.DataSize ());
			SequenceMem(T,FieldType).push_back (new CElement <T> (item));
			}
		catch (...)
			{
			delete item;
			throw;
			}
		available = available - item -> ByteCountWithHeader (false);
		iLastRead++;
		}
	ReadFiller (aFile);
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSequence <T, FieldType>::Skip (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize) const
	{
	CSISHeader header (FieldType);
	CSISFieldRoot::TFieldSize available = aContainerSize;
	while ((available > 0) && IsDataThere (aFile, header, CSISFieldRoot::ESISUndefined, true))
		{
		CSISFieldRoot::TFieldSize size = AlignedSize (header.DataSize ());
		aFile.seek (size + header.ByteCount (false), std::ios_base::cur);
		available = available - size;
		}
	ReadFiller (aFile);
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSequence <T, FieldType>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	assert (! aIsArrayElement);	// sequences in arrays don't work
	for (SequenceIter(T) iterMemb = SequenceMem(T,FieldType).begin (); iterMemb != SequenceMem(T,FieldType).end (); iterMemb++)
		{
		(**iterMemb) -> Write (aFile, false);
		}
	WriteFiller (aFile);
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		CSISFieldRoot::TFieldSize CSequence <T, FieldType>::ByteCount (const bool aIsArray) const
	{
	assert (! aIsArray);
	return CContainer <CElement <T>, FieldType>::ByteCount (false);
	}

template <class T, CSISFieldRoot::TFieldType FieldType>
		CSISFieldRoot::TFieldSize CSequence <T, FieldType>::ByteCountWithHeader (const bool aInsideArray) const
	{
	assert (! aInsideArray);
	return ByteCount (aInsideArray);
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		void CSequence <T, FieldType>::Push (const T& aData)
	{
	T* ptr = new T (aData);
	try 
		{
		CContainer<CElement <T>, FieldType>::Push (new CElement <T> (ptr));
		}
	catch (...)
		{
		delete ptr;
		throw;
		}
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		const T& CSequence <T, FieldType>::Last () const
	{
	return * (CContainer <CElement <T>, FieldType>::Last ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		T& CSequence <T, FieldType>::Last ()
	{
	return * (CContainer <CElement <T>, FieldType>::Last ());
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		const T& CSequence <T, FieldType>::operator [] (const SequenceMemSize(T,FieldType)& aIndex) const
	{
	return * (CContainer <CElement <T>, FieldType>::operator [] (aIndex));
	}

template <class T, CSISFieldRoot::TFieldType FieldType> inline
		T& CSequence <T, FieldType>::operator [] (const SequenceMemSize(T,FieldType)& aIndex)
	{
	return * (CContainer <CElement <T>, FieldType>::operator [] (aIndex));
	}


template <class T, CSISFieldRoot::TFieldType FieldType>
		void CSequence <T, FieldType>::SkipOldWriteNew (TSISStream& aFile) const
	{
	TUint32 count = 0;
	for (SequenceIter(T) iterMemb = SequenceMem(T,FieldType).begin (); iterMemb != SequenceMem(T,FieldType).end (); iterMemb++)
		{
		if (count++ >= iLastRead)
			{
			(**iterMemb) -> Write (aFile, false);
			}
		}
	WriteFiller (aFile);
	}

#endif // SEQUENCE

