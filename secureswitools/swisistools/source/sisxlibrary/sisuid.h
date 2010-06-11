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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISUID_H__
#define __SISUID_H__

#include "structure.h"
#include "numeric.h"

static const TUint32 KNullUid = 0x00000000;


class CSISUid : public CStructure <CSISFieldRoot::ESISUid>
	{
public:
	typedef TUint32 TUid;

public:
	/**
	 * Default constructor
	 */
	CSISUid ();
	/**
	 * Copy constructor
	 */
	CSISUid (const CSISUid& aInitialiser);
	/**
	 * Retrieve the UID value.
	 */
	CSISUid::TUid UID1 () const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	/**
	 * Set a new value for UID.
	 * @param aUID UIDs new value
	 */
	void SetUID (const CSISUid::TUid aUID);
	/**
	 * Check whether two UIDs are same or not.
	 */
	bool operator == (const CSISUid& aCompare) const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	inline void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	CSISUInt32 iUID;

	};




inline CSISUid::CSISUid ()
	{
	InsertMember (iUID);
	}


inline CSISUid::CSISUid (const CSISUid& aInitialiser) :
		CStructure <CSISFieldRoot::ESISUid> (aInitialiser),
		iUID (aInitialiser.iUID)
	{
	InsertMember (iUID);
	}


inline CSISUid::TUid CSISUid::UID1 () const
	{
	return iUID;
	}


inline void CSISUid::SetUID (const CSISUid::TUid aUID)
	{ 
	iUID = aUID; 
	}


inline bool CSISUid::operator == (const CSISUid& aCompare) const
	{
	return	(iUID == aCompare.iUID);
	}


inline std::string CSISUid::Name () const
	{
	return "Uid";
	}

inline void CSISUid::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	aStream << L"0x" << std::hex << iUID << std::dec;
	}

#ifdef GENERATE_ERRORS
inline void CSISUid::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iUID = rand ();
		}
	}
#endif // GENERATE_ERRORS

#endif // __SISXTYPE_H__

