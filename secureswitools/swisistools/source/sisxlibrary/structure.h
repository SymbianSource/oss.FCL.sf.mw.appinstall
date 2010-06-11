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
* generic SIS structure container, provides common functionality on members, see fieldroot.h for purpose of functions
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include "container.h"
#include "fieldroot.h"

template <CSISFieldRoot::TFieldType FieldType> class CStructure : public CContainer <CSISFieldRoot, FieldType>

	{
public:
	CStructure ();
	CStructure (const bool aRequired);
	CStructure (const CStructure& s);

	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;  
	virtual CSISFieldRoot::TFieldSize ByteCount (const bool aIsArray) const;
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;

	TUint64 SkipRead (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType, const TUint32 aSkipCount, const bool aCompleteWithNormalRead);
	void RawSkipWrite (TSISStream& aFile, const bool aIsArrayElement, TUint8* aRawBuffer, const TUint64 aBufferSize, const TUint32 aSkipCount) const;
	
protected:
	void InsertMember (CSISFieldRoot& aMember);	
		// intended to be used by inheriting constructors *ONLY*

	};



template <CSISFieldRoot::TFieldType FieldType> inline
	CStructure <FieldType>::CStructure ()
	{
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		CStructure <FieldType>::CStructure (const bool aRequired) :
			CContainer<CSISFieldRoot, FieldType> (aRequired)
	{
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		CStructure <FieldType>::CStructure (const CStructure& s) :
			CContainer<CSISFieldRoot, FieldType> (s, 0)
	{ 
	}



template <CSISFieldRoot::TFieldType FieldType>
		void CStructure <FieldType>::Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	SetPreHeaderPos (aFile.tell ());

	CSISHeader header (aArrayType);
	if (IsDataThere (aFile, header, aArrayType))
		{
		SetByteCount (header.DataSize ());
		TSISStream::pos_type pos = aFile.tell ();

		SetPostHeaderPos (pos);
#ifdef _DEBUG
		TSISStream::pos_type expected = pos + static_cast <TSISStream::pos_type> (AlignedSize (header.DataSize ()));
#endif
		TUint64 available = aContainerSize;
		for (ContainerIter(CSISFieldRoot) iterMemb = ContainerMem(CSISFieldRoot, FieldType).begin(); 
				iterMemb != ContainerMem(CSISFieldRoot, FieldType).end(); 
				++iterMemb)
			{
			(*iterMemb) -> Read (aFile, available);
			available = aContainerSize - (aFile.tell () - pos);
			}
		ReadFiller (aFile);
#ifdef _DEBUG
		TSISStream::pos_type got = aFile.tell ();
		assert (expected == got);
#endif
		}
	}


template <CSISFieldRoot::TFieldType FieldType>
		TUint64 CStructure <FieldType>::SkipRead (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType, const TUint32 aSkipCount, const bool aCompleteWithNormalRead)
	{
	CSISHeader header (CSISFieldRoot::ESISUndefined);
	TUint32 skipped = 0;
	TUint64 offset = 0;
	if (IsDataThere (aFile, header, aArrayType))
		{
		SetByteCount (header.DataSize ());
		TSISStream::pos_type pos = aFile.tell ();
#ifdef _DEBUG
		TSISStream::pos_type expected = pos + static_cast <TSISStream::pos_type> (AlignedSize (header.DataSize ()));
#endif
		TUint64 available = aContainerSize;
		for (ContainerIter(CSISFieldRoot) iterMemb = ContainerMem(CSISFieldRoot,FieldType).begin (); iterMemb != ContainerMem(CSISFieldRoot, FieldType).end (); iterMemb++)
			{
			if (! aCompleteWithNormalRead || (skipped < aSkipCount))
				{
				(*iterMemb) -> Skip (aFile, available);
				if (++skipped == aSkipCount)
					{
					offset = static_cast <TUint64> (aFile.tell ()) - pos;
					assert (AlignedSize (offset) == offset);
					}
				}
			else
				{
				(*iterMemb) -> Read (aFile, available);
				}
			available = aContainerSize - (aFile.tell () - pos);
			}
		ReadFiller (aFile);
#ifdef _DEBUG
		TSISStream::pos_type got = aFile.tell ();
		assert (expected == got);
#endif
		}
	return offset;
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CStructure <FieldType>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
#ifdef GENERATE_ERRORS
	unsigned int unknownLen = rand()%1000;
#endif // GENERATE_ERRORS

	bool writeData = ! WasteOfSpace ();
	if (writeData)
		{
#ifdef GENERATE_ERRORS
		bool unknownField = CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugUnknownField);
#endif // GENERATE_ERRORS
#ifdef _DEBUG
		TSISStream::pos_type before = aFile.tell ();
#endif
		CSISHeader (
#ifdef GENERATE_ERRORS
						static_cast <const CSISFieldRoot::TFieldType> (unknownField ? 
							static_cast <TUint32> (rand () % 0x0FFF + CSISFieldRoot::EBugUnknownField) :
							FieldType),
#else // GENERATE_ERRORS
						FieldType, 
#endif // GENERATE_ERRORS
						ByteCount (false)).Write (aFile, aIsArrayElement);
#ifdef _DEBUG
		TSISStream::pos_type pos = aFile.tell ();
		TSISStream::pos_type expected = aFile.tell () + static_cast <TSISStream::pos_type> (AlignedSize (ByteCount (false)));
#endif
		for (ContainerIter(CSISFieldRoot) iterMemb = ContainerMem(CSISFieldRoot,FieldType).begin(); iterMemb != ContainerMem(CSISFieldRoot,FieldType).end(); iterMemb++)
			{
#ifdef GENERATE_ERRORS
			if (! CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugMissingField))
				{
				if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugUnexpectedField))
					{
					(*iterMemb) -> Write (aFile, false);
					}
				(*iterMemb) -> Write (aFile, false);
				}
			if (CSISFieldRoot::IsBugToBeCreated(CSISFieldRoot::EBugUnknownData))
				{
				CSISFieldRoot::AddRawDataValue(aFile,unknownLen);
				}
