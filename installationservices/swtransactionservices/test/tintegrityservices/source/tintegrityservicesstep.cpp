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
 
#include "tintegrityservicesstep.h"
#include <s32file.h>
#include "journal.h"
#include "journalfile.h"


/////////////////////////////////////////////////////////////////////
// CIntegrityServicesStep
/////////////////////////////////////////////////////////////////////
CIntegrityServicesStepBase::CIntegrityServicesStepBase(CStsTestServer &aStsTestServer):CStsBaseTestStep(aStsTestServer)
	{
	}
void CIntegrityServicesStepBase::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::ImplTestStepPreambleL()"));
	__UHEAP_MARK;

	// Install an active scheduler
	CActiveScheduler* s = new (ELeave) CActiveScheduler;
	s->Install(s);
	
	User::LeaveIfError(iIntegritySession.Connect());

	ReadFailureSettingsL();	
	}

void CIntegrityServicesStepBase::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::ImplTestStepPostambleL()"));
	// Remove the installed active scheduler
	CActiveScheduler* s = CActiveScheduler::Current();
	s->Install(NULL);
	delete s;

	iIntegritySession.Close();

__UHEAP_MARKEND;

	
	}

void CIntegrityServicesStepBase::GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray)
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::GetStringArrayFromConfigL()"));
	_LIT(KKeyFormat, "-%02d");
	HBufC* buf = HBufC::NewLC(aKeyName.Length() + KKeyFormat().Length());
	TPtr ptr(buf->Des());
	INFO_PRINTF2(_L("Parsing attribute: %S"), &aKeyName);

	TInt i = 0;
	TBool cont = ETrue;
	do
		{
		++i;
		ptr = aKeyName;
		ptr.AppendFormat(KKeyFormat(), i);
		TPtrC val;

		cont = GetStringFromConfig(aSectName, ptr, val);
		if (cont)
			{
			User::LeaveIfError(aArray.Append(val));
			}
		} while (cont);

	INFO_PRINTF2(_L("Element count: %d"), i-1);
	CleanupStack::PopAndDestroy(buf);
	}

TBool CIntegrityServicesStepBase::CheckFilesL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::CheckFilesL()"));
	TInt result = ETrue;
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KPresent, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		if(FileExistsL(fileArray[file]))
			{
			INFO_PRINTF2(_L("%S was found"), &fileArray[file]);
			}
		else
			{
			ERR_PRINTF2(_L("%S was not found"), &fileArray[file]);
			result = EFalse;
			}
		}
	fileArray.Reset();
	
	GetStringArrayFromConfigL(ConfigSection(), KAbsent, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		if(FileExistsL(fileArray[file]))
			{
			ERR_PRINTF2(_L("%S was found"), &fileArray[file]);
			result = EFalse;
			}
		else
			{
			INFO_PRINTF2(_L("%S was not found"), &fileArray[file]);
			}
		}
	fileArray.Reset();
	CleanupStack::PopAndDestroy(&fileArray);
	return result;
	}

void CIntegrityServicesStepBase::ReadFailureSettingsL()
	{		
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::ReadFailureSettings()"));
	TPtrC failType;
	if(!GetStringFromConfig(ConfigSection(), KFailType, failType))
		{
		failType.Set(KNullDesC);
		}
		
	TPtrC failPosition;
	if(!GetStringFromConfig(ConfigSection(), KFailPosition, failPosition))
		{
		failPosition.Set(KNullDesC);
		}
		
	TPtrC failFileName;
	if(!GetStringFromConfig(ConfigSection(), KFailFileName, failFileName))
		{
		failFileName.Set(KNullDesC);
		}
	
	iIntegritySession.SetSimulatedFailureL(failType, failPosition, failFileName);
	}
	
void CIntegrityServicesStepBase::doInstallL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::doInstallL()"));
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KRemoveFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("RemovingFile: %S"), &fileArray[file]);
		
		iIntegritySession.RemoveL(fileArray[file]);
		}
	fileArray.Reset();
	
	
	GetStringArrayFromConfigL(ConfigSection(), KAddFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("AddingFile: %S"), &fileArray[file]);
		iIntegritySession.AddL(fileArray[file]);
		iIntegritySession.CreateNewTestFileL(fileArray[file]);
		}
	fileArray.Reset();

	GetStringArrayFromConfigL(ConfigSection(), KTempFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		INFO_PRINTF2(_L("TemporaryFile: %S"), &fileArray[file]);
		iIntegritySession.TemporaryL(fileArray[file]);
		iIntegritySession.CreateTempTestFileL(fileArray[file]);
		}
	fileArray.Reset();

	RArray<TPtrC> operationArray;
	CleanupClosePushL(operationArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KArbitraryOpFile, fileArray);
	GetStringArrayFromConfigL(ConfigSection(), KOperation, operationArray);
	TInt fileCount = fileArray.Count();
	
	if (fileCount != operationArray.Count())
		{
		ERR_PRINTF3(_L("Number of files (%d) does not match number of operations (%d)"), fileCount, operationArray.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		for (TInt file = 0; file < fileCount;file++)
			{
			if (operationArray[file].CompareF(KAddFile) == 0)
				{
				INFO_PRINTF2(_L("AddingFile: %S"), &fileArray[file]);
				iIntegritySession.AddL(fileArray[file]);
				iIntegritySession.CreateNewTestFileL(fileArray[file]);
				}
			else if (operationArray[file].CompareF(KRemoveFile) == 0)
				{
				INFO_PRINTF2(_L("RemovingFile: %S"), &fileArray[file]);
				iIntegritySession.RemoveL(fileArray[file]);
				}
			else if (operationArray[file].CompareF(KTempFile) == 0)
				{
				INFO_PRINTF2(_L("TemporaryFile: %S"), &fileArray[file]);
				iIntegritySession.TemporaryL(fileArray[file]);
				iIntegritySession.CreateTempTestFileL(fileArray[file]);
				}
			else
				{
				ERR_PRINTF3(_L("Operation %S not understood, skipping file %S"), &operationArray[file], &fileArray[file]);
				}
			}
		}
	CleanupStack::PopAndDestroy(2, &fileArray);
	
	iIntegritySession.CommitL();
	}

