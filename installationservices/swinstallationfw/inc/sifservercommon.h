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
* Common stuff for the Universal Software Install Framework server and client
*
*/


/**
 @file
 @internalComponent
*/

#ifndef SIFSERVERCOMMON_H
#define SIFSERVERCOMMON_H

#include <e32uid.h>

namespace Usif
	{

	/** The name of the SIF Server */
	_LIT(KSifServerName,"!SifServer");

	const TInt KSifVerMajor = 1;	///< SIF server version major component.
	const TInt KSifVerMinor = 0;	///< SIF server version minor component.
	const TInt KSifVerBuild = 0;	///< SIF server version build number.

	/** Executable image which runs the server process. */
	_LIT(KServerImageName, "sifserver.exe");

	/**
		Returns the version of the SIF Server.

		This function is defined because there is no literal constructor for TVersion.

		@return	Defines a version number which the client can use to open the server.  If the client
				was built with a higher version number, then it cannot open the server.  This ensures that
				a client only talks to a server whose version is at least as high as its own.
	 */
	inline TVersion Version()
		{
		TVersion v(KSifVerMajor, KSifVerMinor, KSifVerBuild);
		return v;
		}

	} // End of namespace Usif

#endif	// #ifndef SIFSERVERCOMMON_H
