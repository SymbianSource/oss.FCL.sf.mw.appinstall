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

#ifndef __WATCHERSTEP_H__
#define __WATCHERSTEP_H__

#include <test/testexecutestepbase.h>

#include "tdaemonstep.h"
#include "drivewatcher.h"

namespace Swi
{
class CAppInfo;

namespace Test
{
_LIT(KDriveWatcherStep,"DriveWatcherStep");

class CWatcherActiveStep;

class CWatcherStep : public CDaemonTestStep, public MDriveObserver
	{
public:
	CWatcherStep();
	~CWatcherStep();

	virtual TVerdict runTestStepL(TBool aOomTest);

	/**
	 * Checks the last media change was as expected. 
	 * Leaves on error and sets Test step to failure.
	 *
	 * @param aChangeNumber The number of the expected last change
	 * @param aDrive			The expected drive of the last change.
	 * @param aChangeType	The expected changet type of the last change
	 */
	void CheckMediaChangeL(TInt aChangeNumber, TInt aExpectedDrive, TInt aExpectedChangeType, const TDesC& aDescription);

	void SetTestFailure(TInt aError);
	
protected:
	void MediaChangeL(TInt aDrive, TChangeType aChangeType);


	void FreeMemory();
	
	/**
	 * Class to hold information about a notification
	 */
	class TNotification
		{
	public:
		TNotification(TInt aDrive, TChangeType aChangeType);
		
		TInt iDrive;
		
		TChangeType iChangeType;
		};
	
	CWatcherActiveStep* iActiveStep;
	
	RArray<TNotification> iNotifications;		
	
	TInt iError;
	};


class CWatcherActiveStep : public CActive
	{
	enum TTestState { EStateStartup, EStateTestNotify, EStateTestNotify2, EStateFinish };

public:
	static CWatcherActiveStep* NewL(CWatcherStep& aWatcherStep, TInt aPriority = CActive::EPriorityStandard);
	static CWatcherActiveStep* NewLC(CWatcherStep& aWatcherStep, TInt aPriority = CActive::EPriorityStandard);
	
	~CWatcherActiveStep();

	void NotifyMediaChange();

	void StartTest();

private:	
	CWatcherActiveStep(CWatcherStep& aWatcherStep, TInt aPriority);
	void ConstructL();

// from CActive
	void DoCancel();
	void RunL();

	void WaitForNotify();

	/// Run the next test 
	void RunNextTest(TTestState aNextState);

	void CheckResults();
	void RunTestsL();
	
TInt RunError(TInt aError);

	void SetTestFailure(TInt aError);
private:


	CWatcherStep& iWatcherStep;
	
	TTestState iTestState;
	
	CDriveWatcher* iDriveWatcherX;
	
	RFs iFs;
	
	TBool iMediaChange;
	};

} // namespace Swi::Test

} //namespace Swi

#endif // #ifndef __TIMEOUTSTEP_H__

