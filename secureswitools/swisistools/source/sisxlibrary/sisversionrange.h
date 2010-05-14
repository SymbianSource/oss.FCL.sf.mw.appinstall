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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISVERSIONRANGE_H__
#define __SISVERSIONRANGE_H__

#include "sisversion.h"
#include "version.h"

class CSISVersionRange : public CStructure <CSISFieldRoot::ESISVersionRange>

	{
private:
	void InsertMembers ();

public:
	/**
	 * Default constructor
	 */
	explicit CSISVersionRange (const bool aRequired = true);
	/**
	 * Copy constructor
	 */
	CSISVersionRange (const CSISVersionRange& aInitialiser);
	/**
	 * Verifies the object state. In case of any inconsistency the 
	 * function will throw an exception.
	 * @param aLanguages Language count.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Check if the object is required or not. The function will 
	 * return false if either the object is mandatory(Required() 
	 * returns true) or any of the version (To or From) is specified.
	 */
	virtual bool WasteOfSpace () const;
	/**
	 * Set a new value for version range.
	 * @param aFrom New from version
	 * @param aTo New to version.
	 */
	void SetVersionRange (const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the value of from version.
	 */
	inline const CSISVersion& FromVersion() const;
	/**
	 * Get the value of to version.
	 */
	inline const CSISVersion& ToVersion() const;

private:
	CSISVersion iFromVersion;
	CSISVersion iToVersion;
	};




inline void CSISVersionRange::InsertMembers ()
	{
	InsertMember (iFromVersion);
	InsertMember (iToVersion);
	}

inline CSISVersionRange::CSISVersionRange (const bool aRequired) :
		CStructure <CSISFieldRoot::ESISVersionRange> (aRequired),
		iFromVersion (true, true),
		iToVersion (false, true)
	{
	InsertMembers ();
	}

inline CSISVersionRange::CSISVersionRange (const CSISVersionRange& aInitialiser) :
		CStructure <CSISFieldRoot::ESISVersionRange> (aInitialiser),
		iFromVersion (aInitialiser.iFromVersion),
		iToVersion (aInitialiser.iToVersion)
	{
	InsertMembers ();
	}

inline void CSISVersionRange::SetVersionRange (const TVersion& aFrom, const TVersion& aTo)
	{
	iFromVersion.SetVersion (aFrom);
	iToVersion.SetVersion (aTo);
	}

inline const CSISVersion& CSISVersionRange::FromVersion() const
	{
	return iFromVersion;
	}

inline const CSISVersion& CSISVersionRange::ToVersion() const
	{
	return iToVersion;
	}


#endif // __SISVERSIONRANGE_H__

