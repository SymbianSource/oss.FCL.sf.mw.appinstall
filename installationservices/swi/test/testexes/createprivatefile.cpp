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
*
*/


/**
 @file
*/

//Create a simple exe that creates a file in its private directory when it is run
#include <e32def.h>
#include <e32std.h>
#include <f32file.h>


static TInt MainL(void)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.CreatePrivatePath(RFs::GetSystemDrive()));
	
	RFile file;	
	User::LeaveIfError(file.Create(fs, _L("helloworld.txt"), EFileWrite | EFileShareExclusive));
	CleanupClosePushL(file);
	CleanupStack::PopAndDestroy(2, &fs);
	return 0;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
	}
	
