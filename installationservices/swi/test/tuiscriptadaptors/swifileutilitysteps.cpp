/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "swifileutilitysteps.h"
#include <e32std.h>

CSwiCopyFilesStep::CSwiCopyFilesStep()
	{
	SetTestStepName(KSwiCopyFilesStep);
	}

TVerdict CSwiCopyFilesStep::doTestStepL()
	{
	_LIT(KNumCopy, "numcopy");
	_LIT(KSource, "source");
	_LIT(KDest, "dest");
	TInt numberFilesToCopy;
	
	TPtrC sourceFile;
	TPtrC destFile;
	
	SetTestStepResult(EFail);
	
	RTestUtilSessionSwi testUtil;
	User::LeaveIfError(testUtil.Connect());
	CleanupClosePushL(testUtil);
	
	GetIntFromConfig(ConfigSection(), KNumCopy(), numberFilesToCopy);
	
	INFO_PRINTF2(_L("Config file read.  Number of files to copy: %d"), numberFilesToCopy);
	
	HBufC* lookup = HBufC::NewLC((7 + numberFilesToCopy/10));
	TPtr lookupPtr = lookup->Des();
	
	for (TInt i=0; i<numberFilesToCopy; ++i)
		{
		lookupPtr.Zero();
		lookupPtr.Append(KSource());
		lookupPtr.AppendNum(i);
		
		GetStringFromConfig(ConfigSection(), lookupPtr, sourceFile);
		
		if (!testUtil.FileExistsL(sourceFile))
			{
			ERR_PRINTF2(_L("Source file for copy not found: "), &sourceFile);
			return TestStepResult();	
			}
			
		lookupPtr.Zero();
		lookupPtr.Append(KDest());
		lookupPtr.AppendNum(i);
		
		GetStringFromConfig(ConfigSection(), lookupPtr, destFile);	
		
		INFO_PRINTF3(_L("Copying file from %S to %S ..."), &sourceFile, &destFile);
		User::LeaveIfError(testUtil.Copy(sourceFile, destFile));
		INFO_PRINTF1(_L("...Done."));		
		}
	
	CleanupStack::PopAndDestroy(2);
		
	SetTestStepResult(EPass);	
	return TestStepResult();
	}

CSwiDeleteFilesStep::CSwiDeleteFilesStep()
	{
	SetTestStepName(KSwiDeleteFilesStep);
	}

TVerdict CSwiDeleteFilesStep::doTestStepL()
	{
	_LIT(KNumDelete, "numdelete");
	_LIT(KDelete, "delete");
	TInt numberFilesToDelete;
	
	TPtrC target;
	
	SetTestStepResult(EFail);
	
	RTestUtilSessionSwi testUtil;
	User::LeaveIfError(testUtil.Connect());
	CleanupClosePushL(testUtil);
	
	GetIntFromConfig(ConfigSection(), KNumDelete(), numberFilesToDelete);
	
	INFO_PRINTF2(_L("Config file read.  Number of files to delete: %d"), numberFilesToDelete);
	
	HBufC* lookup = HBufC::NewLC((7 + numberFilesToDelete/10));
	TPtr lookupPtr = lookup->Des();
	
	for (TInt i=0; i<numberFilesToDelete; ++i)
		{
		lookupPtr.Zero();
		lookupPtr.Append(KDelete());
		lookupPtr.AppendNum(i);
		
		GetStringFromConfig(ConfigSection(), lookupPtr, target);
		
		if (!testUtil.FileExistsL(target))
			{
			ERR_PRINTF2(_L("Target file for delete not found: "), &target);
			continue;
			}	
		
		INFO_PRINTF2(_L("Deleting file %S ..."), &target);
		User::LeaveIfError(testUtil.Delete(target));
		INFO_PRINTF1(_L("...Done."));		
		}
	
	CleanupStack::PopAndDestroy(2);
		
	SetTestStepResult(EPass);	
	return TestStepResult();
	}


CSwiFileExistsStep::CSwiFileExistsStep()
    {
    SetTestStepName(KSwiFileExistsStep);
    }


TVerdict CSwiFileExistsStep::doTestStepL()
    {
    SetTestStepResult(EFail);
    
    RTestUtilSessionSwi testUtil;
    User::LeaveIfError(testUtil.Connect());
    CleanupClosePushL(testUtil);
    
    _LIT(KNoOfFiles, "FilesCount");
    _LIT(KFile, "File");
    
    TInt filesCount(0);
    GetIntFromConfig(ConfigSection(), KNoOfFiles(), filesCount);
    
    if(filesCount == 0)
        {
        User::Leave(KErrArgument);
        }
    
    for(TInt i=0; i<filesCount; ++i)
        {
        TBuf<12> file;
        file = KFile;
        GenerateIndexedAttributeNameL(file, filesCount);
        
        TPtrC fileName;
        GetStringFromConfig(ConfigSection(), file, fileName);
        
        HBufC* fileNameBuf = fileName.AllocLC();
        TBool result= testUtil.FileExistsL(*fileNameBuf);
        
        if(!result)
            {
            INFO_PRINTF2(_L("File not found - %S ."), fileNameBuf);
            CleanupStack::PopAndDestroy(fileNameBuf);
            return TestStepResult();
            }
        else
            {
            INFO_PRINTF2(_L("File exists -  %S ."), fileNameBuf);
            CleanupStack::PopAndDestroy(fileNameBuf);
            }
        }
    
    CleanupStack::PopAndDestroy(&testUtil);
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }


void CSwiFileExistsStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
    {
    const TInt MAX_INT_STR_LEN = 8;
    TBuf<MAX_INT_STR_LEN> integerAppendStr;
    integerAppendStr.Format(_L("%d"), aIndex);
    aInitialAttributeName.Append(integerAppendStr);
    }
