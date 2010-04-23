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

CTStepRestoreExtraExecutable::CTStepRestoreExtraExecutable()
	{
	SetTestStepName(KTStepRestoreExtraExecutable);
	}

TVerdict CTStepRestoreExtraExecutable::doTestStepL()
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
		session.StartPackageL(PackageUID(), *metadata);
		while (NextFile())
			{
			RFile sourcefile;
			User::LeaveIfError(sourcefile.Open(fs, BackupFileName(), EFileRead));
			if (WrongInstalledFileName().Length())
				{
				TDesC destfilename(WrongInstalledFileName());
				TRAPD (err, session.RestoreFileL(sourcefile, destfilename));
				if (err != KErrNone)
					{
	    			INFO_PRINTF1(_L("Swi::MRestore::RestoreFileL unexpectedly succeeded"));
					SetTestStepResult(EFail);
					}
				}
			else
				{
				TDesC destfilename(InstalledFileName());
				session.RestoreFileL(sourcefile, destfilename);
				}
			}
		TRAPD (err, session.CommitPackageL());
		if (err != KErrNone)
			{
	    	INFO_PRINTF1(_L("CommitPackageL left"));
			SetTestStepResult(EFail);
			}			
		CleanupStack::PopAndDestroy(metadata);
		}	
	CleanupStack::PopAndDestroy(2, &fs);	// session	
__UHEAP_MARKEND;
	return TestStepResult();
	}
