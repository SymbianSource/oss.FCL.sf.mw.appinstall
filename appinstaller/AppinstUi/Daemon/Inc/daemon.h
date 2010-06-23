/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 * @file 
 *
 * @internalComponent
 * @prototype
 */

#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "drivewatcher.h"

namespace Swi
{
class CDriveWatcher;

class MDaemonBehaviour : public MDriveObserver
	{
public:
	/**
	 * Called by the daemon when it is first started
	 *
	 * @return ETrue to continue, or EFalse to stop the daemon
	 */
	virtual TBool StartupL()=0;
	};

/**
 * The main daemon class which sets up the watchers.
 */
class CDaemon : public CBase
	{
public:
	static CDaemon* NewL(MDaemonBehaviour& aBehaviour);
	static CDaemon* NewLC(MDaemonBehaviour& aBehaviour);
	
	~CDaemon();
	
	const RPointerArray<CDriveWatcher>& Watchers() const; 
private:
	CDaemon(MDaemonBehaviour& aBehaviour);
	void ConstructL();
	
	void StartWatchersL();
	
	// This function should not leave, it is not an inherent functionality of the daemon
	void LockSensitiveFiles(); 
	
	void StartRecoveryL();
	
	/// an M-Class specifying the behaviour of the daemon
	MDaemonBehaviour& iBehaviour;
	
	/// An array of watchers, which watch removable drives
	RPointerArray<CDriveWatcher> iWatchers;	
	
	RFs iFs;
		
	// Used for preventing exclusing locks on the SWI policy file	
	RFile iPolicyLockFile; 
	
	};


// inline functions from CDaemon

inline const RPointerArray<CDriveWatcher>& CDaemon::Watchers() const
	{
	return iWatchers;
	}

} // namespace Swi

#endif // #ifndef __DAEMON_H__
