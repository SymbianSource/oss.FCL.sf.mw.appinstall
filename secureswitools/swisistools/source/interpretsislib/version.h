/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* version.cpp
*
*/


#ifndef	VERSION_H
#define	VERSION_H

#include <istream>
#include "serialisable.h"
#include "serialiser.h"
#include "deserialiser.h"
#include "symbiantypes.h"

class Serialiser;
class Deserialiser;
/**
* @file VERSION.H
*
* @internalComponent
* @released
*/
class Version : public Serialisable//<Version>
	{
public:
	/**
	Constructor
	*/
	Version () : iMajor(0), iMinor(0), iBuild(0) {}

	Version (TInt aMa, TInt aMi, TInt aB )
		: iMajor(aMa),
		  iMinor(aMi),
		  iBuild(aB) {}

	~Version () {};

	virtual void Internalize(Deserialiser& des)
	{
	des >> iMajor >> iMinor >> iBuild;
	}
	virtual void Externalize(Serialiser& ser)
	{
	ser << iMajor << iMinor << iBuild;
	}
	//virtual Serialisable* Make() { return new Version; }

	TInt GetMajor() const { return iMajor;}
	TInt GetMinor() const { return iMinor;}
	TInt GetBuild() const { return iBuild;}

	/**
	 * Determines whether the iMajor, iMinor and iBuild components of the stored version are valid and
	 * within range.
	 *
	 * Note: This function does NOT support Wildcards
	 * 
	 * @return		True is returned if ALL of the following conditions result to True:
	 * 					- iMajor ranges between 0 and 127 inclusive
	 * 					- iMinor ranges between 0 and 99 inclusive
	 * 					- iBuild ranges between 0 and 32767 inclusive
	 * 
	 * 				Otherwise, False is returned. 
	 */
	bool IsValid() const;

	friend bool operator==(const Version& aLhs, const Version& aRhs);
	friend bool operator<(const Version& aLhs, const Version& aRhs);
	friend bool operator>(const Version& aLhs, const Version& aRhs);
	friend bool operator!=(const Version& aLhs, const Version& aRhs);
	friend bool operator<=(const Version& aLhs, const Version& aRhs);
	friend bool operator>=(const Version& aLhs, const Version& aRhs);

private:
	TInt iMajor;
	TInt iMinor;
	TInt iBuild;
	};


std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os,
										Version& ver);


bool operator==(const Version& aLhs, const Version& aRhs);
bool operator<(const Version& aLhs, const Version& aRhs);
bool operator>(const Version& aLhs, const Version& aRhs);
bool operator!=(const Version& aLhs, const Version& aRhs);
bool operator<=(const Version& aLhs, const Version& aRhs);
bool operator>=(const Version& aLhs, const Version& aRhs);

#endif	/* VERSION_H */
