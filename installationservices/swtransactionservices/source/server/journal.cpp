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
* CJournal implementation
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "journal.h"
#include "journalfile.h"
#include "operationfunctions.h"
#include "usiflog.h"
#include <f32file.h>

using namespace Usif;

CJournal* CJournal::NewL(RFs& aFs, RLoader& aLoader, TStsTransactionId aTransactionID, const TDesC& aPath)
	{
	CJournal* self = CJournal::NewLC(aFs, aLoader, aTransactionID, aPath);
	CleanupStack::Pop(self);
	return self;
	}

CJournal* CJournal::NewLC(RFs& aFs, RLoader& aLoader, TStsTransactionId aTransactionID, const TDesC& aPath)
	{
	CJournal* self = new(ELeave) CJournal(aFs, aLoader);
	CleanupStack::PushL(self);
	self->ConstructL(aTransactionID, aPath);
	return self;
	}

CJournal::~CJournal()
	{
	iCompletedDrives.Reset();
	iAllDrives.Reset();
	iJournalFiles.ResetAndDestroy();
	}

CJournal::CJournal(RFs& aFs, RLoader& aLoader) : iFs(aFs), iLoader(aLoader)
	{
	}

void CJournal::ConstructL(TStsTransactionId aTransactionID, const TDesC& aPath)
	{
	// construct the generic journal filename (does not include a drive)
	CJournal::CreateJournalFileNameL(aTransactionID, aPath, iJournalFileName);
	// construct the filename for the drives journal (located on the system drive)
	CJournal::CreateDrvFileNameL(aTransactionID, aPath, iDriveArrayFileName);	
	InitJournalsL();
	}

void CJournal::InitJournalsL()
	{	
	RefreshDrivesArrayL();
	
	// attempt to read the journals from all drives
	
	TInt drivesCount(iAllDrives.Count());
	RBuf journal;
	journal.CreateL(KMaxFileName);
	CleanupClosePushL(journal);
	for (TInt i = 0; i < drivesCount; ++i)
		{
		TDriveUnit drive(iAllDrives[i]);
		journal = drive.Name();
		journal.Append(iJournalFileName);

		// do not load completed drives
		if (iCompletedDrives.Find(drive) != KErrNotFound)
			{
			continue;
			}
		
		CJournalFile* journalFile = NULL;
		TRAPD(err, journalFile = CJournalFile::NewL(iFs, iLoader, journal, iAllDrives[i]));
		
		// ignore error'd drives. These will either not be rolled back,
		// or the user will find out later we can't write or commit to them.
		
		if (err == KErrNone)
			{
			CleanupStack::PushL(journalFile);
			iJournalFiles.AppendL(journalFile);
			CleanupStack::Pop(journalFile);
			}
		}
	CleanupStack::PopAndDestroy(&journal);
	}

TInt CJournal::FindJournalFileIndexL(TInt aDrive) const
	{
	for(TInt index = 0; index < iJournalFiles.Count(); ++index)
		if (iJournalFiles[index]->Drive() == aDrive)
			return index;
		
	return KErrNotFound;
	}

void CJournal::DeleteJournalFilesL()
	{
	// delete all journal files
	for(TInt drive = 0; drive < iJournalFiles.Count(); drive++)
		{		
		DeleteJournalFileL(iJournalFiles[drive]->Drive());		
		}	
	iJournalFiles.ResetAndDestroy();
	
	// delete the drive array only after all journals have been completed
	// (committed or rolled back)
	if (iAllDrives.Count() == iCompletedDrives.Count())
		{
		DeleteDrivesFileL();
		}
	}

void VerifyDirectoryDeletionErrorL(TInt err)
	{
	if(err != KErrNone && err != KErrNotFound && err != KErrPathNotFound && err != KErrInUse && err != KErrAccessDenied)
		{
		User::Leave(err);
		}	
	}


