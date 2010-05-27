/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tbackuprestorestep.h"
#include <test/testexecutelog.h>

#include <f32file.h>
#include "swi/sisregistrysession.h"
#include "swi/sisregistryentry.h"

CTStepCheckAbsent::CTStepCheckAbsent()
	{
	SetTestStepName(KTStepCheckAbsent);
	}

TVerdict CTStepCheckAbsent::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	Swi::RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	while (NextPackage())
		{
		TBool installed = session.IsInstalledL(PackageUID());
		
		Swi::RSisRegistryEntry entry;
		TBool entryInROM(EFalse);	
		TInt err = entry.Open(session, PackageUID());	
		if (err == KErrNone)
			{
			CleanupClosePushL(entry);
			entryInROM = entry.IsInRomL();
			CleanupStack::PopAndDestroy(&entry);
			}
	
		if (installed && !entryInROM)
			{
			INFO_PRINTF2(_L("Package %d is present"), PackageUID().iUid);
			SetTestStepResult(EFail);
			}

		while (NextFile())
			{
			if (Exists(InstalledFileName()))
				{
				TPtrC filename = InstalledFileName();
				INFO_PRINTF2(_L("%S still extant"), &filename);
				SetTestStepResult(EFail);
				}
			}
			
		// check the data files don't exist too
		while (NextDataFile())
			{
			if (Exists(DataFileName()))
				{
				TPtrC filename = DataFileName();
				INFO_PRINTF2(_L("%S still extant"), &filename);
				SetTestStepResult(EFail);
				}
			}
		
		}
	
	CleanupStack::PopAndDestroy(&session);	
	return TestStepResult();
	}

