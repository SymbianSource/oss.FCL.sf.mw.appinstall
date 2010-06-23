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
#include "log.h"
#include <f32file.h>

using namespace Swi;

CJournal* CJournal::NewL(RFs& aFs, RLoader& aLoader, TInt64 aTransactionID, const TDesC& aPath)
	{
	CJournal* self = CJournal::NewLC(aFs, aLoader, aTransactionID, aPath);
	CleanupStack::Pop(self);
	return self;
	}

CJournal* CJournal::NewLC(RFs& aFs, RLoader& aLoader, TInt64 aTransactionID, const TDesC& aPath)
	{
	CJournal* self = new(ELeave) CJournal(aFs, aLoader);
	CleanupStack::PushL(self);
	self->ConstructL(aTransactionID, aPath);
	return self;
	}

CJournal::~CJournal()
	{
	iJournalDrives.Reset();
	iCompletedDrives.Reset();
	iAllDrives.Reset();
	iJournalFiles.ResetAndDestroy();
	}

CJournal::CJournal(RFs& aFs, RLoader& aLoader) : iFs(aFs), iLoader(aLoader)
	{
	}

void CJournal::ConstructL(TInt64 aTransactionID, const TDesC& aPath)
	{
	// construct the generic journal filename (does not include a drive)
	iJournalFileName = aPath;
	iJournalFileName.AppendNumUC(aTransactionID, EHex);
	iJournalFileName.Append(KExtDelimiter);
	iJournalFileName.Append(KJournalExt);
	
	// construct the filename for the drives journal (located on the system drive)
	TDriveUnit systemDrive(RFs::GetSystemDrive());
	iDriveArrayFileName = systemDrive.Name();
	iDriveArrayFileName.Append(aPath);
	iDriveArrayFileName.AppendNumUC(aTransactionID, EHex);
	iDriveArrayFileName.Append(KExtDelimiter);
	iDriveArrayFileName.Append(KDriveExt);
	
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
			
			iJournalDrives.AppendL(drive);
			}
		}
	CleanupStack::PopAndDestroy(&journal);
	}

void CJournal::DeleteJournalFilesL()
	{
	// close the journal files in preparation for delete
	iJournalFiles.ResetAndDestroy();
	
	// delete all journal files
	for(TInt drive = 0; drive < iJournalDrives.Count(); drive++)
		{
		DeleteJournalFileL(iJournalDrives[drive]);
		}
	
	// delete the drive array only after all journals have been completed
	// (committed or rolled back)
	if (iAllDrives.Count() == iCompletedDrives.Count())
		{
		DeleteDrivesFileL();
		}
	}
	
void CJournal::DeleteJournalFileL(TInt aDrive)
	{
	TDriveUnit journalDrive(aDrive);
	RBuf journal;
	journal.CreateL(journalDrive.Name(), KMaxFileName);
	CleanupClosePushL(journal);
	journal.Append(iJournalFileName);

	DEBUG_PRINTF2(_L("Integrity Services - deleting journal %S"), &journal);
	User::LeaveIfError(iLoader.Delete(journal));

	// record that we have completed this drive
	UpdateDrivesArrayL(aDrive);
	iCompletedDrives.InsertInOrder(aDrive);
	TInt err = iFs.RmDir(journal);
	if(err != KErrNone && err != KErrInUse && err != KErrAccessDenied && 
		err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}	
	CleanupStack::PopAndDestroy(&journal);
	}
	
void CJournal::DeleteDrivesFileL()
	{
	iJournalDrives.Reset();
	iCompletedDrives.Reset();
	TInt err = KErrNone;
	
	DEBUG_PRINTF2(_L("Integrity Services - deleting drive array %S"), &iDriveArrayFileName);
	err = iLoader.Delete(iDriveArrayFileName);
			
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
			if(err == KErrInUse || err == KErrAccessDenied || 
				err == KErrNotFound || err == KErrPathNotFound)
				{
				break;
				}
			else
				{
				User::Leave(err);
				}
			}
		User::LeaveIfError(directory.PopDir());
		}
	}

void CJournal::RefreshDrivesArrayL()
	{
	// clear existing journal drive arrays prior to reloading them from file
	iCompletedDrives.Reset();
	iAllDrives.Reset();
	
	RFileReadStream journalStream;
	TInt err = journalStream.Open(iFs, iDriveArrayFileName, EFileStream);
	if(err == KErrNone)
		{
		CleanupClosePushL(journalStream);
		while(err == KErrNone)
			{
			// read the next journal entry
			TInt drive = 0;
			TRAP(err, drive = journalStream.ReadInt32L());
			
			// Checks boundaries for Drive
			if( drive <  EDriveA || drive > EDriveZ )
				{
				User::Leave(KErrCorrupt);
				}
						
			if(err == KErrNone)
				{
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
			else if (err != KErrEof)
				{
				User::Leave(err);
				}
			}
		CleanupStack::PopAndDestroy(&journalStream);
		}
	else if(err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}
	}

void CJournal::UpdateDrivesArrayL(TInt aDrive)
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
	// To commit, all drives must exist. No exceptions.
	if (iJournalDrives.Count() != iAllDrives.Count())
		{
		User::Leave(KErrNotReady);
		}
		
	// none of the drives must yet be completed...
	if (iCompletedDrives.Count() != 0)
		{
		User::Leave(KErrNotSupported);
		}
		
	// synch up all the drives
	TInt drivesCount(iJournalDrives.Count());
	for (TInt i = 0; i < drivesCount; ++i)
		{
 		//Check if Journal drives are present.
 		TDriveInfo info;
 		if (iFs.Drive(info, iJournalDrives[i])!=KErrNone || info.iType==EMediaNotPresent)
 			{
 			User::Leave(KErrNotReady);	
 			}
		iJournalFiles[i]->SynchL();
		}		
	}