void CJournal::DeleteJournalFileL(TInt aDrive, TBool aRecordAllRollbackEvents /* = ETrue */)
	{	
	TInt index = FindJournalFileIndexL(aDrive);
	iJournalFiles[index]->Close();
	TDriveUnit journalDrive(aDrive);
	RBuf journal;
	journal.CreateL(journalDrive.Name(), KMaxFileName);
	CleanupClosePushL(journal);
	journal.Append(iJournalFileName);

	User::LeaveIfError(iFs.Delete(journal));

	// record that we have completed this drive
	if (aRecordAllRollbackEvents)
		UpdateDrivesFileL(aDrive);
		
	iCompletedDrives.InsertInOrder(aDrive);
	
	VerifyDirectoryDeletionErrorL(iFs.RmDir(journal));
	
	CleanupStack::PopAndDestroy(&journal);
	}
	
void CJournal::DeleteDrivesFileL()
	{
	iCompletedDrives.Reset();
	TInt err = KErrNone;
	
	err = iFs.Delete(iDriveArrayFileName);
			
	if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
		{
		User::Leave(err);
		}
    
    // try removing the journal path
	TParse directory;
	User::LeaveIfError(directory.SetNoWild(iDriveArrayFileName, NULL, NULL));
	while(!directory.IsRoot())
		{
		// try to remove this directory
		TInt err = iFs.RmDir(directory.DriveAndPath());
		if(err != KErrNone)
			{
			VerifyDirectoryDeletionErrorL(err);
			break;
			}
		User::LeaveIfError(directory.PopDir());
		}
	}

// This function parses all the drives registered in the main drive file for this transaction.
// This function also checks which drives have been completed.
// As a result, two main drive sets: iCompletedDrives and iAllDrives are populated.
void CJournal::RefreshDrivesArrayL()
	{
	// clear existing journal drive arrays prior to reloading them from file
	iCompletedDrives.Reset();
	iAllDrives.Reset();
	
	RFileReadStream journalStream;
	TInt err = journalStream.Open(iFs, iDriveArrayFileName, EFileStream);
	if (err == KErrNotFound || err == KErrPathNotFound)
		return;
	User::LeaveIfError(err);

	CleanupClosePushL(journalStream);
	while(ETrue)
		{
		// read the next entry in the drives' file
		TInt drive = 0;
		TRAP(err, drive = journalStream.ReadInt32L());
		if (err == KErrEof)
			break;
		
		User::LeaveIfError(err);

		if(iAllDrives.Find(drive) == KErrNotFound)
			{
			// first instance indicates this drive was part of this
			// transaction
			iAllDrives.InsertInOrder(drive);
			}
		else
			{
			// second instance indicates this drive has been committed
			// or rolled back
			iCompletedDrives.InsertInOrder(drive);
			}
		}
	CleanupStack::PopAndDestroy(&journalStream);
	}

void CJournal::UpdateDrivesFileL(TInt aDrive)
	{
	RFile file;
	CleanupClosePushL(file);
	// try opening the file if it already exists
	TInt err = file.Open(iFs, iDriveArrayFileName, EFileWrite);
	if (err != KErrNone)
		{
		if (err == KErrNotFound || err == KErrPathNotFound)
			{
			err = iFs.MkDirAll(iDriveArrayFileName);
			if(err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}
			// journal does not exist, try creating one
			User::LeaveIfError(file.Create(iFs, iDriveArrayFileName, EFileWrite));
			}
		else
			{
			User::Leave(err);
			}
		}
	
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	
	// attach to end of file for writing
	RFileWriteStream stream;
	stream.Attach(file, fileSize);	
	CleanupStack::Pop(&file); // file ownership
	CleanupClosePushL(stream);// transfered to stream
	stream.WriteInt32L(aDrive);
	CleanupStack::PopAndDestroy(&stream);
	}

void CJournal::StartCommitL()
	{
	DEBUG_PRINTF3(_L("CJournal::StartCommitL() - iJournalFileName %S   iDriveArrayFileName %S"), &iJournalFileName, &iDriveArrayFileName);							
	// To commit, all drives must exist. No exceptions.
	if (iJournalFiles.Count() != iAllDrives.Count())
		{
		User::Leave(KErrNotReady);
		}
		
	// none of the drives must yet be completed...
	if (iCompletedDrives.Count() != 0)
		{
		User::Leave(KErrNotSupported);
		}
		
	// synch up all the drives
	TInt drivesCount(iJournalFiles.Count());
	for (TInt i = 0; i < drivesCount; ++i)
		{
 		//Check if Journal drives are present.
 		TDriveInfo info;
 		if (iFs.Drive(info, iJournalFiles[i]->Drive())!=KErrNone || info.iType==EMediaNotPresent)
 			{
 			User::Leave(KErrNotReady);	
 			}
		}		
	}

