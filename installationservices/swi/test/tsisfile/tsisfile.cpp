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
* Main class for the sisfile testing.
*
*/


/**
 @file tsisfile.cpp
*/

#include <e32cons.h>
#include <e32std.h>
#include <f32file.h>

#include "sisparser.h"
#include "siscontents.h"
#include "sisdataprovider.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"


#include "siscontroller.h"
#include "sisinfo.h"

using namespace Swi;
using namespace Swi::Sis;
//using namespace Swi::Test;

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

void PrintError(TInt aErr, CConsoleBase* aConsole)
	{
	if (aErr != KErrNone) 
		{
		TBuf<138> error;
		_LIT(KBuf, "\n**** Failed with Error %d ****\n");
		error.Format(KBuf, aErr);
		aConsole->Printf(error);
		aConsole->Printf(_L("\n"));		
		}	
	}

void RunTestL() 
	{
	// Get the SISX file to install
	CConsoleBase* console = Console::NewL(_L("SISX Parsing Test"), 
										  TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);

	HBufC* sisxFile = NULL;

	TRAPD(err, sisxFile = GetArgumentL(0));
	CleanupStack::PushL(sisxFile);
	
	if ((err != KErrNone) || (*sisxFile == _L("")))
		{
		console->Printf(_L("Usage: tsisfile SISX_file_name"));
		console->Printf(_L("\n"));
		console->Printf(_L("Press Any Key to Continue..."));
		console->Getch();
		CleanupStack::PopAndDestroy(sisxFile);		
		CleanupStack::PopAndDestroy(console);		
		return;		
		}

	TFileName fileName(*sisxFile);
	RFs fs;
	CleanupClosePushL(fs);
	err = fs.Connect();

	if ((err != KErrNone) || (*sisxFile == _L("")))
		{
		PrintError(err, console);
		console->Printf(_L("Press Any Key to Continue..."));
		console->Getch();
		User::Leave(err);
		}	

	CFileSisDataProvider* dataProvider=NULL;
	
	TRAP (err, dataProvider = CFileSisDataProvider::NewL(fs, fileName)); 
	CleanupStack::PushL(dataProvider);
	
	if (err != KErrNone) 
		{
		PrintError(err, console);
		console->Printf(_L("Press Any Key to Continue..."));
		console->Getch();
		User::Leave(err);
		}
		
	CContents* contents = NULL;
	TRAP(err, contents = Parser::ContentsL(*dataProvider));
	CleanupStack::PushL(contents);

	if (err != KErrNone) 
		{
		PrintError(err, console);
		console->Printf(_L("Press Any Key to Continue..."));
		console->Getch();
		User::Leave(err);
		}
			
	HBufC8* controllerData = NULL;
	
	TRAP(err, controllerData = contents->ReadControllerL());
	CleanupStack::PushL(controllerData);

	PrintError(err, console);
	
	if (err == KErrNone) 
		{
		CDesDataProvider* controllerProvider= CDesDataProvider::NewLC(*controllerData);
		CController* controller = NULL;
		TRAP(err, controller = CController::NewL(*controllerProvider));
		CleanupStack::PushL(controller);
		
		PrintError(err, console);
		if (err == KErrNone)
			{
			console->Printf(_L("SISFile name: %S"), 
							&controller->Info().Names()[0]->Data());			
			}
			
		CleanupStack::PopAndDestroy(controller);	
		CleanupStack::PopAndDestroy(controllerProvider);	
		}	
 
	console->Printf(_L("\n"));
	console->Printf(_L("Press Any Key to Continue..."));
	console->Getch();
	
	CleanupStack::PopAndDestroy(controllerData);	
	CleanupStack::PopAndDestroy(contents);	
	CleanupStack::PopAndDestroy(dataProvider);	
	CleanupStack::PopAndDestroy(&fs);	
	CleanupStack::PopAndDestroy(sisxFile);	
	CleanupStack::PopAndDestroy(console);	
	}
        

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAP_IGNORE(RunTestL());
	//ASSERT(err == KErrNone);
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }
