/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CInstallManager implementation
*
*
*/


#include <swi/swispubsubdefs.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <e32base.h>
#include "ADMAppUi.h"
#include "ADMInstallManager.h"

#include "macros.h"
#include "debug.h"

class TSisInfo
	{
public:
	TVersion iVersion;
	TChar iInstalledDrive;
	RArray<TUid> iSids;
	};

_LIT8(KMIME,"x-epoc/x-sisx-app");

#ifdef USE_LOGFILE
CInstallManager* CInstallManager::NewLC(MInstallClient* aInstallClient, RFileLogger& aLogger)
	{
	CInstallManager* object = new ( ELeave ) CInstallManager(aInstallClient, aLogger);
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CInstallManager* CInstallManager::NewL(MInstallClient* aInstallClient, RFileLogger& aLogger)
	{
	CInstallManager* object = CInstallManager::NewLC(aInstallClient, aLogger);
	CleanupStack::Pop();
	return object;
	}

CInstallManager::CInstallManager(MInstallClient* aInstallClient, RFileLogger& aLogger) :
	CActive(EPriorityStandard),
	iLog(aLogger),
	iInstallClient(aInstallClient)
	{
	CActiveScheduler::Add(this);
	}
#else
CInstallManager* CInstallManager::NewLC(MInstallClient* aInstallClient)
	{
	CInstallManager* object = new ( ELeave ) CInstallManager(aInstallClient);
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CInstallManager* CInstallManager::NewL(MInstallClient* aInstallClient)
	{
	CInstallManager* object = CInstallManager::NewLC(aInstallClient);
	CleanupStack::Pop();
	return object;
	}

CInstallManager::CInstallManager(MInstallClient* aInstallClient) :
	CActive(EPriorityStandard),
	iInstallClient(aInstallClient)
	{
	CActiveScheduler::Add(this);
	}
#endif

CInstallManager::~CInstallManager()
	{
	Cancel();
	iLauncher.Close();
	DELETE_IF_NONNULL( iInstallFilename );
	}

void CInstallManager::RunL()
	{
	if ( ((iStatus.Int() == SwiUI::KSWInstErrBusy )||(iStatus.Int() == KErrInUse) || (iStatus.Int() == KErrServerBusy) ) &&
			((++iInstallAttempt) <= KAttemptsToAccessInstaller) )
		{
		LOG( "CInstallManager: Installer busy, waiting" );
		User::After( KIterationTimer );
		// Start asynchronous silent installation again
		if( !IsActive() )
			{
			SetActive();
			}

		iStatus = KRequestPending;

		iLauncher.SilentInstall( iStatus, *iInstallFilename, iOptionsPckg );
		}
	else
		{
		LOG2( "CInstallManager: Install completed (%d)", iInstallAttempt );
		iInstallClient->HandleInstallCompletedL( iStatus.Int() );
		}
	}

void CInstallManager::DoCancel()
	{
	CancelInstallation();
	CancelUninstallation();
	}

void CInstallManager::ConstructL()
	{
	User::LeaveIfError(iLauncher.Connect());
	}

TBool CInstallManager::IsPackagePresentL( const TUid& aUid )
	{
	TSisInfo sisInfo;
	TBool isInstalled;

	sisInfo = GetPackageInfoL(aUid, isInstalled);

	return isInstalled;
	}

TBool CInstallManager::GetPackageSidsL(const TUid& aUid, RArray<TUid>& aSids)
	{
	TSisInfo sisInfo;
	TBool isInstalled;

	sisInfo = GetPackageInfoL(aUid, isInstalled);

	if (isInstalled)
		{
		aSids = sisInfo.iSids;
		}

	return isInstalled;
	}

TBool CInstallManager::GetPackageInstalledDriveL(const TUid& aUid, TChar& aWrapperDrive)
	{
	TSisInfo sisInfo;
	TBool isInstalled;

	sisInfo = GetPackageInfoL(aUid, isInstalled);

	if (isInstalled)
		{
		aWrapperDrive = sisInfo.iInstalledDrive;
		}

	return isInstalled;
	}

TBool CInstallManager::GetPackageVersionInfoL(const TUid& aUid, TVersion& aVersion)
	{
	TSisInfo sisInfo;
	TBool isInstalled;

	sisInfo = GetPackageInfoL(aUid, isInstalled);

	if (isInstalled)
		{
		aVersion = sisInfo.iVersion;
		}

	return isInstalled;
	}

TSisInfo CInstallManager::GetPackageInfoL(const TUid& aUid, TBool& aIsPkgInstalled)
	{
	TSisInfo sisInfo;
	// Connect to SisRegistry to fetch Package info
	Swi::RSisRegistrySession registrySession;

	User::LeaveIfError( registrySession.Connect() );
	CleanupClosePushL(registrySession);

	aIsPkgInstalled = registrySession.IsInstalledL( aUid );

	if ( aIsPkgInstalled )
		{
		Swi::RSisRegistryEntry entry;

		// Open registry entry to get version information.
		User::LeaveIfError(entry.Open(registrySession, aUid));
		CleanupClosePushL(entry);

		// Get the data
		sisInfo.iVersion = entry.VersionL();
		sisInfo.iInstalledDrive = entry.SelectedDriveL();
		entry.SidsL(sisInfo.iSids);

		CleanupStack::PopAndDestroy(&entry);
		}

	CleanupStack::PopAndDestroy(&registrySession);

	return sisInfo;
	}

void CInstallManager::SilentInstallPackageL(const TDesC& aInstallFilename, const TChar aInstallDrive)
	{
	LOG2( "+ SilentInstallPackage(): '%S'", &aInstallFilename );

	DELETE_IF_NONNULL( iInstallFilename );
	iInstallFilename = aInstallFilename.AllocL();
	iInstallAttempt = 0;

	SwiUI::TInstallOptions Options;

	Options.iUpgrade = SwiUI::EPolicyAllowed;
	Options.iOCSP    = SwiUI::EPolicyNotAllowed;
	RFs::DriveToChar(aInstallDrive, Options.iDrive);
	Options.iUntrusted = SwiUI::EPolicyNotAllowed;
	Options.iCapabilities = SwiUI::EPolicyAllowed;
	Options.iOverwrite = SwiUI::EPolicyAllowed;

	iOptionsPckg = Options;

	// We have to cancel any pending async request, because we might
	// get called during installation (user cancel, for instance)
	if (IsActive())
		{
		Cancel();
		}

	// Start asynchronous silent installation
	iStatus = KRequestPending;
	SetActive();
	iLauncher.SilentInstall( iStatus, *iInstallFilename, iOptionsPckg);
	}

TInt CInstallManager::SilentUnInstallPackage(const TUid& aPkgUid)
	{
	TInt ret;

	SwiUI::TUninstallOptions Options;
	SwiUI::TUninstallOptionsPckg OptionsPckg;

	// TODO: Missing iBreakDependency: Allowed or not?
	Options.iBreakDependency = SwiUI::EPolicyAllowed;
	Options.iKillApp = SwiUI::EPolicyAllowed;
	OptionsPckg = Options;

	// Start silent uninstallation
	// If connection to Install server fails,
	// make KAttemptsToAccessInstaller connection attempts
	for (TInt i = 0; i < KAttemptsToAccessInstaller; i++)
		{
		// Silent Uninstallation
		ret = iLauncher.SilentUninstall( aPkgUid, OptionsPckg, KMIME );

		// SWI returns KErrInUse & KErrServerBusy as well.
		// SwiUI errors are returned as is
		if ( ret == SwiUI::KSWInstErrBusy ||
			 ret == KErrInUse ||
			 ret == KErrServerBusy)
			{
			LOG3( "Installer busy, retrying (%d: %d)", i, ret );
			// Wait a while and then retry the uninstallation again
			User::After( KIterationTimer );
			}
		else
			{
			break;
			}
		}

	return ret;
	}

void CInstallManager::SilentUninstallPackageAsync(const TUid& aPkgUid)
	{
	SwiUI::TUninstallOptions options;

	// TODO: Missing iBreakDependency: Allowed or not?
	options.iBreakDependency = SwiUI::EPolicyAllowed;
	options.iKillApp = SwiUI::EPolicyAllowed;
	iUninstallOptionsPckg = options;

	// We have to cancel any pending async request, because we might
	// get called during uninstallation (user cancel, for instance)
	if (IsActive())
		{
		Cancel();
		}

	// Start asynchronous silent uninstallation
	iStatus = KRequestPending;
	SetActive();
	iLauncher.SilentUninstall( iStatus, aPkgUid, iUninstallOptionsPckg, KMIME );
	}

void CInstallManager::CancelAll()
	{
	Cancel();
	CancelInstallation();
	CancelUninstallation();
	}

TInt CInstallManager::CancelInstallation()
	{
	return iLauncher.CancelAsyncRequest(SwiUI::ERequestSilentInstall);
	}

TInt CInstallManager::CancelUninstallation()
	{
	return iLauncher.CancelAsyncRequest(SwiUI::ERequestSilentUninstall);
	}

TInt CInstallManager::InstallPackage(const TDesC& aInstallFilename)
	{
	LOG2( "+ InstallPackage(): '%S'", &aInstallFilename );

	DELETE_IF_NONNULL( iInstallFilename );
	iInstallFilename = aInstallFilename.AllocL();

	TInt ret;
	SwiUI::RSWInstLauncher Launcher;

	ret = Launcher.Connect();
	if (ret != KErrNone)
		{
		return ret;
		}

	// Start Installation
	// If connection to Install server fails,
	// make KAttemptsToAccessInstaller connection attempts
	for (TInt i = 0; i < KAttemptsToAccessInstaller; i++)
		{
		// Normal installation
		ret = Launcher.Install(*iInstallFilename);

		// SWI returns KErrInUse & KErrServerBusy as well.
		// SwiUI errors are returned as is
		if ( ret == SwiUI::KSWInstErrBusy ||
			 ret == KErrInUse ||
			 ret == KErrServerBusy)
			{
			LOG3( "Installer busy, retrying (%d: %d)", i, ret );
			// Wait a while and then retry the uninstallation again
			User::After( KIterationTimer );
			}
		else
			{
			break;
			}
		}
	Launcher.Close();
	return ret;
	}

TUid CInstallManager::GetPackageUidL(const TUid& aSid)
	{
	// Connect to SisRegistry to fetch Package info
	Swi::RSisRegistrySession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);

	Swi::CSisRegistryPackage* pkg = NULL;
	pkg = registrySession.SidToPackageL(aSid);

	CleanupStack::PopAndDestroy(&registrySession);

	TUid uid = TUid::Uid(0);
	if ( pkg )
		{
		uid = pkg->Uid();
		}

	delete pkg;

	return uid;
	}
