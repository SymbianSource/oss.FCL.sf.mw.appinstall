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
* This file implements the client side of the SIF Transport library
*
*/


#include <f32file.h>
#include <usif/sif/sif.h>
#include <scs/scscommon.h>
#include "siftransportclient.h"
#include "siftransportcommon.h"

using namespace Usif;

	EXPORT_C RSifTransportClient::RSifTransportClient()
	: RScsClientBase(), iArgBuf(NULL), iResBuf(NULL), iOngoingFunction(0)
	{
	}

EXPORT_C TInt RSifTransportClient::Connect(const TDesC& aServerName, const TVersion& aRequiredVersion, const TDesC& aServerImageName, TUid aServerUid)
	{
	TUidType uidType(KExecutableImageUid, KNullUid, aServerUid);
	return RScsClientBase::Connect(aServerName, aRequiredVersion, aServerImageName, uidType);
	}

EXPORT_C void RSifTransportClient::Close()
	{
	delete iArgBuf;
	delete iResBuf;

	RScsClientBase::Close();
	}

EXPORT_C void RSifTransportClient::GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs(&aFileName);
	TRAPD(err, aComponentInfo.PrepareForIpcL(ipcArgs, 2));
	if (err)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err);
		return;
		}
	
	CallSifFunction(EGetComponentInfoByFileName, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs;
	TInt transferErr = aFileHandle.TransferToServer(ipcArgs, 0, 1);
	TRAPD(err, aComponentInfo.PrepareForIpcL(ipcArgs, 2));
	if (err || transferErr)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err ? err : transferErr);
		return;
		}
	
	CallSifFunction(EGetComponentInfoByFileHandle, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::Install(const TDesC& aFileName, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	TIpcArgs ipcArgs(&aFileName, aExclusiveOperation);
	CallSifFunction(EInstallByFileName, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	TIpcArgs ipcArgs(&aFileName, aExclusiveOperation);
	TRAPD(err, PrepareOpaqueParamsL(ipcArgs, aArguments, aResults));
	if (err)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err);
		return;
		}
	
	CallSifFunction(EInstallByFileNameWithOpaqueData, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::Install(RFile& aFileHandle, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	TIpcArgs ipcArgs(0, 0, aExclusiveOperation);
	TInt err = aFileHandle.TransferToServer(ipcArgs, 0, 1);
	if (err)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err);
		return;
		}	

	CallSifFunction(EInstallByFileHandle, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	// An axiliary synchronous call to pass the aExclusiveOperation flag the server as a fifth parameter
	TIpcArgs ipcArgs1(aExclusiveOperation);
	CallSessionFunction(EInstallByFileHandleWithOpaqueDataPreamble, ipcArgs1);

	// Proper asynchronous call
	TIpcArgs ipcArgs2;
	TInt transferErr = aFileHandle.TransferToServer(ipcArgs2, 0, 1);
	TRAPD(err, PrepareOpaqueParamsL(ipcArgs2, aArguments, aResults));
	if (err || transferErr)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err ? err : transferErr);
		return;
		}
	CallSifFunction(EInstallByFileHandleWithOpaqueData, ipcArgs2, aStatus);
	}

EXPORT_C void RSifTransportClient::Uninstall(TComponentId aComponentId, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	TIpcArgs ipcArgs(aComponentId, aExclusiveOperation);
	CallSifFunction(EUninstall, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
											COpaqueNamedParams& aResults, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	TIpcArgs ipcArgs(aComponentId, aExclusiveOperation);
	TRAPD(err, PrepareOpaqueParamsL(ipcArgs, aArguments, aResults));
	if (err)
		{
		TRequestStatus *status(&aStatus);
		User::RequestComplete(status, err);
		return;
		}
	
	CallSifFunction(EUninstallWithOpaqueData, ipcArgs, aStatus);
	}
	
EXPORT_C void RSifTransportClient::Activate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs(aComponentId);
	CallSifFunction(EActivate, ipcArgs, aStatus);
	}
	
	
EXPORT_C void RSifTransportClient::Deactivate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs(aComponentId);
	CallSifFunction(EDeactivate, ipcArgs, aStatus);
	}

EXPORT_C void RSifTransportClient::CancelOperation()
	{
	if (iOngoingFunction > 0)
		{
		CancelSessionFunction(iOngoingFunction);
		iOngoingFunction = 0;
		}
	}
	
void RSifTransportClient::CallSifFunction(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iOngoingFunction = aFunction;
	CallSessionFunction(aFunction, aArgs, aStatus);
	}

void RSifTransportClient::PrepareOpaqueParamsL(TIpcArgs& aIpcArgs, const COpaqueNamedParams& aArguments,
											COpaqueNamedParams& aResults)
	{
	aArguments.PrepareArgumentsForIpcL(aIpcArgs, 2);
	aResults.PrepareResultsForIpcL(aIpcArgs, 3);
	}
