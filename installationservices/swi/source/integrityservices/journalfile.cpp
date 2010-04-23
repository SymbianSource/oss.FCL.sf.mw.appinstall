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
* CJournalFile implementation
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "journalfile.h"

_LIT(KBackupExt, "bak"); // extension for removed files in backup location

using namespace Swi;

CJournalFile* CJournalFile::NewL(RFs& aFs, RLoader& aLoader, const TDesC& aFileName, TInt aDrive)
	{
	CJournalFile* self = CJournalFile::NewLC(aFs, aLoader, aFileName, aDrive);
	CleanupStack::Pop(self);
	return self;
	}

CJournalFile* CJournalFile::NewLC(RFs& aFs, RLoader& aLoader, const TDesC& aFileName, TInt aDrive)
	{
	CJournalFile* self = new(ELeave) CJournalFile(aFs, aLoader, aDrive);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	return self;
	}

CJournalFile::CJournalFile(RFs& aFs, RLoader& aLoader, TInt aDrive) 
	: iFs(aFs), iLoader(aLoader), iLastEvent(ENone), iDrive(aDrive)
	{
	}

CJournalFile::~CJournalFile()
	{
	iWriteStream.Close();
	delete iTreeRoot;
	}

void CJournalFile::ConstructL(const TDesC& aFileName)
	{
	iJournalFileName = aFileName;
	
	TChar drive;
	User::LeaveIfError(iFs.DriveToChar(iDrive, drive));
	
	TBuf<2> driveSpec;
	driveSpec.Append(drive);
	driveSpec.Append(KDriveDelimiter);
	iTreeRoot = CIntegrityTreeNode::NewL(driveSpec);
	
	// read any existing entries from this journal
	TRAPD(err, ReadL());
	if(err != KErrNone)
		{
		// if necessary create the path
		if(err == KErrPathNotFound)
			{
			err = iFs.MkDirAll(iJournalFileName);
			if(err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}
			}
		else if (err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	// now prepare for writing
	PrepareToWriteL();
	}

void CJournalFile::ReadL()
	{
	RFileReadStream journalStream;
	User::LeaveIfError(journalStream.Open(iFs, iJournalFileName, EFileWrite | EFileShareAny));
	CleanupClosePushL(journalStream);
	
	// we need to check that any filenames read from the journal are on the
	// same drive as the journal itself
	TDriveUnit journalDrive(iJournalFileName);
	
	TInt err = KErrNone;
	while(err == KErrNone)
		{
		// read the next journal entry
		TRAP(err, ReadEntryL(journalStream, journalDrive));
		if(err != KErrNone && err != KErrEof)
			{
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&journalStream);
	}

void CJournalFile::ReadEntryL(RFileReadStream& aJournalStream, TInt aDrive)
	{
	TIntegrityServicesEvent event = static_cast<TIntegrityServicesEvent> (aJournalStream.ReadInt32L());
	
	HBufC* removedFile = NULL;
	HBufC* backupFile = NULL;
	HBufC* tempFile = NULL;
	HBufC* addedFile = NULL;
	
	switch(event)
		{
		case ERemovedFile:
			{
			if(iLastEvent != ENone && iLastEvent != ERemovedFile &&
				iLastEvent != ETempFile && iLastEvent != EAddedFile)
				{
				User::Leave(KErrCorrupt);
				}

			removedFile = HBufC::NewLC(aJournalStream, KMaxFileName);
			// should be followed by the backup event
			if(aJournalStream.ReadInt32L() != EBackupFile)
				{
				User::Leave(KErrCorrupt);
				}
			backupFile = HBufC::NewLC(aJournalStream, KMaxFileName);
			// make sure files are located on the same drive as the journal
			if(CJournalFile::CheckFileNameL(iFs, *removedFile) != aDrive 
				|| CJournalFile::CheckFileNameL(iFs, *backupFile) != aDrive)
				{
				User::Leave(KErrCorrupt);
				}
	
			CIntegrityTreeLeaf* removeLeaf = iTreeRoot->AddNodeL(*removedFile, ERemovedFile, iJournalFileName);
			CIntegrityTreeLeaf* backupLeaf = iTreeRoot->AddNodeL(*backupFile, EBackupFile, iJournalFileName);

			removeLeaf->SetPeer(backupLeaf);
			backupLeaf->SetPeer(removeLeaf);

			CleanupStack::PopAndDestroy(backupFile);
			CleanupStack::PopAndDestroy(removedFile);
			iBackupFilesCount++;
			iLastEvent = ERemovedFile;
			}
			break;
			
		case EBackupFile:
			// cannot have a backup file on its own
			User::Leave(KErrCorrupt);
			break;
			
		case ETempFile:
			if(iLastEvent != ENone && iLastEvent != ERemovedFile &&
				iLastEvent != ETempFile && iLastEvent != EAddedFile)
				{
				User::Leave(KErrCorrupt);
				}
			
			tempFile = HBufC::NewLC(aJournalStream, KMaxFileName);
			// make the file is located on the same drive as the journal
			if(CJournalFile::CheckFileNameL(iFs, *tempFile) != aDrive)
				{
				User::Leave(KErrCorrupt);
				}
			iTreeRoot->AddNodeL(*tempFile, ETempFile, iJournalFileName);
			CleanupStack::PopAndDestroy(tempFile);
			iLastEvent = ETempFile;
			break;
			
		case EAddedFile:
			if(iLastEvent != ENone && iLastEvent != ERemovedFile &&
				iLastEvent != ETempFile && iLastEvent != EAddedFile)
				{
				User::Leave(KErrCorrupt);
				}
			
			addedFile = HBufC::NewLC(aJournalStream, KMaxFileName);
			// make the file is located on the same drive as the journal
			if(CJournalFile::CheckFileNameL(iFs, *addedFile) != aDrive)
				{
				User::Leave(KErrCorrupt);
				}
			iTreeRoot->AddNodeL(*addedFile, EAddedFile, iJournalFileName);
			CleanupStack::PopAndDestroy(addedFile);
			iLastEvent = EAddedFile;
			break;
			
		case EInstallComplete:
			if(iLastEvent != ERemovedFile && iLastEvent != ETempFile &&
				iLastEvent != EAddedFile)
				{
				User::Leave(KErrCorrupt);
				}
				
			iLastEvent = EInstallComplete;
			break;
			
		case EBackupFilesRemoved:
			if(iLastEvent != EInstallComplete)
				{
				User::Leave(KErrCorrupt);
				}
			iLastEvent = EBackupFilesRemoved;
			break;
			
		case EAddedFilesRemoved:
			if(iLastEvent != EInstallComplete && iLastEvent != ERemovedFile &&
				iLastEvent != ETempFile && iLastEvent != EAddedFile)
				{
				User::Leave(KErrCorrupt);
				}
			iLastEvent = EAddedFilesRemoved;
			break;
			
		case ETempFilesRemoved:
			if(iLastEvent != EBackupFilesRemoved || iLastEvent != ERemovedFilesRestored)
				{
				User::Leave(KErrCorrupt);
				}
			iLastEvent = ETempFilesRemoved;
			break;
			
		case ERemovedFilesRestored:
			if(iLastEvent != EAddedFilesRemoved)
				{
				User::Leave(KErrCorrupt);
				}
			iLastEvent = ERemovedFilesRestored;
			break;
			
		default:
			User::Leave(KErrCorrupt);
			break;
   		}
	}

void CJournalFile::PrepareToWriteL()
	{
	RFile file;
	CleanupClosePushL(file);
	// try opening the journal if it already exists
	TInt err = file.Open(iFs, iJournalFileName, EFileWrite | EFileShareAny);
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			// journal does not exist, try creating one
			User::LeaveIfError(file.Create(iFs, iJournalFileName, EFileWrite | EFileShareAny));
			}
		else
			{
			User::Leave(err);
			}
		}
	
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	
	// attach to end of file for writing
	iWriteStream.Attach(file, fileSize);
	CleanupStack::Pop(&file); // file ownership transfered to stream
	
	// record the last known size.
	iLastSize = fileSize;
	}

void CJournalFile::EventL(TIntegrityServicesEvent aEvent)
	{
	iWriteStream.WriteInt32L(static_cast<TInt> (aEvent));
	iWriteStream.CommitL();
	
	iLastEvent = aEvent;
	iLastSize = iWriteStream.Sink()->SizeL();
	}

void CJournalFile::AddL(const TDesC& aFileName)
	{
	iTreeRoot->AddNodeL(aFileName, EAddedFile, iJournalFileName);

	// write filename to journal
	iWriteStream.WriteInt32L(static_cast<TInt> (EAddedFile));
	iWriteStream << aFileName;
	iWriteStream.CommitL();
	
	iLastEvent = EAddedFile;
	iLastSize = iWriteStream.Sink()->SizeL();
	}

void CJournalFile::RemoveL(const TDesC& aFileName, TDes& aBackupFileName)
	{
	// Only journal removals if the file hasn't been added in the same
	// journalfile.
	TInt err = iTreeRoot->FindNode(aFileName, EAddedFile);
	if (err == KErrNotFound)
		{
		CIntegrityTreeLeaf* removalNode = iTreeRoot->AddNodeL(aFileName, ERemovedFile, iJournalFileName);

		// generate the next backup filename for this journal
		NextBackupFileNameL(aBackupFileName);
	
		CIntegrityTreeLeaf* backupNode = iTreeRoot->AddNodeL(aBackupFileName, EBackupFile, iJournalFileName);
	
		// set peers on both nodes
		removalNode->SetPeer(backupNode);
		backupNode->SetPeer(removalNode);
		
		// write filenames to journal
		iWriteStream.WriteInt32L(static_cast<TInt> (ERemovedFile));
		iWriteStream << aFileName;
		iWriteStream.WriteInt32L(static_cast<TInt> (EBackupFile));
		iWriteStream << aBackupFileName;
		iWriteStream.CommitL();
	
		iLastEvent = ERemovedFile;
		iLastSize = iWriteStream.Sink()->SizeL();
		}
	else
		{
		User::LeaveIfError(err);
		}
	}
	
void CJournalFile::TemporaryL(const TDesC& aFileName)
	{
	iTreeRoot->AddNodeL(aFileName, ETempFile, iJournalFileName);
	
	// write filename to journal
	iWriteStream.WriteInt32L(static_cast<TInt> (ETempFile));
	iWriteStream << aFileName;
	iWriteStream.CommitL();
	
	iLastEvent = ETempFile;
	iLastSize = iWriteStream.Sink()->SizeL();
	}
	
void CJournalFile::SynchL()
	{
	// check if the journal is in synch with the underlying media
	iWriteStream.Sink()->SynchL();
	TInt size = iWriteStream.Sink()->SizeL();
	if (size != iLastSize)
		{
		// it isn't in synch! read from the underlying media.
		delete iTreeRoot;
		iTreeRoot = NULL;
		iWriteStream.Close();
		iBackupFilesCount = 0;
		
		TChar drive;
		User::LeaveIfError(iFs.DriveToChar(iDrive, drive));
		
		TBuf<2> driveSpec;
		driveSpec.Append(drive);
		driveSpec.Append(KDriveDelimiter);
		iTreeRoot = CIntegrityTreeNode::NewL(driveSpec);
		
		ReadL();
		PrepareToWriteL();
		}
	}	
	
void CJournalFile::JournalOperationL(TTreeWalkFunctionL aFunc, TIntegrityServicesEvent aTypeFilter,
	CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	CFileMan* fileman = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileman);
	iTreeRoot->WalkTreeL(aFunc, aTypeFilter, iFs, iLoader, *fileman, aIntegrityServices, aFailType);
	CleanupStack::PopAndDestroy(fileman);
	}