void CIntegrityServicesStepBase::doRecoverL()
	{
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::doRecoverL()"));
	iIntegritySession.RollBackL(EFalse);
	}

void CIntegrityServicesStepBase::doCleanupL()
	{
	
	INFO_PRINTF1(_L("CIntegrityServicesStepBase::doCleanupL()"));
	RArray<TPtrC> fileArray;
	CleanupClosePushL(fileArray);
	
	GetStringArrayFromConfigL(ConfigSection(), KCleanupFile, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		TRAPD(err, DeleteFileL(fileArray[file]));
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	fileArray.Reset();
	GetStringArrayFromConfigL(ConfigSection(), KCleanupDirectory, fileArray);
	for (TInt file = 0; file < fileArray.Count();file++)
		{
		TRAPD(err, DeleteFileL(fileArray[file]));
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	fileArray.Reset();
	CleanupStack::PopAndDestroy(&fileArray);
	}

/////////////////////////////////////////////////////////////////////
// CInstallStep
/////////////////////////////////////////////////////////////////////
CInstallStep::CInstallStep(CStsTestServer &aStsTestServer):CIntegrityServicesStepBase(aStsTestServer)
	{
	SetTestStepName(KInstall);
	}

void CInstallStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CInstallStep::ImplTestStepL()"));
	TRAPD(err, doInstallL());
	
	if(err == KErrNone || err == KIntegrityServicesSimulatedBatteryFailure)
		{
		if(CheckFilesL())
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		User::Leave(err);
		}
	
	}

/////////////////////////////////////////////////////////////////////
// CRecoverStep
/////////////////////////////////////////////////////////////////////
CRecoverStep::CRecoverStep(CStsTestServer &aStsTestServer):CIntegrityServicesStepBase(aStsTestServer)
	{
	SetTestStepName(KRecover);
	}

void CRecoverStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CRecoverStep::ImplTestStepL()"));
	// begin recovery
	TRAPD(err, doRecoverL());
	
	if(err == KErrNone || err == KIntegrityServicesSimulatedBatteryFailure)
		{
		if(CheckFilesL())
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		User::Leave(err);
		}

	
	}

/////////////////////////////////////////////////////////////////////
// CCleanupStep
/////////////////////////////////////////////////////////////////////
CCleanupStep::CCleanupStep(CStsTestServer &aStsTestServer):CIntegrityServicesStepBase(aStsTestServer)
	{
	SetTestStepName(KCleanup);
	}

void CCleanupStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CCleanupStep::ImplTestStepL()"));
	doCleanupL();
	
	SetTestStepResult(EPass);
	
	}

/////////////////////////////////////////////////////////////////////
// CRecoverStep
/////////////////////////////////////////////////////////////////////
CCheckStep::CCheckStep(CStsTestServer &aStsTestServer):CIntegrityServicesStepBase(aStsTestServer)
	{
	SetTestStepName(KCheck);
	}

void CCheckStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CCheckStep::ImplTestStepL()"));
	// Just check for files
	if(CheckFilesL())
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
		
	
	}


//---------- new unit tests developed as part of the STS framework ----------
using namespace Usif;

//define event names for logging purposes (used in journal file unit tests and journal unit tests	
_LIT(KENone, "ENone");
_LIT(KERemovedFile,"ERemovedFile");
_LIT(KEBackupFile,"EBackupFile");
_LIT(KETempFile,"ETempFile");
_LIT(KEAddedFile,"EAddedFile");
_LIT(KECommitted,"ECommitted");
_LIT(KEBackupFilesRemoved,"EBackupFilesRemoved");
_LIT(KEAddedFilesRemoved,"EAddedFilesRemoved");
_LIT(KERemovedFilesRestored,"ERemovedFilesRestored");
_LIT(KETempFilesRemoved, "ETempFilesRemoved");
	

const TDesC* eventNames[] = {
				&KENone,
				&KERemovedFile,
				&KEBackupFile,
				&KETempFile,
				&KEAddedFile,
				&KECommitted,
				&KEBackupFilesRemoved,
				&KEAddedFilesRemoved,
				&KERemovedFilesRestored,
				&KETempFilesRemoved
				};


const TFileMode defaultJournalFileMode = TFileMode (TFileMode(EFileWrite) | TFileMode(EFileShareAny));

CJournalFileUnitTest::CJournalFileUnitTest(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KJournalFileUnitTest);
	}

CJournalFileUnitTest::~CJournalFileUnitTest()
	{
	delete testJournalFile;
	iFs.Close();
	iLoader.Close();
	}


void CJournalFileUnitTest::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("JournalFileUnitTest Preamble"));
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLoader.Connect());
	testJournalFile = CTestFile::NewL(iFs);
	SetTestStepResult(EPass);
	
	}

