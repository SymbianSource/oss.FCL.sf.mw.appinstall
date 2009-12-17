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
* This program kills all instances of a running exes specified in the command
* line.
* Usage: killrunningprocess <uid in hex> [delay in microseconds]
* If no delay is specified the kill will be attempted immediately.
*
*/


/**
 @file
*/

#include <e32base.h>
#include <bacline.h>

// private
LOCAL_C void mainL();
TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

	TRAPD(error,mainL()); // Trap main execution
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return error; // and return
	}

LOCAL_C void mainL()
	{
	CCommandLineArguments* cmdLine = CCommandLineArguments::NewLC();
	TInt argTotal=cmdLine->Count();
	if (argTotal < 2 || argTotal > 3)
		{
		User::Leave(KErrArgument);
		}
	TUint sidRequested;
	TLex processLex(cmdLine->Arg(1));
	User::LeaveIfError(processLex.Val(sidRequested, EHex));
	if (sidRequested == 0)
		{
		User::Leave(KErrArgument);
		}
	
	if (argTotal == 3)
		{
		TUint timeout;
		TLex timeoutLex(cmdLine->Arg(2));
		User::LeaveIfError(timeoutLex.Val(timeout));
		if (timeout > 0)
			{
			User::After(timeout);
			}
		}

	TUid sid = {sidRequested};
	TBool needToScanFullList;
	TFullName fullName;

	do
		{
		needToScanFullList = EFalse;
		TFindProcess findProcess;

		while(findProcess.Next(fullName) == KErrNone)
			{
			RProcess process;
			TInt ret=KErrNone;
			TInt count=0;
			do
			{
			count++;
 			ret=process.Open(findProcess);
			if (ret!=KErrNone)
				{
				User::After(200000);
				}
			}while (ret!=KErrNone && count<=10);
			if (ret!=KErrNone)
				{
				User::Leave(ret);
				}

			TUid processSid(process.SecureId());
			if (processSid == sid && process.ExitType() == EExitPending)
				{
				process.Kill(KErrNone);
				needToScanFullList = ETrue;
				}
			process.Close();
			}
		} while (needToScanFullList);

	CleanupStack::PopAndDestroy(cmdLine);
	}
