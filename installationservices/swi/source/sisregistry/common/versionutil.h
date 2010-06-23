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
* a selection of operators facilitating the handling of TVersion data
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __VERSIONUTIL_H__
#define __VERSIONUTIL_H__

#include <e32base.h>
#include "sisversion.h"

class RReadStream;
class RWriteStream;

namespace Swi
{
const TInt KIrrelevant =-1;

TBool operator==(const TVersion& a, const TVersion& b);

TBool operator>=(const TVersion& aVersion1, const TVersion& aVersion2);

TBool operator<=(const TVersion& aVersion1, const TVersion& aVersion2);

void operator<<(RWriteStream& aStream, const TVersion& aVersion); 

void operator>>(RReadStream& aStream, TVersion& aVersion);

void Set(TVersion& aVersion, const Sis::CVersion& aSisVersion);


} //namespace Swi

inline TBool Swi::operator == (const TVersion& a, const TVersion& b)
{
 return 
	    ((a.iMajor == b.iMajor) &&
		 (a.iMinor == b.iMinor) &&
		 (a.iBuild == b.iBuild)
		 ) ? ETrue:EFalse;	
}
#endif //__VERSIONUTIL_H__
