/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "operationfunctions.h"
#include "usiflog.h"

#include <f32file.h>

using namespace Usif;

_LIT(KTransactionPath, "\\sys\\install\\integrityservices\\");

CIntegrityServices::TFailType CIntegrityServices::iFailType = EFailNone;
CIntegrityServices::TFailPosition CIntegrityServices::iFailPosition = EBeforeJournal;
TFileName CIntegrityServices::iFailFileName;
TBool CIntegrityServices::iIsFailureTestingEnabled = EFalse;


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

 CIntegrityServices* CIntegrityServices::NewL(TStsTransactionId aTransactionID)
	{
	CIntegrityServices* self = CIntegrityServices::NewLC(aTransactionID);
	CleanupStack::Pop(self);
	return self;
	}

 CIntegrityServices* CIntegrityServices::NewLC(TStsTransactionId aTransactionID)
	{
	CIntegrityServices* self = new(ELeave) CIntegrityServices(aTransactionID);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

 CIntegrityServices::CIntegrityServices(TStsTransactionId aTransactionID) : iTransactionID(aTransactionID)
	{
	}

 CIntegrityServices::~CIntegrityServices()
	{
	delete iJournal;
	iFs.Close();
	
	iLoader.Close();
	}

 void CIntegrityServices::ConstructL()
	{
	DEBUG_PRINTF2(_L("CIntegrityServices::ConstructL() - Opening session with  Session ID %X."), iTransactionID);
	
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected()); //needed as new RFiles are to be passed back to client's side
	User::LeaveIfError(iLoader.Connect());

	// store the journal path and create the journal
	TParsePtrC pathPtr(KTransactionPath);
	iJournalPath = pathPtr.Path();
	iJournal = CJournal::NewL(iFs, iLoader, iTransactionID, iJournalPath);
	iSystemDrive = ::RFs::GetSystemDrive();
	}
 
const TInt KIntegrityServicesSimulatedBatteryFailure=-10205; 
 