void CJournal::FinishCommitL()
	{
	DEBUG_PRINTF3(_L("CJournal::FinishCommitL() - iJournalFileName %S   iDriveArrayFileName %S"), &iJournalFileName, &iDriveArrayFileName);							
	DeleteJournalFilesL();
	
	// return the journal to a state where it can be used again,
	// in the insane event that someone wants to...
	// (I'm looking at you sisregistry)
	iCompletedDrives.Reset();
	iAllDrives.Reset();
	}

void CJournal::FinishRollbackL(TInt aDrive, TBool aRecordAllRollbackEvents /* = ETrue */)
	{
	DEBUG_PRINTF4(_L("CJournal::FinishRollbackL() - iJournalFileName %S   iDriveArrayFileName %S   aDrive %d"), &iJournalFileName, &iDriveArrayFileName, aDrive);
	DeleteJournalFileL(aDrive, aRecordAllRollbackEvents);
	
	if (iCompletedDrives.Count() == iAllDrives.Count())
		{
		DeleteDrivesFileL();
		}
	}

// This function verifies whether a journal exists for this drive in this transaction.
// If it doesn't, then the journal file is created and added to iJournalFiles
TInt CJournal::PrepareToWriteL(TInt aDrive)
	{	
	TInt index = FindJournalFileIndexL(aDrive);
	if (index >= 0) // The journal already exists
		{
		return index;
		}
	
	__ASSERT_ALWAYS(index == KErrNotFound, User::Invariant());
	// The journal does not exist - we need to create one
	
	TDriveUnit drive(aDrive);
	RBuf journalPath;
	journalPath.CreateL(drive.Name(), KMaxFileName);
	CleanupClosePushL(journalPath);
	journalPath.Append(iJournalFileName);

	CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, journalPath, aDrive);
	iJournalFiles.AppendL(journalFile);
	CleanupStack::Pop(journalFile);
	CleanupStack::PopAndDestroy(&journalPath);
	
	UpdateDrivesFileL(aDrive);
	
	User::LeaveIfError(iAllDrives.InsertInOrder(aDrive));

	return iJournalFiles.Count() - 1; // Since we appended the entry, we return the last index
	}
	
void CJournal::DeleteFilesL(TIntegrityServicesEvent aTypeFilter)
	{
	TInt journalsCount(iJournalFiles.Count());
	for (TInt i = 0; i < journalsCount; ++i)
		{
		iJournalFiles[i]->JournalOperationL(IntegrityDeleteFileL, aTypeFilter, CIntegrityServices::EFailDeletingFile);
		}
	}
	