void CJournalFileUnitTest::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("JournalFileUnitTest in test step"));
	SetTestStepResult(EPass);
	
	TPtrC journalFilePathInIni = GetTargetFilePathFromConfigL();
	TParsePtrC journalFilePathParse(journalFilePathInIni);
	TDriveUnit journaldrive(journalFilePathParse.Drive());
	HBufC* targetFileName = HBufC::NewLC(KMaxFileName);
	HBufC* backupFileName = HBufC::NewLC(KMaxFileName);
	
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);

	//feed incorrect file to the journalfile (ie: file's on other drive)
	TPtr targetFileNameBasePtr(targetFileName->Des());
	targetFileNameBasePtr.Append(TDriveUnit(journaldrive+1).Name());
	targetFileNameBasePtr.Append(journalFilePathParse.Path());

	TPtr targetFileNamePtr(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummyadd.txt"));

	TPtr targetBackupFileNamePtr(backupFileName->Des());

	TInt err(0);
	
	TInt seekPosition(0);
	TInt seekToEndPosition(0);
	TIntValue intValue = {0};
	TIntValue extraByte = {0xFF};

	TRAP(err, journalFile->AddL(targetFileNamePtr));
	if(err!=KErrBadName)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::AddL() returned incorrect error code. KErrBadName is expected %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}
	targetFileNamePtr.Set(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummytemp.txt"));
	TRAP(err, journalFile->TemporaryL(targetFileNamePtr));
	if(err!=KErrBadName)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::RemoveL() returned incorrect error code. KErrBadName is expected %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	targetFileNamePtr.Set(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummyremove.txt"));
	TRAP(err, journalFile->RemoveL(targetFileNamePtr, targetBackupFileNamePtr));
	if(err!=KErrBadName)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::RemoveL() returned incorrect error code. KErrBadName is expected %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	*targetFileName = KEmptyString;
	targetFileNameBasePtr.Set(targetFileName->Des());
	targetFileNameBasePtr.Append(journalFilePathParse.DriveAndPath());

	CleanupStack::PopAndDestroy(journalFile);
	DeleteFileL(journalFilePathInIni);	
//-------------------------------------------------------------------------------------	
//Steps: 
//	1.) create a CJournalFile object
//	2.) set last event by invoking CJournalFile::EventL() (this sets the iLastEvent as well as writes the event value to the journal file on disk)
//	3.) destroy object 
//	4.) corrupt information by overwriting the event value in the file to requested invalid value
//	5.) recreate CjournalFile object -> this will trigger CJournalFile::ReadEntryL
//  note: the purpose of this testcase to excersise the main switch-case in CJournalFile::ReadEntryL		
//-------------------------------------------------------------------------------------
	
	TIntegrityServicesEvent eventsToSet[] = {ERemovedFile, 
											 EBackupFile, 
											 ETempFile,
											 EAddedFile,
											 ECommitted,
											 EBackupFilesRemoved,
											 EAddedFilesRemoved,
											 //ETempFilesRemoved,
											 ERemovedFilesRestored,
											 };
	TInt numOfInvalidEvents[] = {6, //number of invalid events for ERemovedFile
								 1, //number of invalid events for EBackupFile
								 6, //number of invalid events for ETempFile
								 6, //number of invalid events for EAddedFile
								 7, //number of invalid events for ECommitted
								 9, //number of invalid events for EBackupFilesRemoved
								 6, //number of invalid events for EAddedFilesRemoved
								 //ETempFilesRemoved
								 9 //number of invalid events for ERemovedFilesRestored
	                             };
	TIntegrityServicesEvent invalidEvents[][10] = {{EBackupFile, ECommitted, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for ERemovedFile
			                                        {EBackupFile}, //invalid events for EBackupFile, this is not really invalid value for EBackupFile, actually EBackupFile itself is invalid on its own in the file
													{EBackupFile, ECommitted, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for ETempFile
													{EBackupFile, ECommitted, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for EAddedFile
													{ENone, EBackupFile, ECommitted, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for ECommitted
													{ENone, ERemovedFile, EBackupFile, ETempFile, EAddedFile, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for EBackupFilesRemoved
													{ENone, EBackupFile, EBackupFilesRemoved, EAddedFilesRemoved, ERemovedFilesRestored, ETempFilesRemoved}, //invalid events for EAddedFilesRemoved
													//{}, //invalid events for ETempFilesRemoved
													{ENone, ERemovedFile, EBackupFile, ETempFile, EAddedFile, ECommitted, EBackupFilesRemoved, ETempFilesRemoved, ERemovedFilesRestored} //invalid events for ERemovedFilesRestored
	                                               };
	
	ASSERT(sizeof(eventsToSet)/sizeof(TIntegrityServicesEvent) == sizeof(numOfInvalidEvents)/sizeof(TInt) == sizeof(invalidEvents)/(sizeof(TIntegrityServicesEvent)*10));
	for (TInt i=0; i<sizeof(eventsToSet)/sizeof(TIntegrityServicesEvent); ++i)
	{
	for(TInt j=0; j<numOfInvalidEvents[i]; ++j)
		{
		journalFile=0;
		journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
		journalFile->EventL(invalidEvents[i][j]); //set last event to any of the requested invalid events (also writes it to jounral file)
		CleanupStack::PopAndDestroy(journalFile);
		intValue.int32 = static_cast<TInt32>(eventsToSet[i]);
		seekPosition=0;
		OpenJournalFileL(journalFilePathInIni, defaultJournalFileMode);
		SeekInJournalFileL(ESeekStart, seekPosition);
		WriteIntToJournalFileL(intValue, Mode32Bit); //write the requested event to journal file (overwrites last event set by EventL())
		seekToEndPosition=0;
		SeekInJournalFileL(ESeekEnd, seekToEndPosition);
		WriteIntToJournalFileL(extraByte, Mode8Bit);
		CloseJournalFile();
		//force a Sync and catch the error that should be KErrCorrupt
		journalFile=0;
		TRAPD(err, 
				journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
				CleanupStack::PopAndDestroy(journalFile);
				);
		if(err!=KErrCorrupt)
			{
			ERR_PRINTF4(_L("<font color=red>CJournalFile::SynchL() returned incorrect error code. KErrCorrupt is expected and %d is returned! Conditions: last event set via EventL(): %S  Event written into journal file: %S</font>"), err, eventNames[invalidEvents[i][j]], eventNames[eventsToSet[i]]);
			SetTestStepResult(EFail);
			}
		DeleteFileL(journalFilePathInIni);
		}
	}	
//-------------------------------------------------------------------------------------	
//Steps: 
//	1.) create a journal file using CJournalFile::NewLC
//  2.) add an item via AddL() or TemporaryL()	
//	2.) corrupt disk information in the filename written into the file
//	3.) try to recreate journal object using CJournalFile::NewLC
//	4.) expect KErrCorrupt
//-------------------------------------------------------------------------------------

	targetFileNamePtr.Set(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummyadd_or_temp.txt"));
	for(TInt i=0; i<2; i++)
		{
		//create a journal file and add an item via AddL() or TemporaryL()
		journalFile=0;
		journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
		if(i==0)
			{
			journalFile->AddL(targetFileNamePtr);
			}
		else if(i==1)
			{
			journalFile->TemporaryL(targetFileNamePtr);
			}
		CleanupStack::PopAndDestroy(journalFile);
		//open journal file and corrupt it by overwriting the drive letter in the file's path
		seekPosition=sizeof(TInt32)+1;
		OpenJournalFileL(journalFilePathInIni, defaultJournalFileMode);
		SeekInJournalFileL(ESeekStart, seekPosition);
		intValue = ReadIntFromJournalFileL(Mode8Bit);
		intValue.int8++; //corrupt drive letter (ie: 'c' ==> 'd')
		SeekInJournalFileL(ESeekStart, seekPosition);
		WriteIntToJournalFileL(intValue, Mode8Bit);
		CloseJournalFile();
		//check that CJournalFile recognizes corruption
		journalFile=0;
		TRAP(err, 
				  journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
		          CleanupStack::PopAndDestroy(journalFile);
			 );
		if(err!=KErrCorrupt)
			{
			ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrCorrupt is expected and %d is returned!</font>"), err);
			SetTestStepResult(EFail);
			}
		DeleteFileL(journalFilePathInIni);
		}
//-------------------------------------------------------------------------------------	
//Steps: 
//	1.) create a journal file using CJournalFile::NewLC
//  2.) add an item via RemoveyL()	
//	2.) corrupt disk information in various ways in the file
//	3.) try to recreate journal object using CJournalFile::NewL
//	4.) expect KErrCorrupt
//-------------------------------------------------------------------------------------

	journalFile=0;
	journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
	targetFileNamePtr.Set(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummyremove.txt"));
	journalFile->RemoveL(targetFileNamePtr, targetBackupFileNamePtr);
	CleanupStack::PopAndDestroy(journalFile);

	OpenJournalFileL(journalFilePathInIni, defaultJournalFileMode);
	seekPosition=sizeof(TInt32)+1+targetFileNamePtr.Length();
	SeekInJournalFileL(ESeekStart, seekPosition);
	intValue.int32 = static_cast<TInt32>(EAddedFile); //corrupt operation: originally EBackupFile is recorded here
	WriteIntToJournalFileL(intValue, Mode32Bit);
	
	//check that CJournalFile recognizes corruption
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrCorrupt)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrCorrupt is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	SeekInJournalFileL(ESeekStart, seekPosition);
	intValue.int32 = static_cast<TInt32>(EBackupFile); //restore original record
	WriteIntToJournalFileL(intValue, Mode32Bit);

	//check that journal file is not corrupted
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrNone is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	seekPosition=sizeof(TInt32)+1;
	SeekInJournalFileL(ESeekStart, seekPosition);
	intValue = ReadIntFromJournalFileL(Mode8Bit); //read first file's drive letter
	intValue.int8++; //corrupt drive letter (ie: 'c' ==> 'd')
	SeekInJournalFileL(ESeekStart, seekPosition);
	WriteIntToJournalFileL(intValue, Mode8Bit); //write corrupt drive letter to file

	//check that CJournalFile recognizes corruption
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrCorrupt)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrCorrupt is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	intValue.int8--; //restore original drive letter
	SeekInJournalFileL(ESeekStart, seekPosition);
	WriteIntToJournalFileL(intValue, Mode8Bit); //write original drive letter to file

		
	//check that journal file is not corrupted
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrNone is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}

	seekPosition=sizeof(TInt32)+1+targetFileNamePtr.Length()+sizeof(TInt32)+1;
	SeekInJournalFileL(ESeekStart, seekPosition);
	intValue = ReadIntFromJournalFileL(Mode8Bit); //read second file's drive letter (backup file name)
	intValue.int8++; //corrupt drive letter (ie: 'c' ==> 'd')
	SeekInJournalFileL(ESeekStart, seekPosition);
	WriteIntToJournalFileL(intValue, Mode8Bit); //write corrupt drive letter to file

	
	//check that CJournalFile recognizes corruption
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrCorrupt)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrCorrupt is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}
	
	intValue.int8--; //restore original drive letter
	SeekInJournalFileL(ESeekStart, seekPosition);
	WriteIntToJournalFileL(intValue, Mode8Bit); //write original drive letter to file

	//check that journal file is not corrupted
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrNone is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}
	CloseJournalFile();
	INFO_PRINTF1(_L("Deleting journal file - Line 769"));
	DeleteFileL(journalFilePathInIni);

	
	CreateJournalFileL(journalFilePathInIni, TFileMode(TFileMode(EFileWrite) | TFileMode(EFileShareExclusive)));
	journalFile=0;
	TRAP(err, 
			journalFile = CJournalFile::NewLC(iFs, iLoader, journalFilePathInIni, journaldrive);
    		CleanupStack::PopAndDestroy(journalFile);
		);
	if(err!=KErrInUse )
		{
		ERR_PRINTF2(_L("<font color=red>CJournalFile::NewL() returned incorrect error code. KErrInUse is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}
	CloseJournalFile();
	INFO_PRINTF1(_L("Deleting journal file - Line 783"));
	DeleteFileL(journalFilePathInIni);

	CleanupStack::PopAndDestroy(2, targetFileName);
	}

void CJournalFileUnitTest::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("JournalFileUnitTest Postamble"));
	delete testJournalFile;
	testJournalFile=0;
	iFs.Close();
	iLoader.Close();
	}

void CJournalFileUnitTest::OpenJournalFileL(const TDesC& aFileName, TFileMode aMode)
	{
	testJournalFile->OpenJournalFileL(aFileName, aMode);
	}

void CJournalFileUnitTest::CreateJournalFileL(const TDesC& aFileName, TFileMode aMode)
	{
	testJournalFile->CreateJournalFileL(aFileName, aMode);
	}

void CJournalFileUnitTest::SeekInJournalFileL(TSeek aSeekMode, TInt &aPos)
	{
	testJournalFile->SeekInJournalFileL(aSeekMode, aPos);
	}

TIntValue CJournalFileUnitTest::ReadIntFromJournalFileL(TIntMode mode)
	{
	return testJournalFile->ReadIntFromJournalFileL(mode);
	}

void CJournalFileUnitTest::CloseJournalFile()
	{
	testJournalFile->CloseJournalFile();
	}

void CJournalFileUnitTest::WriteIntToJournalFileL(TIntValue value, TIntMode mode)
	{
	testJournalFile->WriteIntToJournalFileL(value, mode);
	}



/*static*/ CTestFile* CTestFile::NewL(RFs& aFs)
	{
	CTestFile* self = new(ELeave) CTestFile(aFs);
	return self;
	}

CTestFile::CTestFile(RFs& aFs): iFs(aFs)
	{
	
	}

CTestFile::~CTestFile()
	{
	CloseJournalFile();
	}

void CTestFile::OpenJournalFileL(const TDesC& aFileName, TFileMode aMode)
	{
	TInt err = journalFile.Open(iFs, aFileName, aMode);
	User::LeaveIfError(err);
	}

void CTestFile::CreateJournalFileL(const TDesC& aFileName, TFileMode aMode)
	{
	TInt err = iFs.MkDirAll(aFileName);
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	err = journalFile.Create(iFs, aFileName, aMode);
	User::LeaveIfError(err);
	}

void CTestFile::CloseJournalFile()
	{
	journalFile.Close();
	}

void CTestFile::WriteIntToJournalFileL(TIntValue value, TIntMode mode)
	{
	HBufC8* fileIOBuffer = HBufC8::NewLC(10);
	TPtr8  fileIOBufferPtr(fileIOBuffer->Des());
	TPckgC<TInt32> pckg(value.int32);
	
	switch(mode)
		{
		case Mode8Bit:
		case Mode16Bit:
		case Mode32Bit:
			fileIOBufferPtr.Append(pckg);
			User::LeaveIfError(journalFile.Write(fileIOBufferPtr, static_cast<TInt>(mode)));
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}
	
	CleanupStack::PopAndDestroy(fileIOBuffer);
	}

TIntValue CTestFile::ReadIntFromJournalFileL(TIntMode mode)
	{
	HBufC8* fileIOBuffer = HBufC8::NewLC(10);
	TPtr8  fileIOBufferPtr(fileIOBuffer->Des());
	User::LeaveIfError(journalFile.Read(fileIOBufferPtr, static_cast<TInt>(mode)));
	TIntValue ret = {0};
	
	switch(mode)
		{
		case Mode8Bit: 
		case Mode16Bit:
		case Mode32Bit:
			{
			for(TInt i=0; i<static_cast<TInt>(mode); ++i)
				{
				ret.int32 |=  TInt32((*fileIOBuffer)[i]) << (8*i);
				}
			break;
			}
		default:
			User::Leave(KErrArgument);
			break;
		}
	CleanupStack::PopAndDestroy(fileIOBuffer);
	return ret;
	}
	
void CTestFile::SeekInJournalFileL(TSeek aSeekMode, TInt &aPos)
	{
	User::LeaveIfError(journalFile.Seek(aSeekMode, aPos));
	}



//------------------------------------------------------------------------------------------------------------

CJournalFileUnitTestScenarioBase::CJournalFileUnitTestScenarioBase(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer), iCleanupNeeded(ETrue)
	{
	}

CJournalFileUnitTestScenarioBase::~CJournalFileUnitTestScenarioBase()
	{
	iFs.Close();
	iLoader.Close();
	delete iTargetFileName;
	delete iBackupFileName;
	}


void CJournalFileUnitTestScenarioBase::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestScenarioBase Preamble"));
	iJournalFileName.Set(GetTargetFilePathFromConfigL());
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLoader.Connect());
	iTargetFileName = HBufC::NewL(KMaxFileName);
	iBackupFileName = HBufC::NewL(KMaxFileName);
	TParsePtrC journalFilePathParse(iJournalFileName);
	TPtr ptr(iTargetFileName->Des());
	ptr.Append(journalFilePathParse.DriveAndPath());
	ptr.Append(_L("journalfile_oom_test_dummyfile.txt"));
	iJournaldrive = journalFilePathParse.Drive();
	SetTestStepResult(EPass);
	
	}

void CJournalFileUnitTestScenarioBase::ImplTestStepL()
	{
	CleanupStack::PushL(TCleanupItem(&CJournalFileUnitTestScenarioBase::InTestCleanup, this));
	ImplTestCaseScenarioL();
	CleanupStack::PopAndDestroy(this);
	}

void CJournalFileUnitTestScenarioBase::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestScenarioBase Postamble"));
	
	delete iTargetFileName; 
	iTargetFileName=0;
	delete iBackupFileName; 
	iBackupFileName=0;
	iFs.Close();
	iLoader.Close();
	TRAP_IGNORE(DeleteFileL(iJournalFileName));
	}

