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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "sisdate.h"
#include "exception.h"

void CSISDate::InsertMembers ()
	{
	InsertMember (iYear);
	InsertMember (iMonth);
	InsertMember (iDay);
	}


CSISDate::CSISDate () :
		iYear (2004), 
		iMonth (0), 
		iDay (1)
	{
	InsertMembers (); 
	}


CSISDate::CSISDate (const CSISDate& aInitialiser) :
		CStructure <CSISFieldRoot::ESISDate> (aInitialiser),
		iYear (aInitialiser.iYear), 
		iMonth (aInitialiser.iMonth), 
		iDay (aInitialiser.iDay)
	{
	InsertMembers (); 
	}


void CSISDate::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISDate>::Verify (aLanguages);

		// unsophisticated, use OS calls if anything clever is required
	CSISException::ThrowIf ((iMonth >= 12),
							CSISException::EVerification,
							"invalid month");

	CSISException::ThrowIf ((iDay < 1) || (iDay > 31),
							CSISException::EVerification,
							"there are at most 31 days in a month");
	}


std::string CSISDate::Name () const
	{
	return "Date";
	}


void CSISDate::Set (const TUint16 aYear, const TUint8 aMonth, const TUint8 aDay)
	{
	iYear = aYear;
	iMonth = aMonth;
	iDay = aDay;
	}

void CSISDate::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	// Month starts with 0
	aStream << static_cast<int>(iYear)		<< L"/";
	aStream << static_cast<int>(iMonth)+1	<< L"/";
	aStream << static_cast<int>(iDay);
	}

