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

#include "watcherstep.h"

#include "drivewatcher.h"

#include "testutilclientswi.h"

namespace Swi
{

namespace Test
{

/**
 * This fakes a media change sicne we are using ENotifyEntry by creating
 * a dir under the private path we're receiving notifications.
 * This could possibly be improved to generate real notifications.
 */
void GenerateMediaChangeL(const TDesC& aPath)
	{
	
	RTestUtilSessionSwi fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	HBufC* path=HBufC::NewLC(aPath.Length()+128);
	TPtr pathPtr(path->Des());
	pathPtr.Format(_L("%Smedia_change_trigger"), &aPath);
	
	TInt err=fs.MkDirAll(pathPtr);
	if (err==KErrAlreadyExists)
		{
		err=fs.RmDir(pathPtr);
		}
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(path);
	CleanupStack::PopAndDestroy(&fs);
	}
	
const TInt KDriveE='e'-'a'; // Drive E is our removable drive


CWatcherStep::CWatcherStep()
	{
	iError=KErrNone;
	}

CWatcherStep::~CWatcherStep()
	{
	}

void CWatcherStep::FreeMemory()
	{
	iNotifications.Reset();
	}

TVerdict CWatcherStep::runTestStepL(TBool /*aOomTest*/)
	{
	iActiveStep=CWatcherActiveStep::NewL(*this);
	iActiveStep->StartTest();
	delete iActiveStep;

	User::LeaveIfError(iError);
	return EPass;
	}


void CWatcherStep::SetTestFailure(TInt aError)
	{
	iError=aError;	
	}

void CWatcherStep::CheckMediaChangeL(TInt aChangeNumber, 
						TInt aExpectedDrive, TInt aExpectedChangeType, const TDesC& aDescription)
	{
	if (aChangeNumber != iNotifications.Count() ||
		iNotifications[aChangeNumber - 1].iDrive != aExpectedDrive ||
		iNotifications[aChangeNumber - 1].iChangeType != aExpectedChangeType)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF2(_L("Error, expected change %S"), &aDescription);
		User::Leave(KErrGeneral);
		}
	}

void CWatcherStep::MediaChangeL(TInt aDrive, TChangeType aChangeType)
	{
	iNotifications.AppendL(TNotification(aDrive,aChangeType));	
	
	if (iActiveStep)
		{
		iActiveStep->NotifyMediaChange();
		}
	}

// CWatcherStep::TNotification

CWatcherStep::TNotification::TNotification(TInt aDrive, TChangeType aChangeType)
	:iDrive(aDrive), iChangeType(aChangeType)
	{
	}

	
// CWatcherActiveStep

/*static*/ CWatcherActiveStep* CWatcherActiveStep::NewL(CWatcherStep& aWatcherStep, TInt aPriority)
	{
	CWatcherActiveStep* self=NewLC(aWatcherStep, aPriority);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CWatcherActiveStep* CWatcherActiveStep::NewLC(CWatcherStep& aWatcherStep, TInt aPriority)
	{
	CWatcherActiveStep* self=new(ELeave) CWatcherActiveStep(aWatcherStep, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

CWatcherActiveStep::CWatcherActiveStep(CWatcherStep& aWatcherStep, TInt aPriority)
	: CActive(aPriority), iWatcherStep(aWatcherStep), iMediaChange(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CWatcherActiveStep::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}
	
CWatcherActiveStep::~CWatcherActiveStep()
	{
	delete iDriveWatcherX;
	iFs.Close();
	}

void CWatcherActiveStep::StartTest()
	{
	//  setup state
	iTestState=EStateStartup;
	WaitForNotify();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	
	// start new level of scheduler
	CActiveScheduler::Start();
	}

void CWatcherActiveStep::DoCancel()
	{
	}

void CWatcherActiveStep::RunL()
	{
	if (iTestState==EStateFinish)
		{
		Deque();
		CActiveScheduler::Stop();
		return;
		}

	if (iMediaChange)
		{
		iMediaChange=EFalse;
		CheckResults();
		}
	else
		{
		RunTestsL();
		}
	}

void CWatcherActiveStep::RunTestsL()
	{
		switch (iTestState)
		{
		case EStateStartup:	// Startup, test initial notify.
			{
			WaitForNotify();
			iDriveWatcherX=CDriveWatcher::NewL(iFs, KDriveE, iWatcherStep);
			break;
			}
		case EStateTestNotify:	// Test notification which should apply to a watched drive
			{
			WaitForNotify();
			GenerateMediaChangeL(_L("E:\\private\\10202DCE\\mediachange\\"));
			break;
			}
			
		case EStateTestNotify2:
			{
			WaitForNotify();
			TRAPD(err, GenerateMediaChangeL(_L("A:\\private\\10202DCE\\mediachange\\")));
			if (err!=KErrNotReady && err!=KErrNone)
				{
				User::Leave(err);
				}

			TRAP(err, GenerateMediaChangeL(_L("B:\\private\\10202DCE\\mediachange\\")));
			if (err!=KErrNotReady && err!=KErrNone)
				{
				User::Leave(err);
				}
			
			_LIT(KPath ,"\\private\\10202DCE\\mediachange\\");
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TBuf<128> mediaChangePath (sysDrive.Name());
			mediaChangePath.Append(KPath);
			
			TRAP(err, GenerateMediaChangeL(mediaChangePath));
			if (err!=KErrNotReady && err!=KErrNone)
				{
				User::Leave(err);
				}
			
			TRAP(err, GenerateMediaChangeL(_L("D:\\private\\10202DCE\\mediachange\\")));
			if (err!=KErrNotReady && err!=KErrNone)
				{
				User::Leave(err);
				}

			GenerateMediaChangeL(_L("E:\\private\\10202DCE\\mediachange\\"));
			break;
			}
		}
	}

void CWatcherActiveStep::CheckResults()
	{
	switch (iTestState)
		{
		case EStateStartup:	// Startup, test initial notify.
			{
			iWatcherStep.CheckMediaChangeL(1, KDriveE, MDriveObserver::EMediaInserted, _L("Expect EMediaInserted at creation time"));
			RunNextTest(EStateTestNotify);
			break;
			}

		case EStateTestNotify:	// test notify.
			{
			iWatcherStep.CheckMediaChangeL(2, KDriveE, MDriveObserver::EMediaInserted, _L("Expect EMediaInserted"));
			RunNextTest(EStateTestNotify2);
			break;
			}
			
		case EStateTestNotify2:	// test notify.
			{
			iWatcherStep.CheckMediaChangeL(3, KDriveE, MDriveObserver::EMediaInserted, _L("Expect EMediaInserted"));
			RunNextTest(EStateFinish);
			break;
			}
		}
	}

TInt CWatcherActiveStep::RunError(TInt aError)
	{
	if (aError==KErrGeneral)
		{
		RunNextTest(EStateFinish);
		return KErrNone;			
		}
	else
		{
		SetTestFailure(aError);
		return KErrNone;
		}
	}

void CWatcherActiveStep::SetTestFailure(TInt aError)
	{
	iWatcherStep.SetTestFailure(aError);
	}

void CWatcherActiveStep::NotifyMediaChange()
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	iMediaChange=ETrue;
	}

void CWatcherActiveStep::WaitForNotify()
	{
	iStatus=KRequestPending;
	SetActive();
	}

void CWatcherActiveStep::RunNextTest(TTestState aNextState)
	{
	iTestState=aNextState;
	WaitForNotify();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	}


} // namespace Swi::Test

} //namespace Swi

