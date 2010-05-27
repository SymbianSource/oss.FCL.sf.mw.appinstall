/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Launcher static class is used by clients (EpocInstall, Installer UI, or 
* Control Panel applet) to start installation or uninstallation.
*
*/


/**
 @file
*/

#include "swi/launcher.h"
#include "uissclienthandler.h"
#include "../uiss/server/uissserver.h"
#include "sisuihandler.h"
#include "installclientserver.h"
#include "sishelper.h"
#include "log.h"

#include "sisregistrypackage.h"
#include "sisregistrysession.h"
#include "sisregistryentry.h"


namespace Swi
{

EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler, 
								const TFileName& aFileName,
								const CInstallPrefs& aInstallPrefs)
	{
	DEBUG_PRINTF2(_L("Sis Helper without Device supported languages - Launching Syncronous Install by filename, SIS: %S."),
		&aFileName);
	RArray<TInt> deviceSupportedLanguages;
	TRAPD(err, err=InstallL(aUiHandler, aFileName, aInstallPrefs,deviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper without Device supported languages - Install Result Was %d."), err);
	return err;

	}

EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler, 
								const TFileName& aFileName,
								const CInstallPrefs& aInstallPrefs, const RArray<TInt>& aDeviceSupportedLanguages)
	{
	DEBUG_PRINTF2(_L("Sis Helper with Device supported languages - Launching Syncronous Install by filename, SIS: %S."),
		&aFileName);
	TRAPD(err, err=InstallL(aUiHandler, aFileName, aInstallPrefs,aDeviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper with Device supported languages - Install Result Was %d."), err);
	return err;
	}


TInt Launcher::InstallL(MUiHandler& aUiHandler, 
						const TFileName& aFileName,
						const CInstallPrefs& aInstallPrefs,
						const RArray<TInt>& aDeviceSupportedLanguages)
	{
	TSisHelperStartParams params(aFileName);
	RThread server;
	TInt err = CSisHelperServer::StartSisHelper(params, server);
	
	if (err == KErrNone)
		{
		// Connect to UISS and start installation using asynchronous calls
		// and reverse completion
		CleanupClosePushL(server);
		CUissClientHandler* uissHandler = CUissClientHandler::NewLC(aUiHandler, false);
		CleanupStack::Pop(2,&server);
		CleanupStack::PushL(uissHandler);
		
		TRequestStatus requestStatus;
		uissHandler->InstallL(aInstallPrefs, aDeviceSupportedLanguages, requestStatus, server);
		uissHandler->WorkUntilCompleteL();
		User::WaitForRequest(requestStatus);
		err = requestStatus.Int();
		
		CleanupStack::PopAndDestroy(uissHandler);
		}
	return err;
	}

EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler,
								RFile& aFileHandle, 
								const CInstallPrefs& aInstallPrefs)
	{
	DEBUG_PRINTF(_L8("Sis Helper without Device supported languages - Launching Syncronous Install by filehandle."));
	RArray<TInt> deviceSupportedLanguages;
	TRAPD(err, err=InstallL(aUiHandler, aFileHandle, aInstallPrefs,deviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper without Device supported languages - Install Result Was %d."), err);
	return err;

	}

EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler,
								RFile& aFileHandle, 
								const CInstallPrefs& aInstallPrefs,
								const RArray<TInt>& aDeviceSupportedLanguages)
	{
	DEBUG_PRINTF(_L8("Sis Helper with Device supported languages - Launching Syncronous Install by filehandle."));
	TRAPD(err, err=InstallL(aUiHandler, aFileHandle, aInstallPrefs,aDeviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper with Device supported languages - Install Result Was %d."), err);
	return err;
	}

	
TInt Launcher::InstallL(MUiHandler& aUiHandler,
						RFile& aFileHandle, 
						const CInstallPrefs& aInstallPrefs,
						const RArray<TInt>& aDeviceSupportedLanguages)
	{
	TSisHelperStartParams params(aFileHandle);
	RThread server;
	TInt err = CSisHelperServer::StartSisHelper(params, server);

	if (err == KErrNone)
		{
		// Connect to UISS and start installation using asynchronous calls
		// and reverse completion
		CleanupClosePushL(server);
		CUissClientHandler* uissHandler = CUissClientHandler::NewLC(aUiHandler, false);
		CleanupStack::Pop(2,&server);
		CleanupStack::PushL(uissHandler);
		
		TRequestStatus requestStatus;
		uissHandler->InstallL(aInstallPrefs, aDeviceSupportedLanguages, requestStatus, server);
		uissHandler->WorkUntilCompleteL();
		User::WaitForRequest(requestStatus);
		err = requestStatus.Int();
		
		CleanupStack::PopAndDestroy(uissHandler);
		}
	return err;
	}


//
// An override that takes a data provider interface pointer instead of a 
// file name
//
EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler,
								MSisDataProvider& aDataProvider, 
								const CInstallPrefs& aInstallPrefs)
	{
	DEBUG_PRINTF(_L8("Sis Helper without Device supported languages - Launching Syncronous Install by filehandle."));
	RArray<TInt> deviceSupportedLanguages;
	TRAPD(err, err=InstallL(aUiHandler, aDataProvider, aInstallPrefs,deviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper without Device supported languages - Install Result Was %d."), err);
	return err;
	}

EXPORT_C TInt Launcher::Install(MUiHandler& aUiHandler,
								MSisDataProvider& aDataProvider, 
								const CInstallPrefs& aInstallPrefs,
								const RArray<TInt>& aDeviceSupportedLanguages)
	{
	DEBUG_PRINTF(_L8("Sis Helper with Device supported languages - Launching Syncronous Install by filehandle."));
	TRAPD(err, err=InstallL(aUiHandler, aDataProvider, aInstallPrefs,aDeviceSupportedLanguages));
	DEBUG_PRINTF2(_L8("Sis Helper with Device supported languages - Install Result Was %d."), err);
	return err;
	}


TInt Launcher::InstallL(MUiHandler& aUiHandler,
						MSisDataProvider& aDataProvider, 
						const CInstallPrefs& aInstallPrefs,
						const RArray<TInt>& aDeviceSupportedLanguages)
	{
	TSisHelperStartParams params(aDataProvider);
	RThread server;
	TInt err = CSisHelperServer::StartSisHelper(params, server);
	
	if (err == KErrNone)
		{
		// Connect to UISS and start installation using asynchronous calls
		// and reverse completion
		CleanupClosePushL(server);
		CUissClientHandler* uissHandler = CUissClientHandler::NewLC(aUiHandler, false);
		CleanupStack::Pop(2,&server);
		CleanupStack::PushL(uissHandler);
		
		TRequestStatus requestStatus;
		uissHandler->InstallL(aInstallPrefs, aDeviceSupportedLanguages, requestStatus, server);
		uissHandler->WorkUntilCompleteL();
		User::WaitForRequest(requestStatus);
		err = requestStatus.Int();
		
		CleanupStack::PopAndDestroy(uissHandler);
		}
	return err;
	}
	


EXPORT_C TInt Launcher::Uninstall(MUiHandler& aUiHandler, const TUid& aUid)
	{
	DEBUG_PRINTF2(_L8("Sis Helper - Launching uninstall by UID 0x%08x."), aUid.iUid);
	
	CSisRegistryPackage* package=NULL;

	TRAPD(err, package = CSisHelperServer::MainPackageEntryL(aUid));
	
	if (err==KErrNone && package)
		{
		// Uninstall base index
		package->SetIndex(0);

		TRAP(err, err=UninstallL(aUiHandler, *package));
		}
	
	delete package;
	
	DEBUG_PRINTF2(_L8("Sis Helper - Uninstall Result Was %d."), err);
	return err;
	}

EXPORT_C TInt Launcher::Uninstall(MUiHandler& aUiHandler, const CSisRegistryPackage& aPackage)
	{
	DEBUG_PRINTF4(_L("Sis Helper - Launching uninstall by package. UID: 0x%08x, Name: %S, Vendor: %S."),
		aPackage.Uid().iUid, &(aPackage.Name()), &(aPackage.Vendor()));
	
	TRAPD(err, err=UninstallL(aUiHandler, aPackage));
	
	DEBUG_PRINTF2(_L8("Sis Helper - Uninstall result was %d."), err);
	return err;
	}

TInt Launcher::UninstallL(MUiHandler& aUiHandler, const CSisRegistryPackage& aPackage)
	{
	// Connect to UISS and start uninstallation using asynchronous calls
	// and reverse completion
	CUissClientHandler* uissHandler = CUissClientHandler::NewLC(aUiHandler, false);
	
	TRequestStatus requestStatus;
	uissHandler->UninstallL(aPackage, requestStatus);
	uissHandler->WorkUntilCompleteL();
	User::WaitForRequest(requestStatus);
	
	CleanupStack::PopAndDestroy(uissHandler);
	return requestStatus.Int();
	}

///\short Cancels [un]installation in progress.
EXPORT_C int Launcher::Cancel()
	{
	RUissSession serverSession;
	TInt err=serverSession.Connect();
	
	if (err<0)
		{
		return err;
		}
	
	err=serverSession.Cancel();
	
	serverSession.Close();
	return err;
	}

} // namespace Swi
