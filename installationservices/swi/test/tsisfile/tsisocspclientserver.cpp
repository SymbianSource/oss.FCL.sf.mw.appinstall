/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "tocspclientserver.h"

using namespace Swi::Test;

//
// Starts the Ocsp Server in a new process
//
static TInt StartOcspServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KOcspServerUid3);
	RProcess server;
	TInt err=server.Create(Swi::Test::KOcspServerImage, KNullDesC, serverUid);

	if (err!=KErrNone)
		{
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);

	if (stat!=KRequestPending)
		{
		server.Kill(0); // abort startup
		}
	else
		{
		server.Resume(); // logon OK, start the server
		}

	User::WaitForRequest(stat); // wait for start or death
	// We can't use the 'exit reason' if the server panicked as this is the
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone.
	err=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

EXPORT_C TInt ROcspServerSession::Connect()
	{
	TInt retry=2;
	for(;;)
		{
		TInt err=CreateSession(KOcspServerName, TVersion(0,0,0), 1);
		if (err!=KErrNotFound && err!=KErrServerTerminated)
			{
			return err;
			}

		if (--retry==0)
			{
			return err;
			}

		err=StartOcspServer();
		if (err!=KErrNone && err!=KErrAlreadyExists)
			{
			return err;
			}
		}
	}

EXPORT_C TInt ROcspServerSession::Check()
	{
	return SendReceive(ECheck, TIpcArgs());	
	}