/*static*/void CJournalFileUnitTestScenarioBase::InTestCleanup(TAny* aObject)
	{
	CJournalFileUnitTestScenarioBase* object=static_cast<CJournalFileUnitTestScenarioBase*>(aObject);
	object->INFO_PRINTF1(_L("CJournalFileUnitTestScenarioBase::InTestCleanup()"));
	if(object->iCleanupNeeded)
		{
		TRAP_IGNORE(object->DeleteFileL(object->iJournalFileName));
		}
	}

//------------------------------------------------------------------------------------------------------------
CJournalFileUnitTestAdd::CJournalFileUnitTestAdd(CStsTestServer &aStsTestServer): CJournalFileUnitTestScenarioBase(aStsTestServer)
	{
	SetTestStepName(KJournalFileUnitTestAdd);
	}
void CJournalFileUnitTestAdd::ImplTestCaseScenarioL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestAdd in test step"));
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, iJournalFileName, iJournaldrive);
	journalFile->AddL(*iTargetFileName);
	CleanupStack::PopAndDestroy(journalFile);
	}
//------------------------------------------------------------------------------------------------------------
CJournalFileUnitTestTemp::CJournalFileUnitTestTemp(CStsTestServer &aStsTestServer): CJournalFileUnitTestScenarioBase(aStsTestServer)
	{
	SetTestStepName(KJournalFileUnitTestTemp);
	}

