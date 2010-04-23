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
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __DAEMONBEHAVIOUR_H__
#define __DAEMONBEHAVIOUR_H__

#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>

#include "daemon.h"
#include "daemoninstaller.h"
#include <swidaemonplugin.h>

namespace Swi
{
/**
 * This class implements the specific behaviour when a media insertion
 * or removal is detected, or the daemon is started.
 */
class CDaemonBehaviour : public CBase, public MDaemonBehaviour, public MDaemonInstallBehaviour
	{
public:
	static CDaemonBehaviour* NewL();
	static CDaemonBehaviour* NewLC();
	~CDaemonBehaviour();

	// from MDaemonBahviour
public:	
	/**
	 * Function called on startup of the daemon.
	 * This currently performs no opeartions.
	 */
	TBool StartupL();

	/**
	 * Function called on a media change.
	 * If media is inserted, then this processes pre-installed files on
	 * the card, and notifies IAR. On removal IAR is notified only.
	 */
	void MediaChangeL(TInt aDrive, TChangeType aChangeType);

// Accessor functions
	RApaLsSession& ApaSession();

	// from MDaemonInstallBehaviour
public:	
	/**
	 * Verifies that the application architecture server has a cached list of applications
	 * and return next state of install
	 */
	MDaemonInstallBehaviour::TSisInstallState VerifyAppCacheListL();
	
	/**
	 * Verifies that the Software Installer property has been defined
	 * and return next state of install
	 */
	MDaemonInstallBehaviour::TSisInstallState VerifySwisPropertyL();
	
	/**
	 * Checks the software installer property, to see if busy
	 * and return next state of install
	 */
	MDaemonInstallBehaviour::TSisInstallState VerifySwisIdleL();
	
	/// See MDaemonInstallBehaviour::DoInstallRequestL
	void DoInstallRequestL(const TDesC& aFileName);
	
	/// See MDaemonInstallBehaviour::DoNotifyMediaProcessingComplete
	void DoNotifyMediaProcessingComplete();
	
private:
	void ConstructL();
	
	/**
	 * Processes any pre-installed files present on the drive.
	 *
	 * @param aDrive The drive to check for pre-installed SISX files.
	 */
	void ProcessPreinstalledFilesL(TInt aDrive);
	
	/**
	 * Processes any pre-installed files in the given directory on the drive.
	 @param aDrive The drive to check for pre-installed SISX files.
	 @param aDirectory The directory to search for PreInstalled Files
	 
	 */
	void ProcessPreinstalledFilesL(TInt aDrive, const TDesC& aDirectory);

	void ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3);

	/// Invoke Installation
	void DoInstallL(RFile &aFile,const TUid &appUid);

private:
	RFs iFs;
		
	// The RApaLsSession instance
	RApaLsSession iApaSession;
	
	// The sisx file processor
	CSisInstaller* iSisInstaller;

	CSwiDaemonPlugin* iSwiDaemonPlugin;

	// Drive currently being processed
	TInt iDrive;
	
	// Has MediaProcessingStart already been called?
	TBool iStartNotified;
	};

// inline functions
inline RApaLsSession& CDaemonBehaviour::ApaSession()
	{
	return iApaSession;
	}

	
} // namespace Swi

#endif // #ifndef __DAEMONBEHAVIOUR_H__

