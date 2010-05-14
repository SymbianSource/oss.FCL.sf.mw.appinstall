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
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SISREGISTRYHELPERCOMMON_H
#define SISREGISTRYHELPERCOMMON_H

#include <e32cmn.h>
#include <e32std.h>
#include <e32ver.h>
#include <e32uid.h>

namespace Swi
{
	/** Identifies the Scr Helper Symbian OS Server. */
	_LIT(KSisRegistryHelperServerName, "!SisRegistryHelper");
	
	/**
		Executable which hosts SCR Helper Server.  Required to launch the process.
		@see KSisRegistryHelperServerUid3
	 */
	_LIT(KSisRegistryHelperServerImg, "sisregistryhelperserver.exe");
	
	/**
		Hosting the executable's secure ID.  Required to launch the process.
		@see KScrHelperServerImg
	 */
	static const TUid KSisRegistryHelperServerUid3 = {0x20025BA8};
	
	static const TInt KSisRegistryHelperServerVersionMajor = 1;  ///< SISRegistry Helper Server version major component.
	static const TInt KSisRegistryHelperServerVersionMinor = 0;  ///< SISRegistry Helper Server version minor component.
	static const TInt KSisRegistryHelperServerVersionBuild = 0;  ///< SISRegistry Helper Server version build component.
	
	enum TSisRegistryHelperServerMessages
	/**
		Functions supported by a SISRegistry Helper Server session.
	 */
		{
		EGetEquivalentLanguages
		};
	
	/**
		Delay in microseconds before the SISRegistry Helper Server is shut down,
		after the last remaining session has been closed.
	 */
	const TInt KSisRegistryHelperServerShutdownPeriod = 2 * 1000 * 1000;
	
	const TInt KMaxEquivalentLanguages = 17;
	
	} // namespace SWI

#include "sisregistryhelpercommon.inl"

#endif /* SISREGISTRYHELPERCOMMON_H */
