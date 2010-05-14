/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of CJournal
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __JOURNAL_H__
#define __JOURNAL_H__

#include <e32base.h>
#include <usif/sts/stsdefs.h>
#include "journalfile.h"
#include "integrityservicesevent.h"

class RFs;

namespace Usif
{

_LIT(KJournalExt, "log"); // extension for journal files on each drive
_LIT(KDriveExt, "drv"); // extension for this transaction's drive array

/**
 * The journal stores integrity services events and their related filenames
 * in files spread across several drives. Each drive has its own journal file
 * so that recovery of drives can occur independantly. This class handles both
 * the reading and writing of these files. The filename of each journal is:
 *
 * JournalPath|TransactionID
 *   |--------|--------|
 * ?:\????????\00000000.log
 *
 * @released
 * @internalTechnology 
 */
class CJournal : public CBase
	{
	public:

		/**
		 * Constructs a new CJournal
		 *
		 * @code
		 *
		 *	TTime currentTime;
		 *	currentTime.UniversalTime();
		 *	_LIT(KIntegrityServicesPath, "\\private\\SID\\");
		 *	iIntegrityServices = CIntegrityServices::NewL(currentTime.Int64(), 
		 *											KIntegrityServicesPath);
		 *
		 * @endcode
		 *
		 * @param aFs reference to the file system provided by CIntegrityServices
		 * @param aLoader reference to the RLoader provided by CIntegrityServices
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		static CJournal* NewL(RFs& aFs, RLoader& aLoader, TStsTransactionId aTransactionID, const TDesC& aPath);
		
		/**
		 * Constructs a new CJournal and places it on the cleanup stack
		 *
		 * @code
		 *
		 *	TTime currentTime;
		 *	currentTime.UniversalTime();
		 *	_LIT(KIntegrityServicesPath, "\\private\\SID\\");
		 *	iIntegrityServices = CIntegrityServices::NewL(currentTime.Int64(), 
		 *											KIntegrityServicesPath);
		 *
		 * @endcode
		 *
		 * @param aFs reference to the file system provided by CIntegrityServices
		 * @param aLoader reference to the RLoader provided by CIntegrityServices
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		static CJournal* NewLC(RFs& aFs, RLoader& aLoader, TStsTransactionId aTransactionID, const TDesC& aPath);
		
		virtual ~CJournal();

		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * so that it can be removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		void AddL(const TDesC& aFileName);
		
		/**
		 * Removes the specified file or directory, first backing it up before
		 * deleting it. A record is created in the journal file on the
		 * appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 * @param aFileName - the generated backup filename to return
		 */
		void RemoveL(const TDesC& aFileName, TDes& aBackupFileName);
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * that must later be removed. A record is created in the journal file
		 * on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		void TemporaryL(const TDesC& aFileName);
		
		/** 
		 * Commits all journals for this transaction 
		 * */
		void CommitL();		
				
		/**
		 * Performs a file restore operation on a single drive
		 * 
		 * @param aDrive The drive to apply this operation to
		 */
		
		void RestoreFilesL(TInt aDrive);
			
		/**
		 * Rolls back the journal on all drives
		 @see CIntegrityServices::RollbackL on explanation for aRecordAllRollbackEvents
		 */
		void RollBackL(TBool aRecordAllRollbackEvents = ETrue);
		
		/**
		 * Creates the <sysdrive>:\<journal path>\<transactionID>.drv file's name 
		 * This file is used by the journal to keep track of affected drives.
		 * @param aTransactionID The transaction id for which the filename is to be assembled
		 * @param aPath The journal path to be used when assembling the filename.
		 * @param aDrvFileName The assembled fully qualfied filename is returned in this descriptor
		 */
		 static void CreateDrvFileNameL(TStsTransactionId aTransactionID, const TDesC& aPath, TDes& aDrvFileName);
			
		/**
		 * Creates drive independent journal file name eg: \<journal path>\<transactionID>.<journalfileextension> and appends to aJournalFileName  
		 * @param aTransactionID The transaction id for which the filename is to be assembled
		 * @param aPath The journal path to be used when assembling the filename.
		 * @param aJournalFileName The assembled drive independent filename is appended to the end of this descriptor
		 */
		 static void CreateJournalFileNameL(TStsTransactionId aTransactionID, const TDesC& aPath, TDes& aJournalFileName);
		 
		 static TInt RecoverTransactionIdFromDrvFileName(const TDesC& aDrvFileName, TStsTransactionId& aTransactionID);
		 
	private:
	
		CJournal(RFs& aFs, RLoader& aLoader);
		
		/**
		 * Second phase constructor for CJournal
		 *
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		void ConstructL(TStsTransactionId aTransactionID, const TDesC& aPath);

		/**
		 * Closes then deletes all files used by this journal. Records completed
		 * drives to the drives array.
		 */
		void DeleteJournalFilesL();
		
