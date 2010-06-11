/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file is Not needed
*
*/


#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include "swi/backuprestore.h"

CTStepRestoreNoCommit::CTStepRestoreNoCommit()
	{
	SetTestStepName(KTStepRestoreNoCommit);
	}

TVerdict CTStepRestoreNoCommit::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();
	
	Swi::RRestoreSession session;
	TInt err = session.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Swi::RRestoreSession::Connect failed with %d"), err);
		User::Leave(err);
		}
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
		HBufC8* metadata = ReadMetaDataL();
		CleanupStack::PushL(metadata);

		TRAP (err, session.StartPackageL (PackageUID(), *metadata));
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Swi::RRestoreSession::StartPackageLC left %d"), err);
			User::Leave(err);
			}

		while (NextFile())
			{
			RFile sourcefile;
			User::LeaveIfError(sourcefile.Open(fs, BackupFileName(), EFileRead));
			TDesC destfilename(InstalledFileName());
			TRAP (err, session.RestoreFileL(sourcefile, destfilename));
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Swi::RRestoreSession::RestoreFileL left %d"), err);
				User::Leave(err);
				}
			}
//		TRAP (err, package->CommitL());
//		if (err != KErrNone)
//			{
//			INFO_PRINTF2(_L("Swi::MRestore::CommitL left %d"), err);
//			User::Leave(err);
//			}
		CleanupStack::PopAndDestroy(metadata);
		}	
	CleanupStack::PopAndDestroy(2, &fs);	// session	
__UHEAP_MARKEND;
	return TestStepResult();
	}
