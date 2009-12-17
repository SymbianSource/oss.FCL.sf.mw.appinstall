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
#include <e32property.h>
#include <connect/sbdefs.h>
#include <swi/swiobserverclient.h>
#include <scs/raprtestwrapper.h>
#include <swi/swiobcommon.h>
 
using namespace Swi;
 
/** Test object. */
static RTestWrapper test(_L("SwiObserverTests"));

/** The name of the log file. We do not need this name in these tests. So it is defined as global.*/
static TFileName gFileName;

/** The observation log folder of the SWI Observer. */
_LIT(KSwiObserverLogFolder,"!:\\private\\102836c3\\log_swi\\");
/** The process log folder of the SWI Observer. */
_LIT(KSwiProcessLogFolder,"!:\\private\\102836c3\\log_obs\\");

static void CreateLogL(RSwiObserverSession& aSession)
 /**
 	Creates an observation log so that SWI Observer can process it.
  */
 	{
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
	data = CObservationData::NewL(KEventPath2,sId,EFileDeleted);
	CleanupStack::PushL(data);
	aSession.AddEventL(*data);
	CleanupStack::PopAndDestroy(data);
	
	pUid = TUid::Uid(298546);
	sId = 0x8765432;	
		
	header = CObservationHeader::NewL(pUid,EStandardPatch,EOpInstall);
	CleanupStack::PushL(header);
	aSession.AddHeaderL(*header);
	CleanupStack::PopAndDestroy(header);
	
	_LIT(KeventPath3,"c:\\resource\\plugins\\sample3.doc");	
	data = CObservationData::NewLC(KeventPath3,sId,0);		
	aSession.AddEventL(*data);
	CleanupStack::PopAndDestroy(data);
	
	//For coverage reasons
	CObservationFilter *tmp = CObservationFilter::NewL();
	delete tmp;
 	}
 	
// -------- open / close session --------

static void TestOpenCloseSession()
/**
	Open and close a connection to the UPS server.
 */
	{
	test.Start(_L("TestOpenCloseSession"));
	
	RSwiObserverSession s;
	TInt r = s.Connect();
	test(r == KErrNone);
	test.Next(_L("Session has been opened sucessfully."));
	
	s.Close();
	test.Next(_L("Session has been closed.\n"));
	
	test.End();
	}

	
// -------- Commit succeed --------
static void TestCommitL()
/**
	Gets a file handle and writes sample data into that file. Then
	commits the session.
 */
	{
	test.Start(_L("TestCommit"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("Session has been opened sucessfully."));
 	
	TRequestStatus status;
	testSession.Register(status);
	test.Next(_L("Applied for registration."));
	
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("Registered sucessfully."));
	
	testSession.GetFileHandleL(gFileName);
	test.Next(_L("Obtained a log file handle."));
	
	CreateLogL(testSession);
	test.Next(_L("Wrote some logs into the log file."));
		
	testSession.CommitL();
	test.Next(_L("Committed the session."));
	
	CleanupStack::PopAndDestroy(&testSession);
	test.Next(_L("Closed the session.\n"));
	
	test.End();
	}
 
 // -------- Commit empty log file --------
static void TestCommitEmptyFileL()
/**
	Gets a file handle and writes sample data into that file. Then
	commits the session.
 */
	{
	test.Start(_L("TestCommitEmptyFile"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("Session has been opened sucessfully."));
 	
	TRequestStatus status;
	testSession.Register(status);
	test.Next(_L("Applied for registration."));
	
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("Registered sucessfully."));
	
	testSession.GetFileHandleL(gFileName);
	test.Next(_L("Obtained a log file handle."));
	
	testSession.CommitL();
	test.Next(_L("Committed the session."));
	
	CleanupStack::PopAndDestroy(&testSession);
	test.Next(_L("Closed the session.\n"));
	
	test.End();
	}
	
