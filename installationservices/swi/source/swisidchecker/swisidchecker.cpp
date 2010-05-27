/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "swisidchecker.h"

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "swiinstallmonitor.h"
#include <f32file.h>

//////////////////////////////
// ECOM Implementation Table
//////////////////////////////


TBool E32Dll()
	{
	return (ETrue);
	}

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10281FBD, CSwiSidChecker::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

//////////////////////////////
// CSwiSidChecker
//////////////////////////////

CSwiSidChecker* CSwiSidChecker::NewL()
	{
	CSwiSidChecker* self = new(ELeave) CSwiSidChecker();
	return self;
	}

CSwiSidChecker::CSwiSidChecker()
	{
	}
	

CSwiSidChecker::~CSwiSidChecker()
	{
	}

TBool CSwiSidChecker::AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive)
	{
	TBool present = EFalse; // init to avoid arm compiler warning
	TRAPD(err, present = AppRegisteredAtInternalL(aSid, aDrive));
	return (err == KErrNone) && present;
	}

TBool CSwiSidChecker::AppRegisteredAtInternalL(const TUid& aSid, TDriveUnit aDrive)
	{
	TBool present = EFalse; // init to avoid arm compiler warning

	Swi::RSisRegistrySession sisRegSession;
	User::LeaveIfError(sisRegSession.Connect());
	CleanupClosePushL(sisRegSession);

	present = sisRegSession.IsSidPresentL(aSid);

	if(present)
		{
		// Check drive as well
		TFileName fileName;
		sisRegSession.SidToFileNameL(aSid, fileName, aDrive);
		TInt drive;
		User::LeaveIfError(RFs::CharToDrive(fileName[0], drive));
		if(drive != aDrive)
			{
			present = EFalse;
			}
		}
	

	CleanupStack::PopAndDestroy(&sisRegSession);
	
	return present;
	}
	
void CSwiSidChecker::SetRescanCallBackL(const TCallBack &aCallBack)
	{
	if(iSwiInstallMonitor)
		{
		delete iSwiInstallMonitor;
		iSwiInstallMonitor = 0;
		}

	if(aCallBack.iFunction)
		{
		iSwiInstallMonitor = CSwiInstallMonitor::NewL(aCallBack);
		iSwiInstallMonitor->Start();
		}
	}

// End of file