void CJournalFileUnitTestTemp::ImplTestCaseScenarioL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestTemp in test step"));
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, iJournalFileName, iJournaldrive);
	journalFile->TemporaryL(*iTargetFileName);
	CleanupStack::PopAndDestroy(journalFile);
	}

//------------------------------------------------------------------------------------------------------------
CJournalFileUnitTestRemove::CJournalFileUnitTestRemove(CStsTestServer &aStsTestServer): CJournalFileUnitTestScenarioBase(aStsTestServer)
	{
	SetTestStepName(KJournalFileUnitTestRemove);
	}

void CJournalFileUnitTestRemove::ImplTestCaseScenarioL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestRemove in test step"));
	TPtr backupFileNamePtr(iBackupFileName->Des());
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, iJournalFileName, iJournaldrive);
	journalFile->RemoveL(*iTargetFileName, backupFileNamePtr);
	CleanupStack::PopAndDestroy(journalFile);
	}
//------------------------------------------------------------------------------------------------------------
CJournalFileUnitTestRead::CJournalFileUnitTestRead(CStsTestServer &aStsTestServer): CJournalFileUnitTestScenarioBase(aStsTestServer)
	{
	iCleanupNeeded=EFalse;
	SetTestStepName(KJournalFileUnitTestRead);
	}

