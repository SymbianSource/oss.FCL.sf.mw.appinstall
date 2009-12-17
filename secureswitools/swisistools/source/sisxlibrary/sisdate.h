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

#ifndef __SISDATE_H__
#define __SISDATE_H__


#include "structure.h"
#include "numeric.h"


class CSISDate : public CStructure <CSISFieldRoot::ESISDate>
	{
private:
	void InsertMembers ();

public:
	/**
	 * Default constructor
	 */
	CSISDate ();
	/**
	 * Copy constructor.
	 */
	CSISDate (const CSISDate& aInitialiser);
	/**
	 * Sanity check for the class. This class will call Verify 
	 * on all its members.
	 * @param aLanguages Number of languages in the sis.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Class name
	 */
	virtual std::string Name () const;
	/**
	 * Set new date
	 * @param aYear New year to set.
	 * @param aMonth New month to set.
	 * @param aDay New day to set.
	 */
	void Set (const TUint16 aYear, const TUint8 aMonth, const TUint8 aDay);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the year
	 */
	inline TUint16 Year() const;
	/**
	 * Get the month
	 */
	inline TUint8 Month() const;
	/**
	 * Get the day
	 */
	inline TUint8 Day() const;

private:
	CSISUInt16 iYear;
	CSISUInt8 iMonth;
	CSISUInt8 iDay;
	};

inline TUint16 CSISDate::Year() const
	{
	return iYear;
	}

inline TUint8 CSISDate::Month() const
	{
	return iMonth;
	}

inline TUint8 CSISDate::Day() const
	{
	return iDay;
	}

#endif // __SISDATE_H__

