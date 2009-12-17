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

#ifndef __SISDEPENDENCY_H__
#define __SISDEPENDENCY_H__


#include "sisuid.h"
#include "sisversionrange.h"
#include "stringarray.h"


class CSISDependency : public CStructure <CSISFieldRoot::ESISDependency>
	{
public:
	enum TDependencyType
		{
		EPkgDependency,
		ETargetDependency
		};
public:
	/**
	 * Default constructor.
	 */
	CSISDependency ();
	/**
	 * Copy constructor.
	 */
	CSISDependency (const CSISDependency& aInitialiser);
	/**
	 * Create a CSISDependency object from UID and version range of 
	 * the dependency.
	 * @param aUID UID of the dependency.
	 * @param aFrom From part of version range
	 * @param aFrom To part of version range
	 */
	CSISDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Sanity check for the class. This class will call Verify 
	 * on all its members. For each language a separate dependency 
	 * name should be provided. Therefor language count and 
	 * dependency name count should match.
	 * 
	 * @param aLanguages Number of languages in the sis.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * class name
	 */
	virtual std::string Name () const;
	/**
	 * Add a new dependency name into the existing list of names.
	 * @param aName new dependency name
	 */
	void AddDependencyName (const std::wstring& aName);
	/**
	 * Check whether the given uid matches with the dependency
	 * UID.
	 * @param aUID UID to check.
	 */
	bool HasUID (const CSISUid& aUID) const;
	/**
	 * Get the language count from the dependency name count.
	 * For each language a separate dependency name should be provided.
	 * Therefor language count and dependency name count should match.
	 */
	TUint32 LanguageCount () const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Set whether the dependency is a target(device) or package dependency.
	 * @param aDependencyType new dependency type.
	 */
	inline void SetDependencyType(TDependencyType aDependencyType) const;
	/**
	 * Get the UID of the dependency.
	 */
	inline const CSISUid& SisUID() const;
	/**
	 * Get the version range of the dependency
	 */
	inline const CSISVersionRange& VersionRange() const;

private:
	void InsertMembers ();

private:
	CSISUid				iUid;
	CSISVersionRange	iVersionRange;
	CSISArrayOfStrings	iDependencyNames;
	mutable TDependencyType		iDependencyType;
	};



inline CSISDependency::CSISDependency () :
		iVersionRange (true),
		iDependencyType(EPkgDependency)
	{
	InsertMembers (); 
	}


inline void CSISDependency::AddDependencyName (const std::wstring& aName)
	{ 
	iDependencyNames.Push (CSISString (aName)); 
	}


inline bool CSISDependency::HasUID (const CSISUid& aUID) const
	{ 
	return iUid == aUID; 
	}


inline TUint32 CSISDependency::LanguageCount () const
	{ 
	return iDependencyNames.size (); 
	}

inline void CSISDependency::SetDependencyType(TDependencyType aDependencyType) const
	{
	iDependencyType = aDependencyType;
	}

inline const CSISUid& CSISDependency::SisUID() const
	{
	return iUid;
	}

inline const CSISVersionRange& CSISDependency::VersionRange() const
	{
	return iVersionRange;
	}

#endif // __SISDEPENDENCY_H__