void CJournal::FinishCommitL()
	{
	DeleteJournalFilesL();
	
	// return the journal to a state where it can be used again,
	// in the insane event that someone wants to...
	// (I'm looking at you sisregistry)
	iCompletedDrives.Reset();
	iJournalDrives.Reset();
	iAllDrives.Reset();
	}

void CJournal::StartRollbackL(TInt aDrive)
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);

	// check there is a Journal file
	if (index >= iJournalFiles.Count())
		{
		// This can happen when the journal file is missing due to OOM
		// So handle this as if OOM
		User::LeaveIfError( KErrNoMemory );
		}

	iJournalFiles[index]->SynchL();
	}

void CJournal::FinishRollbackL(TInt aDrive)
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->Close();
	
	DeleteJournalFileL(aDrive);
	
	if (iCompletedDrives.Count() == iAllDrives.Count())
		{
		DeleteDrivesFileL();
		}
	}

void CJournal::SynchL()
	{
	TInt drivesCount(iAllDrives.Count());
	RefreshDrivesArrayL();
	if (drivesCount != iAllDrives.Count())
		{
		// another process has added a drive. Read the log created.
		RBuf journal;
		journal.CreateL(KMaxFileName);
		CleanupClosePushL(journal);
		for (TInt i = 0; i < iAllDrives.Count(); ++i)
			{
			TChar drive = iAllDrives[i];
			if (iJournalDrives.Find(drive) == KErrNotFound &&
				iCompletedDrives.Find(drive) == KErrNotFound)
				{
				User::LeaveIfError(iJournalDrives.InsertInOrder(drive));
		
				TInt index = iJournalDrives.FindInOrder(drive);
				User::LeaveIfError(index);

				TDriveUnit driveUnit(drive);
				journal = driveUnit.Name();
 				journal.Append(iJournalFileName);

				CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, journal, drive);
				User::LeaveIfError(iJournalFiles.Insert(journalFile, index));
				CleanupStack::Pop(journalFile);
				}
			}
		CleanupStack::PopAndDestroy(&journal);
		}
	}

TInt CJournal::PrepareToWriteL(TInt aDrive)
	{
	SynchL();
	
	TInt index(0);
	TInt err = iJournalDrives.FindInOrder(aDrive);
	if (err >= 0)
		{
		// already exists, ensure it is in synch.
		index = err;
		iJournalFiles[index]->SynchL();
		}
	else if (err == KErrNotFound)
		{
		// Save the current count of our array's in case something goes wrong
  		TInt previousJournalDrivesCount = iJournalDrives.Count();
  		TInt previousAllDrivesCount = iAllDrives.Count();
  
 		// As there are multiple instances of calls which can leave
		// we need to provide a TRAP block to enable us to perform
 		// proper cleanup before propagating the leave up the call stack
  		TRAPD(err,
   			{
   			
    		// create a new journal for this file.
			User::LeaveIfError(iJournalDrives.InsertInOrder(aDrive));
			User::LeaveIfError(iAllDrives.InsertInOrder(aDrive));
		
			index = iJournalDrives.FindInOrder(aDrive);
			User::LeaveIfError(index);

			UpdateDrivesArrayL(aDrive);
		
			TDriveUnit drive(aDrive);
			RBuf journal;
			journal.CreateL(drive.Name(), KMaxFileName);
			CleanupClosePushL(journal);
 			journal.Append(iJournalFileName);

			CJournalFile* journalFile = CJournalFile::NewLC(iFs, iLoader, journal, aDrive);
			User::LeaveIfError(iJournalFiles.Insert(journalFile, index));
			CleanupStack::Pop(journalFile);
			CleanupStack::PopAndDestroy(&journal);

   			}); // TRAPD 
   			
		// If an error occurred at all in the preceding block we need
		// to ensure that the iJournalDrives and iJournalFiles arrays are
		// in sync before we leave.
		if( err != KErrNone )
			{

			// Check the iJournalDrives array first
    		if( iJournalDrives.Count() != previousJournalDrivesCount )
     			{
     			// We added aDrive to the array before leaving
     			index = iJournalDrives.FindInOrder( aDrive );
     
     			if( index >=0 && index < iJournalDrives.Count() )
      				{
      				iJournalDrives.Remove(index);
      				}
      
     			// Now check the iAllDrives array
     			if( iAllDrives.Count() != previousAllDrivesCount )
      				{
					index = iAllDrives.FindInOrder( aDrive );
      
					if( index >=0 && index < iAllDrives.Count() )
       					{
       					iAllDrives.Remove( index );
       					}
      				}
     			}
      
    		// We can now leave as nature intended.  We don't need to do anything
    		// with iJournalFiles as if this succeeded (and modified the array)
    		// then we wouldn't have left in the first place.
    		User::Leave( err );  	
			}
		}
	else
		{
		User::Leave(err);
		}
	return index;
	}
	