// -------- Commit succeed --------
static void TestUnCommitedL()
/**
	Gets a file handle and writes sample data into that file. Then
	closes the session without committing it.
 */
	{
	test.Start(_L("TestUnCommited"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("Session has been opened sucessfully."));
 	
	TRequestStatus status;
	testSession.Register(status);
	test.Next(_L("Applied for registration."));
	
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("Registered sucessfully."));
	
	testSession.GetFileHandleL(gFileName);
	test.Next(_L("Obtained a log file handle."));
	
	CreateLogL(testSession);
	test.Next(_L("Wrote some logs into the log file."));
	
	CleanupStack::PopAndDestroy(&testSession);
	test.Next(_L("Closed the session without committing it.\n"));
	
	test.End();
	}
	
// -------- Commit succeed --------
static void TestBusyProcessorL()
/**
	A session is opened and an observation file is committed to be processed.
	While the log file is being processed another registration request is sent.
	The second request is completed when the processor becomes idle.
 */
	{
	test.Start(_L("TestBusyProcessor"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("First session has been opened."));
 	
	TRequestStatus status;
	testSession.Register(status);
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("First session has registered sucessfully."));
	
	RSwiObserverSession anotherSession;
 	User::LeaveIfError(anotherSession.Connect());
 	CleanupClosePushL(anotherSession);
	test.Next(_L("Second session has been opened."));
	
	testSession.GetFileHandleL(gFileName);
	CreateLogL(testSession);
	testSession.CommitL();
	test.Next(_L("First session has been committed. The processor is now busy."));
	
	TRequestStatus anotherStatus;
	anotherSession.Register(anotherStatus);
	test.Next(_L("Second session has sent its registration request."));
	User::WaitForRequest(anotherStatus);
	test(anotherStatus == KErrNone);
	test.Next(_L("Second session has registered sucessfully."));
	
	CleanupStack::Pop(&anotherSession);
	CleanupStack::PopAndDestroy(&testSession);
	CleanupClosePushL(anotherSession);
	test.Next(_L("First session disconnected."));
	
	anotherSession.GetFileHandleL(gFileName);
	CreateLogL(anotherSession);
	anotherSession.CommitL();
	test.Next(_L("Second session has been committed."));
	
	CleanupStack::PopAndDestroy(&anotherSession);
	test.Next(_L("Second session disconnected.\n"));
	
	test.End();
	}
 
// -------- Cancel registration request --------
static void TestCancelRegistrationL()
/**
	A session is opened and registers to the SWI Observer. After that,
	a second session is opened. The next step is to commit the first
	session. Then, the second session sends a registration request.
	Since the server processor is busy, it will wait until the process
	finishes. However, it prefers to cancel registration request.
	At that point, a third session is opened and then the previous 
	sessions are closed. The third session commits its session successfully
	and disconnects from the server.
 */
	{
	test.Start(_L("TestCancelRegistration"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("First session has been opened."));
 	
	TRequestStatus status;
	testSession.Register(status);
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("First session has registered sucessfully."));
	
	RSwiObserverSession anotherSession;
 	User::LeaveIfError(anotherSession.Connect());
 	CleanupClosePushL(anotherSession);
	test.Next(_L("Second session has been opened."));
	
	testSession.GetFileHandleL(gFileName);
	CreateLogL(testSession);
	testSession.CommitL();
	test.Next(_L("First session has been committed. The processor is now busy."));
	
	TRequestStatus anotherStatus;
	anotherSession.Register(anotherStatus);
	test.Next(_L("Second session has sent its registration request."));
	anotherSession.CancelRegistration();
	test(anotherStatus == KErrCancel);
	test.Next(_L("Second session has cancelled its registration request."));
	
	RSwiObserverSession thirdSession;
 	User::LeaveIfError(thirdSession.Connect());

	test.Next(_L("Third session has been opened."));
	
	CleanupStack::PopAndDestroy(2, &testSession);
	CleanupClosePushL(thirdSession);
	test.Next(_L("Both first and second session disconnected.\n"));
	
	TRequestStatus thirdStatus;
	thirdSession.Register(thirdStatus);
	User::WaitForRequest(thirdStatus);
	test(thirdStatus == KErrNone);
	test.Next(_L("Third session has registered sucessfully."));
		
	thirdSession.GetFileHandleL(gFileName);
	CreateLogL(thirdSession);
	thirdSession.CommitL();
	test.Next(_L("Third session has been committed."));
	
	CleanupStack::PopAndDestroy(&thirdSession);
	test.Next(_L("Third session disconnected.\n"));
	
	test.End();
	}
 
 
static void TestUnRegisteredSessionL()
/**
	Connects to the SWI Observer and open a session. However, it tries to
	use that session without registration.
 */
	{
	test.Start(_L("TestUnRegisteredSession"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("Session has been opened sucessfully."));
	
	test.Next(_L("Trying to obtain a log file handle."));
	TRAPD(err,testSession.GetFileHandleL(gFileName));
	test(err == KErrAccessDenied);
	
	test.Next(_L("Trying to commit the session."));
	TRAP(err,testSession.CommitL());
	test(err == KErrAccessDenied);
	
	TRequestStatus status;
	testSession.Register(status);
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("Session has registered sucessfully."));
	
	test.Next(_L("Second registration is being tried."));
	testSession.Register(status);
	User::WaitForRequest(status);
	test(status == KErrServerBusy);
	
	CleanupStack::PopAndDestroy(&testSession);
	test.Next(_L("Closed the session.\n"));
	
	test.End();
	}

// -------- Commit corrupted file --------
static void TestCommitCorruptedLogL(RFs& aFs)
/**
	Copies a corrupted file into the log file directory and then
	starts the server. Gets a file handle and commits it without 
	writing. It is expected that the SWI Observer will manage to
	process both corrupted and empty log files.
 */
	{
	test.Start(_L("TestCommitCorruptedLog"));
	
	RSwiObserverSession testSession;
 	User::LeaveIfError(testSession.Connect());
 	CleanupClosePushL(testSession);
 	test.Next(_L("Session has been opened sucessfully."));
 	
	TRequestStatus status;
	testSession.Register(status);
	test.Next(_L("Applied for registration."));
	
	User::WaitForRequest(status);
	test(status == KErrNone);
	test.Next(_L("Registered sucessfully."));
	
	testSession.GetFileHandleL(gFileName);
	test.Next(_L("Obtained a log file handle."));
	
	CFileMan *fm = CFileMan::NewL(aFs);
	CleanupStack::PushL(fm);
	TFileName logName(KSwiObserverLogFolder);
	logName[0] = aFs.GetSystemDriveChar();
	logName.Append(_L("corrupted"));
	User::LeaveIfError(fm->Copy(_L("z:\\tswi\\tswiobserver\\scripts\\data\\corrupted"),logName));
	logName.Append(_L("_2"));
	User::LeaveIfError(fm->Copy(_L("z:\\tswi\\tswiobserver\\scripts\\data\\corrupted_2"),logName));
	CleanupStack::PopAndDestroy(fm);
	
	TRAPD(error,testSession.CommitL());
	test(error == KErrNone);
	test.Next(_L("Committed the session."));
	
	CleanupStack::PopAndDestroy(&testSession);
	test.Next(_L("Closed the session.\n"));
	
	test.End();
	}