void CJournalFile::NextBackupFileNameL(TDes& aBackupFileName)
	{
	TParsePtrC fileNamePtr(iJournalFileName);
	aBackupFileName = fileNamePtr.DriveAndPath();
	aBackupFileName.Append(fileNamePtr.Name());
	aBackupFileName.Append(KPathDelimiter);
	aBackupFileName.AppendNumUC(iBackupFilesCount, EHex);
	aBackupFileName.Append(KExtDelimiter);
	aBackupFileName.Append(KBackupExt);
	
	iBackupFilesCount++;
	}


TIntegrityServicesEvent CJournalFile::LastEvent() const
	{
	return iLastEvent;
	}

TInt CJournalFile::Drive()
	{
	return iDrive;
	}
	
void CJournalFile::Close()
	{
	iWriteStream.Close();
	}

/*static*/ TInt CJournalFile::CheckFileNameL(RFs&, const TDesC& aFileName)
	{
	TParse parse;
	// We allow wildcards because this is also used to generate the
	// uninstall log which may contain FN entries using wildcards.
	User::LeaveIfError(parse.Set(aFileName, NULL, NULL));
	User::LeaveIfError(parse.DrivePresent());
	User::LeaveIfError(parse.PathPresent());
	
	TDriveUnit drive(parse.Drive());
	return drive;
	}
