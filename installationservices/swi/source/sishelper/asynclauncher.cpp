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
* This file defines the Swi::AsyncLauncher class that is used to
* launch asynchronous installation or removal.
*
*/


/**
 @file
*/

#include "swi/asynclauncher.h"
#include "uissclienthandler.h"
#include "sishelper.h"
#include "log.h"
#include "sisregistrypackage.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swi/nativecomponentinfo.h"
#include <usif/sif/sifcommon.h>
#endif

namespace Swi
{

EXPORT_C CAsyncLauncher *CAsyncLauncher::NewLC()
	{
	CAsyncLauncher* self=new(ELeave) CAsyncLauncher();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CAsyncLauncher *CAsyncLauncher::NewL()
	{
	CAsyncLauncher* self=NewLC();
	CleanupStack::Pop(self);
	return self;
	}


CAsyncLauncher::CAsyncLauncher()
	{
	}

EXPORT_C CAsyncLauncher::~CAsyncLauncher()
	{
	delete iUissClientHandler;
	iUissClientHandler = 0;
	}

TBool CAsyncLauncher::VerifyBusyL(TRequestStatus &aRequest)
	{
	if(iUissClientHandler && iUissClientHandler->IsBusy())
		{
		TRequestStatus *reqPtr = &aRequest;
		User::RequestComplete(reqPtr, KErrServerBusy);
		return ETrue;
		}	
	return EFalse;
	}
	
void CAsyncLauncher::DoInstallL(TSisHelperStartParams& aHelperParams,
								MUiHandler& aUiHandler,
								const CInstallPrefs& aInstallPrefs,
								const RArray<TInt>& aDeviceSupportedLanguages,
								TRequestStatus &aRequest)
	{
	RThread server;
	User::LeaveIfError(CSisHelperServer::StartSisHelper(aHelperParams, server));

	delete iUissClientHandler;
	iUissClientHandler = 0;
	CleanupClosePushL(server);
	iUissClientHandler = CUissClientHandler::NewL(aUiHandler);
	iUissClientHandler->InstallL(aInstallPrefs, aDeviceSupportedLanguages, aRequest, server);
	CleanupStack::Pop(&server);		
	}
	
EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler,
									   const TFileName& aFileName, 
									   const CInstallPrefs& aInstallPrefs,
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF2(_L("Sis Helper - Lauching Asynchronous install by filename %S."), &aFileName);
	RArray<TInt> deviceSupportedLanguages;
	InstallL(aUiHandler,aFileName, aInstallPrefs, deviceSupportedLanguages, aRequest);
	
	}
	
EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler,
									   const TFileName& aFileName, 
									   const CInstallPrefs& aInstallPrefs,
									   const RArray<TInt>& aDeviceSupportedLanguages, 
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF2(_L("Sis Helper - Lauching Asynchronous install by filename %S."), &aFileName);
	
	if (VerifyBusyL(aRequest))
		return;

	TSisHelperStartParams params(aFileName);
	DoInstallL(params, aUiHandler, aInstallPrefs, aDeviceSupportedLanguages, aRequest);
	}

EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler, 
									   MSisDataProvider& aDataProvider,
									   const CInstallPrefs& aInstallPrefs,
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Launching Asyncronous install by data provider."));
	RArray<TInt> deviceSupportedLanguages;
	InstallL(aUiHandler,aDataProvider, aInstallPrefs, deviceSupportedLanguages, aRequest);
	}

EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler, 
									   MSisDataProvider& aDataProvider,
									   const CInstallPrefs& aInstallPrefs,
									   const RArray<TInt>& aDeviceSupportedLanguages,
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Launching Asyncronous install by data provider."));
	
	if (VerifyBusyL(aRequest))
		return;

	TSisHelperStartParams params(aDataProvider);
	DoInstallL(params, aUiHandler, aInstallPrefs, aDeviceSupportedLanguages, aRequest);
	}

EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler, 
									   RFile& aFileHandle,
									   const CInstallPrefs& aInstallPrefs,
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Launching Asynchronous install by file handle."));
	RArray<TInt> deviceSupportedLanguages;
	InstallL(aUiHandler,aFileHandle, aInstallPrefs, deviceSupportedLanguages, aRequest);
	}

