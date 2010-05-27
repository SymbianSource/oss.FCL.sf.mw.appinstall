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
* Implementation of TCheckedVersion
*
*/


/**
 @file
*/

#include "checkedversion.h"

using namespace Swi;

TCheckedVersion::TCheckedVersion(const TVersion aVersion) : TVersion(aVersion)
	{
	}

TCheckedVersion::TCheckedVersion(const Sis::CVersion& aVersion)
	: TVersion(aVersion.Major(), aVersion.Minor(), aVersion.Build())
	{
	}

TBool TCheckedVersion::IsValid()
	{
	return	(((iMajor >= 0 && iMajor <= 127) 	|| (iMajor==KWildCard)) &&
			 ((iMinor >= 0 && iMinor <= 99) 	|| (iMinor==KWildCard)) &&
			 ((iBuild >= 0 && iBuild <= 32767) 	|| (iBuild==KWildCard)));
	}

TBool TCheckedVersion::operator==(const TCheckedVersion& aOther)
	{
	return
		((iMajor==aOther.iMajor) || (aOther.iMajor==KWildCard) || (iMajor==KWildCard)) &&
		((iMinor==aOther.iMinor) || (aOther.iMinor==KWildCard) || (iMinor==KWildCard)) &&
		((iBuild==aOther.iBuild) || (aOther.iBuild==KWildCard) || (iBuild==KWildCard));
	}

TBool TCheckedVersion::operator>=(const TCheckedVersion& aOther)
	{
	if ((iMajor!=KWildCard)&&(aOther.iMajor!=KWildCard)&&(iMajor!=aOther.iMajor))
		{
		return iMajor>aOther.iMajor;
		}
	if ((iMinor!=KWildCard)&&(aOther.iMinor!=KWildCard)&&(iMinor!=aOther.iMinor))
		{
		return iMinor>aOther.iMinor;
		}
	if ((iBuild!=KWildCard)&&(aOther.iBuild!=KWildCard)&&(iBuild!=aOther.iBuild))
		{
		return iBuild>aOther.iBuild;
		}
	// All wildcarded or equal
	return ETrue;
	}

TBool TCheckedVersion::operator<=(const TCheckedVersion& aOther)
	{
	if ((iMajor!=KWildCard)&&(aOther.iMajor!=KWildCard)&&(iMajor!=aOther.iMajor))
		{
		return iMajor<aOther.iMajor;
		}
	if ((iMinor!=KWildCard)&&(aOther.iMinor!=KWildCard)&&(iMinor!=aOther.iMinor))
		{
		return iMinor<aOther.iMinor;
		}
	if ((iBuild!=KWildCard)&&(aOther.iBuild!=KWildCard)&&(iBuild!=aOther.iBuild))
		{
		return iBuild<aOther.iBuild;
		}
	// All wildcarded or equal
	return ETrue;
	}

TBool TCheckedVersion::operator<(const TCheckedVersion& aOther)
	{
	if ((iMajor!=KWildCard)&&(aOther.iMajor!=KWildCard)&&(iMajor!=aOther.iMajor))
		{
		return iMajor<aOther.iMajor;
		}
	if ((iMinor!=KWildCard)&&(aOther.iMinor!=KWildCard)&&(iMinor!=aOther.iMinor))
		{
		return iMinor<aOther.iMinor;
		}
	if ((iBuild!=KWildCard)&&(aOther.iBuild!=KWildCard)&&(iBuild!=aOther.iBuild))
		{
		return iBuild<aOther.iBuild;
		}
	
	// Check to determine whether all components are equal and neither contain wildcards
	if(((iMajor != KWildCard) && (iMajor == aOther.iMajor)) &&
		((iMinor != KWildCard) && (iMinor == aOther.iMinor)) &&
		((iBuild != KWildCard) && (iBuild == aOther.iBuild)))
		{
		return EFalse;
		}
	
	// Wildcards encountered during comparison
	return ETrue;
	}

TBool TCheckedVersion::operator>(const TCheckedVersion& aOther)
	{
	if ((iMajor!=KWildCard)&&(aOther.iMajor!=KWildCard)&&(iMajor!=aOther.iMajor))
		{
		return iMajor>aOther.iMajor;
		}
	if ((iMinor!=KWildCard)&&(aOther.iMinor!=KWildCard)&&(iMinor!=aOther.iMinor))
		{
		return iMinor>aOther.iMinor;
		}
	if ((iBuild!=KWildCard)&&(aOther.iBuild!=KWildCard)&&(iBuild!=aOther.iBuild))
		{
		return iBuild>aOther.iBuild;
		}
	
	// Check to determine whether all components are equal and neither contain wildcards
	if(((iMajor != KWildCard) && (iMajor == aOther.iMajor)) &&
		((iMinor != KWildCard) && (iMinor == aOther.iMinor)) &&
		((iBuild != KWildCard) && (iBuild == aOther.iBuild)))
		{
		return EFalse;
		}
	
	// Wildcards encountered during comparison
	return ETrue;
	}

TBool TCheckedVersion::operator!=(const TCheckedVersion& aOther)
	{
	if(((iMajor==KWildCard) || (aOther.iMajor==KWildCard)) ||
		((iMinor==KWildCard) || (aOther.iMinor==KWildCard)) ||
		((iBuild==KWildCard) || (aOther.iBuild==KWildCard)))
		{
		return ETrue;
		}
	
	return (iMajor!=aOther.iMajor || iMinor!=aOther.iMinor || iBuild!=aOther.iBuild);
	}