void CJournal::RestoreFilesL(TInt aDrive)
	{
	TInt index = FindJournalFileIndexL(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->JournalOperationL(IntegrityRestoreFileL, ERemovedFile, CIntegrityServices::EFailRestoringFile);
	}
		
void CJournal::DeleteFilesL(TIntegrityServicesEvent aTypeFilter, TInt aDrive)
	{
	TInt index = FindJournalFileIndexL(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->JournalOperationL(IntegrityDeleteFileL, aTypeFilter, CIntegrityServices::EFailDeletingFile);
	}

void CJournal::WriteJournalEventL(TIntegrityServicesEvent aEvent)
	{
	// write the event to each journal file
	for(TInt index = 0; index < iJournalFiles.Count(); index++)
		{
		iJournalFiles[index]->EventL(aEvent);
		}
	}
	
void CJournal::WriteJournalEventL(TIntegrityServicesEvent aEvent, TInt aDrive, TBool aSerializeEventToJournal)
	{
	TInt index = FindJournalFileIndexL(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->EventL(aEvent, aSerializeEventToJournal);
	}

void CJournal::AddL(const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("CJournal::AddL() - aFileName %S"), &aFileName);							
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->AddL(aFileName);
	}

void CJournal::RemoveL(const TDesC& aFileName, TDes& backupFileName)
	{
	DEBUG_PRINTF2(_L("CJournal::RemoveL() - aFileName %S"), &aFileName);							
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->RemoveL(aFileName, backupFileName);
	}

void CJournal::TemporaryL(const TDesC& aFileName)
	{
	DEBUG_PRINTF2(_L("CJournal::TemporaryL() - aFileName %S"), &aFileName);							
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->TemporaryL(aFileName);
	}

TIntegrityServicesEvent CJournal::LastEventL() const
	{
	TIntegrityServicesEvent lastEvent = ENone;
	// work out the "real" last event
	// journals states may be at different since they cannot all be written
	// to simultaneously
	for(TInt index = 0; index < iJournalFiles.Count(); index++)
		{
		TInt position = iCompletedDrives.Find(iJournalFiles[index]->Drive());
		
		if(position!=KErrNotFound) 
			continue; //don't check completed drives
		
		TIntegrityServicesEvent currentJournalLastEvent = iJournalFiles[index]->LastEvent();
		if (currentJournalLastEvent >= lastEvent)
			lastEvent = currentJournalLastEvent;
		}//for
	return lastEvent;
	}
	
TIntegrityServicesEvent CJournal::LastEventL(TInt aDrive) const
	{
	DEBUG_PRINTF2(_L("CJournal::LastEventL() - aDrive %d"), aDrive);							
	TInt index = FindJournalFileIndexL(aDrive);
	User::LeaveIfError(index);

	return iJournalFiles[index]->LastEvent();
	}

/*static*/ void CJournal::CreateDrvFileNameL(TStsTransactionId aTransactionID, const TDesC& aPath, TDes& aDrvFileName)
	{
	DEBUG_PRINTF3(_L("CJournal::CreateDrvFileNameL() - aTransactionID %X aPath %S"), aTransactionID, &aPath);							
	aDrvFileName = TDriveUnit(::RFs::GetSystemDrive()).Name();
	aDrvFileName.Append(aPath);
	aDrvFileName.AppendNumUC(static_cast<TUint32>(aTransactionID), EHex);
	aDrvFileName.Append(KExtDelimiter);
	aDrvFileName.Append(KDriveExt);
	}

/*static*/ void CJournal::CreateJournalFileNameL(TStsTransactionId aTransactionID, const TDesC& aPath, TDes& aJournalFileName)
	{
	aJournalFileName.Append(aPath);
	aJournalFileName.AppendNumUC(static_cast<TUint32>(aTransactionID), EHex);
	aJournalFileName.Append(KExtDelimiter);
	aJournalFileName.Append(KJournalExt);
	}

/*static*/ TInt CJournal::RecoverTransactionIdFromDrvFileName(const TDesC& aDrvFileName, TStsTransactionId& aTransactionID)
	{
	TLex lex(aDrvFileName);
	TUint32 tmp;
	TInt result = lex.Val(tmp, EHex);
	aTransactionID = static_cast<TStsTransactionId>(tmp);
	return result;
	}

void CJournal::RollBackDriveL(TInt aDrive, TBool aRecordAllRollbackEvents)
	{
	switch (LastEventL())
		{
		// Transaction did not complete, rollback required
		case ERemovedFile:
		case EBackupFile:
		case ETempFile:
		case EAddedFile:
		case EAddedFilesRemoved:
		case ERemovedFilesRestored:
			// rollback this individual journal from where it last got to.
			switch (LastEventL(aDrive))
				{
			case ERemovedFile:
			case EBackupFile:
			case ETempFile:
			case EAddedFile:
				DeleteFilesL(EAddedFile, aDrive);
				
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailNewFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);				
				WriteJournalEventL(EAddedFilesRemoved, aDrive, aRecordAllRollbackEvents);
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailNewFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);
				
			// fall-through - automatically proceed to the next state, we'll start from EAddedFilesRemoved in case the RestoreFileL has failed
			case EAddedFilesRemoved:
				RestoreFilesL(aDrive);				
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailOldFilesRestored, CIntegrityServices::EBeforeJournal, KNullDesC);			
				WriteJournalEventL(ERemovedFilesRestored, aDrive, aRecordAllRollbackEvents);
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailOldFilesRestored, CIntegrityServices::EAfterJournal, KNullDesC);

			// fall-through - automatically proceed to the next state
			case ERemovedFilesRestored:				
				DeleteFilesL(ETempFile, aDrive);
				
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);				
				WriteJournalEventL(ETempFilesRemoved, aDrive, aRecordAllRollbackEvents);
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);
				break;
				
			case ETempFilesRemoved:
				break;
		
			// nothing was done, just delete the journal file
			case ENone:
				break;
		
			// Erk! Bad state, bad state!
			default:
				User::Leave(KErrCorrupt);
				break;
				}
			break;
			
		// Transaction complete, just need to remove the backup
		case ECommitted:
		case EBackupFilesRemoved:
			switch (LastEventL(aDrive))
				{
			// At least one journal had a complete transaction...
			// roll forwards all journal files.
			case ERemovedFile:
			case EBackupFile:
			case ETempFile:
			case EAddedFile:
			case ECommitted:
				DeleteFilesL(EBackupFile, aDrive);
			
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailBackupFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);			
				WriteJournalEventL(EBackupFilesRemoved, aDrive, aRecordAllRollbackEvents);
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailBackupFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);

				//fall-through - automatically proceed to the next state
			case EBackupFilesRemoved:				
				DeleteFilesL(ETempFile, aDrive);
			
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);
				WriteJournalEventL(ETempFilesRemoved, aDrive, aRecordAllRollbackEvents);
				CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);
				break;
							
			case ETempFilesRemoved:
				break;
		
			// nothing was done, just delete the journal file
			case ENone:
				break;
		
			// unknown state	
			default:
				User::Leave(KErrCorrupt);
				break;
				}
			break;
			
		case ETempFilesRemoved:
			break;
		
		// nothing was done, just delete the journal file
		case ENone:
			break;
		
		// unknown state	
		default:
			User::Leave(KErrCorrupt);
			break;
		}
		
	FinishRollbackL(aDrive, aRecordAllRollbackEvents);
	}

