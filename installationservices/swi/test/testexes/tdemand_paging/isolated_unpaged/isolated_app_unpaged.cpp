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
* This isolated_app_unpaged executable 
*
*/


/**
 @file
 isolated_app implementation
*/

//System include
#include <e32base.h> 
#include <e32cons.h>
#include <bacline.h>
#include <f32file.h>
#include <e32des8.h>

#include "dempage_exe.h"

_LIT(KTxtHelloMessage,"Hello!"); 

/**
Description:  main method      
@internalTechnology
@return void
@test
*/
namespace Swi
{
 	void mainL()
	{ 
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	User::LeaveIfError(fs.ShareProtected());
	
	//CFileObserver* iObserver = CFileObserver::NewL();
	// public 
	// write all your messages to this 
	CSmallArray* myArray = CSmallArray::NewLC( KTxtHelloMessage);
	
 //	CFileWatcher* pFileWatcher= CFileWatcher::NewL(fs,*myArray, *iObserver,0);

	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(1);	

	}
}

/**
Description:  main function called by E32
@internalTechnology
@return TInt - return the error value
@test
*/
TInt E32Main() 
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
	TRAPD(error,Swi::mainL());
	delete scheduler;
	delete cleanup;
	return error;
	}


