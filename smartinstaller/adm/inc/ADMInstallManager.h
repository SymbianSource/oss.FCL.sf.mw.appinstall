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
*     Declares the CInstallManager class for ADM application.
*
*
*/


#ifndef __ADMINSTALLMANAGER_H__
#define __ADMINSTALLMANAGER_H__

#include <e32base.h>
#include <swinstdefs.h>
#include <swinstapi.h>

//TODO: KNoDriveSelected is currently redefined in statemachine.cpp.
//#include <sisregistrysession.h> //for KNoDriveSelected

class TSisInfo;

class CInstallManager: public CActive
	{
public:
	class MInstallClient
		{
	public:
		virtual void HandleInstallCompletedL(TInt aStatus) = 0;
		};
#ifdef USE_LOGFILE
public:
	static CInstallManager* NewL(MInstallClient* aInstallClient, RFileLogger& aLogger);
	static CInstallManager* NewLC(MInstallClient* aInstallClient, RFileLogger& aLogger);
private:
	CInstallManager(MInstallClient* aInstallClient, RFileLogger& aLogger);

private:
	RFileLogger& iLog;
#else
public:
	static CInstallManager* NewL(MInstallClient* aInstallClient);
	static CInstallManager* NewLC(MInstallClient* aInstallClient);
private:
	CInstallManager(MInstallClient* aInstallClient);
#endif

public:
	~CInstallManager();

public:
	TBool IsPackagePresentL( const TUid& aUid );
	TBool GetPackageVersionInfoL(const TUid& aUid, TVersion& aVersion);
	TBool GetPackageInstalledDriveL(const TUid& aUid, TChar& aWrapperDrive);
	TBool GetPackageSidsL(const TUid& aUid, RArray<TUid>& aSids);
	void SilentInstallPackageL(const TDesC& aInstallFilename, const TChar aInstallDrive = 'C');
	TInt SilentUnInstallPackage(const TUid& aPkgUid);
	void SilentUninstallPackageAsync(const TUid& aPkgUid);
	TInt InstallPackage(const TDesC& aInstallFilename);
	TUid GetPackageUidL(const TUid& aSid);
	void CancelAll();
	TInt CancelInstallation();
	TInt CancelUninstallation();

protected:
	TSisInfo GetPackageInfoL(const TUid& aUid, TBool& aIsInstalled);
	void RunL();
	void DoCancel();

private:
	void ConstructL();

private:
	MInstallClient*    iInstallClient;
	HBufC*             iInstallFilename;
	TInt               iInstallAttempt;

	SwiUI::TInstallOptionsPckg     iOptionsPckg;
	SwiUI::TUninstallOptionsPckg   iUninstallOptionsPckg;
	SwiUI::RSWInstSilentLauncher   iLauncher;
	};

#endif  //__ADMINSTALLMANAGER_H__
