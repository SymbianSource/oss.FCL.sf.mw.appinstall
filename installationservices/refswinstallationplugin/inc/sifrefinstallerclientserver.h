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
* This file defines constants for the Refrence Installer client/server layer.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef SIFREFINSTALLERCLIENTSERVER_H
#define SIFREFINSTALLERCLIENTSERVER_H

#include <e32uid.h>

namespace Usif
	{
	/**
	The name of the Reference Installer Server.
	*/
	_LIT(KSifRefInstallerServerName,"!SifRefInstallerServer");

	/**
	The version of the Reference Installer Server.
	*/
	const TInt KSifRefServerVerMajor = 1;	///< SIF Reference Installer Server version major component.
	const TInt KSifRefServerVerMinor = 0;	///< SIF Reference Installer Server version minor component.
	const TInt KSifRefServerVerBuild = 0;	///< SIF Reference Installer Server version build number.

	/**
	Returns the version of the Reference Installer Server as a TVersion object.

	This function is defined because there is no literal constructor for TVersion.
	*/
	TVersion Version()
		{
		TVersion v(KSifRefServerVerMajor, KSifRefServerVerMinor, KSifRefServerVerBuild);
		return v;
		}

	} // End of namespace Usif

#endif	// #ifndef SIFREFINSTALLERCLIENTSERVER_H
