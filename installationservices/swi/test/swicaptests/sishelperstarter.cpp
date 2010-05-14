/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "sishelperclient.h"
#include "sishelper.h"

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>

void MainL();

using namespace Swi;

GLDEF_D TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err,MainL());
	delete cleanup;
	return err;
	}

HBufC* GetDesParameterL(TInt aParam)
	{
	// Get buffer from parameter passed
	TInt length=User::ParameterLength(aParam);
	User::LeaveIfError(length);
	
	HBufC* des=HBufC::NewLC(length);
	TPtr desPtr=des->Des();
	User::LeaveIfError(User::GetDesParameter(aParam, desPtr));

	CleanupStack::Pop(des);
	return des;
	}

TInt SisHelperThreadFunction(TAny *aPtr)
	{
	if (aPtr==NULL)
		return KErrArgument;
		
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	TSisHelperStartParams* params=
		static_cast<TSisHelperStartParams*>(aPtr);

	CActiveScheduler* scheduler=new CActiveScheduler;

	CActiveScheduler::Install(scheduler);
	CSisHelperServer* server=NULL;
	
	TRAPD(err, server=CSisHelperServer::NewL(*params));

	if (err==KErrNone)
		{
		// only continue launching the server if no error
		RThread::Rendezvous(KErrNone);
		scheduler->Start();
		CActiveScheduler::Install(NULL);
		}

	delete server;
	delete scheduler;
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;

	return err;
	}
	
TInt StartSisHelper(TSisHelperStartParams& aParams)
	{
	// To deal with the unique thread (+semaphore!) naming in Symbian OS, and
	// that we may be trying to restart a server that has just exited we 
	// attempt to create a unique thread name for the server
	TName name(KSisHelperServerName);
	name.AppendNum(Math::Random(), EHex);
	RThread server;
	const TInt KSisHelperServerStackSize=0x2000;
	const TInt KSisHelperServerInitHeapSize=0x1000;
	const TInt KSisHelperServerMaxHeapSize=0x1000000;
	TInt err=server.Create(name, SisHelperThreadFunction, 
		KSisHelperServerStackSize, KSisHelperServerInitHeapSize, 
		KSisHelperServerMaxHeapSize, static_cast<TAny*>(&aParams), 
		EOwnerProcess);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		return err;
	
	// The following code is the same whether the server runs in a new thread 
	// or process
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0); // abort startup
	else
		server.Resume(); // logon OK, start the server
	User::WaitForRequest(stat); // wait for start or death
	
	RProcess::Rendezvous(stat.Int());
	if (stat.Int()==KErrNone)
		{
		server.Logon(stat);
		User::WaitForRequest(stat);
		}
	
	// we can't use the 'exit reason' if the server panicked as this is the 
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone
	err=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

void MainL()
	{
	HBufC* fileName=GetDesParameterL(7);
	CleanupStack::PushL(fileName);
	TSisHelperStartParams params(*fileName);
	User::LeaveIfError(StartSisHelper(params));
	CleanupStack::PopAndDestroy(fileName);
	}
