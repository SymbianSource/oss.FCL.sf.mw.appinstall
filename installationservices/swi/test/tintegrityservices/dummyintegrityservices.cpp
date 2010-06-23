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


/**
 @file 
 @test
 @internalTechnology
*/

#include "dummyintegrityservices.h"

//_LIT(KFailNone, "EFailNone");
_LIT(KFailAddingNewFile, "EFailAddingNewFile");
_LIT(KFailRemovingFile, "EFailRemovingFile");
_LIT(KFailAddingTempFile, "EFailAddingTempFile");
_LIT(KFailRestoringFile, "EFailRestoringFile");
_LIT(KFailDeletingFile, "EFailDeletingFile");
_LIT(KFailInstallComplete, "EFailInstallComplete");
_LIT(KFailNewFilesRemoved, "EFailNewFilesRemoved");
_LIT(KFailOldFilesRestored, "EFailOldFilesRestored");
_LIT(KFailTempFilesRemoved, "EFailTempFilesRemoved");
_LIT(KFailBackupFilesRemoved, "EFailBackupFilesRemoved");

_LIT(KFailBeforeJournal, "EBeforeJournal");
_LIT(KFailAfterJournal, "EAfterJournal");
_LIT(KFailBeforeAction, "EBeforeAction");
_LIT(KFailAfterAction, "EAfterAction");

using namespace Swi;
using namespace Swi::Test;

CDummyIntegrityServices* CDummyIntegrityServices::NewL(TInt64 aTransactionID, const TDesC& aPath)
	{
	CDummyIntegrityServices* self = CDummyIntegrityServices::NewLC(aTransactionID, aPath);
	CleanupStack::Pop(self);
	return self;
	}

CDummyIntegrityServices* CDummyIntegrityServices::NewLC(TInt64 aTransactionID, const TDesC& aPath)
	{
	CDummyIntegrityServices* self = new(ELeave) CDummyIntegrityServices(aTransactionID);
	CleanupStack::PushL(self);
	self->ConstructL(aPath);
	return self;
	}

CDummyIntegrityServices::CDummyIntegrityServices(TInt64 aTransactionID) : CIntegrityServices(aTransactionID)
	{
	}

void CDummyIntegrityServices::ConstructL(const TDesC& aPath)
	{
	CIntegrityServices::ConstructL(aPath);
	}

void CDummyIntegrityServices::SetSimulatedFailure(TPtrC16 aFailType, TPtrC16 aFailPosition, TPtrC16 aFailFileName)
	{
	if(aFailType.CompareF(KFailAddingNewFile) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailAddingNewFile;
		}
	else if(aFailType.CompareF(KFailRemovingFile) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailRemovingFile;
		}
	else if(aFailType.CompareF(KFailAddingTempFile) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailAddingTempFile;
		}
	else if(aFailType.CompareF(KFailRestoringFile) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailRestoringFile;
		}
	else if(aFailType.CompareF(KFailDeletingFile) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailDeletingFile;
		}
	else if(aFailType.CompareF(KFailInstallComplete) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailInstallComplete;
		}
	else if(aFailType.CompareF(KFailNewFilesRemoved) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailNewFilesRemoved;
		}
	else if(aFailType.CompareF(KFailOldFilesRestored) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailOldFilesRestored;
		}
	else if(aFailType.CompareF(KFailTempFilesRemoved) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailTempFilesRemoved;
		}
	else if(aFailType.CompareF(KFailBackupFilesRemoved) == 0)
		{
		iFailType = CDummyIntegrityServices::EFailBackupFilesRemoved;
		}
	else
		{
		// default to EFailNone
		iFailType = CDummyIntegrityServices::EFailNone;
		}
	
	if(aFailPosition.CompareF(KFailBeforeJournal) == 0)
		{
		iFailPosition = CDummyIntegrityServices::EBeforeJournal;
		}
	else if(aFailPosition.CompareF(KFailAfterJournal) == 0)
		{
		iFailPosition = CDummyIntegrityServices::EAfterJournal;
		}
	else if(aFailPosition.CompareF(KFailBeforeAction) == 0)
		{
		iFailPosition = CDummyIntegrityServices::EBeforeAction;
		}
	else if(aFailPosition.CompareF(KFailAfterAction) == 0)
		{
		iFailPosition = CDummyIntegrityServices::EAfterAction;
		}
	else
		{
		if(iFailType != CDummyIntegrityServices::EFailNone)
			{
			// scripting error, FailType specified without FailPosition
			User::Leave(KErrCorrupt);
			}
		}
		
	iFailFileName = aFailFileName;
	}

void CDummyIntegrityServices::SimulatePowerFailureL(TFailType aFailType, TFailPosition aFailPosition, const TDesC& aFailFileName)
	{
	if(iFailType == aFailType && iFailPosition == aFailPosition && iFailFileName == aFailFileName)
		{
		User::Leave(KIntegrityServicesSimulatedBatteryFailure);
		}
	}

void CDummyIntegrityServices::CreateNewTestFileL(TDesC& aFileName)
	{
	if (aFileName.Length() > 0 && aFileName[aFileName.Length() - 1] == KPathDelimiter)
		{  // A new test directory is created
		User::LeaveIfError(iFs.MkDir(aFileName));
		}
	else
		{  // A new test file is created
		RFile newFile;
		CleanupClosePushL(newFile);
		SimulatePowerFailureL(EFailAddingNewFile, EBeforeAction, aFileName);
		User::LeaveIfError(newFile.Create(iFs, aFileName, EFileWrite));
		SimulatePowerFailureL(EFailAddingNewFile, EAfterAction, aFileName);
		CleanupStack::PopAndDestroy(&newFile);		
		}
	}

void CDummyIntegrityServices::CreateTempTestFileL(TDesC& aFileName)
	{
	RFile tempFile;
	CleanupClosePushL(tempFile);
	SimulatePowerFailureL(EFailAddingTempFile, EBeforeAction, aFileName);
	User::LeaveIfError(tempFile.Create(iFs, aFileName, EFileWrite));
	SimulatePowerFailureL(EFailAddingTempFile, EAfterAction, aFileName);
	CleanupStack::PopAndDestroy(&tempFile);
	}
