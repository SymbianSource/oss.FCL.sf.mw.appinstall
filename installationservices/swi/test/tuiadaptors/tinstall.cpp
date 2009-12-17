/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Tests the installation procedure. It installs a sis file from the command line.
*
*/


/**
 @file tinstall.cpp
*/

#include "consoleclientserver.h"

#include <swi/launcher.h>
#include "tuiadaptors.h"

/**
 * Extracts the nPos command line argument.
 */
HBufC* GetArgumentL(TInt nPos)
	{
	HBufC *argv = HBufC::NewLC(User::CommandLineLength());
	TPtr cmd(argv->Des());
	User::CommandLine(cmd);

	TLex arguments(cmd);

	// finds nth parameter
	while(nPos && !arguments.Eos())
		{
		TPtrC token = arguments.NextToken();
		if(token.Length() > 0)
			{
			nPos--;
			}
		}

	HBufC* result = NULL;
	if(!arguments.Eos())
		{
		TPtrC testfile(arguments.NextToken());

		if(testfile.Length() > 0)
			{
			result = testfile.AllocL();
			}
		};

	// no parameter found, but must return something so..
	if(!result)
		{
		result = HBufC::NewL(0);
		}

	CleanupStack::PopAndDestroy(argv);

	return result;
	}
        

void RunTestL() 
	{
	TBuf<128> mymsg;
	_LIT(KNewLine, "\n");	
	
	Swi::Test::RConsoleServerSession consoleSession;
	CleanupClosePushL(consoleSession);

	User::LeaveIfError(consoleSession.Connect());
	consoleSession.ShareAuto();
	
	HBufC* sisxFile = NULL;

	TRAPD(err, sisxFile = GetArgumentL(0));
	CleanupStack::PushL(sisxFile);
	
	if ((err != KErrNone) || (*sisxFile == _L("")))
		{
		mymsg.Format(_L("Usage: tinstall SISX_file_name"));
		consoleSession.Printf(mymsg);
		mymsg.Format(KNewLine);
		consoleSession.Printf(mymsg);
		mymsg.Format(_L("Press Any Key to Continue..."));		
		consoleSession.Printf(mymsg);
		consoleSession.Getch();

		CleanupStack::PopAndDestroy(sisxFile);		
		CleanupStack::PopAndDestroy(&consoleSession);		
		return;		
		}

	// we got the file name! Start up the installation!
	TFileName filename(*sisxFile); 

	// Create a UI handler
	
	Swi::Test::SisUiAdapter* uiadapter = Swi::Test::SisUiAdapter::NewLC(consoleSession);	

	// Launch the installation
	mymsg.Format(_L("Launching the installation, fasten your seatbelts...\n"));
	consoleSession.Printf(mymsg);	

	Swi::CInstallPrefs* prefs = Swi::CInstallPrefs::NewLC();
	err=Swi::Launcher::Install(*uiadapter, filename, *prefs);
	CleanupStack::PopAndDestroy(prefs);
	if (err!=KErrNone)
		{
		mymsg.Format(_L("\n*** Error: (%d)"), err);
		consoleSession.Printf(mymsg);
		}
	mymsg.Format(KNewLine);
	consoleSession.Printf(mymsg);
	mymsg.Format(_L("Press Any Key to Continue..."));
	consoleSession.Printf(mymsg);
	consoleSession.Getch();

	CleanupStack::PopAndDestroy(uiadapter);
	CleanupStack::PopAndDestroy(sisxFile);
	CleanupStack::PopAndDestroy(&consoleSession);
	}

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(err, RunTestL());
	if(err != KErrNone)
		{
		_LIT(KPanicCategory,"tinstall.cppMain");
		User::Panic(KPanicCategory,ESwiInstallerPanic);
		}
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }
