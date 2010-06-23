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

#ifndef __SISVERSION_H__
#define __SISVERSION_H__

#include "numeric.h"
#include "structure.h"
#include "version.h"

class CSISVersion : public CStructure <CSISFieldRoot::ESISVersion>
	{
public:
	typedef TInt32	TMajor;
	typedef TInt32	TMinor;
	typedef TInt32	TBuild;

public:
	/**
	 * Default constructor.
	 */
	explicit CSISVersion (const bool aRequired = false);
	/**
	 * Creates an instance of this class.
	 * @param aRequired Whether this field is an optional field or not.
	 * @param aPermitAny Allow any ((*,*,*) or (-1, -1, -1) version number. 
	 */
	CSISVersion (const bool aRequired, const bool aPermitAny);
	/**
	 * Copy constructor.
	 */
	CSISVersion (const CSISVersion& aInitialiser);
	/**
	 * Verify the structure. In case of any invalid data function
	 * will throw an exception.
	 * @param aLanguages Language count.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Check whether the class is used or not.
	 */
	virtual bool WasteOfSpace () const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	/**
	 * Check if the version is specified or not.
	 */
	bool Specified () const;
	/**
	 * Get the major version number
	 */
	TMajor	Major () const; 
	/**
	 * Get the minor version number
	 */
	TMinor	Minor () const; 
	/**
	 * Get the build number
	 */
	TBuild	Build () const;
	/**
	 * Set a new value for version.
	 */
	void SetVersion (const TVersion& aVersion);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	void InsertMembers ();

private:
	const bool	iPermitAny;
	CSISInt32	iMajor;
	CSISInt32	iMinor;
	CSISInt32	iBuild;

	};



bool operator == (const CSISVersion& aLHS, const CSISVersion& aRHS);
bool operator < (const CSISVersion& aLHS, const CSISVersion& aRHS);




inline bool CSISVersion::Specified () const
	{ 
	return (iMajor != KIrrelevant); 
	}

inline CSISVersion::TMajor CSISVersion::Major () const
	{ 
	return iMajor;
	} 

inline CSISVersion::TMinor CSISVersion::Minor () const
	{
	return iMinor;
	} 

inline CSISVersion::TBuild CSISVersion::Build () const
	{
	return iBuild;
	} 

inline void CSISVersion::SetVersion (const TVersion& aVersion)
	{
	iMajor = aVersion.Major (); 
	iMinor = aVersion.Minor (); 
	iBuild = aVersion.Build (); 
	}

inline bool CSISVersion::WasteOfSpace () const
	{
	return ! Required () && ! Specified (); 
	}


inline std::string CSISVersion::Name () const
	{
	return "Version";
	}




#endif // __SISVERSION_H__

