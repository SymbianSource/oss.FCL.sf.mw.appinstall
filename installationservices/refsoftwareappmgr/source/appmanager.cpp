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
*
*/


#include <usif/scr/scr.h>
#include <e32cons.h>
#include "command.h"

using namespace Usif;

_LIT(KTxtAppTitle, "Reference Application Manager\n");
_LIT(KTxtAppVersion, "Version 1.00(0)\n");
_LIT(KTxtCopyright, "Copyright (c) Symbian Software Ltd 2008. All rights reserved.\n");

_LIT(KTxtEPOC32EX,"EXAMPLES");
_LIT(KTxtFormatFailed,"\nFailed with leave code = %d\n");
_LIT(KTxtPressToExit, "Press any key to exit...");
_LIT(KTxtPressToContinue, "\nPress any key to continue...\n\n");

_LIT(KTxtHelp01, "\nThe syntax of the command is incorrect. Use the following options:\n\n");
_LIT(KTxtHelp02, "--list [--name name] [--vendor vendor] [--type type] [--removable|nonremovable]\n");
_LIT(KTxtHelp03, "[--activated|deactivated] [--drives drivelist] [--property name value [language]]\n");
_LIT(KTxtHelp04, "[--intproperty name value] [--file path] [--locale language]\n\n");

_LIT(KTxtHelp05, "OPTIONS\n\n");
_LIT(KTxtHelp06, "    --name NAME\n");
_LIT(KTxtHelp07, "        lists components with name NAME\n\n");
_LIT(KTxtHelp08, "    --vendor VENDOR\n");
_LIT(KTxtHelp09, "        lists components with vendor VENDOR\n\n");
_LIT(KTxtHelp10, "    --type TYPE\n");
_LIT(KTxtHelp11, "        lists components with software type TYPE\n\n");
_LIT(KTxtHelp12, "    --removable\n");
_LIT(KTxtHelp13, "        lists removable components only\n\n");
_LIT(KTxtHelp16, "    --activated\n");
_LIT(KTxtHelp17, "        lists components with SCOMO state Activated\n\n");
_LIT(KTxtHelp18, "    --deactivated\n");
_LIT(KTxtHelp19, "        lists components with SCOMO state Deactivated\n\n");
_LIT(KTxtHelp20, "    --drives DRIVES\n");
_LIT(KTxtHelp21, "        lists components whose files were installed to DRIVES\n\n");
_LIT(KTxtHelp22, "    --property N V L\n");
_LIT(KTxtHelp23, "        lists components with string property name N and value V and optional language code L, for example English=1, French=2, German=3\n\n");
_LIT(KTxtHelp26, "    --locale LANGUAGE\n");
_LIT(KTxtHelp27, "        defines language LANGUAGE to be used to find the component info, for example English=1, French=2, German=3\n\n");

_LIT(KTxtHelp30, "--delete ComponentID\n");
_LIT(KTxtHelp31, "    deletes component with ComponentID. The ID of a component to be removed can be obtained using the 'list' command.\n\n");

_LIT(KTxtHelp40, "EXAMPLES\n\n");
_LIT(KTxtHelp41, "    refappmanager --list\n\n");
_LIT(KTxtHelp42, "    refappmanager --list --type native\n\n");
_LIT(KTxtHelp43, "    refappmanager --list --vendor Symbian\n\n");
_LIT(KTxtHelp44, "    refappmanager --list --type java --drives cef --activated --locale 2\n\n");
_LIT(KTxtHelp45, "    refappmanager --delete 6\n\n");

// ==================================================================================

// Start-up functions and helpers
LOCAL_C void InitL();
LOCAL_C void ExecCommandL(CConsoleBase& aConsole);
LOCAL_C void DisplayHelp(CConsoleBase& aConsole);

// Main function called by E32
GLDEF_C TInt E32Main() 
	{
	// Get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();

	__UHEAP_MARK;

	// Call InitL() and catch all leaves
	TRAPD(error,InitL());
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));

	__UHEAP_MARKEND;

	// Destroy clean-up stack
	delete cleanup;

	return 0;
    }

// ==================================================================================

LOCAL_C void InitL()
	{
	// Create console
	CConsoleBase* console = Console::NewL(KTxtAppTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	// Call ExecCommandL() and catch all leaves
	TRAPD(err, ExecCommandL(*console));
	if (err == KErrArgument)
		{
		DisplayHelp(*console);
		}
	else if (err != KErrNone)
		{
		console->Printf(KTxtFormatFailed, err);
		}

	// Wait for any key
	console->Printf(KTxtPressToExit);
	console->Getch();

	CleanupStack::PopAndDestroy(console);
	}

// ==================================================================================

LOCAL_C void ExecCommandL(CConsoleBase& aConsole)
	{
	// Display info
	aConsole.Printf(KTxtAppTitle);
	aConsole.Printf(KTxtAppVersion);
	aConsole.Printf(KTxtCopyright);

	// Create parser
	CCommandLineParser* parser = CCommandLineParser::NewLC();

	// Instantiate and register the 'list' and 'delete' commands. If we had
	// more commands it could be more efficient to implement a command factory
	// in order to instantiate only a single command on demand. This example
	// application provides only two commands and therefore the above mentioned
	// optimisation isn't needed here.
	parser->RegisterCommandL(*CListCommand::NewLC());
	parser->RegisterCommandL(*CDeleteCommand::NewLC());

	// Parse input and find the matching command
	CConsoleCommand& cmd = parser->ParseL();

	// Execute the command
	cmd.ExecuteL(aConsole);

	// Cleanup the parser and commands
	CleanupStack::PopAndDestroy(3, parser);
	}

// ==================================================================================

LOCAL_C void DisplayHelp(CConsoleBase& aConsole)
	{
	aConsole.Printf(KTxtHelp01);
	aConsole.Printf(KTxtHelp02);
	aConsole.Printf(KTxtHelp03);
	aConsole.Printf(KTxtHelp04);
	aConsole.Printf(KTxtHelp05);
	aConsole.Printf(KTxtHelp06);
	aConsole.Printf(KTxtHelp07);
	aConsole.Printf(KTxtHelp08);
	aConsole.Printf(KTxtHelp09);

	aConsole.Printf(KTxtPressToContinue);
	aConsole.Getch();

	aConsole.Printf(KTxtHelp10);
	aConsole.Printf(KTxtHelp11);
	aConsole.Printf(KTxtHelp12);
	aConsole.Printf(KTxtHelp13);
	aConsole.Printf(KTxtHelp16);
	aConsole.Printf(KTxtHelp17);
	aConsole.Printf(KTxtHelp18);
	aConsole.Printf(KTxtHelp19);
	aConsole.Printf(KTxtHelp20);
	aConsole.Printf(KTxtHelp21);

	aConsole.Printf(KTxtPressToContinue);
	aConsole.Getch();

	aConsole.Printf(KTxtHelp22);
	aConsole.Printf(KTxtHelp23);
	aConsole.Printf(KTxtHelp26);
	aConsole.Printf(KTxtHelp27);
	aConsole.Printf(KTxtHelp30);
	aConsole.Printf(KTxtHelp31);

	aConsole.Printf(KTxtPressToContinue);
	aConsole.Getch();

	aConsole.Printf(KTxtHelp40);
	aConsole.Printf(KTxtHelp41);
	aConsole.Printf(KTxtHelp42);
	aConsole.Printf(KTxtHelp43);
	aConsole.Printf(KTxtHelp44);
	aConsole.Printf(KTxtHelp45);
	}
