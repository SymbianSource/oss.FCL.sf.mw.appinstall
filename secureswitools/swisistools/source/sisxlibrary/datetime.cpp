/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sisdatetime.h"
#include <time.h>


void CSISDateTime::InsertMembers ()
	{
	InsertMember (iDate);
	InsertMember (iTime);
	}


CSISDateTime::CSISDateTime ()
	{ 
	InsertMembers ();
	}


CSISDateTime::CSISDateTime (const CSISDateTime& aInitialiser) :
		CStructure <CSISFieldRoot::ESISDateTime> (aInitialiser),
		iDate (aInitialiser.iDate),
		iTime (aInitialiser.iTime)
	{ 
	InsertMembers (); 
	}


std::string CSISDateTime::Name () const
	{
	return "Date Time";
	}



void CSISDateTime::SetNow ()
	{
	time_t now;
	struct tm *gmt;

	time (&now);
	gmt = gmtime (&now);
	assert (gmt);

	iDate.Set (gmt -> tm_year + 1900, gmt -> tm_mon, gmt -> tm_mday);
	iTime.Set (gmt -> tm_hour, gmt -> tm_min, gmt -> tm_sec);
	}

void CSISDateTime::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	iDate.AddPackageEntry(aStream, aVerbose);
	aStream << L" ";
	iTime.AddPackageEntry(aStream, aVerbose);
	}

