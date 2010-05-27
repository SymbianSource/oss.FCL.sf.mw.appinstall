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
* generic base for all SIS binary large objects
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __BLOB_H__
#define __BLOB_H__

#include <iostream>
#include <algorithm>
#include <assert.h>
#include "utility_interface.h"

#include "field.h"
#include "exception.h"
#include "utility.h"

template <CSISFieldRoot::TFieldType FieldType> class CBlob : public CSISField <FieldType>
	{
public:
	/**
	 * Default constructor.
	 */
	CBlob ();
	/**
	 * Copy constructor
	 */
	CBlob (const CBlob& aInitialiser);
	/**
	 * Destructor
	 */
	virtual ~CBlob ();
	
public:
	/**
	 * Internalize the class
	 * @param aFile File stream from where data needs to be read.
	 * @param aContainerSize size of the content to be read.
	 * @param aArrayType Type of the array 
	 */
	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * Externalize the class
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
	virtual void SetByteCount (const CSISFieldRoot::TFieldSize aSize);
	/**
	 * Dump the entire content in hex format into the stream
	 */
	virtual void Dump (std::ostream& aStream, const int aLevel) const;
	/**
	 * Checks if the structure is required or not. Used for externalizing
	 * the class. If its a waste of space then we need not write this into
	 * the file.
	 */
	virtual bool WasteOfSpace () const;
	/**
	 * This function verifies the structure
	 * @param aLanguage - language
	 */ 
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Calculates CRC of the content
	 * @param aCRC CRC value of the content
	 * @param aIsArrayElement whether the structure is part of an array or not. 
	 */ 
	virtual void CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;

	/**
	 * Named destructor. Deletes the buffer and resets the state.
	 */
	void Dispose ();
	/**
	 * Load the file into memory. 
	 * @param aFileName file name which needs to be loaded
	 * @param size file size.
	 */
	void Load (const std::wstring& aFileName, TUint64* size = NULL);
	/**
	 * @return data buffer  
	 */
	const TUint8* Data () const;
	/**
	 * @return data buffer  
	 */
	TUint8* Data ();
	/**
	 * Replace the existing data with this data.
	 * @param aData data pointer
	 * @param aSize size of data
	 */
	void Assign (const TUint8* aData, const CSISFieldRoot::TFieldSize aSize);
	/**
	 * Append the data after the existing data.
	 * @param aData data pointer
	 * @param aSize size of data
	 */
	void Append (const TUint8* aData, const CSISFieldRoot::TFieldSize aSize);
	/**
	 * Append the data before the existing data.
	 * @param aData data pointer
	 * @param aSize size of data
	 */
	void Prepend(const TUint8* aData, const CSISFieldRoot::TFieldSize aSize);
	/**
	 * Delete the previous content and allocate the new memory
	 * @param aData data pointer
	 * @param aSize size of data
	 */
	void Alloc (const CSISFieldRoot::TFieldSize aSize);
	/**
	 * Delete the previous content and allocate the new memory. 
	 * And also copy the content of the previous memory into the new memory
	 * @param aData data pointer
	 * @param aSize size of data
	 */
	void ReAlloc (const CSISFieldRoot::TFieldSize aSize);
	/**
	 * @return data size.
	 */
	TUint64 Size () const;
	/**
	 * Reset all the offset and data pointers. It does not delete any pointers.
	 * Can be used in ownership transfer.
	 */
	TUint8* GiveAway ();
	/**
	 * Compare the data for equality
	 */
 	bool operator==(const CBlob& aBlob) const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
 	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	void Duplicate (const CBlob <FieldType>& aInitialiser);
	 
private:
	TUint8*						iData;
	CSISFieldRoot::TFieldSize	iSize;
	};



