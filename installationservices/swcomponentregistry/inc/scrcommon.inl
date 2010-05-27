/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The functions defined in this file provide constant data which is
* used by both the SCR server and client side implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SCRCOMMON_INL
#define SCRCOMMON_INL

#include <usif/scr/scrcommon.h>

namespace Usif
	{
	inline TVersion ScrServerVersion()
	/**
		This function is defined because there is no literal constructor for TVersion.

		@return					Defines a version number which the client side code	
								can use to open the SCR server. If the client code
								was built with a higher version number, then it cannot
								open the SCR server.  This ensures that a client only
								talks to the SSCR serevr version is at least as high as
								its own.
	*/
		{
		TVersion version(KScrVerMajor, KScrVerMinor, KScrVerBuild);
		return version;
		}

	inline TUidType ScrServerImageFullUid()
	/**
		This function is defined because there is no literal constructor
		for TUidType.  It returns the SCR server executable's UID, which is used
		to ensure the client side code launches the correct server process, as opposed
		to another application which uses the same executable name.

		@return TUidType		The SCR server executable's full UID.
	 */
		{
		TUidType typ(KExecutableImageUid, KNullUid, KUidScrServer);
		return typ;
		}

	} // End of namespace Usif 
	
#endif // SCRCOMMON_INL
