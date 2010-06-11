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

CTStepRestoreCorruptController::CTStepRestoreCorruptController()
	{
	SetTestStepName(KTStepRestoreCorruptController);
	}

TVerdict CTStepRestoreCorruptController::doTestStepL()
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
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
		HBufC8* metadata = ReadMetaDataL();
		CleanupStack::PushL(metadata);
		metadata->Des()[metadata->Length()-4] += 1;	// Corrupt the data
		session.StartPackageL(PackageUID(), *metadata);
		while (NextFile())
			{
			RFile sourcefile;
			User::LeaveIfError(sourcefile.Open(fs, BackupFileName(), EFileRead));
			TDesC destfilename(InstalledFileName());
			session.RestoreFileL(sourcefile, destfilename);
			}
		session.CommitPackageL();
		CleanupStack::PopAndDestroy(metadata);
		}	
	CleanupStack::PopAndDestroy(2, &fs);	// session
__UHEAP_MARKEND;
	return TestStepResult();
	}

