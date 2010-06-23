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
* The functions defined in this file provide constant data which is
* used by both the SWI Observer and client side implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SWIOBCOMMON_INL
#define SWIOBCOMMON_INL

#include <swi/swiobcommon.h>

namespace Swi
	{
	inline TVersion SwiObserverVersion()
	/**
		This function is defined because there is no literal constructor for TVersion.

		@return					Defines a version number which the client side code	
								can use to open the SWI Obserevr. If the client code
								was built with a higher version number, then it cannot
								open the SWI Observer.  This ensures that a client only
								talks to the SWI Observer version is at least as high as
								its own.
	*/
		{
		TVersion v(KSwiObserverVerMajor, KSwiObserverVerMinor, KSwiObserverVerBuild);
		return v;
		}

	inline TUidType SwiObserverImageFullUid()
	/**
		This function is defined because there is no literal constructor
		for TUidType.  It returns the SWI Observer executable's UID, which is used
		to ensure the client side code launches the correct server process, as opposed
		to another application which uses the same executable name.

		@return TUidType		The SWI Observer executable's full UID.
	 */
		{
		TUidType t(KExecutableImageUid, KNullUid, KUidSwiObserver);
		return t;
		}

	} // End of namespace Swi 
	
#endif
