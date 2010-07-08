/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistryAccess - client interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include "sisregistryaccess_client.h"
#include "sisregistryaccess_common.h"

static TInt StartSisRegistryAccessServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KSisRegistryAccessServerUid3);
	RProcess server;
	TInt err = server.Create(KSisRegistryAccessServerBinary, KNullDesC, serverUid);
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
TInt RSisRegistryAccessSession::Connect()
	{
	TInt retry = 2;
	for (;;)
		{
		TInt err = CreateSession(KSisRegistryAccessServerName, TVersion(0, 0, 0), 2);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry == 0)
			{
			return err;
			}
		err = StartSisRegistryAccessServer();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

void RSisRegistryAccessSession::AddEntryL(const TDesC8& aControllerData, TInt& aSpentTimeInMillisecond)
	{
	TPckg<TInt> time(aSpentTimeInMillisecond);
	User::LeaveIfError(SendReceive(EAddEntry, TIpcArgs(&aControllerData, &time)));
	}

void RSisRegistryAccessSession::UpdateEntryL(const TDesC8& aControllerData, TInt& aSpentTimeInMillisecond)
	{
	TPckg<TInt> time(aSpentTimeInMillisecond);
	User::LeaveIfError(SendReceive(EUpdateEntry, TIpcArgs(&aControllerData, &time)));
	}

TInt RSisRegistryAccessSession::DeleteEntryL(Swi::CSisRegistryPackage& aPackage, TInt& aSpentTimeInMillisecond)
	{
	TPckgC<TUid> packageUid(aPackage.Uid());
	TInt index = aPackage.Index();
	TPckg<TInt> time(aSpentTimeInMillisecond);
	return SendReceive(EDeleteEntry, TIpcArgs(index, &packageUid, &time));
	}

TInt RSisRegistryAccessSession::AddAppRegInfoL(const TDesC& aAppRegFile, TInt& aSpentTimeInMillisecond)
    {
    TPckg<TInt> time(aSpentTimeInMillisecond);
    return SendReceive(EAddAppRegInfo, TIpcArgs(&aAppRegFile, &time));
    }

TInt RSisRegistryAccessSession::RemoveAppRegInfoL(const TDesC& aAppRegFile, TInt& aSpentTimeInMillisecond)
    {
    TPckg<TInt> time(aSpentTimeInMillisecond);
    return SendReceive(ERemoveAppRegInfo, TIpcArgs(&aAppRegFile, &time));
    }
