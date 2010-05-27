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
* test the device info support code
*
*/


#include <e32std.h>
#include <e32base.h>
#include "devinfosupportclient.h"

// always define __SWI_LOGGING2__ so that we get some output (can't
// use the console because it doesn't have TCB and this is a TCB test
// harness).  Also if you don't define it you get an unused argument
// warning on aStepText
#define __SWI_LOGGING2__
#include "oldlog.h"
using Swi::FPrint;
using Swi::RDeviceInfo;
using Swi::KCustomLogName;

LOCAL_C void PrintLeaveIfErrorL(const TPtrC &aStepText, TInt aErrorCode)
	{
	FTRACE(FPrint(aStepText));
	if (aErrorCode != KErrNone)
		{
		FTRACE(FPrint(_L("tdevinfosupport:  - Failed: error returned was %d"), aErrorCode));
		User::Leave(aErrorCode);
		}
	else
		{
		FTRACE(FPrint(_L("tdevinfosupport:  - Success.")));
		}
	}
	
LOCAL_C void CreateandDestroyConnectionL()	
	{
	FLOG(_L("tdevinfosupport: tdevinfosupport test harness running for create connection and destroying it..."));		
	RDeviceInfo deviceInfo;
	CleanupClosePushL(deviceInfo);
	
	FLOG(_L("tdevinfosupport: Creating connection..."));
	PrintLeaveIfErrorL(_L("tdevinfosupport: deviceInfo.Connect()"),
					   deviceInfo.Connect());
	
	FLOG(_L("tdevinfosupport: Destroying connection..."));
	CleanupStack::PopAndDestroy(); //deviceInfo				   
	}

LOCAL_C void callSetupAndRunL() // initialize and call example code under cleanup stack
    {
	FLOG(_L("tdevinfosupport: tdevinfosupport test harness running for get the device information.."));
	RDeviceInfo deviceInfo;
	CleanupClosePushL(deviceInfo);
	PrintLeaveIfErrorL(_L("tdevinfosupport: deviceInfo.Connect()"),
					   deviceInfo.Connect());

	FLOG(_L("tdevinfosupport: deviceInfo.DeviceIdsL()"));
	const RPointerArray<HBufC>& deviceIds = deviceInfo.DeviceIdsL();

	FTRACE(FPrint(_L("tdevinfosupport: deviceIds.Count() = %d"), deviceIds.Count()););
	for (TInt i = 0; i < deviceIds.Count(); i++)
		{
		FTRACE(FPrint(_L("tdevinfosupport: deviceIds[%d] = %S"), i, deviceIds[i]););
		}

	CleanupStack::PopAndDestroy(); //deviceInfo
    }

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	
	// delete the log file to start with a clean slate.  Ignore all
	// errors - we don't _really_ care if we can't delete the log
	// file.
	RFs fs;
	fs.Connect();
	fs.Delete(KCustomLogName);
	fs.Close();
	
	TInt totaltests = 2;
	TInt testfailed = 0;
	// Create connection and immediately destroyed
	TRAPD(error, CreateandDestroyConnectionL());
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("tdevinfosupport: CreateandDestroyedConnectionL() left with error code %d"), error););
		++testfailed;
		}
	else
		{
		FLOG(_L("tdevinfosupport: Tests CreateandDestroyedConnectionL is OK... "));			
		}	
		
	// Get the Device Id list 
	TRAPD(error1, callSetupAndRunL());
	User::After(15000000); // 15 second wait for server to close
	if (error1 != KErrNone)
		{
		FTRACE(FPrint(_L("tdevinfosupport: callSetupAndRunL() left with error code %d"), error););
		++testfailed;
		}
	else
		{
		FLOG(_L("tdevinfosupport: Tests callSetupAndRunL is OK... "));						
		}	

	if(testfailed == 0)		
		{
		FLOG(_L("tdevinfosupport: Tests completed OK"));
		FTRACE(FPrint(_L("0 tests failed out of %d"), totaltests););
		}
	else	
		{
		FTRACE(FPrint(_L("%d tests failed out of %d"),testfailed, totaltests););
		}
		
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }
