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
* base class for TLV (Type Length Value) classes found in SIS files
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __FIELD_H__
#define __FIELD_H__

#include "header.h"
#include "exception.h"
#include "element.h"




template <CSISFieldRoot::TFieldType FieldType> class CSISField : public CSISFieldRoot
	{
public:
	CSISField ();
	CSISField (const bool aRequired);
	CSISField (const CSISField& aInitialiser);
	
public:
	/**
	 * Retrieves the size of the structure including the header size.
	 * @param aInsideArray - whether the structure is part of an array or not. 
	 * @return byte count.
	 */
	virtual TFieldSize ByteCountWithHeader (const bool aInsideArray) const;
	/**
	 * Skip the file reading for this field. Read pointer will be moved to the
	 * next field to be read.
	 * @param aFile stream for which the data read should be skipped.
	 * @param aContainerSize size of the data to be skipped. 
	 */
	virtual void Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const;
	/**
	 * stream offset before the header
	 */
	virtual TSISStream::pos_type PreHeaderPos () const;
	/**
	 * stream offset after the header
	 */
	virtual TSISStream::pos_type PostHeaderPos () const;
	/**
	 * Set pre header stream offset
	 * @param aPos new offset
	 */
	void SetPreHeaderPos (const TSISStream::pos_type aPos);
	/**
	 * Set pre header stream offset
	 * @param aPos new offset
	 */
	void SetPostHeaderPos (const TSISStream::pos_type aPos);
	/**
	 * Checks whether this field is a mandatory field or not.
	 * @return true if the field is mandatory else false.
	 */
	bool Required () const;
	/**
	 * Set the required field
	 * @param aValue true if the field is mandatory else false.
	 */
	void SetRequired (bool aValue);

protected:
	bool IsDataThere (	TSISStream& aFile, 
						CSISHeader& aHeader, 
						const CSISFieldRoot::TFieldType aArrayType,
						const bool aPeek = false) const;

private:
	bool					iRequired;
	TSISStream::pos_type	iPreHeaderPos;
	TSISStream::pos_type	iPostHeaderPos;
	};




template <CSISFieldRoot::TFieldType FieldType> inline
		CSISField <FieldType>::CSISField () :
			iPreHeaderPos (0),
			iPostHeaderPos (0),
			iRequired (true)
	{ 
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		CSISField <FieldType>::CSISField (const bool aRequired) :
			iPreHeaderPos (0),
			iPostHeaderPos (0),
			iRequired (aRequired)
	{ 
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		CSISField <FieldType>::CSISField (const CSISField& aInitialiser) :
			iPreHeaderPos (aInitialiser.iPreHeaderPos),
			iPostHeaderPos (aInitialiser.iPostHeaderPos),
			iRequired (aInitialiser.Required ())
	{
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		bool CSISField <FieldType>::Required () const
	{ 
	return iRequired; 
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		void CSISField <FieldType>::SetRequired (bool aValue)
	{ 
	iRequired = aValue;
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		bool CSISField <FieldType>::IsDataThere (	
			TSISStream& aFile, CSISHeader& aHeader, const CSISFieldRoot::TFieldType aArrayType, const bool aPeek) const
	{
	if (CSISHeader::IsNextAsExpected (aFile, aHeader, FieldType, aArrayType, aPeek)) 
		{
		return true;
		}
	CSISException::ThrowIf (Required (), CSISException::EFileFormat, "expected data not found");
	return false;
	}


template <CSISFieldRoot::TFieldType FieldType>
		CSISFieldRoot::TFieldSize CSISField <FieldType>::ByteCountWithHeader (const bool aInsideArray) const
	{
	if (WasteOfSpace ()) 
		{
		return 0;
		}
	CSISFieldRoot::TFieldSize bytes (ByteCount (false));
	return AlignedSize (bytes) + CSISHeader (FieldType, bytes).ByteCount (aInsideArray);
	}


template <CSISFieldRoot::TFieldType FieldType>
		void CSISField <FieldType>::Skip (TSISStream& aFile, const TFieldSize& aContainerSize) const
	{
	CSISHeader header;
	if (! CSISHeader::IsNextAsExpected (aFile, header, FieldType, CSISFieldRoot::ESISUndefined, false)) 
		{
		CSISException::ThrowIf (Required (), CSISException::EFileFormat, "expected data missing");
		}
	else
		{
		CSISException::ThrowIf (header.DataSize () > aContainerSize,
								CSISException::EFileFormat,
								"data larger than container");
		aFile.seek (AlignedSize (header.DataSize ()), std::ios_base::cur);
		}
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		TSISStream::pos_type CSISField <FieldType>::PreHeaderPos () const
	{
	return iPreHeaderPos;
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		TSISStream::pos_type CSISField <FieldType>::PostHeaderPos () const
	{
	return iPostHeaderPos;
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CSISField <FieldType>::SetPreHeaderPos (const TSISStream::pos_type aPos)
	{
	iPreHeaderPos = aPos;
	assert ((iPostHeaderPos == 0) || (iPreHeaderPos <= iPostHeaderPos));
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CSISField <FieldType>::SetPostHeaderPos (const TSISStream::pos_type aPos)
	{
	iPostHeaderPos = aPos;
	assert ((iPreHeaderPos == 0) || (iPreHeaderPos <= iPostHeaderPos));
	}

#endif // __FIELD_H__