template <CSISFieldRoot::TFieldType FieldType> inline
		CBlob <FieldType>::CBlob () :
			iData (NULL),
			iSize (0)
	{
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		CBlob <FieldType>::CBlob (const CBlob <FieldType>& aInitialiser)	:
			CSISField <FieldType> (aInitialiser),
			iData (NULL),
			iSize (0)
	{
	Duplicate (aInitialiser); 
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		void CBlob <FieldType>::Alloc (const CSISFieldRoot::TFieldSize aSize)
	{
	if (iSize != aSize)
		{
		Dispose ();
		if (aSize != 0)
			{
			iData = new TUint8 [aSize]; 
			iSize = aSize;
			memset (iData, 0, iSize);
			}
		}
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		void CBlob <FieldType>::ReAlloc (const CSISFieldRoot::TFieldSize aSize)
	{
	if (aSize <= iSize)
		{
		return;
		}
	if (aSize == 0)
		{
		Dispose ();
		}
	else
		{
		TUint8* data = new TUint8 [aSize];
		if (aSize > iSize)
			{
			memset (&data [iSize], 0, aSize - iSize);
			}
		memcpy (data, iData, std::min (aSize, iSize));
		Dispose ();
		iData = data;
		iSize = aSize;
		}
	}


template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Duplicate (const CBlob <FieldType>& aInitialiser)
	{
	try
		{
		Assign (aInitialiser.iData, aInitialiser.iSize);
		}
	catch (...)
		{
		iSize = 0;
		delete iData;
		iData = NULL;
		}
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		CBlob <FieldType>::~CBlob ()
	{
	Dispose ();
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Dispose ()
	{
	delete [] iData;
	iData = NULL;
	iSize = 0;
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Read (TSISStream& aFile, const TUint64& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	CSISHeader header (aArrayType);
	header.Read (aFile, aContainerSize, aArrayType);
	SetByteCount (header.DataSize ());
	if (! WasteOfSpace ())
		{
		assert (! IsBadWritePtr (iData, iSize));
		aFile.read (iData, iSize);
		ReadFiller (aFile);
		}
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Write (TSISStream& aFile, const bool aIsArrayElement) const
	{
	if (! WasteOfSpace ())
		{
#ifdef GENERATE_ERRORS
		if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInsaneBlob))
			{
			CSISHeader (FieldType, rand ()).Write (aFile, aIsArrayElement);
			}
		else
#endif // GENERATE_ERRORS
			{
			CSISHeader (FieldType, ByteCount (false)).Write (aFile, aIsArrayElement);
			}
		assert (! IsBadReadPtr (iData, iSize));
		aFile.write (iData, iSize);
		WriteFiller (aFile);
		}
	}
  
template <CSISFieldRoot::TFieldType FieldType> inline
		CSISFieldRoot::TFieldSize CBlob <FieldType>::ByteCount (const bool aInsideArray) const
	{
	return iSize;
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CBlob <FieldType>::SetByteCount (const CSISFieldRoot::TFieldSize aSize)
	{
	Alloc (aSize);
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Dump (std::ostream& aStream, const int aLevel) const
	{
	aStream << iSize << ": ";
	const TUint64 size = 20;
	for (unsigned index = 0; index < std::min (size, iSize); index++)
		{
#define HEX(x) (((x)<10)?((x)+'0'):((x)-10+'A'))
		aStream << static_cast <char> (HEX ((iData [index] >> 4) & 0xF));
		aStream << static_cast <char> (HEX (iData [index] & 0xF));
#undef HEX
		}
	if (20 < iSize)
		{
		aStream << "...";
		}
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		bool CBlob <FieldType>::WasteOfSpace () const
	{
	return ! Required () && (iSize == 0);
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CBlob <FieldType>::Verify (const TUint32 aLanguages) const
	{
	assert ((! iData) == (! iSize));
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::CalculateCrc (TCRC& aCRC, const bool aIsArrayElement) const
	{
	if(!WasteOfSpace())
		{
		CSISHeader (CSISFieldRoot::ESISBlob, iSize).CalculateCrc (aCRC, aIsArrayElement); 
		CSISFieldRoot::DoPaddedCrc (aCRC, iData, iSize);
		}
	}


template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Load (const std::wstring& aFileName, TUint64* aSize)
	{
	TUint64 blobSize;
	HANDLE file = OpenFileAndGetSize (aFileName, &blobSize);
	SetByteCount (blobSize);
	ReadAndCloseFile (file, blobSize, iData);
	if (aSize)
		{
		*aSize = blobSize;
		}
	}


template <CSISFieldRoot::TFieldType FieldType> inline
		std::string CBlob <FieldType>::Name () const
	{
	return "Binary";
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		const TUint8* CBlob <FieldType>::Data () const
	{
	return iData;
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		TUint8* CBlob <FieldType>::Data ()
	{
	return iData;
	}

template <CSISFieldRoot::TFieldType FieldType> 
		void CBlob <FieldType>::Assign (const TUint8* aData, const CSISFieldRoot::TFieldSize aSize)
	{
	Alloc (aSize);
	if (aSize != 0)
		{
		memcpy (iData, aData, iSize);
		}
	}

template <CSISFieldRoot::TFieldType FieldType> 
		void CBlob <FieldType>::Append (const TUint8* aData, const CSISFieldRoot::TFieldSize aSize)
	{
	CSISFieldRoot::TFieldSize oldSize = iSize;
	ReAlloc (aSize + iSize);
	if (aSize > 0)
		{
		memcpy (&iData [oldSize], aData, aSize);
		}
	}

template <CSISFieldRoot::TFieldType FieldType>
		void CBlob <FieldType>::Prepend (const TUint8* aData, const CSISFieldRoot::TFieldSize aSize)
	{
	
	if (aSize > 0)
		{

		TUint8* data = new TUint8[iSize + aSize];
		memcpy(data, aData, aSize);
		memcpy(&(data[aSize]), iData, iSize);

		CSISFieldRoot::TFieldSize oldSize = iSize;

		Dispose();
		iData = data;
		iSize = aSize + oldSize;

		}

	}

template <CSISFieldRoot::TFieldType FieldType> 
		TUint8* CBlob <FieldType>::GiveAway ()
	{
	TUint8* data = iData;
	iData = NULL;
	iSize = 0;
	return data;
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		TUint64 CBlob <FieldType>::Size () const
	{
	return iSize;
	}
	
	
template <CSISFieldRoot::TFieldType FieldType> inline
		bool CBlob <FieldType>::operator==(const CBlob& aBlob) const
	{
	// Return false if the size of the data in each blob doesn't match.				 	
	if (iSize != aBlob.iSize)
		{			  
		return false;
		}
	
	// Compare the data 
	return (!(memcmp(iData, aBlob.iData, iSize )));
 
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CBlob <FieldType>::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(aVerbose)
		{
		aStream << std::hex;
		for (TUint32 i=0; i<iSize; i++)
			{
			aStream.fill('0');
			aStream.width(2);
			aStream << (iData[i] & 0xFF);
			}
		aStream.width(0);
		aStream << std::dec;
		}
	}

#endif // __SISBLOB_H__
