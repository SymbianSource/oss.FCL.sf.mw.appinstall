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
* CIntegrityServices implementation
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "integrityservices.h"
#include "journal.h"
#include "log.h"
#include "cleanuputils.h"
#include "operationfunctions.h"


#include <f32file.h>

using namespace Swi;

namespace Swi
{
/**
 * This is a trivial C class that just encapsulates a TEntryArray object in order to 
 * facilitate its storage on the heap.
 *
 * @released
 * @internalComponent 
 */
class CEntryArray : public CBase
	{
public:
	inline TEntryArray& operator()();

private:
	/**
	 * Container to hold file entries
	 */
	TEntryArray iEntryArray;
	};

inline TEntryArray& CEntryArray::operator()()
	{
	return iEntryArray;
	}
} //namespace

EXPORT_C CIntegrityServices* CIntegrityServices::NewL(TInt64 aTransactionID, const TDesC& aPath)
	{
	CIntegrityServices* self = CIntegrityServices::NewLC(aTransactionID, aPath);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CIntegrityServices* CIntegrityServices::NewLC(TInt64 aTransactionID, const TDesC& aPath)
	{
	CIntegrityServices* self = new(ELeave) CIntegrityServices(aTransactionID);
	CleanupStack::PushL(self);
	self->ConstructL(aPath);
	return self;
	}

EXPORT_C CIntegrityServices::CIntegrityServices(TInt64 aTransactionID) : iTransactionID(aTransactionID)
	{
	}

EXPORT_C CIntegrityServices::~CIntegrityServices()
	{
	delete iJournal;
	iFs.Close();
	
	iLoader.Close();
	}

EXPORT_C void CIntegrityServices::ConstructL(const TDesC& aPath)
	{
	DEBUG_PRINTF2(_L8("Integrity Services - Opening session with  Session ID %Ld."), iTransactionID);
	
	// check that the supplied path is valid
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.IsValidName(aPath));
	
	User::LeaveIfError(iLoader.Connect());
	
	// store the journal path and create the journal
	TParsePtrC pathPtr(aPath);
	iJournalPath = pathPtr.Path();
	iJournal = CJournal::NewL(iFs, iLoader, iTransactionID, iJournalPath);
	iSystemDrive = RFs::GetSystemDrive();
	}
	
void CIntegrityServices::SimulatePowerFailureL(TFailType /*aFailType*/, TFailPosition /*aFailPosition*/, const TDesC& /*aFailFileName*/)
	{
	// only implemented in derived test class
	}

/*static*/ void CIntegrityServices::NormalizeDirectoryName(TDes& aFileName)
{
	// Directories are represented in the integrity tree and integrity journal exactly as files,
	// without the trailing slash 
	TInt lastCharPos = aFileName.Length() - 1;
	if ( lastCharPos >= 0 && aFileName[lastCharPos] == KPathDelimiter &&
		 aFileName.Locate(KPathDelimiter) != lastCharPos) // Take care not to remove slash from "c:\" and the like
		{
		aFileName.Delete(lastCharPos, 1);
		}			
}

EXPORT_C void CIntegrityServices::AddL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("Integrity Services - Session %Ld, Adding File: %S."),
		iTransactionID, &aFileName);

	HBufC* localFilenameHeap = aFileName.AllocLC();
	TPtr localFilename = localFilenameHeap->Des();
	NormalizeDirectoryName(localFilename); // If it is a directory name, make sure to normalize it
		
	// Record the added file or directory in the journal
	SimulatePowerFailureL(EFailAddingNewFile, EBeforeJournal, aFileName);
	iJournal->AddL(localFilename);
	SimulatePowerFailureL(EFailAddingNewFile, EAfterJournal, aFileName);
	CleanupStack::PopAndDestroy(localFilenameHeap);
	}

