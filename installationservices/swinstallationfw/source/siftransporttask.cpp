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
* sifinstallerrequest.cpp
* This file implements the CSifTransportTask class.
*
*/


#include <usif/sif/siftransporttask.h>
#include <usif/usiferror.h>

using namespace Usif;

EXPORT_C TTransportTaskParams::TTransportTaskParams()
	: iComponentInfo(NULL),
	iComponentId(EInvalidComponentId),
	iFileName(NULL),
	iFileHandle(NULL),
	iCustomArguments(NULL),
	iCustomResults(NULL),
	iRequestStatus(NULL)
	{
	}

// ===========================================================================================================

EXPORT_C CSifTransportTask::CSifTransportTask(TTransportTaskParams& aParams, TBool aAutoCompletion)
	: iParams(aParams), iAutoCompletion(aAutoCompletion)
	{
	}

EXPORT_C CSifTransportTask::~CSifTransportTask()
	{
	}

TBool CSifTransportTask::Execute()
	{
	TBool done(EFalse);
	TRAPD(err, done = ExecuteImplL());
	if (err != KErrNone)
		{
		TRequestStatus* status(iParams.iRequestStatus);
		User::RequestComplete(status, err);
		done = ETrue;
		}
	else if (iAutoCompletion)
		{
		TRequestStatus* status(iParams.iRequestStatus);
		User::RequestComplete(status, KErrNone);
		}
	return done;
	}

void CSifTransportTask::Cancel()
	{
	CancelImpl();
	}

EXPORT_C void CSifTransportTask::CancelImpl()
	{
	}

EXPORT_C TComponentId CSifTransportTask::ComponentId() const
	{
	return iParams.iComponentId;
	}

EXPORT_C const TDesC* CSifTransportTask::FileName() const
	{
	return iParams.iFileName;
	}

EXPORT_C RFile* CSifTransportTask::FileHandle()
	{
	return iParams.iFileHandle;
	}

EXPORT_C const COpaqueNamedParams* CSifTransportTask::CustomArguments() const
	{
	return iParams.iCustomArguments;
	}

EXPORT_C COpaqueNamedParams* CSifTransportTask::CustomResults()
	{
	return iParams.iCustomResults;
	}

EXPORT_C const TSecurityContext* CSifTransportTask::SecurityContext() const
	{
	return iParams.iSecurityContext;
	}

EXPORT_C TRequestStatus* CSifTransportTask::RequestStatus()
	{
	return iParams.iRequestStatus;
	}

EXPORT_C CComponentInfo* CSifTransportTask::ComponentInfo()
	{
	return iParams.iComponentInfo;
	}
