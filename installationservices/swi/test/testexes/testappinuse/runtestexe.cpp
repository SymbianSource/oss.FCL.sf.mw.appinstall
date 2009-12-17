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
* This program launches another executable in its own process and exits
* without waiting for completion.
* This is needed to run the console app "console_app" asynchronously
* from a testexecute script.
*
*/


/**
 @file
*/

#include <e32base.h>
#include <bacline.h>
#include <f32file.h>

_LIT(KTxtEPOC32EX,"runtestexe: mainL failed");
_LIT(KSpace, " ");
_LIT(KConsoleApp, "\\sys\\bin\\console_app.exe");


const TInt KMaxCommandLineLength = 256;

// private
LOCAL_C void mainL();
TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,mainL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
	}

LOCAL_C void mainL() // initialize and call example code under cleanup stack
	{
	CCommandLineArguments* cmdLine = CCommandLineArguments::NewL();
		
	TInt argTotal=cmdLine->Count();
	if (argTotal > 1)
		{
		TBuf<KMaxCommandLineLength> childArgs;
		for(TInt loop = 2; loop < argTotal; loop++)
			{
			childArgs.Append(cmdLine->Arg(loop));
			childArgs.Append(KSpace);
			}
		RProcess childProcess;
		if(childProcess.Create(cmdLine->Arg(1), childArgs) == KErrNone)
			childProcess.Resume();
		childProcess.Close();
		}
	else
		{
		RProcess childProcess;
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TBuf<128> consoleAppName (sysDrive.Name());
		consoleAppName.Append(KConsoleApp);
		if(childProcess.Create(consoleAppName, KNullDesC()) == KErrNone)
			childProcess.Resume();
		childProcess.Close();			
		}
	delete cmdLine;
	}
