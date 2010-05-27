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

#ifndef __SISTIME_H__
#define __SISTIME_H__


#include "structure.h"
#include "numeric.h"

class CSISTime : public CStructure <CSISFieldRoot::ESISTime>
	{
public:
	/**
	 * Default constructor.
	 */
	CSISTime ();
	/**
	 * Copy constructor.
	 */
	CSISTime (const CSISTime& aInitialiser);

	/**
	 * Verify the instance of class. In case of any invalid
	 * data it will throw an exception.
	 * @param aLanguages Language count.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	/**
	 * Set a new time.
	 * @param aHours New value for hours.
	 * @param aMinutes New value for minutes.
	 * @param aSeconds New value for seconds. 
	 */
	void Set (const TUint8 aHours, const TUint8 aMinutes, const TUint8 aSeconds);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get value for Hours
	 */
	inline TUint8 Hours() const;
	/**
	 * Get value for Minutes
	 */
	inline TUint8 Minutes() const;
	/**
	 * Get value for Seconds
	 */
	inline TUint8 Seconds() const;

private:
	void InsertMembers ();

private:
	CSISUInt8 iHours;
	CSISUInt8 iMinutes;
	CSISUInt8 iSeconds;
	};


inline CSISTime::CSISTime ()
	{
	InsertMembers ();
	}


inline std::string CSISTime::Name () const
	{
	return "Time";
	}

inline TUint8 CSISTime::Hours() const
	{
	return iHours;
	}

inline TUint8 CSISTime::Minutes() const
	{
	return iMinutes;
	}

inline TUint8 CSISTime::Seconds() const
	{
	return iSeconds;
	}




#endif // __SISTIME_H__