// -------- Process Logs left from previous successfull session --------
static void TestProcessLogsL(RFs& aFs)
/**
	Copies a valid swi observartion log file into the log file directory
	and then starts the SWI Observer. As soon as SWI Observer starts, 
	it processes that log file.
 */
	{
	test.Start(_L("TestProcessLogs"));
	User::After(2500000); //Make sure that Swi Observer has shutdown
	CFileMan *fm = CFileMan::NewL(aFs);
	CleanupStack::PushL(fm);
	TFileName logName(KSwiObserverLogFolder);
	logName[0] = aFs.GetSystemDriveChar();
	logName.Append(_L("resume"));
	User::LeaveIfError(fm->Copy(_L("z:\\tswi\\tswiobserver\\scripts\\data\\log_resume_notification"),logName));
	//Now create the flagfile so that the RSwiObserverSession::ProcessLogsL properly starts up SWI observer 
	TFileName flagName;
	flagName.Append(aFs.GetSystemDriveChar());
	flagName.AppendFormat(KObserverFlagFileNameFormat, KUidSwiObserver.iUid);
	User::LeaveIfError(fm->Copy(_L("z:\\tswi\\tswiobserver\\scripts\\data\\log_resume_notification"),flagName));
	CleanupStack::PopAndDestroy(fm);
	
	RSwiObserverSession testSession;
 	TRAPD(err, testSession.ProcessLogsL(aFs));
 	test(err == KErrNone);
 	test.Next(_L("SWI Observer has been started to process the log file."));
 	 	
 	testSession.Close();
 	test.Next(_L("Closed the session.\n"));
	
	User::After(1000000);
	RFile file;
	err = file.Open(aFs,logName,EFileRead);
	test(KErrNotFound == err);
	test.Next(_L("Copied log file could not found. It has been processed.\n"));
 	
	test.End();
	}
		
// -------- Backup/Restore Property Tests --------
static void TestBRPropertyBackupL()
/**
	Sets the backup/restore property to Backup mode and then
	commit a session.
 */
	{
	RProcess propchanger;
	TInt err = propchanger.Create(_L("propertychanger.exe"),_L("backup"));
	
	if (err==KErrNone)
		{
		TRequestStatus stat;
		propchanger.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			propchanger.Kill(0); // abort startup
			}
		else
			{
			propchanger.Resume(); // logon OK, start the server
			}

		User::WaitForRequest(stat); // wait for start or death
		}
	
	TestCommitL();
	}
	
static void TestBRPropertyNormalL()
/**
	First sets the phone mode to Restore. After that commit a session
    Finally sets the backup/restore property to Normal mode.
 */
	{
	RProcess propchanger;
	TInt err = propchanger.Create(_L("propertychanger.exe"),_L("restore"));
	
	if (err==KErrNone)
		{
		TRequestStatus stat;
		propchanger.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			propchanger.Kill(0); // abort startup
			}
		else
			{
			propchanger.Resume(); // logon OK, start the server
			}

		User::WaitForRequest(stat); // wait for start or death
		}
	
	TestCommitL();
	
	err = propchanger.Create(_L("propertychanger.exe"),_L("normal"));
	
	if (err==KErrNone)
		{
		TRequestStatus stat;
		propchanger.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			propchanger.Kill(0); // abort startup
			}
		else
			{
			propchanger.Resume(); // logon OK, start the server
			}

		User::WaitForRequest(stat); // wait for start or death
		}
	}
				
static void MainL()
 /**
 	Main function performing all test steps.
  */
 	{
 	test.Title(_L("c:\\swiobservertest.log"));
	test.Start(_L("Testing SWI Observer"));
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TFileName dirName(KSwiObserverLogFolder);
	dirName[0] = fs.GetSystemDriveChar();
	fs.RmDir(dirName);
	dirName.Copy(KSwiProcessLogFolder);
	dirName[0] = fs.GetSystemDriveChar();
	fs.RmDir(dirName);
	
	//Test 1
	TestOpenCloseSession();
	//Test 2
	TestCommitL();
	//Test 3
	TestCommitEmptyFileL();
	//Test 4
	TestUnCommitedL();
	//Test 5
	TestBusyProcessorL();
	//Test 6
	TestProcessLogsL(fs);
	//Test 7
	TestUnRegisteredSessionL();
	//Test 8
	TestCommitCorruptedLogL(fs);
	//Test 9
	TestCancelRegistrationL();
	//Test10
	TestBRPropertyBackupL();
	//Test11
	TestBRPropertyNormalL();
	CleanupStack::PopAndDestroy(&fs);
	
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
