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


#include <e32std.h>
#include "tintegrityservicesclientserver.h"
#include "tintegrityservicesserver.h"
#include "log.h"
#include <e32uid.h>
#include <e32debug.h>

using namespace Swi;
using namespace Swi::Test;

//
// Starts the IntegrityServices Server in a new process
//
static TInt StartIntegrityServicesServer()
	{
	const TUidType serverUid(KExecutableImageUid, KNullUid, KIntegrityServicesServerUid3);
	RProcess server;
	TInt err = server.Create(KIntegrityServicesServerImage, KNullDesC);

	if (err != KErrNone)
		{
		RDebug::Print(_L("StartIntegrityServicesServer: %d"), err);
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);

	if (stat != KRequestPending)
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
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

EXPORT_C TInt RIntegrityServicesServerSession::Connect()
	{
	TInt retry = 2;
	RDebug::Print(_L("RIntegrityServicesServerSession::Connect:"));
	//	DEBUG_PRINTF(_L("RIntegrityServicesServerSession::Connect:"));
	for(;;)
		{
		TInt err = CreateSession(KIntegrityServicesServerName, TVersion(0,0,0), 1);
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			RDebug::Print(_L("RIntegrityServicesServerSession::Connect: %d"), err);
			return err;
			}

		if (--retry == 0)
			{
			return err;
			}

		err = StartIntegrityServicesServer();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

EXPORT_C void RIntegrityServicesServerSession::AddL(const TDesC& aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::AddL: %S"), &aFileName);
	  
	  User::LeaveIfError(SendReceive(EAdd, TIpcArgs(&aFileName)));
	}

EXPORT_C void RIntegrityServicesServerSession::RemoveL(const TDesC& aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::Remove: %S"), &aFileName);
	  
	  User::LeaveIfError(SendReceive(ERemove, TIpcArgs(&aFileName)));
	}
	
EXPORT_C void RIntegrityServicesServerSession::TemporaryL(const TDesC& aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::TemporaryL: %S"), &aFileName);
	  
	  User::LeaveIfError(SendReceive(ETemporary, TIpcArgs(&aFileName)));
	}
	
EXPORT_C void RIntegrityServicesServerSession::CommitL()
	{
	  DEBUG_PRINTF(_L("RIntegrityServicesServerSession::CommitL: "));
	  
	  User::LeaveIfError(SendReceive(ECommit));
	}
	
EXPORT_C void RIntegrityServicesServerSession::RollBackL(TBool aAllTransactions)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::RollBackL: %d"), aAllTransactions);
	  TPckg<TBool> all(aAllTransactions);
	  
	  User::LeaveIfError(SendReceive(ERollBack, TIpcArgs(&all)));
	}
	
EXPORT_C void RIntegrityServicesServerSession::SetSimulatedFailure(TPtrC aFailType, TPtrC aFailPosition, TPtrC aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::SetSimulatedFailure: %S %S %S"), &aFailType, &aFailPosition, &aFileName);
	  
	  User::LeaveIfError(SendReceive(ESetSimulatedFailure, TIpcArgs(&aFailType, &aFailPosition, &aFileName)));
	}
	
EXPORT_C void RIntegrityServicesServerSession::CreateNewTestFileL(const TDesC& aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::CreateNewTestFileL: %S"), &aFileName);
	  
	  User::LeaveIfError(SendReceive(ECreateNewTestFile, TIpcArgs(&aFileName)));
	}
	
EXPORT_C void RIntegrityServicesServerSession::CreateTempTestFileL(const TDesC& aFileName)
	{
	  RDebug::Print(_L("RIntegrityServicesServerSession::CreateTempTestFileL: %S"), &aFileName);
	  
	  User::LeaveIfError(SendReceive(ECreateTempTestFile, TIpcArgs(&aFileName)));
	}
