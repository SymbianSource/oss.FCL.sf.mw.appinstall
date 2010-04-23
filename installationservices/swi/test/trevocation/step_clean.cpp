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

#include <swi/launcher.h>
#include "tui.h"

CTStepClean::CTStepClean()
	{
	SetTestStepName(KTStepClean);
	}

TVerdict CTStepClean::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
	while (NextPackage())
		{
		TUI ui;	
		Launcher::Uninstall(ui, PackageUID());
		}
__UHEAP_MARKEND;
	return TestStepResult();
	}