void CJournalFileUnitTestRead::ImplTestCaseScenarioL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestRead in test step"));
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, iJournalFileName, iJournaldrive);
	CleanupStack::PopAndDestroy(journalFile);
	}
//------------------------------------------------------------------------------------------------------------
CJournalFileUnitTestEvent::CJournalFileUnitTestEvent(CStsTestServer &aStsTestServer): CJournalFileUnitTestScenarioBase(aStsTestServer)
	{
	}

void CJournalFileUnitTestEvent::ImplTestCaseScenarioL()
	{
	INFO_PRINTF1(_L("CJournalFileUnitTestEvent in test step"));
	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, iJournalFileName, iJournaldrive);
	journalFile->EventL(ECommitted);
	CleanupStack::PopAndDestroy(journalFile);
	}
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
CJournalUnitTest::CJournalUnitTest(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KJournalUnitTest);
	}

CJournalUnitTest::~CJournalUnitTest()
	{
	delete iTestFileInterface;
	iFs.Close();
	iLoader.Close();
	}

void CJournalUnitTest::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("JournalFileUnitTest Preamble"));
	iJournalPathInIni.Set(GetTargetFilePathFromConfigL());
	iTrID = GetTransactionIdFromConfigL();
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLoader.Connect());
	iTestFileInterface = CTestFile::NewL(iFs);
	SetTestStepResult(EPass);
	
	}

void CJournalUnitTest::ImplTestStepL()
	{
	INFO_PRINTF1(_L("JournalUnitTest in test step"));
	SetTestStepResult(EPass);
	TestCase1L();
	}

void CJournalUnitTest::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("JournalFileUnitTest Postamble"));
	delete iTestFileInterface;
	iTestFileInterface=0;
	iFs.Close();
	iLoader.Close();
	}

void CJournalUnitTest::TestCase1L()
	{
	INFO_PRINTF1(_L("JournalFileUnitTest Test Case1"));

//-------------------------------------------------------------------------------------	
//Steps: 
//	1.) create a corrupt journal file on C drive (by writing invalid event into the file /eg: with a value of EAddedFile+1000/) 
//	2.) Create a journal
//	3.) Add a file to the journal on C drive by invoking CJounral::AddL() 
//	4.) this will create a new journal file object representing journal activity on drive C, however due to the corrupt file being already on the C drive CJournalFile::NewLC will throw KErrCorrupt
//  5.) expect KErrCorrupt	
//  note: the purpose of this testcase to excersise the conditional branches in CJournal::PrepareToWriteL		
//-------------------------------------------------------------------------------------
	
	TInt err=0;
	TDriveUnit driveUnitC(KDriveLetterC);
	TDriveUnit driveUnitE(KDriveLetterE);
	
	
	HBufC* journalFileName = HBufC::NewLC(KMaxFileName);
	HBufC* targetFileName = HBufC::NewLC(KMaxFileName);
	
	TPtr journalFileNamePtr(journalFileName->Des());
	journalFileNamePtr.Append(driveUnitC.Name());
	CJournal::CreateJournalFileNameL(iTrID, iJournalPathInIni, journalFileNamePtr); 
	
	//create file path base (eg: drive:\path\)
	TPtr targetFileNameBasePtr(targetFileName->Des());
	targetFileNameBasePtr.Append(driveUnitC.Name());
	targetFileNameBasePtr.Append(iJournalPathInIni);
	
	//open up the file via the test file interface (for writing corrupt data)
	iTestFileInterface->CreateJournalFileL(journalFileNamePtr, defaultJournalFileMode);
	TInt seekPosition=0;
	TIntValue intValue = {0};
	intValue.int32=static_cast<TInt32>(EAddedFile)+1000; //corrupt data in file (eg: write operation value to an invalid value)
	//corrupt data in journal file
	iTestFileInterface->SeekInJournalFileL(ESeekStart, seekPosition);
	iTestFileInterface->WriteIntToJournalFileL(intValue, Mode32Bit);
	iTestFileInterface->CloseJournalFile();

	TPtr targetFileNamePtr(targetFileNameBasePtr);
	targetFileNamePtr.Append(_L("dummyadd.txt"));
	
	//create a journal and add a file on C drive: that will force CJournalFile to read in and process the  
	//previously created corrupt file. As the file contains corrupt information CJournalFile construction is expected to fail with KErrCorrupt 
	CJournal* journal = CJournal::NewLC(iFs, iLoader, iTrID, iJournalPathInIni);
	TRAP(err, journal->AddL(targetFileNamePtr));
	if(err!=KErrCorrupt)
		{
		ERR_PRINTF2(_L("<font color=red>CJournal::AddL() returned incorrect error code. KErrCorrupt is expected and %d is returned!</font>"), err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(journal); journal=0;

	//clean up: delete created journal file and drive file (.drv)
	DeleteFileL(*journalFileName);
	//reuse targetFileName buffer to create .drv filename to delete that file
	*targetFileName=KEmptyString;
	targetFileNamePtr.Set(targetFileName->Des());
	CJournal::CreateDrvFileNameL(iTrID, iJournalPathInIni, targetFileNamePtr);
	TRAP_IGNORE(DeleteFileL(targetFileNamePtr)); //clean up .drv file in case it's been created but avoid causing leave in case it hasn't 
	
	CleanupStack::PopAndDestroy(targetFileName);
	CleanupStack::PopAndDestroy(journalFileName);
	}

//------------------------------------------------------------------------------------------------------------


CJournalUnitTestScenarioBase::CJournalUnitTestScenarioBase(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer) 
	{
	iDriveUnitC = _L("c"); 
	iDriveUnitE = _L("e");
	}

CJournalUnitTestScenarioBase::~CJournalUnitTestScenarioBase()
	{
	iFs.Close();
	iLoader.Close();
	delete iTargetFileName;
	delete iJournalFileName;
	delete iDrvFileName;
	delete iBackupFileName;
	}

void CJournalUnitTestScenarioBase::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CJournalUnitTestScenarioBase::ImplTestStepPreambleL()"));
	iJournalPathInIni.Set(GetTargetFilePathFromConfigL());
	iTrID = GetTransactionIdFromConfigL();
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLoader.Connect());
	iTargetFileName = HBufC::NewL(KMaxFileName);
	iJournalFileName = HBufC::NewL(KMaxFileName);
	iDrvFileName = HBufC::NewL(KMaxFileName);
	iBackupFileName = HBufC::NewL(KMaxFileName);

	TPtr ptr(iJournalFileName->Des());
	ptr.Append(iDriveUnitC.Name());
	ptr.Append(iJournalPathInIni);
	ptr.AppendNumUC(iTrID, EHex);
	ptr.Append(KKournalFileExt);
	
	ptr.Set(iTargetFileName->Des());
	ptr.Append(iDriveUnitC.Name());
	ptr.Append(iJournalPathInIni);

	ptr.Set(iDrvFileName->Des());
	CJournal::CreateDrvFileNameL(iTrID, iJournalPathInIni, ptr);

	SetTestStepResult(EPass);
	}

