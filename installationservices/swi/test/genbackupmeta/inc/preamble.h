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
* For validating SIS file format.
* @internalComponent
*
*/



#ifndef	PREAMBLE_H
#define	PREAMBLE_H

#include "symbiantypes.h"

/**
@file
@internalComponent
Support for handling the preamble of a SIS file i.e. the Symbian standard quad-UID
*/

static const TUint32 KUIDSisxFile = 0x10201A7A;

class CFileContents;

class Preamble
	{
	public:
		
		/**
		Checks if the file is a normal SIS file.
		@param aFileName	SIS File name

		@return bool		true if file is a normal SIS file else false
		*/
		static bool	IsPreamble(CFileContents& aFileContents);
		
		/**
		Checks if the file is a valid SIS file based on the UID provided in Options.
		@param Options&

		@return true	if the package UID of the file is same as specified in Options.
		*/
		static bool	IsUidValid (int aUid);
		
	public:
		enum TUidOffset
			{
			ESISXUid=0,
			EReservedUid,
			EPackageUid,
			EUidCheckSum,
			EUidCount,
			};

	private:
		static TUint32 iUIDs[EUidCount];

	};

#endif	/* __PREAMBLE_H__ */
