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
* Console based SWI.
* @internalComponent
*
*/

 
#include <swi/launcher.h>
#include "cswiconsole.h"
#include "cconsoleuihandler.h"
#include "cpreferences.h"
#include "ciohandler.h"
#include "cpackageinfo.h"
#include "swiconsoleerrors.h"

// Extern variable defined in main.cpp
extern CIoHandler* gIoHandler;

CSwiConsole::CSwiConsole(const CPreferenceHandler& aPreferenceHandler):
				iPreferenceHandler(aPreferenceHandler)
	{
	}
	
CSwiConsole* CSwiConsole::NewL(const CPreferenceHandler& aPreferenceHandler)
	{
	CSwiConsole* self = CSwiConsole::NewLC(aPreferenceHandler);
	CleanupStack::Pop(self);
	
	return self;
	}

CSwiConsole* CSwiConsole::NewLC(const CPreferenceHandler& aPreferenceHandler)
	{
	CSwiConsole* self = new (ELeave)CSwiConsole(aPreferenceHandler);
	CleanupStack::PushL(self);
	
	self->ConstructL();
	
	return self;
	}
	
void CSwiConsole::ConstructL()
	{
		iUiHandler = CConsoleUiHandler::NewL(iPreferenceHandler);
		iPackageInfo = CPackageInfo::NewL();
	}
	
CSwiConsole::~CSwiConsole()
	{
		delete iUiHandler;
		delete iPackageInfo;
	}
	
void CSwiConsole::InstallL()
	{
	VerifyInputL(ESwiInstall);
	Swi::CInstallPrefs *installPref = Swi::CInstallPrefs::NewLC();
	
	if(iPreferenceHandler.GetOcspServerUri().Length() != 0)
		{
		installPref->SetRevocationServerUriL(iPreferenceHandler.GetOcspServerUri());
		}
		
	installPref->SetPerformRevocationCheck(iPreferenceHandler.GetOcspCheckStatus());

	int retValue = Swi::Launcher::Install(*iUiHandler, iPreferenceHandler.GetSisFileName(), *installPref);
	
	switch(retValue)
		{
		case KErrNone:
			break;
		case KErrCancel:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_INSTALLATION_CANCELLED_MSG);
			User::Leave(retValue);
			break;
		case KErrNotFound:
		case KErrPathNotFound:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_SIS_NOT_FOUND_MSG);
			User::Leave(retValue);
			break;
		default:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_EVENT_INSTALL_ABORTED_MSG);
			gIoHandler->WriteLineL(R_ERROR_IN_INSTALLATION_MSG, EIoLogType);
			User::Leave(retValue);
		}
	
	CleanupStack::PopAndDestroy(installPref);
	}
	
void CSwiConsole::UninstallL()
	{
	VerifyInputL(ESwiUnInstall);
	TUid pkgUid = iPreferenceHandler.GetPackageUid();
	
	TInt retValue = KErrNone;
	
	Swi::CSisRegistryPackage* regPackage = NULL;
	TRAP(retValue, regPackage = iPackageInfo->GetSisRegistryPackageL(pkgUid, iPreferenceHandler.GetPackageName(), iPreferenceHandler.GetVendorName()));
	
	if(KErrNone == retValue)
		{
		CleanupStack::PushL(regPackage);
		retValue = Swi::Launcher::Uninstall(*iUiHandler, *regPackage);
		CleanupStack::PopAndDestroy(regPackage);
		}
	
	switch(retValue)
		{
		case KErrNone:
			break;
		case KErrNotFound:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_PKG_NOT_INSTALLED_MSG);
			User::Leave(retValue);
			break;
		case KErrCancel:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_UNINSTALLATION_CANCELLED_MSG);
			User::Leave(retValue);
			break;
		default:
			gIoHandler->WriteL(KLineBreaker);
			gIoHandler->WriteLineL(R_EVENT_UNINSTALL_ABORTED_MSG);
			gIoHandler->WriteLineL(R_ERROR_IN_UNINSTALLATION_MSG);
			User::Leave(retValue);
		}
	}
	
	
void CSwiConsole::ListInstalledPackagesL()
	{
	TUid pkgUid = iPreferenceHandler.GetPackageUid();

	if(pkgUid.iUid)
		{
		iPackageInfo->DisplayPackageL(pkgUid);
		}
	else
		{
		iPackageInfo->ListInstalledAppsL();
		}
	}
	
void CSwiConsole::VerifyInputL(TOperation aOperation)
	{
	TInt stringId = 0;
	switch(aOperation)
		{
		case ESwiInstall:
			{
			if(iPreferenceHandler.GetSisFileName().Length() > 0)
				{
				return;
				}
			stringId = R_ERR_INSTALL_INPUT_MSG;
			break;
			}
		case ESwiUnInstall:
			{
			if(iPreferenceHandler.GetPackageUid().iUid != 0 &&
			   iPreferenceHandler.GetPackageName().Length() > 0 &&
			   iPreferenceHandler.GetVendorName().Length() > 0)
				{
				return;
				}
			stringId = R_ERR_UNINSTALL_INPUT_MSG;
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, User::Panic(KGeneralPanicString, KSwiInvalidSwitchCase));
			}
		}
	gIoHandler->WriteLineL(stringId);
	User::Leave(KMissingMandatoryOption);
	}
