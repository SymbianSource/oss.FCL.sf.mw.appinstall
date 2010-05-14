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


#include "deletescrdbstep.h"

// -----------CScrDeleteDbFileStep-----------------

CScrDeleteDbFileStep::CScrDeleteDbFileStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrDeleteDbFileStep::ImplTestStepPreambleL()
	{
	User::LeaveIfError(iScrAccessor.Connect());
	}

void CScrDeleteDbFileStep::DeleteFileL(const TDesC& aFilePath)
	{
	TInt err = iScrAccessor.DeleteFile(aFilePath);
	TInt tries = 0;
	
	while((KErrNone != err && KErrNotFound != err && KErrPathNotFound != err) && tries++ < 60)
		{
		User::After(500000);
		err = iScrAccessor.DeleteFile(aFilePath);
		}
	if(KErrNone != err && KErrNotFound != err && KErrPathNotFound != err)
		{
		ERR_PRINTF3(_L("%S file couldn't be deleted! Error=%d"), &aFilePath, err);
		SetTestStepResult(EFail);
		}
	}

void CScrDeleteDbFileStep::ImplTestStepL()
	{
	TPtrC dbFilePath;
	if (!GetStringFromConfig(ConfigSection(), _L("DbFilePath"), dbFilePath))
		PrintErrorL(_L("Database File Path was not found!"), KErrNotFound);
	
	TPtrC journalFilePath;
	if (!GetStringFromConfig(ConfigSection(), _L("JournalFilePath"), journalFilePath))
		PrintErrorL(_L("Journal File Path was not found!"), KErrNotFound);
		
	DeleteFileL(dbFilePath);
	DeleteFileL(journalFilePath);
	User::After(500000);
	}

void CScrDeleteDbFileStep::ImplTestStepPostambleL()
	{
	iScrAccessor.Close();
	}

// -----------CScrCopyDbFileStep-----------------

CScrCopyDbFileStep::CScrCopyDbFileStep(CScrTestServer& aParent)	: CScrTestStep(aParent)
	{
	}

void CScrCopyDbFileStep::ImplTestStepPreambleL()
	{
	User::LeaveIfError(iScrAccessor.Connect());
	}


void CScrCopyDbFileStep::ImplTestStepL()
	{
	TPtrC sourceFilePath;
	if (!GetStringFromConfig(ConfigSection(), _L("SourceFilePath"), sourceFilePath))
		PrintErrorL(_L("Source File Path was not found!"), KErrNotFound);
	
	TPtrC targetFilePath;
	if (!GetStringFromConfig(ConfigSection(), _L("TargetFilePath"), targetFilePath))
		PrintErrorL(_L("Target File Path was not found!"), KErrNotFound);
		
	User::LeaveIfError(iScrAccessor.CopyFile(sourceFilePath, targetFilePath));
	}

void CScrCopyDbFileStep::ImplTestStepPostambleL()
	{
	iScrAccessor.Close();
	}
