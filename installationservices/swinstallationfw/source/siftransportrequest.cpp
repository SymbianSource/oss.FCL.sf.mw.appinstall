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
* Implements an AsyncRequest object for the SIF Transport library
*
*/


#include <e32base.h>
#include <e32std.h>
#include <apgcli.h>
#include <scs/ipcstream.h>
#include <scs/nullstream.h>
#include <scs/cleanuputils.h>
#include <usif/usiferror.h>
#include "usiflog.h"
#include "siftransportserver.h"
#include "siftransportcommon.h"

using namespace Usif;

void CSifTransportRequest::CreateAndExecuteL(TInt aFunction, CSifTransportSession* aSession, TransportTaskFactory::GenerateTask aTaskFactory, const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("CSifTransportRequest::CreateAndExecuteL for aFunction = %d\n"),aFunction);
		
	CSifTransportRequest* self = new (ELeave) CSifTransportRequest(aFunction, aSession, aMessage);
	CleanupStack::PushL(self);
	
	// Validate the file handle adoption.
	TInt err = self->AdoptFileHandle(aFunction, aMessage);
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Failed to adopt a file handle from RMessage2, error = %d\n"),err);
		aMessage.Panic(KSifTransportServerRequestError, err);
		User::Leave(err);
		}
		
	self->PrepareParamsL();
	self->SetupRequestL(aTaskFactory);
	self->TransferToScsFrameworkL();
	CleanupStack::Pop(self);
	self->SetActive();
	self->LaunchTask();
	}

// Read file handle from aMessage for all request by file handle.
TInt CSifTransportRequest::AdoptFileHandle(TInt aFunction, const RMessage2& aMessage)
	{	
	if (aFunction & EFileHandleInIpc)
		{
		 return (iFile.AdoptFromClient(aMessage, 0, 1));
		}
	return KErrNone;
	}

CSifTransportRequest::CSifTransportRequest(TInt aFunction, CSifTransportSession* aSession, const RMessage2& aMessage)
	:	CAsyncRequest(aSession, 0, aMessage), iFunction(aFunction), iSecurityContext(aMessage)
	// iFile and iSecurityContext above cannot be a local variable because we pass it to SIF plug-ins during asynchronous calls.
	// Hence, their lifetime must be guaranteed.
	{
	if (iFunction & EInstall)
		{
		iComponentId = EInvalidComponentId;
		}

	// Read component id from aMessage.
	if (aFunction & EComponentIdInIpc)
		{
		iComponentId = aMessage.Int0();
		}
	}

CSifTransportRequest::~CSifTransportRequest()
	{
	DEBUG_PRINTF2(_L8("CSifRequest::~CSifRequest() for aFunction = %d\n"),iFunction);

	Cancel();
	
	if (iParams.iFileHandle != NULL)
		{
		iParams.iFileHandle->Close();
		}

	delete iParams.iFileName;
	delete iParams.iComponentInfo;
	delete iParams.iCustomArguments;
	delete iParams.iCustomResults;
	delete iTask;
	}

void CSifTransportRequest::PrepareParamsL()
	{
	DEBUG_PRINTF2(_L8("CSifRequest::PrepareParamsL() for aFunction = %d\n"),iFunction);

	// Make sure that the buffers for opaque containers are not allocated
	ASSERT(iParams.iComponentInfo == NULL);
	ASSERT(iParams.iCustomArguments == NULL);
	ASSERT(iParams.iCustomResults == NULL);

	// Read file name from iMessagePtr2
	if (iFunction & EFileNameInIpc)
		{
		// iFileName below cannot be a local variable because we pass it to installation
		// requests during asynchronous calls. Hence, the lifetime of it must be guaranteed.
		HBufC* fileName = HBufC::NewL(KMaxFileName);
		iParams.iFileName = fileName;
		TPtr bufFileName(fileName->Des());
		iMessagePtr2.ReadL(0, bufFileName);
		}

	// Create component info
	if (iFunction & EComponentInfoInIpc)
		{
		iParams.iComponentInfo = CComponentInfo::NewL();
		}

	// Create containers for custom params
	if (iFunction & EOpaqueDataToPlugin)
		{
		// We have to also create these containers for asynchronous requests
		// without opaque params. This is because a request API may not
		// provide methods without opaque params in order to keep it small.
		iParams.iCustomArguments = COpaqueNamedParams::NewL();
		iParams.iCustomResults = COpaqueNamedParams::NewL();
		}

	// Read custom arguments from iMessagePtr2
	if ((iFunction & EOpaqueDataInIpc) && (iFunction & EOpaqueDataToPlugin))
		{
		RIpcReadStream ipcrstream;
		ipcrstream.Open(iMessagePtr2, 2);
		CleanupClosePushL(ipcrstream);
		ipcrstream >> const_cast<COpaqueNamedParams&>(*iParams.iCustomArguments);
		CleanupStack::PopAndDestroy(&ipcrstream);
		}
	}

