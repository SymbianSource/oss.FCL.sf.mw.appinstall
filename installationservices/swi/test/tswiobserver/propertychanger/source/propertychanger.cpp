/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a tool to export/import UPS Decision Database
*
*/


/**
 @file
 @publishedPartner
 @prototype
*/


#include <e32std.h>
#include <f32file.h>
#include <e32cons.h>
#include <bacline.h>
#include <utf.h>
#include <e32property.h>
#include <connect/sbdefs.h>

_LIT(KAppName, "Publish and Subscribe Property Changer");


static void MainL(void)
/**
Takes a User Prompt Service decision database file and dumps it as human readable text to the 
console. The user may also specify the name of an output file on the command line. If so, text
is also written to this file. Also recreates a UPS decision database from a dumped file. Where
the database is created is specified by the user.
*/
	{
	
	CConsoleBase *console = Console::NewL(KAppName,TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	
	CCommandLineArguments *cmdLineArgs = CCommandLineArguments::NewLC();
	
	TInt argc;
	argc = cmdLineArgs->Count();
	
	//At least operation type and database file path must be given
	if(argc < 2)
		{
		User::Leave(KErrArgument);
		}
	
	TPtrC argv = cmdLineArgs->Arg(1);
	
	switch(argv.Ptr()[0])
		{
		case 'b':
		case 'B':
		(void)RProperty::Set(KUidSystemCategory, conn::KUidBackupRestoreKey, conn::EBURBackupFull);		
		break;
		
		case 'r':
		case 'R':
		(void)RProperty::Set(KUidSystemCategory, conn::KUidBackupRestoreKey, conn::EBURRestoreFull);		
		break;

		case 'n':
		case 'N':
		(void)RProperty::Set(KUidSystemCategory, conn::KUidBackupRestoreKey, conn::EBURNormal);		
		break;
					
		}
		
	
	
	CleanupStack::PopAndDestroy(2, console);
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup *cp = CTrapCleanup::New();
	if(cp == NULL)
		{
		return KErrNoMemory;
		}
	
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
		
	__UHEAP_MARK;
	
	TRAPD(retval,MainL());
	
	__UHEAP_MARKEND;
	
	delete cp;
	delete scheduler;
	
	if(retval == KErrArgument)
		{
		return KErrNone;
		}
	return retval;
	}
