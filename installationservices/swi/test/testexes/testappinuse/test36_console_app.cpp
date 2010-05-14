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
* This console executable runs for a limited time (default 5 seconds but
* can be specified on the command line) and optionally holds open
* a file from the same SIS file.
* Used for testing DEF123261
*
*/


#include <e32base.h>
#include <e32cons.h>
#include <bacline.h>
#include <f32file.h>
 

_LIT(KTxtEPOC32EX,"test36_console_app: mainL failed");
_LIT(KTxtExampleCode,"test36_console_app");
_LIT(KFormatRunning ,"Running for %d microseconds. ");
_LIT(KSetShutDownTimeOption, "-s");
_LIT(KHoldFileOpenSwitch, "-h");
_LIT(KDefaultHeldFileName, "\\documents\\insttest\\file.txt");
_LIT(KCommandSwitch, "-");
_LIT(KHoldingFileStartString, "Holding file  ");
_LIT(KHoldingFileEndString, " open.");



#ifdef _CONSOLE_APP_LONG_RUNNING_DEFAULT_
// Default run time exceeds default runwait timeout value.
const TInt KDefaultTimeout = 185000000;
#else
const TInt KDefaultTimeout = 5000000;
#endif

// private
LOCAL_C void mainL();

TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,mainL()); // Run main method
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
	}

LOCAL_C void mainL()
	{
	TBool holdFileOpen(EFalse);

	TInt timerDuration(KDefaultTimeout);
 
 	TBuf<256> nextArg(KNullDesC); 
	TBuf<256> fileToHold(KNullDesC);
		
    CCommandLineArguments* cmdLine = CCommandLineArguments::NewL();
	TInt argTotal=cmdLine->Count();
	for (TInt loop=1 ; loop < argTotal ; ++loop)
		{
		TPtrC arg(cmdLine->Arg(loop));
		
		// Process the file holding option
		if (arg==KHoldFileOpenSwitch)
		{	
			// The -h option has been issued at the command line so we need to 
			// hold a file open. 		
			holdFileOpen = ETrue;
			
			// If there is another argument it may be the filename of 
			// the file to be held.
			if (loop++ < (argTotal-1)) 
			{	
				// Read	the argument in so that it can be checked.				
				fileToHold = cmdLine->Arg(loop);
			
	 
				if (fileToHold.Left(1) == KCommandSwitch)
				{
					// The argument is another switch and not a filename
					// Decrement the loop counter so that the argument can 
					// be processed subsequently.
					loop--;		
					fileToHold = KNullDesC;
				}
			
			}
		 	
		}
		 
		// Process the timer option
		else if ((arg==KSetShutDownTimeOption))
			{
				// If there is another argument it may be the timer value
				if (loop++ < (argTotal-1)) 
				{	
					// Read	the argument in so that it can be checked.					
			 		nextArg = cmdLine->Arg(loop);						   	 
				
	 
					if (nextArg.Left(1) == KCommandSwitch) 
					{
						// The argument is another switch and not a filename
						// Decrement the loop counter so that the argument can 
						// be processed subsequently .
						loop--;		
					}
					else 
					{
						// The argument must be the timer value. Extract the value.
						TLex timeoutLex(cmdLine->Arg(loop));
						timeoutLex.Val(timerDuration);
					}
				}
			 
			}
		}
	delete cmdLine;

	// There's only something useful to do if the timeDuration is
	// a positive value.
	if(timerDuration > 0)
		{
		CConsoleBase* console; // write all your messages to this

		console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
		
		console->Printf(KFormatRunning, timerDuration);

		RTimer timer;
		TRequestStatus timerStatus;
		timer.CreateLocal();
		timer.After(timerStatus, timerDuration);
		
		RFs fileSession;
		fileSession.Connect();

		// Timer started, now open the file if "-h" switch specified.
		if (holdFileOpen)
			{
			RFile heldFile;
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TFileName fileName (sysDrive.Name());
		 
			console->Printf(KHoldingFileStartString);


			if (fileToHold != KNullDesC)
			{
				// Use the filename from the command line if provided.
				fileName.Append(fileToHold);
				console->Printf(fileToHold);
				RDebug::Print(_L("Holding file %S open for %d microseconds"), &fileName, timerDuration);	
			}
			else 
			{
				// Otherwise use the default file name.
				fileName.Append(KDefaultHeldFileName);	
				console->Printf(KDefaultHeldFileName);
				RDebug::Print(_L("Holding default file %S open for %d microseconds"), &fileName, timerDuration);
			}
	 
	 		console->Printf(KHoldingFileEndString);

			// Hold the file
			heldFile.Open(fileSession, fileName, EFileRead);
 
			User::WaitForRequest(timerStatus);
			heldFile.Close();
			}
		else 
			{
			User::WaitForRequest(timerStatus);	
			}

		timer.Close();

		_LIT(KFlaggingFileName, "\\testrun_flag_file.out");
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		RFile flaggingFile;
		TBuf<128> flaggingFileName (sysDrive.Name());
		flaggingFileName.Append(KFlaggingFileName);				
		flaggingFile.Replace(fileSession, flaggingFileName, EFileWrite);
		flaggingFile.Close();
			
		fileSession.Close();
		delete console; // delete console
		
		}
    }

// End of file
