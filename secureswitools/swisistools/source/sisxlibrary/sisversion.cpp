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

#include "sisversion.h"
#include "exception.h"


CSISVersion::CSISVersion (const bool aRequired) :
		CStructure <CSISFieldRoot::ESISVersion> (aRequired),
		iPermitAny (false),
		iMajor (KIrrelevant), 
		iMinor (KIrrelevant), 
		iBuild (KIrrelevant)
	{ 
	InsertMembers (); 
	}


CSISVersion::CSISVersion (const bool aRequired, const bool aPermitAny) :
		CStructure <CSISFieldRoot::ESISVersion> (aRequired),
		iPermitAny (aPermitAny),
		iMajor (KIrrelevant), 
		iMinor (KIrrelevant), 
		iBuild (KIrrelevant)
	{ 
	InsertMembers (); 
	}


CSISVersion::CSISVersion (const CSISVersion& aInitialiser) :
		CStructure <CSISFieldRoot::ESISVersion> (aInitialiser),
		iPermitAny (aInitialiser.iPermitAny),
		iMajor (aInitialiser.iMajor), 
		iMinor (aInitialiser.iMinor), 
		iBuild (aInitialiser.iBuild)
	{ 
	InsertMembers (); 
	}


void CSISVersion::InsertMembers ()
	{
	InsertMember (iMajor);
	InsertMember (iMinor);
	InsertMember (iBuild);
	}


void CSISVersion::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISVersion>::Verify (aLanguages);

	if (iPermitAny)
		{
		CSISException::ThrowIf ((Major () < KIrrelevant) || (Minor () < KIrrelevant) || (Build () < KIrrelevant),
								CSISException::EVerification,
								"bad version");
		}
	else
		{
		CSISException::ThrowIf ((Major () < 0) || (Minor () < 0) || (Build () < 0),
								CSISException::EVerification,
								"bad version");
		}
	}

void CSISVersion::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	(void)aVerbose;
	aStream << iMajor << L", " << iMinor << L", " << iBuild;
	}



#ifdef GENERATE_ERRORS
void CSISVersion::CreateDefects ()
	{
	if (CSISVersion::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iMajor = rand ();
		}
	if (CSISVersion::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iMinor = rand ();
		}
	if (CSISVersion::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iBuild = rand ();
		}
	}
#endif // GENERATE_ERRORS



bool operator == (const CSISVersion& aLHS, const CSISVersion& aRHS)
	{
	return	(aLHS.Major () == aRHS.Major ()) &&
			(aLHS.Minor () == aRHS.Minor ()) &&
			(aLHS.Build () == aRHS.Build ());
	}


bool operator < (const CSISVersion& aLHS, const CSISVersion& aRHS)
	{
	if (aRHS.Major () == KIrrelevant) 
		{
		return false;
		}
	if (aLHS.Major () < aRHS.Major ())
		{
		return true;
		}
	if (	(aLHS.Major () > aRHS.Major ()) ||
			(aRHS.Minor () == KIrrelevant))
		{
		return false;
		}

	if (aLHS.Minor () < aRHS.Minor ())
		{
		return true;
		}
	if (	(aLHS.Minor () > aRHS.Minor ()) ||
			(aRHS.Build () == KIrrelevant))
		{
		return false;
		}
	return aLHS.Build () < aRHS.Build ();
	}



