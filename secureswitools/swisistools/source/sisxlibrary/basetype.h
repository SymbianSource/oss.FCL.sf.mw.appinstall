/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* simple types and local definitions of standard types
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#include "symbiantypes.h"


typedef struct
	{
	wchar_t*		iName;
	TUint32			iId;
	} SKeyword;

// all borrowed from instform.h, to reduce MakeSIS's dependence on a file that's actually
// written for a totall different purpose, and liable to random change so far as MakeIS is concerned.
enum THeaderFlags
	{
	EInstIsUnicode			= 1<<0,
	EInstIsDistributable	= 1<<1,
	EInstIsCompareToMajor	= 1<<2,
	EInstNoCompress			= 1<<3,
	EInstShutdownApps		= 1<<4,
	EInstNonRemovable		= 1<<5,
	EInstROMUpgrade			= 1<<6,
	EInstHide				= 1<<7
	};

enum TVariableToken
	{
	EVarLanguage=0x1000,
	EVarRemoteInstall,
		
	EVarOptionBase=0x2000
	};

#endif // __BASETYPE_H__

