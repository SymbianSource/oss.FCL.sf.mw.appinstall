/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file provides implementation for CTSwiConsoleUtilityStep
*
*/


#include <bautils.h>
#include <test/testexecutelog.h>
#include "tswiconsoleutilitystep.h"

_LIT(KOperationType, "Operation");
_LIT(KFileCount, "FileCount");
_LIT(KFiles, "File");

const TInt KFileIdLength = 25;

CTSwiConsoleUtilityStep::CTSwiConsoleUtilityStep()
	{
	SetTestStepName(KTSwiConsoleUtilityStep);
	}

TVerdict CTSwiConsoleUtilityStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	TInt opType;
	GetIntFromConfig(ConfigSection(),KOperationType, opType);
	if(1 != opType)
		{
		return TestStepResult();
		}
	TInt fileCount;
	GetIntFromConfig(ConfigSection(),KFileCount, fileCount);
	
	BaflUtils utils;
	
	TVerdict testResult = EPass;
	
	for(TInt i = 0; i < fileCount; ++i)
		{
		TPtrC fPtr;
		TBuf<KFileIdLength> temp;
		temp.Format(_L("%d"), i+1);
		TBuf<KFileIdLength> fileId(KFiles());
		fileId.Append(temp);
		GetStringFromConfig(ConfigSection(),fileId, fPtr);
		
		TBool fileExists = utils.FileExists(iFs, fPtr);

		if(!fileExists)
			{
			testResult = EFail;
			break;
			}	
		}
	
	SetTestStepResult(testResult);
	
	return TestStepResult();
	}
