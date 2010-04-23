/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sifintegrationlockfile.cpp
* This executable is used to lock a file in order to simulate errors while deleting files
*
*/


#include <e32base.h>
#include <e32cons.h>
#include <bacline.h>
#include <f32file.h>

LOCAL_C void mainL();

TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,mainL()); // Run main method
	__ASSERT_ALWAYS(!error, User::Panic(_L("SifIntegrationTest"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
	}

LOCAL_C void mainL()
	{
	CCommandLineArguments* cmdLine = CCommandLineArguments::NewLC();
	CConsoleBase* console = Console::NewL(_L("Siftestintegrationlockfile"),TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	TInt argTotal(cmdLine->Count());
	if (argTotal < 2  || argTotal > 3)
		{
		console->Printf(_L("Incorrect arguments specified: expected 1, received %d"), argTotal - 1);
		User::Leave(KErrArgument);
		}
		
	TPtrC filename(cmdLine->Arg(1));
	_LIT(KDoLockFileParam, " lockfile");
	if (argTotal == 2)
		{
		RBuf params;
		params.CreateL(filename.Length() + KDoLockFileParam().Length());
		params.CleanupClosePushL();
		params.Append(cmdLine->Arg(1));
		params.Append(KDoLockFileParam());
		// Since this executable is used by TEF, we wish to lock the file after the launched process has exited, so we spawn this process again with a different set of parameters
		RProcess newInstance;
		User::LeaveIfError(newInstance.Create(_L("Siftestintegrationlockfile"), params));
		CleanupClosePushL(newInstance);
		newInstance.Resume();
		TRequestStatus status;		
		newInstance.Rendezvous(status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		CleanupStack::PopAndDestroy(2, &params); // newInstance
		}
	else
		{
		// This is the execution for locking the file, invoked using the branch above
		console->Printf(_L("Locking file %S for read"), &filename);	
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
	
		RFile file;
		User::LeaveIfError(file.Open(fs, filename, EFileShareReadersOnly|EFileRead));
		CleanupClosePushL(file);
		// Signal the invoker only here, so that the file will definitely get locked before TEF proceeds to the next step
		RProcess::Rendezvous(KErrNone);		
		
		User::After(10*1000*1000); // Wait for 10 seconds
	
		CleanupStack::PopAndDestroy(2 , &fs); // file
		}
	CleanupStack::PopAndDestroy(2, cmdLine); // console,
    }

// End of file
