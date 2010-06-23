/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test program exercises the skeleton SWI Observer API.
* See individual test functions for more information.
*
*/


/**
 @file
 @test
*/
 
 #include <e32ldr.h>
 #include <e32test.h>
 #include <swi/swiobserverclient.h>
 #include <scs/raprtestwrapper.h>
 
 using namespace Swi;
 
 /** Test object. */
 static RTestWrapper test(_L("SwiObserverTestOOM"));
 
 /** The name of the log file. We do not need this name in these tests. So it is defined as global.*/
static TFileName gFileName;


static void Cleanup(RFs& aFs, TDesC& aPrivateFolder)
 /**
 	Scans the private folder of the Swi Observer and deletes
 	the files left from the previous session.
  */
 	{
 	CFileMan* fileMan = CFileMan::NewL(aFs);
    CleanupStack::PushL(fileMan); 
    
    fileMan->Delete(aPrivateFolder,CFileMan::ERecurse);
		
	CleanupStack::PopAndDestroy(fileMan);
 	}
 
 
static void CreateLogL(RSwiObserverSession& aSession)
 /**
 	Creates an observation log so that SWI Observer can process it.
  */
 	{
 	TRequestStatus status;
 	aSession.Register(status);
 	User::WaitForRequest(status);
 	aSession.GetFileHandleL(gFileName);
 	
 	TUid pUid = TUid::Uid(473847);
	TSecureId sId = 0x1234567;
	_LIT(KEventPath1,"c:\\private\\102892\\sample1.dat");
	
	CObservationHeader *header = CObservationHeader::NewLC(pUid,EStandardApplication,EOpInstall);
	aSession.AddHeaderL(*header);		
	CleanupStack::PopAndDestroy(header);
		
	CObservationData *data = CObservationData::NewLC(KEventPath1,sId,0);
	aSession.AddEventL(*data);
	CleanupStack::PopAndDestroy(data);
	
	_LIT(KEventPath2,"c:\\sys\\bin\\sample2.dll");
	data = CObservationData::NewLC(KEventPath2,sId,0);
	aSession.AddEventL(*data);
	CleanupStack::PopAndDestroy(data);
	
	pUid = TUid::Uid(298546);
	sId = 0x8765432;	
		
	header = CObservationHeader::NewLC(pUid,EStandardPatch,EOpInstall);
	aSession.AddHeaderL(*header);
	CleanupStack::PopAndDestroy(header);
	
	_LIT(KeventPath3,"c:\\sys\\bin\\docs\\sample3.doc");	
	data = CObservationData::NewLC(KeventPath3,sId,0);		
	aSession.AddEventL(*data);
	CleanupStack::PopAndDestroy(data);	
 	}
 
 
static void DoTestL(RFs& aFs, TDesC& aPrivateFolder)
 /**
 	Connects to the SWI Observer, obtains a file handle, creates a sample log
 	and then commits the log file so that it can be processed by the SWI Observer.
  */
 	{
 	RSwiObserverSession session;
 	User::LeaveIfError(session.Connect());
 	CleanupClosePushL(session);
 	
 	CreateLogL(session);
 	
 	session.CommitL();
 	
 	User::After(1000000);
 	
 	CleanupStack::PopAndDestroy(&session);
 	
 	Cleanup(aFs,aPrivateFolder);
 	}
 
 
static void MainL()
 /**
 	Main function performing all test steps.
  */
 	{
 	test.Title(_L("c:\\swiobservertestoom.log"));
	test.Start(_L("Testing SWI OBSERVER OOM"));
	
 	RFs fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);
	
	TFileName privPath(_L("!:\\private\\102836c3\\"));
	privPath[0] = fs.GetSystemDriveChar();
	
	RSwiObserverSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	TInt err = KErrNone;
	TInt run = 0;
	TInt passingRuns = 0;
	
	static const TInt maxRun = 1000;
	static const TInt passThreshold = 100;
	
	for(run=1; run<=maxRun; ++run)
		{
		test.Printf(_L("\n\nOOM -- Run %d\n"), run);
		err = session.SetServerHeapFail(run);
		if(KErrNoMemory == err)
			{
			test.Printf(_L("\tRe-initialization failed!\n"));
			session.ResetServerHeapFail();
			continue;
			}
			
		if(KErrNone != err)
			{
			test.Printf(_L("Failed to set heap fail with error code %d"),err);
			test(EFalse);
			break;
			}
			
		//Run the test
		test.Start(_L("DoTestSwiObserver"));
		TRAP(err,DoTestL(fs,privPath));
		test.End();
		
		test.Printf(_L("Resetting heap failure\n"));
		TInt err2 = session.ResetServerHeapFail();
		
		if((KErrServerTerminated == err) || (KErrServerTerminated == err2))
			{
			test.Printf(_L("\tSWI Observer died"));
			test(EFalse);
			break;
			}
			
		if((KErrNone == err) && (KErrNone != err2))
			{
			err = err2;
			}
			
		if(KErrNone == err)
			{
			++passingRuns;
			}
		else
			{
			passingRuns = 0;
			}
		
		if(passingRuns > passThreshold) break;
		
		}// End of OOM loop
		
	if(run > maxRun)
		{
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(2, &fs);
	
	test.End();
	test.Close();
 	}
 
 
static void PanicIfError(TInt aErr)
 /**
 	If the provided parameter is an error code, panics the thread. 
  */
 	{
 	if(KErrNone != aErr)
 		{
 		User::Panic(_L("swiobserver test failed: "), aErr);
 		}
 	}
 
 
 TInt E32Main()
 /**
	Executable entrypoint establishes connection with SWI Observer
	and then invokes tests for each functional area.
	
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
 	{
 	// disable lazy DLL unloading so kernel heap balances at end
 	RLoader loader;
 	PanicIfError(loader.Connect());
 	PanicIfError(loader.CancelLazyDllUnload());
 	loader.Close();
 	
 	__UHEAP_MARK;
 	
 	CTrapCleanup *tc = CTrapCleanup::New();
 	if(0 == tc)
 		{
 		return KErrNoMemory;
 		}
 	
 	TRAPD(err,MainL());
 	
 	if(err != KErrNone)
 		{
 		PanicIfError(err);
 		}
 		
 	delete tc;
 	
 	__UHEAP_MARKEND;
 	
 	return KErrNone;
 	}
