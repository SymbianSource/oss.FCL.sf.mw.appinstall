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
* This console executable runs for a limited time (default 5 seconds but
* can be specified on the command line) and than keeps running infinitely 
* at the background.
* Used for testing CR956
*
*/


#include <e32base.h>
#include <e32cons.h>
#include <bacline.h>
#include <f32file.h>

_LIT(KTxtEPOC32EX,"console_app: mainL failed");
_LIT(KTxtExampleCode,"console_app");
_LIT(KFormatRunning ,"Running for %d microseconds. ");	


#ifdef _CONSOLE_APP_LONG_RUNNING_DEFAULT_
// Default run time exceeds default runwait timeout value.
const TInt KDefaultTimeout = 185000000; // Defaut wait time for 185 secs
#else
const TInt KDefaultTimeout = 5000000; //Wait time for 5 secs
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

	TInt timerDuration(KDefaultTimeout);

	// There's only something useful to do if the timeDuration is
	// a positive value.
	if(timerDuration > 0)
		{
		CConsoleBase* console; // write all your messages to this

		console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
		
		console->Printf(KFormatRunning, timerDuration);

		while(1)
		{
		//console->Printf(KTxtExampleCode);
		User::After(1000000); //After 1 sec
		}

		}
    }

// End of file
