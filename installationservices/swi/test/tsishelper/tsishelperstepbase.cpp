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


/**
 @file tsishelperstep.cpp
*/
#include "tsishelperstep.h"
#include <test/testexecutelog.h>
#include <e32math.h>
#include "sishelperclient.h"
#include "sishelper.h"
#include "swi/launcher.h"

CTSISHelperStepBase::CTSISHelperStepBase()
	{
	}

TVerdict CTSISHelperStepBase::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
TVerdict CTSISHelperStepBase::doTestStepPostambleL()
	{
	iFs.Close();
	return TestStepResult();
	}

TInt CTSISHelperStepBase::startSisHelper(Swi::TSisHelperStartParams& aParams)
	{
	// To deal with the unique thread (+semaphore!) naming in Symbian OS, and
	// that we may be trying to restart a server that has just exited we 
	// attempt to create a unique thread name for the server
	TName name(Swi::KSisHelperServerName);
	name.AppendNum(Math::Random(), EHex);
	RThread server;
	const TInt KSisHelperServerStackSize=0x2000;
	const TInt KSisHelperServerInitHeapSize=0x1000;
	const TInt KSisHelperServerMaxHeapSize=0x1000000;
	TInt err=server.Create(name, sisHelperThreadFunction, 
		KSisHelperServerStackSize, KSisHelperServerInitHeapSize, 
		KSisHelperServerMaxHeapSize, static_cast<TAny*>(&aParams), 
		EOwnerProcess);
	if (err!=KErrNone)
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
	
	// we can't use the 'exit reason' if the server panicked as this is the 
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone
	err=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

TInt CTSISHelperStepBase::sisHelperThreadFunction(TAny *aPtr)
	{
	if (aPtr==NULL)
		{
		return KErrArgument;
		}
		
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	Swi::TSisHelperStartParams* params=
		static_cast<Swi::TSisHelperStartParams*>(aPtr);

	CActiveScheduler* scheduler=new CActiveScheduler;

	CActiveScheduler::Install(scheduler);
	Swi::CSisHelperServer* server=NULL;
	
	TRAPD(err, server=Swi::CSisHelperServer::NewL(*params));

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