EXPORT_C void CAsyncLauncher::InstallL(MUiHandler& aUiHandler, 
									   RFile& aFileHandle,
									   const CInstallPrefs& aInstallPrefs,
									   const RArray<TInt>& aDeviceSupportedLanguages,
									   TRequestStatus &aRequest)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Launching Asynchronous install by file handle."));
	
	if (VerifyBusyL(aRequest))
		return;

	TSisHelperStartParams params(aFileHandle);
	DoInstallL(params, aUiHandler, aInstallPrefs, aDeviceSupportedLanguages, aRequest);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
void CAsyncLauncher::DoGetComponentInfoL(TSisHelperStartParams& aHelperParams, MUiHandler& aUiHandler, const CInstallPrefs& aInstallPrefs,
										Usif::CComponentInfo& aComponentInfo, TRequestStatus &aRequest)
	{
	RThread server;
	User::LeaveIfError(CSisHelperServer::StartSisHelper(aHelperParams, server));

	delete iUissClientHandler;
	iUissClientHandler = 0;
	CleanupClosePushL(server);
	iUissClientHandler = CUissClientHandler::NewL(aUiHandler);
	iUissClientHandler->GetComponentInfoL(aInstallPrefs, aComponentInfo, aRequest, server);
	CleanupStack::Pop(&server);		
	}	

// Get the component information using the given SIS file name.
EXPORT_C void CAsyncLauncher::GetComponentInfoL(MUiHandler& aUiHandler,
									   const TFileName& aFileName, 
									   const CInstallPrefs& aInstallPrefs,
									   Usif::CComponentInfo& aComponentInfo,
									   TRequestStatus &aRequest)
	{	
	DEBUG_PRINTF2(_L("Sis Helper - Lauching asynchronous component info for filename %S."), &aFileName);
	if (VerifyBusyL(aRequest))
		return;

	TSisHelperStartParams params(aFileName);
	DoGetComponentInfoL(params, aUiHandler, aInstallPrefs, aComponentInfo, aRequest);
	}
	
// Get the component information using the given SIS file handle.
EXPORT_C void CAsyncLauncher::GetComponentInfoL(MUiHandler& aUiHandler, 
									   RFile& aFileHandle,
									   const CInstallPrefs& aInstallPrefs,
									   Usif::CComponentInfo& aComponentInfo,
									   TRequestStatus &aRequest)
	{	
	DEBUG_PRINTF(_L8("Sis Helper - Launching asynchronous component info by file handle."));
	
	if (VerifyBusyL(aRequest))
		return;

	TSisHelperStartParams params(aFileHandle);
	DoGetComponentInfoL(params, aUiHandler, aInstallPrefs, aComponentInfo, aRequest);
	}
#endif

EXPORT_C void CAsyncLauncher::UninstallL(MUiHandler& aUiHandler, 
										 const TUid& aUid,
										 TRequestStatus &aRequest)
	{
	DEBUG_PRINTF2(_L8("Sis Helper - Launching Asynchronous uninstall by UID 0x%08x."), aUid.iUid);
	
	CSisRegistryPackage* package = CSisHelperServer::MainPackageEntryL(aUid);
	CleanupStack::PushL(package);

	// Uninstall base index
	package->SetIndex(0);

	UninstallL(aUiHandler, *package, aRequest);

	CleanupStack::PopAndDestroy(package);
	}

EXPORT_C void CAsyncLauncher::UninstallL(MUiHandler& aUiHandler,
										 const CSisRegistryPackage& aPackage,
										 TRequestStatus &aRequest)
	{
	DEBUG_PRINTF4(_L("Sis Helper - Launching Asyncronous uninstall by package. UID: 0x%08x, Name: %S, Vendor: %S."),
		aPackage.Uid().iUid, &(aPackage.Name()), &(aPackage.Vendor()));
	
	if(iUissClientHandler && iUissClientHandler->IsBusy())
		{
		TRequestStatus *reqPtr = &aRequest;
		User::RequestComplete(reqPtr, KErrServerBusy);
		return;
		}

	delete iUissClientHandler;
	iUissClientHandler = 0;
	iUissClientHandler = CUissClientHandler::NewL(aUiHandler);
	iUissClientHandler->UninstallL(aPackage, aRequest);
	}

EXPORT_C void CAsyncLauncher::CancelOperation()
	{
	DEBUG_PRINTF(_L8("Sis Helper - Cancelling Async. Install/Uninstall Operation."));
	
	// Cancel our request which will complete the our client's
	// TRequestStatus
	if (iUissClientHandler)
		{
		iUissClientHandler->CancelOperation();
		}
	}

} // namespace Swi

// End of file