		/**
		 * Removes the journal file from the indicated drive if possible.
		 * @see CIntegrityServices::RollbackL on explanation for aRecordAllRollbackEvents		 
		 *
		 */

		void DeleteJournalFileL(TInt aDrive, TBool aRecordAllRollbackEvents = ETrue );
		
		/**
		 * Removes the drives file from the system drive for this journal.
		 *
		 */
		
		void DeleteDrivesFileL();
		
		/**
		 * Reads the array of drives used by this transaction from a file.
		 */
		void RefreshDrivesArrayL();
		
		/**
		 * Opens a CJournalFile object on this drive, locking the journal file
		 * and preventing use by other instances of CIntegrityServices using
		 * the same Transaction ID. If a journal file does not exist it will
		 * be created.
		 *
		 * @param aDrive the drive on which to open the journal file
		 */
		TInt PrepareToWriteL(TInt aDrive);
		
		/**
		 * Adds the specified drive to the drives array used by this
		 * transaction an records it in a file.
		 *
		 * @param aDrive the drive to add
		 */
		void UpdateDrivesFileL(TInt aDrive);
		
		/**
		 * Performs the initial read of the drives file, and creates journal file
		 * objects for every present drive already used in this transaction.
		 */
		void InitJournalsL();
		
		/**
		 * Closes, then deletes the journal file opened with StartRollbackL().
		@see CIntegrityServices::RollbackL on explanation for aRecordAllRollbackEvents		 
		 */
		void FinishRollbackL(TInt aDrive, TBool aRecordAllRollbackEvents = ETrue );
		
		/**
		 * Returns the last event found in the journal files.
		 * (used to determine how far the installation progressed)
		 *
		 * @return TIntegrityServicesEvent representing the last event in the journal
		 */
		TIntegrityServicesEvent LastEventL() const;
				
		/**
		 * Performs a file delete operation on all drives referenced by this journal
		 * 
		 * @param aTypeFilter The type of file to apply this operation to
		 * @param aFailType The type of test failure to induce during testing
		 */	
		void DeleteFilesL(TIntegrityServicesEvent aTypeFilter);

		/**
		 * Performs a file delete operation on a single drive
		 * 
		 * @param aTypeFilter The type of file to apply this operation to
		 * @param aDrive The drive to apply this operation to
		 * @param aFailType The type of test failure to induce during testing
		 */
		
		void DeleteFilesL(TIntegrityServicesEvent aTypeFilter, TInt aDrive);		
		
		/**
		 * Write a journal event to a specific drive
		 *
		 * @param aEvent the TEvent describing the current operation
		 * @param aDrive The drive to write the event to
		 * @param aSerializeEventToJournal Specifies whether the journal event should be serialized
		 */
		 
		void WriteJournalEventL(TIntegrityServicesEvent aEvent, TInt aDrive, TBool aSerializeEventToJournal);
		
  		/**
  		 * Performs recovery of the journal for a particular drive.
  		 * 
  		 * @param aDrive	The drive to rollback
		 * @param aRecordAllRollbackEvents @see CIntegrityServices::RollbackL
  		 */
  		void RollBackDriveL(TInt aDrive, TBool aRecordAllRollbackEvents);
  		
		/**
		 * Writes the event to all journal files
		 *
		 * @param aEvent the TEvent describing the current operation
		 */
		void WriteJournalEventL(TIntegrityServicesEvent aEvent);
		
		/**
		 * Opens all journal file(s) belonging to this transaction, first
		 * reading and verifying the existing contents. The files remain
		 * open (and therefore locked) until FinishCommitL() is called.
		 */
		void StartCommitL();
		
		/*
		 * Closes, then deletes the journal file(s) opened with StartCommitL().
		 */
		void FinishCommitL();

		/**
		 * Retrieves the last event from the journal on a specific drive
		 *
		 * @param aDrive The drive for which to retrieve the last event
		 * @return TIntegrityServicesEvent representing the last event in the journal
		 */
		
		TIntegrityServicesEvent LastEventL(TInt aDrive) const;		

		/**
		 * Retrieves the matching index in iJournalFiles for a journal which matches aDrive. Returns KErrNotFound if there's no journal for the drive
		 */

		TInt FindJournalFileIndexL(TInt aDrive) const;
		
	    /**
		 * Reference to opened file server session
		 */
		RFs& iFs;
		
		/**
		 * Reference to RLoader server session
		 */
		RLoader& iLoader;
		/**
		 * The generic journal filename (does not include a drive)
		 */
		TFileName iJournalFileName;
		
		/**
		 * The filename for the drives array (located on the system drive)
		 * This file stores the drives used, and drives completed for this
		 * transaction.
		 */
		TFileName iDriveArrayFileName;

		/**
		 * The array of completed drives for this transaction
		 */
		RArray<TInt> iCompletedDrives;
		 
		RArray<TInt> iAllDrives;
		
		/**
		 * Array of CJournalFile objects
		 */
		RPointerArray<CJournalFile> iJournalFiles;
	};

} ///namespace
#endif
