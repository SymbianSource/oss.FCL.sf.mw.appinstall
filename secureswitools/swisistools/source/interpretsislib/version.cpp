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
*
*/


#include "version.h"

bool Version::IsValid() const
{
	return ((iMajor >= 0 && iMajor <= 127) &&
			(iMinor >= 0 && iMinor <= 99)  &&
			(iBuild >= 0 && iBuild <= 32767));
}

bool operator==(const Version& aLhs, const Version& aRhs)
{
	return 
		aLhs.iMajor == aRhs.iMajor 
		&& aLhs.iMinor == aRhs.iMinor 
		&& aLhs.iBuild == aRhs.iBuild;
}

bool operator<(const Version& aLhs, const Version& aRhs)
{
	return (aLhs != aRhs) && !(aLhs > aRhs);
}

bool operator>(const Version& aLhs, const Version& aRhs)
{
	if (aLhs == aRhs || aLhs.iMajor < aRhs.iMajor) return false;

	if (aLhs.iMajor > aRhs.iMajor)
		return true;
	else if (aLhs.iMinor == aRhs.iMinor) // major ==
		return aLhs.iBuild > aRhs.iBuild;
	else if (aLhs.iMinor > aRhs.iMinor) 
		return true;
	else // minor ==
		return aLhs.iBuild > aRhs.iBuild;

	return false;
}

bool operator!=(const Version& aLhs, const Version& aRhs)
{
	return !(aLhs == aRhs);
}

bool operator<=(const Version& aLhs, const Version& aRhs)
{
	return (aLhs == aRhs) || aLhs < aRhs;
}

bool operator>=(const Version& aLhs, const Version& aRhs)
{
	return (aLhs == aRhs) || aLhs > aRhs;

}