void CJournalUnitTestScenarioBase::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CJournalUnitTestScenarioBase::ImplTestStepL()"));

	TPtr basePathPtr(iTargetFileName->Des());
	TPtr fileNamePtr(basePathPtr);
	TCleanupItem inTestCleanupItem(&CJournalUnitTestScenarioBase::InTestCleanUp, this);
	CleanupStack::PushL(inTestCleanupItem);

	CJournal* journal = CJournal::NewLC(iFs, iLoader, iTrID, iJournalPathInIni);
	
	*iBackupFileName=KEmptyString;
	TPtr backupFileNamePtr(iBackupFileName->Des());
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("journal_oom_test_dummyremovefile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	journal->RemoveL(fileNamePtr, backupFileNamePtr);
	*iBackupFileName=KEmptyString;
	backupFileNamePtr.Set(iBackupFileName->Des());
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	journal->RemoveL(fileNamePtr, backupFileNamePtr);
	
	  
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("journal_oom_test_dummytempfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	journal->TemporaryL(fileNamePtr);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	journal->TemporaryL(fileNamePtr);
	
	
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("journal_oom_test_dummyaddfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	journal->AddL(fileNamePtr);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	journal->AddL(fileNamePtr);
	  
	ScenarioEndL(journal);
	CleanupStack::PopAndDestroy(journal);
	CleanupStack::PopAndDestroy(this);
	}

void CJournalUnitTestScenarioBase::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CJournalUnitTestScenarioBase::ImplTestStepPostambleL()"));
	delete iTargetFileName;
	iTargetFileName=0;
	delete iJournalFileName;
	iJournalFileName=0;
	delete iDrvFileName;
	iDrvFileName=0;
	delete iBackupFileName;
	iBackupFileName=0;
	iFs.Close();
	iLoader.Close();
	}

/*static*/ void CJournalUnitTestScenarioBase::InTestCleanUp(TAny* aObject)
	{
	CJournalUnitTestScenarioBase* object=static_cast<CJournalUnitTestScenarioBase*>(aObject);
	object->INFO_PRINTF1(_L("CJournalUnitTestScenarioBase::InTestCleanUp()"));

	TPtr ptr(object->iJournalFileName->Des());
	ptr.Replace(0, 2, object->iDriveUnitC.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete journal file on C drive
	ptr.Replace(0, 2, object->iDriveUnitE.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete journal file on E drive
	TRAP_IGNORE(object->DeleteFileL(*(object->iDrvFileName))); //delete drv file on system drive
	}

//------------------------------------------------------------------------------------------------------------

CJournalUnitTestInstall::CJournalUnitTestInstall(CStsTestServer &aStsTestServer): CJournalUnitTestScenarioBase(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KJournalUnitTestInstall);
	}

void CJournalUnitTestInstall::ScenarioEndL(CJournal* aJournal)
	{
	aJournal->CommitL();
	}

//------------------------------------------------------------------------------------------------------------

CJournalUnitTestRollback::CJournalUnitTestRollback(CStsTestServer &aStsTestServer): CJournalUnitTestScenarioBase(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KJournalUnitTestRollback);
	}

void CJournalUnitTestRollback::ScenarioEndL(CJournal* aJournal)
	{
	aJournal->RollBackL();
	}

//------------------------------------------------------------------------------------------------------------


CIntegrityServicesTestScenarioBase::CIntegrityServicesTestScenarioBase(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer) 
	{
	iDriveUnitC = _L("c"); 
	iDriveUnitE = _L("e");
	}

CIntegrityServicesTestScenarioBase::~CIntegrityServicesTestScenarioBase()
	{
	iFs.Close();
	iLoader.Close();
	delete iTargetFileName;
	delete iJournalFileName;
	delete iDrvFileName;
	delete iBackupFileName;
	delete iTransactionBackupDirectoryPath;
	}

void CIntegrityServicesTestScenarioBase::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("IntegrityServicesTestOOM Preamble"));
	iTargetPathInIni.Set(GetTargetFilePathFromConfigL());
	iTrID = GetTransactionIdFromConfigL();
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iLoader.Connect());
	iTargetFileName = HBufC::NewL(KMaxFileName);
	iJournalFileName = HBufC::NewL(KMaxFileName);
	iDrvFileName = HBufC::NewL(KMaxFileName);
	iBackupFileName = HBufC::NewL(KMaxFileName);
	iTransactionBackupDirectoryPath = HBufC::NewL(KMaxFileName);

	TPtr ptr(iJournalFileName->Des());
	ptr.Append(iDriveUnitC.Name());
	CJournal::CreateJournalFileNameL(iTrID, KTransactionPath, ptr);

	ptr.Set(iDrvFileName->Des());
	CJournal::CreateDrvFileNameL(iTrID, KTransactionPath, ptr);
	
	TParsePtrC parse(*iDrvFileName);
	ptr.Set(iTransactionBackupDirectoryPath->Des()); //this is the directory that holds the backup files for the transaction
	ptr.Append(parse.DriveAndPath());
	ptr.Append(parse.Name());
	
	ptr.Set(iTargetFileName->Des());
	ptr.Append(iDriveUnitC.Name());
	ptr.Append(iTargetPathInIni);
	NormalizeDirectoryName(ptr); //removes trailing slash

	SetTestStepResult(EPass);
	}