#else // GENERATE_ERRORS
			(*iterMemb) -> Write (aFile, false);

#endif // GENERATE_ERRORS
#ifdef _DEBUG
			TSISStream::pos_type ici = aFile.tell ();
#endif
			}
		TSISStream::pos_type beforeFiller = aFile.tell ();
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
				CSISHeader (FieldType, beforeFiller - pos).Write (aFile, aIsArrayElement);
				aFile.seek (actual);
				}
			}
#else
			assert (expected == actual);
#endif  // GENERATE_ERRORS
#endif  // DEBUG
		}
	}

template <CSISFieldRoot::TFieldType FieldType>
		void  CStructure <FieldType>::RawSkipWrite (TSISStream& aFile, const bool aIsArrayElement, TUint8* aRawBuffer, const TUint64 aBufferSize, const TUint32 aSkipCount) const
	{
	assert (! WasteOfSpace ());	// not tested
	assert ((! aBufferSize) ^ (aRawBuffer != NULL));
	TUint32 skipped = 0;
	TUint64 before = static_cast <TUint64> (aFile.tell ());
	CSISHeader (FieldType, 0).Write (aFile, aIsArrayElement);
	TUint64 after = static_cast <TUint64> (aFile.tell ());
	if (aRawBuffer)
		{
		aFile.write (aRawBuffer, aBufferSize);
		WriteFiller (aFile);
		}
	for (ContainerIter(CSISFieldRoot) iterMemb = ContainerMem(CSISFieldRoot,FieldType).begin (); iterMemb != ContainerMem(CSISFieldRoot,FieldType).end (); iterMemb++)
		{
		if (skipped++ >= aSkipCount)
			{
			(*iterMemb) -> Write (aFile, false);
			}
		else
		if (skipped == aSkipCount)
			{
			(*iterMemb) -> SkipOldWriteNew (aFile);
			}
		}
	WriteFiller (aFile);
	TUint64 actual = static_cast <TUint64> (aFile.tell ());
	CSISException::ThrowIf (CSISHeader::IsBigHeader (actual - after), 
							CSISException::EFileFormat,
							"inconsistent structure size"); // why is SISController so huge;
	aFile.seek (before); 
	CSISHeader (FieldType, actual - after).Write (aFile, aIsArrayElement);
	aFile.seek (actual);
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CStructure <FieldType>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	if(!WasteOfSpace())
		{
		CSISHeader (FieldType, ByteCount (false)).CalculateCrc (aCRC, aIsArrayElement); 
		CContainer<CSISFieldRoot, FieldType>::CalculateCrc (aCRC, false);
		}
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CStructure <FieldType>::InsertMember (CSISFieldRoot& aMember)
	{
	ContainerMem(CSISFieldRoot,FieldType).push_back (&aMember);
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		CSISFieldRoot::TFieldSize CStructure <FieldType>::ByteCount (const bool aIsArray) const
	{
	assert (! aIsArray);
	return CContainer<CSISFieldRoot, FieldType>::ByteCount (false);
	}


#endif // __STRUCTURE_H__