/*static*/ void CIntegrityServices::SimulatePowerFailureL(TFailType aFailType, TFailPosition aFailPosition, const TDesC& aFailFileName)
	{
	if (!iIsFailureTestingEnabled)
		return;
	
	if(iFailType == aFailType && iFailPosition == aFailPosition && iFailFileName == aFailFileName)
		{
		User::Leave(KIntegrityServicesSimulatedBatteryFailure);
		}
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

 void CIntegrityServices::RegisterNewL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::RegisterNewL() - Session %X, File: %S."),	iTransactionID, &aFileName);

	HBufC* localFilenameHeap = aFileName.AllocLC();
	TPtr localFilename = localFilenameHeap->Des();
	NormalizeDirectoryName(localFilename); // If it is a directory name, make sure to normalize it
		
	// Record the added file or directory in the journal
	SimulatePowerFailureL(EFailAddingNewFile, EBeforeJournal, aFileName);
	iJournal->AddL(localFilename);
	SimulatePowerFailureL(EFailAddingNewFile, EAfterJournal, aFileName);
	CleanupStack::PopAndDestroy(localFilenameHeap);
	}

 void VerifyMkDirErrorL(TInt err)
	{
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}	
	}
 
 void ProcessNewFileRegistrationResultL(TInt err, RFs& aFs, const TDesC& aFileName, RFile& aFile)
	{
	if(err!= KErrNone)
		{
		//if we hit this point it means we successfully created the new file however registering with the transaction has  failed
		//so we have to remove the new file to make the journal and the file system consistent
		aFile.Close();
		aFs.Delete(aFileName);
		User::Leave(err);
		}	
	}

 void CIntegrityServices::CreateNewL(const TDesC& aFileName, RFile &newFile, TUint aFileMode)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::CreateNewL() - Session %X, File: %S."),	iTransactionID, &aFileName);
	TInt err = iFs.MkDirAll(aFileName); //first we have to create the full directory path to aFileName otherwise RFs::Create will fail
	VerifyMkDirErrorL(err);
	User::LeaveIfError(newFile.Create(iFs, aFileName, aFileMode));
	TRAPD(regResult, RegisterNewL(aFileName));	
	ProcessNewFileRegistrationResultL(regResult, iFs, aFileName, newFile); //checks if the registration failed and cleans up the file in the filesystem if it did
	}

 void CIntegrityServices::RemoveL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::RemoveL() - Session %X, File: %S."), iTransactionID, &aFileName);

	// before doing anything check that the file or directory exists
	TEntry entry;
	
	TInt res = iFs.Entry(aFileName, entry);
	if (res == KErrNotFound || res == KErrPathNotFound)
		return; // If the file is not present, do nothing. Returning an error would require the user of the API to do additional checks
	User::LeaveIfError(res);

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
		VerifyMkDirErrorL(err);

		SimulatePowerFailureL(EFailRemovingFile, EBeforeAction, aFileName);
			err = iFs.Rename(localFilename, backupFileName);
			DEBUG_PRINTF4(_L("CIntegrityServices::RemoveL() - Renamed %S as %S error %d"), &localFilename, &backupFileName, err);
			User::LeaveIfError(err);
		SimulatePowerFailureL(EFailRemovingFile, EAfterAction, aFileName);
		}
	else
		{
		DEBUG_PRINTF2(_L("CIntegrityServices::RemoveL() - %S already backed up"), &aFileName);
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

 void CIntegrityServices::RegisterTemporaryL(const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::RegisterTemporaryL() - Session %X, File: %S."), iTransactionID, &aFileName);

	// record the temporary file or directory in the journal
	SimulatePowerFailureL(EFailAddingTempFile, EBeforeJournal, aFileName);
	iJournal->TemporaryL(aFileName);
	SimulatePowerFailureL(EFailAddingTempFile, EAfterJournal, aFileName);
	}

 void CIntegrityServices::CreateTemporaryL(const TDesC& aFileName, RFile &newFile, TUint aFileMode)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::CreateTemporaryL() - Session %X, File: %S."), iTransactionID, &aFileName);

	TInt err = iFs.MkDirAll(aFileName); //first we have to create the full directory path to aFileName otherwise RFs::Create will fail
	VerifyMkDirErrorL(err);
	User::LeaveIfError(newFile.Create(iFs, aFileName, aFileMode));
	TRAPD(regResult, RegisterTemporaryL(aFileName));
	ProcessNewFileRegistrationResultL(regResult, iFs, aFileName, newFile);
	}

 void CIntegrityServices::OverwriteL(const TDesC& aFileName, RFile &newFile, TUint aFileMode)
	{
	DEBUG_PRINTF3(_L("CIntegrityServices::OverwriteL() - Session %X, File: %S."), iTransactionID, &aFileName);

	TBool b;
	TInt err;
	if((err=iFs.IsFileOpen(aFileName, b))== KErrNone) //returned error code shows whether the file exists or not; the bool value is ignored
		{
		//file exists remove first
		RemoveL(aFileName);
		}
	else
		{
		if(err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	CreateNewL(aFileName, newFile, aFileMode);
	}

 void CIntegrityServices::CommitL()
	{
	DEBUG_PRINTF2(_L("CIntegrityServices::CommitL() - Session %X."), iTransactionID);	
	iJournal->CommitL();
	}

 void CIntegrityServices::RollBackL(TBool aRecordAllRollbackEvents /* = ETrue */)
	{
	DEBUG_PRINTF2(_L("CIntegrityServices::RollBackL() - transaction %X"), iTransactionID);
	iJournal->RollBackL(aRecordAllRollbackEvents);
	}

/*static*/ void CIntegrityServices::GetListOfTransactionsL(RArray<TStsTransactionId>& aIdArray)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TDriveUnit systemDrive(::RFs::GetSystemDrive());
	RBuf fileSpec;
	fileSpec.CreateL(systemDrive.Name(), KMaxFileName);
	CleanupClosePushL(fileSpec);
	fileSpec.Append(KTransactionPath);
	fileSpec.Append(KMatchAny);
	fileSpec.Append(KExtDelimiter);
	fileSpec.Append(KDriveExt);

	RDir dir;
	TInt err = dir.Open(fs, fileSpec, KEntryAttNormal);
	CleanupStack::PopAndDestroy(&fileSpec);
	if (err == KErrPathNotFound || err == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(&fs);
		return; // These errors are not considered fatal - there may be no journals present
		}
	User::LeaveIfError(err);
	CleanupClosePushL(dir);
	
	CEntryArray* entryArrayContainer = new (ELeave) CEntryArray;
	TEntryArray& entryArray = (*entryArrayContainer)();
	err = dir.Read(entryArray);
	CleanupStack::PopAndDestroy(&dir);
	CleanupStack::PushL(entryArrayContainer);	
	if (err != KErrNone && err != KErrEof)
		{
		User::Leave(err);
		}
	TInt entryCount(entryArray.Count());
	for (TInt index = 0; index < entryCount; ++index)
		{
		TStsTransactionId transactionID;
		if (CJournal::RecoverTransactionIdFromDrvFileName(
				entryArray[index].iName, transactionID) == KErrNone)
			{
			aIdArray.AppendL(transactionID);
			}
		}
	CleanupStack::PopAndDestroy(entryArrayContainer);
	CleanupStack::PopAndDestroy(&fs);
	}

/*static*/ void CIntegrityServices::RollbackAllL()
	{
	RArray<TStsTransactionId> transactionIDs;
	CleanupClosePushL(transactionIDs);
	CIntegrityServices::GetListOfTransactionsL(transactionIDs);
	TInt numberOfTransactions(transactionIDs.Count());
	TInt lastError=KErrNone;
	DEBUG_PRINTF2(_L("CIntegrityServices::RollbackAllL() %d transactions have been found."), numberOfTransactions );							
	for(TInt i=0; i<numberOfTransactions; ++i)
		{
		DEBUG_PRINTF2(_L("CIntegrityServices::RollbackAllL() Trying to roll back transaction %X"), transactionIDs[i]);
		TRAPD(err,
			CIntegrityServices* transactionPtr = CIntegrityServices::NewLC(transactionIDs[i]);
			transactionPtr->RollBackL(EFalse); // Specify not to record roll back events. If we failed in the middle of a previous roll back, due to lack of resources we shouldn't be trying to record more events
			CleanupStack::PopAndDestroy(transactionPtr);
			); //failing to roll back one transaction shouldn't affect the overall rollback all procedure
		if(err!=KErrNone)
			{
			lastError=err; //remember last error and leave with that error code indicating an error in the overall procedure
			}
		DEBUG_PRINTF2(_L("CIntegrityServices::RollbackAllL() Rolled back transaction %X"), transactionIDs[i]);			
		}
	CleanupStack::PopAndDestroy(&transactionIDs);
	User::LeaveIfError(lastError);
	}
