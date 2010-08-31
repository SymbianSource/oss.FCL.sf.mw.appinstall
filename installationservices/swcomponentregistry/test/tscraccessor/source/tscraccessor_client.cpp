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
* Scr Accessor - client interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include "tscraccessor_client.h"
#include "tscraccessor_common.h"

static TInt StartScrAccessServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KScrAccessServerUid3);
	RProcess server;
	TInt err = server.Create(KScrAccessServerBinary, KNullDesC, serverUid);
	if (err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

//
// Connect to the server, attempting to start it if necessary
//
TInt RScrAccessor::Connect()
	{
	TInt retry = 2;
	for (;;)
		{
		TInt err = CreateSession(KScrAccessServerName, TVersion(0, 0, 0), 2);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry == 0)
			{
			return err;
			}
		err = StartScrAccessServer();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

void RScrAccessor::AddSoftwareTypeL(TAccessorOperationType aOpType, TAccessorOperationResult& aOpResult, TInt& aSpentTimeInMillisecond)
	{
	TPckg<TAccessorOperationResult> opResult(aOpResult);
	TPckg<TInt> time(aSpentTimeInMillisecond);
	User::LeaveIfError(SendReceive(EAddSoftwareType, TIpcArgs(aOpType, &opResult, &time)));
	}

void RScrAccessor::DeleteSoftwareTypeL(TAccessorOperationResult& aOpResult, TInt& aSpentTimeInMillisecond)
	{
	TPckg<TAccessorOperationResult> opResult(aOpResult);
	TPckg<TInt> time(aSpentTimeInMillisecond);
	User::LeaveIfError(SendReceive(EDeleteSoftwareType, TIpcArgs(&opResult, &time)));
	}

TInt RScrAccessor::DeleteFile(const TDesC& aFilePath)
	{
	return SendReceive(EDeleteFile, TIpcArgs(&aFilePath));
	}

TInt RScrAccessor::CopyFile(const TDesC& aSourceFilePath, const TDesC& aDestinationFilePath)
	{
	return SendReceive(ECopyFile, TIpcArgs(&aSourceFilePath, &aDestinationFilePath));
	}

