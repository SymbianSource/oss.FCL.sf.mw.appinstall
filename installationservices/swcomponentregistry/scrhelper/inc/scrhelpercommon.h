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
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRHELPERCOMMON_H
#define SCRHELPERCOMMON_H

#include <e32cmn.h>
#include <e32std.h>
#include <e32ver.h>
#include <e32uid.h>
#include "usiflog.h"

namespace Usif
	{
	/** Identifies the Scr Helper Symbian OS Server. */
	_LIT(KScrHelperServerName, "!ScrHelper");
	
	/**
		Executable which hosts SCR Helper Server.  Required to launch the process.
		@see KScrHelperServerUid3
	 */
	_LIT(KScrHelperServerImg, "scrhelperserver.exe");
	
	/**
		Hosting the executable's secure ID.  Required to launch the process.
		@see KScrHelperServerImg
	 */
	static const TUid KScrHelperServerUid3 = {0x102866F2};
	
	static const TInt KScrHelperServerVersionMajor = 1;  ///< SCR Helper Server version major component.
	static const TInt KScrHelperServerVersionMinor = 0;  ///< SCR Helper Server version minor component.
	static const TInt KScrHelperServerVersionBuild = 0;  ///< SCR Helper Server version build component.
	
	enum TScrHelperServerMessages
	/**
		Functions supported by a SCR Helper Server session.
	 */
		{
		EGetDatabaseFileHandle,
		EGetJournalFileHandle
		};
	
	/** The path of SCR database file */
	_LIT(KScrDatabaseFilePath, "!:\\sys\\install\\scr\\scr.db");
	
	/** The path of SCR database journal file */
	_LIT(KScrJournalFilePath, "!:\\sys\\install\\scr\\scr.db-journal");
	
	/** The full path of the default SCR database stored in ROM. */
	_LIT(KScrDbRomPath,"z:\\sys\\install\\scr\\provisioned\\scr.db");
	
	/**
		Delay in microseconds before the SCR Helper Server is shut down,
		after the last remaining session has been closed.
	 */
	const TInt KScrHelperServerShutdownPeriod = 2 * 1000 * 1000;
	
	} // namespace Usif

#include "scrhelpercommon.inl"

#endif /* SCRHELPERCOMMON_H */
