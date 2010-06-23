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
#include "cleanuputils.h"

CTStepBackup::CTStepBackup()
	{
	SetTestStepName(KTStepBackup);
	}

TVerdict CTStepBackup::doTestStepL()
	{
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;	// Check for memory leaks
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	Swi::RBackupSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
 		RPointerArray<HBufC> files;
  		CleanupResetAndDestroyPushL(files);
  		HBufC8* metadata = session.GetMetaDataL(PackageUID(), files);
  		CleanupStack::PopAndDestroy(&files); // ignore this parameter
		CleanupStack::PushL(metadata);
		WriteMetaDataL(metadata);
		CleanupStack::PopAndDestroy(metadata);
		
		while (NextFile())
			{
			CopyFileL(InstalledFileName(), BackupFileName());
			}
		}
		
	CleanupStack::PopAndDestroy(2, &fs); // session;
__UHEAP_MARKEND;
	return TestStepResult();
	}