void CIntegrityServices::CopyToBackupL(const TDesC& aSource, const TDesC& aBackup)
	{
	// Copying a file isn't atomic so we create a temporary backup file first
	RBuf backupTmpName;
	backupTmpName.Create(aBackup.Length() + 4);
	CleanupClosePushL(backupTmpName);
	backupTmpName.Copy(aBackup);
	_LIT(KTmpExt, ".tmp");
	backupTmpName.Append(KTmpExt);
	
	// Copying a file is not an atomic operation so add the temporary
	// file to the journal to enable cleanup if a power failure occurs before
	// the rename
	SimulatePowerFailureL(EFailAddingTempFile, EBeforeJournal, backupTmpName);
	iJournal->TemporaryL(backupTmpName);
	SimulatePowerFailureL(EFailAddingTempFile, EAfterJournal, backupTmpName);	
		
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);
		
	TInt err = fileMan->Copy(aSource, backupTmpName);
	DEBUG_PRINTF4(_L("CopyToBackupL: Copying %S to %S, err %d"), &aSource, &backupTmpName, err);
	User::LeaveIfError(err);
	
	// Backup is complete, use RFs::Rename as atomic 'commit' of backup
	err = iFs.Rename(backupTmpName, aBackup);			
	DEBUG_PRINTF2(_L("CopyToBackupL: Commit backup returned error %d"), err);	
	User::LeaveIfError(err);	
	CleanupStack::PopAndDestroy(2, &backupTmpName); // backupTmpName, fileMan 
	
	//  Now the backup is safe the original can be deleted
	err = iLoader.Delete(aSource);
	DEBUG_PRINTF3(_L("CopyToBackupL: RLoader::Delete %S returned error %d"), &aSource, err);
	User::LeaveIfError(err);
	}
	
EXPORT_C void CIntegrityServices::RemoveL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("Integrity Services - Session %Ld, Removing File: %S"), iTransactionID, &aFileName);

	// before doing anything check that the file or directory exists
	TEntry entry;
	User::LeaveIfError(iFs.Entry(aFileName, entry));
	
	// We might need to grow this buffer by one byte later
	HBufC* localFilenameHeap = HBufC::NewLC(aFileName.Length() + 1);
	TPtr localFilename = localFilenameHeap->Des();
	localFilename.Copy(aFileName);
	
	TBool isFilenameDir = entry.IsDir();
	// The "if" below is not functionally necessary, but it is a slight optimization - 
	// so that we won't attempt to normalize directory name on files. The optimization is not
	// done in AddL or NormalizeDirectoryName itself, since we don't have future use for TEntry there, and the cost for RFs::Entry overweighs the one for NormalizeDirectoryName
	if ( isFilenameDir ) 
		{
		NormalizeDirectoryName(localFilename);
		}

	RBuf backupFileName;
	backupFileName.CreateL(KMaxFileName);
	CleanupClosePushL(backupFileName);
	SimulatePowerFailureL(EFailRemovingFile, EBeforeJournal, aFileName);
	iJournal->RemoveL(localFilename, backupFileName);
		
	if (backupFileName.Length())
		{
		SimulatePowerFailureL(EFailRemovingFile, EAfterJournal, aFileName);

		TInt err = iFs.MkDirAll(backupFileName);
		if(err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}

		SimulatePowerFailureL(EFailRemovingFile, EBeforeAction, aFileName);
		
		_LIT(KSysBinMatch, "?:\\sys\\bin\\*");
		if (localFilename.MatchF(KSysBinMatch) == 0)
			{
			// A copy is slower than a rename to only use the 
			// demand paging safe API for files in sys\bin
			CopyToBackupL(localFilename, backupFileName);			
			}
		else
			{
			err = iFs.Rename(localFilename, backupFileName);
			DEBUG_PRINTF4(_L("Renamed %S as %S error %d"), &localFilename, &backupFileName, err);
			User::LeaveIfError(err);
			}				
		SimulatePowerFailureL(EFailRemovingFile, EAfterAction, aFileName);
		}
	else
		{
		DEBUG_PRINTF2(_L("%S already backed up"), &aFileName);
		SimulatePowerFailureL(EFailRemovingFile, EBeforeAction, aFileName);
		// If backupFileName is zero-length, the file was added earlier
		// in the same journal and doesn't need to be backed up.
		if (isFilenameDir)
			{
			CFileMan* fileman = CFileMan::NewL(iFs);
			CleanupStack::PushL(fileman);			
			// Make sure to append slash before calling RmDir - otherwise it deletes the parent directory
			if (localFilename[localFilename.Length()-1] != KPathDelimiter)
	  			{
  				localFilename.Append(KPathDelimiter);
  				}			
			User::LeaveIfError(fileman->RmDir(localFilename)); // A directory cannot be a paged exec., so we don't have to use iLoader
			CleanupStack::PopAndDestroy(fileman);
			}
		else
			{
			User::LeaveIfError(iLoader.Delete(aFileName));
			}
		SimulatePowerFailureL(EFailRemovingFile, EAfterAction, aFileName);			
		}

	// Don't leave an empty directory structure, try pruning it
	RemoveDirectoryTreeL(iFs, aFileName);

	CleanupStack::PopAndDestroy(2, localFilenameHeap); // backupFileName
	}

