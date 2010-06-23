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

#ifndef __SISPREREQUISITES_H__
#define __SISPREREQUISITES_H__

#include "sisdependency.h"
#include "sisarray.h"

class CSISPrerequisites : public CStructure <CSISFieldRoot::ESISPrerequisites>
	{
public:
	typedef CSISArray <CSISDependency, CSISFieldRoot::ESISDependency> TDependencyList;

public:
	/**
	 * Default constructor.
	 */
	CSISPrerequisites ();
	/**
	 * Copy constructor.
	 */
	CSISPrerequisites (const CSISPrerequisites& aInitialiser);
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Add new dependency name to the existing list of names.
	 * @param aName New dependency name.
	 */
	void AddDependencyName (const std::wstring& aName);
	/**
	 * Add a new dependency into the existing list of of dependencies.
	 * @param aUID UID of the dependency.
	 * @param aFrom From part of the dependency version range
	 * @param aTo To part of the dependency version range 
	 */
	void AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Add a new target name into the list of existing target names.
	 * @param aName New target dependency name.
	 */
	void AddTargetName (const std::wstring& aName);
	/**
	 * Add a new target dependency into the existing list of of target dependencies.
	 * @param aUID UID of the target.
	 * @param aFrom From part of the target dependency version range
	 * @param aTo To part of the target dependency version range 
	 */
	void AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Checks if the UID is present in the list dependencies.
	 * @param aUid UID to be searched.
	 */
	bool HasUID (const CSISUid& aUid) const;
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the list of dependencies.
	 */
	inline const TDependencyList& DependencyList() const;
	/**
	 * Get the list of target dependencies.
	 */
	inline const TDependencyList& TargetDependencyList() const;

private:
	void InsertMembers ();

private:
	TDependencyList iTargetDevices;
	TDependencyList iDependencies;
	};


inline CSISPrerequisites::CSISPrerequisites ()
	{
	InsertMembers (); 
	}

inline void CSISPrerequisites::InsertMembers ()
	{
	InsertMember (iTargetDevices);
	InsertMember (iDependencies);
	}

inline CSISPrerequisites::CSISPrerequisites (const CSISPrerequisites& aInitialiser) :
			CStructure <CSISFieldRoot::ESISPrerequisites> (aInitialiser),
			iTargetDevices (aInitialiser.iTargetDevices),
			iDependencies (aInitialiser.iDependencies)
	{
	InsertMembers (); 
	}


inline void CSISPrerequisites::AddDependencyName (const std::wstring& aName)
	{
	iDependencies.Last ().AddDependencyName (aName);
	}


inline void CSISPrerequisites::AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iDependencies.Push (CSISDependency (aUID, aFrom, aTo));
	}


inline void CSISPrerequisites::AddTargetName (const std::wstring& aName)
	{
	iTargetDevices.Last ().AddDependencyName (aName);
	}


inline void CSISPrerequisites::AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iTargetDevices.Push (CSISDependency (aUID, aFrom, aTo));
	}


inline std::string CSISPrerequisites::Name () const
	{
	return "Prerequisites";
	}

inline const CSISPrerequisites::TDependencyList& CSISPrerequisites::DependencyList() const
	{
	return iDependencies;
	}
inline const CSISPrerequisites::TDependencyList& CSISPrerequisites::TargetDependencyList() const
	{
	return iTargetDevices;
	}


#endif // __SISPREREQUISITES_H__

