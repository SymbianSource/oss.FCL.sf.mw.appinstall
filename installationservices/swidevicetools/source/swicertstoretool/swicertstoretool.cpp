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
*
*/


#include "CSWICertStoreTool.h"

void RunToolL()
    {
	// Get command line
	HBufC* argv = HBufC::NewLC(User::CommandLineLength());
	TPtr cmd(argv->Des());
	User::CommandLine(cmd);
	
	// Validate command line
	TLex argumentsValidation(cmd);	
	TInt argc = 0;
	while (argumentsValidation.NextToken().Length())
		{
		++argc;
		}

	if (argc != 3)
		{
		User::Leave(KErrArgument);
		}

	TLex arguments(cmd);
	TPtrC inputFile(arguments.NextToken());
	TPtrC outputFile(arguments.NextToken());
	TPtrC logFile(arguments.NextToken());

	CSWICertStoreTool* tool = CSWICertStoreTool::NewLC(inputFile, outputFile, logFile);
	tool->RunToolL();
	CleanupStack::PopAndDestroy(2, argv); // argv, tool
    }

// Entry point
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(error, RunToolL()); 

	delete cleanup;
	__UHEAP_MARKEND;
	return error;
    }
