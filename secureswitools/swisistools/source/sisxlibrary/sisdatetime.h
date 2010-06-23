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

#ifndef __SISDATETIME_H__
#define __SISDATETIME_H__

#include "sisdate.h"
#include "sistime.h"


class CSISDateTime : public CStructure <CSISFieldRoot::ESISDateTime>
	{
public:
	/**
	 * Default constructor.
	 */
	CSISDateTime ();
	/**
	 * Copy constructor.
	 */ 
	CSISDateTime (const CSISDateTime& aInitialiser);
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Set the current date and time.
	 */
	void SetNow ();
	/**
	 * Set a new date.
	 * @param aYear New value of year.
	 * @param aYear New value of month.
	 * @param aYear New value of day.
	 */
	inline void SetDate (const TUint16 aYear, const TUint8 aMonth, const TUint8 aDay);
	/**
	 * Set a new time.
	 * @param aHours New value of Hours.
	 * @param aMinutes New value of minutes.
	 * @param aSeconds New value of seconds.
	 */
	inline void SetTime (const TUint8 aHours, const TUint8 aMinutes, const TUint8 aSeconds);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the date (CSISDate).
	 */
	inline const CSISDate& Date() const;
	/**
	 * Get the time (CSISTime).
	 */
	inline const CSISTime& Time() const;

private:
	void InsertMembers ();

private:
	CSISDate iDate;
	CSISTime iTime;
	};


inline void CSISDateTime::SetDate (const TUint16 aYear, const TUint8 aMonth, const TUint8 aDay)
	{
	iDate.Set(aYear, aMonth, aDay);
	}

inline void CSISDateTime::SetTime (const TUint8 aHours, const TUint8 aMinutes, const TUint8 aSeconds)
	{
	iTime.Set(aHours, aMinutes, aSeconds);
	}

inline const CSISDate& CSISDateTime::Date() const	
	{
	return iDate;
	}

inline const CSISTime& CSISDateTime::Time() const
	{
	return iTime;
	}


#endif // __SISDATETIME_H__