EXPORT_C void CIntegrityServices::TemporaryL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("Integrity Services - Session %Ld, Temporary File: %S."),
		iTransactionID, &aFileName);

	// record the temporary file or directory in the journal
	SimulatePowerFailureL(EFailAddingTempFile, EBeforeJournal, aFileName);
	iJournal->TemporaryL(aFileName);
	SimulatePowerFailureL(EFailAddingTempFile, EAfterJournal, aFileName);
	}

EXPORT_C void CIntegrityServices::CommitL()
	{
	DEBUG_PRINTF2(_L8("Integrity Services - Commiting Session %Ld."), iTransactionID);
	iJournal->SynchL();
	iJournal->StartCommitL();
	
	SimulatePowerFailureL(EFailInstallComplete, EBeforeJournal, KNullDesC);
	iJournal->WriteJournalEventL(EInstallComplete);
	SimulatePowerFailureL(EFailInstallComplete, EAfterJournal, KNullDesC);
	
	DeleteFilesL(*iJournal, EBackupFile);
	
	SimulatePowerFailureL(EFailBackupFilesRemoved, EBeforeJournal, KNullDesC);
	iJournal->WriteJournalEventL(EBackupFilesRemoved);
	SimulatePowerFailureL(EFailBackupFilesRemoved, EAfterJournal, KNullDesC);
	
	DeleteFilesL(*iJournal, ETempFile);
	
	SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
	iJournal->WriteJournalEventL(ETempFilesRemoved);
	SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
	
	iJournal->FinishCommitL();
	}

EXPORT_C void CIntegrityServices::RollBackL(TBool aAllTransactions)
	{
	DEBUG_PRINTF2(_L8("Integrity Services - Rolling Back Session %Ld."), iTransactionID);

	if (aAllTransactions)
		{
		// search for and rollback any transaction drive list
		TDriveUnit systemDrive(iSystemDrive);
		RBuf fileSpec;
		fileSpec.CreateL(systemDrive.Name(), KMaxFileName);
		CleanupClosePushL(fileSpec);
		fileSpec.Append(iJournalPath);
		fileSpec.Append(KMatchAny);
		fileSpec.Append(KExtDelimiter);
		fileSpec.Append(KDriveExt);

		RDir dir;
		TInt err = dir.Open(iFs, fileSpec, KEntryAttNormal);
		if (err == KErrNone)
			{
			CEntryArray* entryArrayContainer = new (ELeave) CEntryArray;
			CleanupStack::PushL(entryArrayContainer);
			TEntryArray& entryArray = (*entryArrayContainer)();
			err = dir.Read(entryArray);
			dir.Close();
			if (err != KErrNone && err != KErrEof)
				{
				User::Leave(err);
				}
			TInt entryCount = entryArray.Count();
			for(TInt index = 0; index < entryCount; ++index)
				{
				// check that the entry is not a directory
				if(!entryArray[index].IsDir())
					{
					TLex lex(entryArray[index].iName);
					TInt64 transactionID;
					if(lex.Val(transactionID, EHex) == KErrNone)
						{
						CJournal* journal = NULL;
						TRAP(err, journal = CJournal::NewL(iFs, iLoader, transactionID, iJournalPath));
						if(err == KErrNone)
							{
							CleanupStack::PushL(journal);
							// start the recovery, do not let corrupt journals prevent
							// recovery of other journals
							iStartedJournalRollback = ETrue;
							TRAP(err, RollBackJournalL(*journal));
							if(err != KErrNone && err != KErrCorrupt)
								{
								// unexpected error
								User::Leave(err);
								}
							CleanupStack::PopAndDestroy(journal);
							}
						}
					}
				}
			CleanupStack::PopAndDestroy(entryArrayContainer);
			}
		else if(err != KErrPathNotFound && err != KErrNotFound && err != KErrNotReady)
			{
			// unexpected error
			User::Leave(err);
			}
		CleanupStack::PopAndDestroy(&fileSpec);
		}
	else
		{
		// rollback only this transaction
		TRAPD(err, RollBackJournalL(*iJournal));
		if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound
				&& err != KErrNotReady)
			{
			// unexpected error
			User::Leave(err);
			}
		}
	}

void CIntegrityServices::RollBackJournalL(CJournal& aJournal)
	{
	aJournal.SynchL();

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
	
	for(TInt index = 0; index < aJournal.Drives().Count(); index++)
		{
		// check to see if this drive has already been completed
		if(aJournal.CompletedDrives().Find(aJournal.Drives()[index]) != KErrNotFound)
			{
			continue;
			}
			
		// only attempt to recover writeable drives that are present
		TDriveInfo info;
		User::LeaveIfError(iFs.Drive(info, aJournal.Drives()[index]));
		if (!(info.iMediaAtt & KMediaAttWriteProtected) && info.iType!=EMediaNotPresent)
			{
			TRAPD(err, RollBackDriveL(aJournal, aJournal.Drives()[index]));
			if(err != KErrNone && err != KErrPathNotFound && err != KErrNotFound
				&& err != KErrNotReady)
				{
				// unexpected error
				User::Leave(err);
				}
			}
		}
	}

