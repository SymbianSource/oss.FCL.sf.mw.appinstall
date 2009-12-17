/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* used by both the SISRegistry Helper server and client side implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SISREGISTRYHELPERCOMMON_INL
#define SISREGISTRYHELPERCOMMON_INL

namespace Swi
	{
	inline TVersion SisRegistryHelperServerVersion()
	/**
		This function is defined because there is no literal constructor for TVersion.

		@return	Defines a version number which the client side code	
				can use to open the SISRegistry Helper server. If the client code
				was built with a higher version number, then it cannot
				open the SISRegistryHelper server. This ensures that a client only
				talks to the SISRegistryHelper server version is at least as high as its own.
	*/
		{
		TVersion version(KSisRegistryHelperServerVersionMajor, KSisRegistryHelperServerVersionMinor, KSisRegistryHelperServerVersionBuild);
		return version;
		}

	inline TUidType SisRegistryHelperServerImageFullUid()
	/**
		This function is defined because there is no literal constructor
		for TUidType.  It returns the SISRegistryHelper server executable's UID, which is used
		to ensure the client side code launches the correct server process, as opposed
		to another application which uses the same executable name.

		@return TUidType		The SISRegistry Helper server executable's full UID.
	 */
		{
		TUidType typ(KExecutableImageUid, KNullUid, KSisRegistryHelperServerUid3);
		return typ;
		}

	} // End of namespace Swi
	
#endif // SISREGISTRYHELPERCOMMON_INL
