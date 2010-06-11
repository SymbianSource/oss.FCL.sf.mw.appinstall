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
*
*/


/**
 @file 
 @internalComponent
 @released
*/


#include "sistime.h"
#include "exception.h"

#include <time.h>

CSISTime::CSISTime (const CSISTime& aInitialiser) :
		CStructure <CSISFieldRoot::ESISTime> (aInitialiser),
		iHours (aInitialiser.iHours),
		iMinutes (aInitialiser.iMinutes),
		iSeconds (aInitialiser.iSeconds)
	{
	InsertMembers (); 
	}


void CSISTime::InsertMembers ()
	{
	InsertMember (iHours);
	InsertMember (iMinutes);
	InsertMember (iSeconds);
	}


void CSISTime::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISTime>::Verify (aLanguages);
	if (	(iHours > 23) ||
			(iMinutes > 59) ||
			(iSeconds > 61))
		{
		const int maxlen = 256;
		char message [maxlen];
		struct tm time;
		memset (&time, 0, sizeof (struct tm));
		time.tm_hour = iHours;
		time.tm_min = iMinutes;
		time.tm_sec = iSeconds;
		::strftime (message, maxlen, "invalid time %c", &time);
		throw CSISException (CSISException::EVerification, message);
		}
	}


void CSISTime::Set (const TUint8 aHours, const TUint8 aMinutes, const TUint8 aSeconds)
	{
	iHours = aHours;
	iMinutes = aMinutes;
	iSeconds = aSeconds;
	}

void CSISTime::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	aStream << static_cast<int>(iHours)		<< L":"; 
	aStream << static_cast<int>(iMinutes)	<< L":";
	aStream << static_cast<int>(iSeconds);
	}


#ifdef GENERATE_ERRORS
void CSISTime::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iHours = rand ();
		}
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iMinutes = rand ();
		}
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iSeconds = rand ();
		}
	}
#endif // GENERATE_ERRORS