void CIntegrityServices::RollBackDriveL(CJournal& aJournal, TInt aDrive)
	{
	aJournal.StartRollbackL(aDrive);
	
	DEBUG_PRINTF3(_L8("Integrity Services - Rolling Back drive %d, last event %d."), aDrive, aJournal.LastEvent());
	switch (aJournal.LastEvent())
		{
		// Transaction did not complete, rollback required
		case ERemovedFile:
		case EBackupFile:
		case ETempFile:
		case EAddedFile:
		case EAddedFilesRemoved:
		case ERemovedFilesRestored:
			// rollback this individual journal from where it last got to.
			switch (aJournal.LastEventL(aDrive))
				{
			case ERemovedFile:
			case EBackupFile:
			case ETempFile:
			case EAddedFile:
				DeleteFilesL(aJournal, EAddedFile, aDrive);
				
				SimulatePowerFailureL(EFailNewFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(EAddedFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailNewFilesRemoved, EAfterJournal, KNullDesC);
				
				RestoreFilesL(aJournal, aDrive);
				
				SimulatePowerFailureL(EFailOldFilesRestored, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ERemovedFilesRestored, aDrive);
				SimulatePowerFailureL(EFailOldFilesRestored, EAfterJournal, KNullDesC);
				
				DeleteFilesL(aJournal, ETempFile, aDrive);
				
				SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ETempFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
				break;
			
			// Transaction did not complete, rollback did not complete, resume rollback
			case EAddedFilesRemoved:
				RestoreFilesL(aJournal, aDrive);
				
				SimulatePowerFailureL(EFailOldFilesRestored, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ERemovedFilesRestored, aDrive);
				SimulatePowerFailureL(EFailOldFilesRestored, EAfterJournal, KNullDesC);
				
				DeleteFilesL(aJournal, ETempFile, aDrive);
				
				SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ETempFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
				break;
				
			case ERemovedFilesRestored:
				DeleteFilesL(aJournal, ETempFile, aDrive);
				
				SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ETempFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
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
		case EInstallComplete:
		case EBackupFilesRemoved:
			switch (aJournal.LastEventL(aDrive))
				{
			// At least one journal had a complete transaction...
			// roll forwards all journal files.
			case ERemovedFile:
			case EBackupFile:
			case ETempFile:
			case EAddedFile:
			case EInstallComplete:
				DeleteFilesL(aJournal, EBackupFile, aDrive);
			
				SimulatePowerFailureL(EFailBackupFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(EBackupFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailBackupFilesRemoved, EAfterJournal, KNullDesC);
			
				DeleteFilesL(aJournal, ETempFile, aDrive);
			
				SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ETempFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
				break;
			
			case EBackupFilesRemoved:
		 		DeleteFilesL(aJournal, ETempFile, aDrive);
			
				SimulatePowerFailureL(EFailTempFilesRemoved, EBeforeJournal, KNullDesC);
				aJournal.WriteJournalEventL(ETempFilesRemoved, aDrive);
				SimulatePowerFailureL(EFailTempFilesRemoved, EAfterJournal, KNullDesC);
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
		
	aJournal.FinishRollbackL(aDrive);
	}

void CIntegrityServices::RestoreFilesL(CJournal& aJournal, TInt aDrive)
	{
	DEBUG_PRINTF2(_L8("Integrity Services - RestoreFilesL drive = %d"), aDrive);
	if (aDrive == -1)
		{
		// This applies to all journals
		aJournal.RestoreFilesL(ERemovedFile, *this, EFailRestoringFile);
		}
	else
		{
		// specific drive version
		aJournal.RestoreFilesL(ERemovedFile, aDrive, *this, EFailRestoringFile);
		}
	}

void CIntegrityServices::DeleteFilesL(CJournal& aJournal, TIntegrityServicesEvent aEvent, TInt aDrive)
	{
	DEBUG_PRINTF3(_L8("Integrity Services - Delete Files drive = %d, event = %d"), aDrive, aEvent);
	if (aDrive == -1)
		{
		// This applies to all journals
		aJournal.DeleteFilesL(aEvent, *this, EFailDeletingFile);
		}
	else
		{
		// specific drive version
		aJournal.DeleteFilesL(aEvent, aDrive, *this, EFailDeletingFile);
		}
	}