void CIntegrityServicesTestScenarioBase::ImplTestStepL()
	{
	TPtr basePathPtr(iTargetFileName->Des());
	TPtr fileNamePtr(basePathPtr);
	RFile newFile;
	TUint fileMode(EFileShareExclusive | EFileWrite);
	
	TCleanupItem inTestCleanupItem(&CIntegrityServicesTestScenarioBase::InTestCleanUp, this);
	CleanupStack::PushL(inTestCleanupItem);

	CIntegrityServices* integrityServices = CIntegrityServices::NewLC(iTrID);
	
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummyremovefile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	integrityServices->RemoveL(fileNamePtr);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	integrityServices->RemoveL(fileNamePtr);
	  
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummyregistertempfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	integrityServices->RegisterTemporaryL(fileNamePtr);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	integrityServices->RegisterTemporaryL(fileNamePtr);
	
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummycreatetempfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	CleanupClosePushL(newFile);
	integrityServices->CreateTemporaryL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	CleanupClosePushL(newFile);
	integrityServices->CreateTemporaryL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummyaddfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	integrityServices->RegisterNewL(fileNamePtr);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	integrityServices->RegisterNewL(fileNamePtr);
	 
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummycreatenewfile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	CleanupClosePushL(newFile);
	integrityServices->CreateNewL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	CleanupClosePushL(newFile);
	integrityServices->CreateNewL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	
	fileNamePtr.Set(basePathPtr);
	fileNamePtr.Append(_L("\\integrityservices_oom_test_dummyoverwritefile.txt"));
	basePathPtr.Replace(0, 2, iDriveUnitC.Name());
	CleanupClosePushL(newFile);
	integrityServices->OverwriteL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	basePathPtr.Replace(0, 2, iDriveUnitE.Name());
	CleanupClosePushL(newFile);
	integrityServices->OverwriteL(fileNamePtr, newFile, fileMode);
	CleanupStack::PopAndDestroy(&newFile);
	  
	ScenarioEndL(integrityServices);
	CleanupStack::PopAndDestroy(integrityServices);
	CleanupStack::PopAndDestroy(this);
	}

void CIntegrityServicesTestScenarioBase::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("IntegrityServicesTestOOM Postamble"));
	delete iTargetFileName;
	iTargetFileName=0;
	delete iJournalFileName;
	iJournalFileName=0;
	delete iDrvFileName;
	iDrvFileName=0;
	delete iBackupFileName;
	iBackupFileName=0;
	delete iTransactionBackupDirectoryPath;
	iTransactionBackupDirectoryPath=0;
	iFs.Close();
	iLoader.Close();
	}

/*static*/void CIntegrityServicesTestScenarioBase::InTestCleanUp(TAny* aObject)
	{
	CIntegrityServicesTestScenarioBase* object=static_cast<CIntegrityServicesTestScenarioBase*>(aObject);
	//first clean up transaction related files
	TPtr ptr(object->iJournalFileName->Des());
	ptr.Replace(0, 2, object->iDriveUnitC.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete journal file on C drive
	ptr.Replace(0, 2, object->iDriveUnitE.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete journal file on E drive
	TRAP_IGNORE(object->DeleteFileL(*(object->iDrvFileName))); //delete drv file on system drive
	ptr.Set(object->iTransactionBackupDirectoryPath->Des());
	ptr.Replace(0, 2, object->iDriveUnitC.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete backup directory on C drive
	ptr.Replace(0, 2, object->iDriveUnitE.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete backup directory on E drive
	
	//then clean up all the added/created/etc... files
	ptr.Set(object->iTargetFileName->Des());
	ptr.Replace(0, 2, object->iDriveUnitC.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete target dir on C drive
	ptr.Replace(0, 2, object->iDriveUnitE.Name());
	TRAP_IGNORE(object->DeleteFileL(ptr)); //delete target dir on E drive
	
	}

void CIntegrityServicesTestScenarioBase::NormalizeDirectoryName(TDes& aFileName)
{
	TInt lastCharPos = aFileName.Length() - 1;
	if ( lastCharPos >= 0 && aFileName[lastCharPos] == KPathDelimiter &&
		 aFileName.Locate(KPathDelimiter) != lastCharPos) // Take care not to remove slash from "c:\" and the like
		{
		aFileName.Delete(lastCharPos, 1);
		}			
}

//------------------------------------------------------------------------------------------------------------

CIntegrityServicesTestInstall::CIntegrityServicesTestInstall(CStsTestServer &aStsTestServer): CIntegrityServicesTestScenarioBase(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KIntegrityservicesUnitTestInstall);
	}

void CIntegrityServicesTestInstall::ScenarioEndL(Usif::CIntegrityServices* aIntegrServ)
	{
	aIntegrServ->CommitL();
	}

//------------------------------------------------------------------------------------------------------------

CIntegrityServicesTestRollback::CIntegrityServicesTestRollback(CStsTestServer &aStsTestServer): CIntegrityServicesTestScenarioBase(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KIntegrityservicesUnitTestRollback);
	}

void CIntegrityServicesTestRollback::ScenarioEndL(Usif::CIntegrityServices* aIntegrServ)
	{
	aIntegrServ->RollBackL();
	}
