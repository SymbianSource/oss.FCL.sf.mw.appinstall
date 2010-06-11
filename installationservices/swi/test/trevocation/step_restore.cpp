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
*
*/

#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include "swi/backuprestore.h"

CTStepRestore::CTStepRestore()
	{
	SetTestStepName(KTStepRestore);
	}

TVerdict CTStepRestore::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();
	
	Swi::RRestoreSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
		HBufC8* metadata = ReadMetaDataL();
		CleanupStack::PushL(metadata);
		session.StartPackageL(PackageUID(), *metadata);
		while (NextFile())
			{
			RFile sourcefile;
			User::LeaveIfError(sourcefile.Open(fs, BackupFileName(), EFileRead));
			TPtrC destfilename = InstalledFileName();
			session.RestoreFileL(sourcefile, destfilename);
			}
		session.CommitPackageL();
		CleanupStack::PopAndDestroy(metadata);
		}	
	CleanupStack::PopAndDestroy(2, &fs);	// session	
__UHEAP_MARKEND;
	return TestStepResult();
	}

