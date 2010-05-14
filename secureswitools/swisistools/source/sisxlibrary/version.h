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

#ifndef __VERSION_H__
#define __VERSION_H__

// tranport for version values

namespace SWIVersion
{

const int KIrrelevant = -1;

class TVersion
	{
public:
	TVersion ();
	TVersion (const TVersion& aVersion);
	TVersion (const TInt32 aMajor, const TInt32 aMinor, const TInt32 aBuild);

	TInt32 Major () const;
	TInt32 Minor () const;
	TInt32 Build () const;

private:
	TInt32	iMajor;
	TInt32	iMinor;
	TInt32	iBuild;
	};
};


/*
 * seclib already has TVersion defined. This causes multiply defined error. 
 * Hence, the namespace.
 *
 * The namespace has been opened so that it does not affect the exisiting implementations.
 */

using namespace SWIVersion;

inline TVersion::TVersion () :
		iMajor (KIrrelevant),
		iMinor (KIrrelevant),
		iBuild (KIrrelevant)
	{
	}

inline TVersion::TVersion (const TVersion& aVersion) :
		iMajor (aVersion.iMajor),
		iMinor (aVersion.iMinor),
		iBuild (aVersion.iBuild)
	{
	}

inline TVersion::TVersion (const TInt32 aMajor, const TInt32 aMinor, const TInt32 aBuild) :
		iMajor (aMajor),
		iMinor (aMinor),
		iBuild (aBuild)
	{
	}


inline TInt32 TVersion::Major () const
	{
	return iMajor;
	}

inline TInt32 TVersion::Minor () const
	{
	return iMinor;
	}

inline TInt32 TVersion::Build () const
	{
	return iBuild;
	}


#endif // __VERSION_H__