void CJournal::CommitL()
	{
	StartCommitL();
	
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailInstallComplete, CIntegrityServices::EBeforeJournal, KNullDesC);
	WriteJournalEventL(ECommitted);
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailInstallComplete, CIntegrityServices::EAfterJournal, KNullDesC);
	
	DeleteFilesL(EBackupFile);
	
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailBackupFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);
	WriteJournalEventL(EBackupFilesRemoved);
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailBackupFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);
	
	DeleteFilesL(ETempFile);
	
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EBeforeJournal, KNullDesC);
	WriteJournalEventL(ETempFilesRemoved);
	CIntegrityServices::SimulatePowerFailureL(CIntegrityServices::EFailTempFilesRemoved, CIntegrityServices::EAfterJournal, KNullDesC);
	
	FinishCommitL();
	}

void CJournal::RollBackL(TBool aRecordAllRollbackEvents /*= ETrue*/)
	{
#ifdef __WINSCW__
	// For 2 minutes after initial boot, DLLs are not unloaded. If we are doing a
	// rollback, we need to make sure any pending unloadeds are actioned, otherwise a
	// previously loaded DLL could cause the rollback to fail on windows (on arm it is legal to
	// delete a loaded DLL/EXE, whilst on windows it is not).
	RLoader loader;
	TInt r = loader.Connect();
	if(r == KErrNone)
		{
		(void)loader.CancelLazyDllUnload();
		loader.Close();
		}
#endif
	
	for(TInt index = 0; index < iJournalFiles.Count(); index++)
		{
		TDriveUnit drive = iJournalFiles[index]->Drive();
		// check to see if this drive has already been completed
		if(iCompletedDrives.Find(drive) != KErrNotFound)
			continue;
			
		// only attempt to recover writeable drives that are present
		TDriveInfo info;
		User::LeaveIfError(iFs.Drive(info, drive));
		if (info.iMediaAtt & KMediaAttWriteProtected || info.iType==EMediaNotPresent)
			continue;

		TRAPD(err, RollBackDriveL(drive, aRecordAllRollbackEvents));
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound
			&& err != KErrNotReady)
			{
			// unexpected error
			User::Leave(err);
			}
		}
	}