void CSifTransportRequest::SetupRequestL(TransportTaskFactory::GenerateTask aTaskFactory)
	{
	DEBUG_PRINTF2(_L8("CSifTransportRequest::SetupInstallerRequestL() for aFunction = %d\n"),iFunction);

	iParams.iSecurityContext = &iSecurityContext;
	iParams.iRequestStatus = &iStatus;

	// Instantiate an appropriate SIF Transport Task using aTaskFactory
	switch (iFunction)
		{
		case EGetComponentInfoByFileName:
		case EGetComponentInfoByFileHandle:
			{
			if (iParams.iFileName == NULL)
				{
				iParams.iFileHandle = &iFile;
				}

			iTask = aTaskFactory(TransportTaskFactory::EGetComponentInfo, iParams);
			break;
			}

		case EInstallByFileName:
		case EInstallByFileHandle:
		case EInstallByFileNameWithOpaqueData:
		case EInstallByFileHandleWithOpaqueData:
			{
			if (iParams.iFileName == NULL)
				{
				iParams.iFileHandle = &iFile;
				}

			iTask = aTaskFactory(TransportTaskFactory::EInstall, iParams);
			break;
			}

		case EUninstall:
		case EUninstallWithOpaqueData:
			{
			iParams.iComponentId = iComponentId;
			iTask = aTaskFactory(TransportTaskFactory::EUninstall, iParams);
			break;
			}

		case EActivate:
			{
			iParams.iComponentId = iComponentId;

			iTask = aTaskFactory(TransportTaskFactory::EActivate, iParams);
			break;
			}

		case EDeactivate:
			{
			iParams.iComponentId = iComponentId;
			iTask = aTaskFactory(TransportTaskFactory::EDeactivate, iParams);
			break;
			}

		default:
			ASSERT(0);
		}
	}

void CSifTransportRequest::DoCancel()
	{
	DEBUG_PRINTF2(_L8("CSifTransportRequest::DoCancel() for aFunction = %d\n"),iFunction);
	
	ASSERT(iTask != NULL);

	iTask->Cancel();
	}

void CSifTransportRequest::DoCleanup()
	{
	DEBUG_PRINTF2(_L8("CSifTransportRequest::DoCleanup() for aFunction = %d\n"),iFunction);
	Cancel();
	}

void CSifTransportRequest::ProcessOpaqueResultsL()
	{
	// Write custom results to iMessagePtr2
	RIpcWriteStream ipcwstream;
	ipcwstream.Open(iMessagePtr2, 3);
	CleanupClosePushL(ipcwstream);
	ipcwstream << *iParams.iCustomResults;
	CleanupStack::PopAndDestroy(&ipcwstream);
	}

void CSifTransportRequest::ProcessTaskResultsL()
	{
	if (iFunction & EComponentInfoInIpc)
		{
		// Write component info to iMessagePtr2
		RIpcWriteStream ipcwstream;
		ipcwstream.Open(iMessagePtr2, 2);
		CleanupClosePushL(ipcwstream);
		ipcwstream << *iParams.iComponentInfo;
		CleanupStack::PopAndDestroy(&ipcwstream);
		}

	if (iFunction & EOpaqueDataInIpc)
		{
		TRAPD(err, ProcessOpaqueResultsL());
		if (err != KErrNone)
			{
			DEBUG_PRINTF3(_L8("CSifRequest::RunL() for aFunction = %d - received error %d while processing opaque results. The error is ignored, since the install operation has already completed"),iFunction, err);
			}
		}
	}

void CSifTransportRequest::RunL()
	{
	DEBUG_PRINTF2(_L8("CSifTransportRequest::RunL() for aFunction = %d\n"),iFunction);
	
	if (iTaskComplete)
		{
		ProcessTaskResultsL();
		CAsyncRequest::RunL();
		}
	else
		{
		SetActive();
		iTaskComplete = iTask->Execute();
		}
	}

void CSifTransportRequest::LaunchTask()
	{
	iTaskComplete = iTask->Execute();
	}