void CJournal::RestoreFilesL(TIntegrityServicesEvent aTypeFilter, CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	TInt journalsCount(iJournalFiles.Count());
	for (TInt i = 0; i < journalsCount; ++i)
		{
		iJournalFiles[i]->JournalOperationL(IntegrityRestoreFileL, aTypeFilter, aIntegrityServices,
			aFailType);
		}
	}

void CJournal::DeleteFilesL(TIntegrityServicesEvent aTypeFilter, CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	TInt journalsCount(iJournalFiles.Count());
	for (TInt i = 0; i < journalsCount; ++i)
		{
		iJournalFiles[i]->JournalOperationL(IntegrityDeleteFileL, aTypeFilter, aIntegrityServices,
			aFailType);
		}
	}
	
void CJournal::RestoreFilesL(TIntegrityServicesEvent aTypeFilter, TInt aDrive,
	CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->JournalOperationL(IntegrityRestoreFileL, aTypeFilter, aIntegrityServices,
		aFailType);
	}
		
void CJournal::DeleteFilesL(TIntegrityServicesEvent aTypeFilter, TInt aDrive,
	CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->JournalOperationL(IntegrityDeleteFileL, aTypeFilter, aIntegrityServices,
		aFailType);
	}

void CJournal::WriteJournalEventL(TIntegrityServicesEvent aEvent)
	{
	// write the event to each journal file
	for(TInt index = 0; index < iJournalFiles.Count(); index++)
		{
		iJournalFiles[index]->EventL(aEvent);
		}
	}
	
void CJournal::WriteJournalEventL(TIntegrityServicesEvent aEvent, TInt aDrive)
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);
	iJournalFiles[index]->EventL(aEvent);
	}

void CJournal::AddL(const TDesC& aFileName)
	{
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->AddL(aFileName);
	}

void CJournal::RemoveL(const TDesC& aFileName, TDes& backupFileName)
	{
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->RemoveL(aFileName, backupFileName);
	}

void CJournal::TemporaryL(const TDesC& aFileName)
	{
	// write the filename to the journal on the same drive
	TInt drive = CJournalFile::CheckFileNameL(iFs, aFileName);
	TInt index = PrepareToWriteL(drive);
	iJournalFiles[index]->TemporaryL(aFileName);
	}

TIntegrityServicesEvent CJournal::LastEvent() const
	{
	TIntegrityServicesEvent lastEvent = ENone;
	// work out the "real" last event
	// journals states may be at different since they cannot all be written
	// to simultaneously
	for(TInt index = 0; index < iJournalFiles.Count(); index++)
		{		
		switch(iJournalFiles[index]->LastEvent())
			{
			case ENone:
				break;
				
			case ERemovedFile:
			case ETempFile:
			case EAddedFile:
				if(lastEvent != EInstallComplete)
					{
					lastEvent = iJournalFiles[index]->LastEvent();
					}
				break;
			
			case EInstallComplete:
				if(lastEvent != EBackupFilesRemoved)
					{
					lastEvent = iJournalFiles[index]->LastEvent();
					}
				break;
			
			case EBackupFilesRemoved:
				if(lastEvent != ETempFilesRemoved)
					{
					lastEvent = iJournalFiles[index]->LastEvent();
					}
				break;
			
			case EAddedFilesRemoved:
				if(lastEvent != ERemovedFilesRestored)
					{
					lastEvent = iJournalFiles[index]->LastEvent();
					}
				break;
			
			case ETempFilesRemoved:
				lastEvent = iJournalFiles[index]->LastEvent();
				break;
			
			case ERemovedFilesRestored:
				if(lastEvent != ETempFilesRemoved)
					{
					lastEvent = iJournalFiles[index]->LastEvent();
					}
				break;
					
			default:
				User::Leave(KErrCorrupt);
				break;
   			}
		}
	return lastEvent;
	}
	
TIntegrityServicesEvent CJournal::LastEventL(TInt aDrive) const
	{
	TInt index = iJournalDrives.Find(aDrive);
	User::LeaveIfError(index);

	// check there is a Journal file
	if (index >= iJournalFiles.Count())
		{
		// This can happen when the journal file is missing due to OOM
		// So handle this as if OOM
		User::LeaveIfError( KErrNoMemory );
		}

	return iJournalFiles[index]->LastEvent();
	}

