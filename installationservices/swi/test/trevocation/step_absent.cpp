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


#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include <f32file.h>
#include "swi/sisregistrysession.h"


CTStepCheckAbsent::CTStepCheckAbsent()
	{
	SetTestStepName(KTStepCheckAbsent);
	}

TVerdict CTStepCheckAbsent::doTestStepL()
	{
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks

	Swi::RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	while (NextPackage())
		{
		TBool installed = session.IsInstalledL(PackageUID());
	
		if (installed)
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
		}
	
	CleanupStack::PopAndDestroy(&session);	
__UHEAP_MARKEND;
	return TestStepResult();
	}

