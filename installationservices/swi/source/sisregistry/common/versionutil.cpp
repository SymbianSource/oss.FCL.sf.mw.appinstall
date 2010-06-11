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
* a collection of operators used to facilitate work with TVersion
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <s32strm.h>
#include "versionutil.h"

using namespace Swi;



TBool Swi::operator>=(const TVersion& aVersion1, const TVersion& aVersion2)
	{
	if ((aVersion1.iMajor != KIrrelevant)&&(aVersion2.iMajor != KIrrelevant)&&(aVersion1.iMajor != aVersion2.iMajor))
		{
		return aVersion1.iMajor > aVersion2.iMajor;
		}
	if ((aVersion1.iMinor != KIrrelevant)&&(aVersion2.iMinor != KIrrelevant)&&(aVersion1.iMinor != aVersion2.iMinor))
		{
		return aVersion1.iMinor > aVersion2.iMinor;
		}
	if ((aVersion1.iBuild != KIrrelevant)&&(aVersion2.iBuild != KIrrelevant)&&(aVersion1.iBuild != aVersion2.iBuild))
		{
		return aVersion1.iBuild > aVersion2.iBuild;
		}
	// All KIrrelevant or equal
	return ETrue;
	}
	
TBool Swi::operator<=(const TVersion& aVersion1, const TVersion& aVersion2)
	{
	if ((aVersion1.iMajor != KIrrelevant)&&(aVersion2.iMajor != KIrrelevant)&&(aVersion1.iMajor != aVersion2.iMajor))
		{
		return aVersion1.iMajor < aVersion2.iMajor;
		}
	if ((aVersion1.iMinor != KIrrelevant)&&(aVersion2.iMinor != KIrrelevant)&&(aVersion1.iMinor != aVersion2.iMinor))
		{
		return aVersion1.iMinor < aVersion2.iMinor;
		}
	if ((aVersion1.iBuild != KIrrelevant)&&(aVersion2.iBuild != KIrrelevant)&&(aVersion1.iBuild != aVersion2.iBuild))
		{
		return aVersion1.iBuild < aVersion2.iBuild;
		}
	// All KIrrelevant or equal
	return ETrue;
	}

void Swi::operator<<(RWriteStream& aStream, const TVersion& aVersion) 
	{
	aStream.WriteInt32L(aVersion.iMajor);
	aStream.WriteInt32L(aVersion.iMinor);
	aStream.WriteInt32L(aVersion.iBuild);
	}
	
void Swi::operator>>(RReadStream& aStream, TVersion& aVersion)
	{
	aVersion.iMajor = aStream.ReadInt32L();
	aVersion.iMinor = aStream.ReadInt32L();
	aVersion.iBuild = aStream.ReadInt32L();
	}
	
void Swi::Set(TVersion& aVersion, const Sis::CVersion& aSisVersion)
	{
	aVersion.iMajor = aSisVersion.Major();
 	aVersion.iMinor = aSisVersion.Minor();
 	aVersion.iBuild = aSisVersion.Build();
	}
