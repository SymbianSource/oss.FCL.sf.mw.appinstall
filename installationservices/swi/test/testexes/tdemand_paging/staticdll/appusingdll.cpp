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
 @internalComponent 
*/
 
#include <e32base.h>
#include <f32file.h>
#include <e32cons.h>

#include "dempage_exe.h"

/**
Description:  main function 
@internalTechnology
@test
*/
void MainL()
	{
	CFileWatcher* pFileWatcher= CFileWatcher::NewLC();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy();	
	}

/**
 * Description: E32Main
 * Entry Point, sets up the cleanup stack and calls the main function.
 */
 
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler=new CActiveScheduler();

	if(cleanup == NULL || scheduler==NULL)
		{
		delete scheduler;
		delete cleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);

	TRAPD(ret, MainL());
	delete scheduler;
	delete cleanup;
	return ret;
	}

// End of file
