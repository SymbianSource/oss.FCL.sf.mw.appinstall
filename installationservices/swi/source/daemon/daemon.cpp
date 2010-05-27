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


#include "daemon.h"
#include "drivewatcher.h"
#include "log.h"
#include "securitypolicy.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <e32property.h>
#include <connect/sbdefs.h>
#include <usif/sts/sts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "swiobserverclient.h"
#else
#include "sisregistrysession.h"
#endif


namespace Swi
{
// CDaemon

/*static*/ CDaemon* CDaemon::NewL(MDaemonBehaviour& aBehaviour)
	{
	CDaemon* self=NewLC(aBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CDaemon* CDaemon::NewLC(MDaemonBehaviour& aBehaviour)
	{
	CDaemon* self=new(ELeave) CDaemon(aBehaviour);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CDaemon::~CDaemon()
	{
	iPolicyLockFile.Close();
	iWatchers.ResetAndDestroy();
	iFs.Close();
	}

CDaemon::CDaemon(MDaemonBehaviour& aBehaviour)
	: iBehaviour(aBehaviour)
	{
	}

void CDaemon::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	LockSensitiveFiles();
	StartRecoveryL();
	iBehaviour.StartupL();
	StartWatchersL();
	}

void CDaemon::StartWatchersL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	User::LeaveIfError(fs.ShareProtected());
	
	TDriveList driveList;
	
	User::LeaveIfError(fs.DriveList(driveList));

	for (TInt drive=0; drive < KMaxDrives; ++drive)
		{
		if (driveList[drive] == 0)
			{
			continue;
			}
		TDriveInfo info;
		
		// Ignore errors since the next drive might work
		if (KErrNone != fs.Drive(info, drive))
			{
			DEBUG_PRINTF2(_L8("SWI Daemon - Not watching drive '%d', could not retrieve drive info."),
				drive); 
			continue;
			}

		if (info.iDriveAtt & KDriveAttRemovable)
			{
			DEBUG_PRINTF2(_L8("SWI Daemon - Watching removable drive '%d'"), drive);
			CDriveWatcher* driveWatcher=CDriveWatcher::NewLC(iFs, drive, iBehaviour);
			User::LeaveIfError(iWatchers.Append(driveWatcher));
			CleanupStack::Pop(driveWatcher);
			}
		}
	CleanupStack::PopAndDestroy(&fs);
	} // StartWatchersL()

void CDaemon::LockSensitiveFiles()
	{
	TInt error = iPolicyLockFile.Open(iFs, KSwisPolicyFile, EFileShareReadersOnly | EFileRead);
	if (error != KErrNone)
		{
		DEBUG_PRINTF2(_L8("SWI Daemon - Could not open SWI policy file with error %d!"), error);
		}	
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CDaemon::StartRecoveryL()
	{
	DEBUG_PRINTF(_L("CDaemon::StartRecoveryL()"));

	// Check to see if a backup is in progress...
	TInt backup = 0;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,conn::KUidBackupRestoreKey,backup));
	
	if(backup == conn::EBURUnset || (backup == (conn::EBURNormal | conn::ENoBackup))) //no backup or restore is in progress
		{
		//no recovery attempt is made during backup/restore as recovery may seriously affect the file system, which might impact backup/restore
		//recovery will be made at next bootup
		Usif::RStsRecoverySession stsRecoverySession;
		// A recovery failure should not affect Daemon start up.
		TRAPD(err, stsRecoverySession.RollbackAllPendingL());
		if(err != KErrNone)
			{
			DEBUG_PRINTF2(_L("CDaemon::StartRecoveryL() - STS server failed to recover transactions. Error code: %d"), err);
			}
		//Start SWI Observer so that it can process the log files left from the previous session.
		RSwiObserverSession swiObserver;
		swiObserver.ProcessLogsL(iFs); 
		swiObserver.Close();
		}
	}
#else
void CDaemon::StartRecoveryL()
	{
	DEBUG_PRINTF(_L8("SWI Daemon - Check to see if a recovery is needed"));
	RSisRegistrySession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);
	
	// A recovery failure should not affect SWI Daemon start up.
	TRAP_IGNORE(registrySession.RecoverL());
	
	CleanupStack::PopAndDestroy(&registrySession);
	DEBUG_PRINTF(_L8("SWI Daemon - Recovery check finished"));
	}
#endif

} // namespace Swi
