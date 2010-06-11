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
* generic base for all SIS checksums
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include "structure.h"
#include "numeric.h"

#include <stdlib.h>


template <CSISFieldRoot::TFieldType FieldType> class CChecksum : public CStructure <FieldType>
	{
public:
	/**
	 * Default constructor
	 */
	CChecksum (const bool aRequired = true);
	/**
	 * Copy Constructor
	 */
	CChecksum (const CChecksum& aInitialiser);
	
public:
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Checks if the structure is required or not. Used for externalizing
	 * the class. If its a waste of space then we need not write this into
	 * the file.
	 */
	virtual bool WasteOfSpace () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	/**
	 * Set the CRC value
	 * @param aCrc new CRC value
	 */
	void Set (const TCRC aCrc);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	CSISUInt16	iDataChecksum;
	};


template <CSISFieldRoot::TFieldType FieldType> inline
	CChecksum <FieldType>::CChecksum (const bool aRequired) :
		CStructure <FieldType> (aRequired)
	{ 
	InsertMember (iDataChecksum); 
	}

template <CSISFieldRoot::TFieldType FieldType> inline
	CChecksum <FieldType>::CChecksum (const CChecksum& aInitialiser) :
		CStructure <FieldType> (aInitialiser),
		iDataChecksum (aInitialiser.iDataChecksum)
	{ 
	InsertMember (iDataChecksum); 
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		std::string CChecksum <FieldType>::Name () const
	{
	return "Checksum";
	}

template <CSISFieldRoot::TFieldType FieldType> inline
		void CChecksum <FieldType>::Set (const TCRC aCrc)
	{
	iDataChecksum = aCrc; 
	}


template <CSISFieldRoot::TFieldType FieldType> inline
	bool CChecksum <FieldType>::WasteOfSpace () const
	{
	return iDataChecksum.Value()? false: true;
	}


#ifdef GENERATE_ERRORS
template <CSISFieldRoot::TFieldType FieldType>
		void CChecksum <FieldType>::CreateDefects ()
	{
	if (IsBugToBeCreated (CSISFieldRoot::EBugCRCError))
		{
		iDataChecksum |= static_cast <TUint16> (rand () & 0xFFFF);
		}
	}
#endif // GENERATE_ERRORS

template <CSISFieldRoot::TFieldType FieldType> inline
		void CChecksum <FieldType>::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	if(aVerbose)
		{
		aStream << L"; CRC16: " << std::hex << iDataChecksum.Value() << std::dec << std::endl;
		}
	}

#endif // __SISDATACHECKSUM_H__

