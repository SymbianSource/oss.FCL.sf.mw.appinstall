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
* Definition of CIntegrityServices
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __INTEGRITYSERVICES_H__
#define __INTEGRITYSERVICES_H__

#include <e32base.h>
#include <f32file.h>
#include <e32ldr_private.h>

#include "integrityservicesevent.h"



namespace Swi
{
class CJournal;
class CIntegrityTreeNode;

/**
 * This class maintains the integrity of installed software by ensuring that
 * the device is always left in a consistent state. If a software modification
 * process (install, upgrade or uninstall) is interrupted then that process is
 * reverted, returning the device to its original state with no orphaned or
 * missing files.
 *
 * @released
 * @internalTechnology 
 */
class CIntegrityServices : public CBase
	{
	public:

		/**
		 * Constructs a new CIntegrityServices object specifying a path for
		 * the journal files
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
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		IMPORT_C static CIntegrityServices* NewL(TInt64 aTransactionID, const TDesC& aPath);
		
		/**
		 * Constructs a new CIntegrityServices object specifying a path for
		 * the journal files and puts it on the cleanup stack
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
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		IMPORT_C static CIntegrityServices* NewLC(TInt64 aTransactionID, const TDesC& aPath);
		
		IMPORT_C virtual ~CIntegrityServices();
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * so that it can be removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		IMPORT_C void AddL(const TDesC& aFileName);
		
		/**
		 * Removes the specified file or directory, first backing it up before
		 * deleting it. A record is created in the journal file on the
		 * appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		IMPORT_C void RemoveL(const TDesC& aFileName);
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * that must later be removed. A record is created in the journal file
		 * on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		IMPORT_C void TemporaryL(const TDesC& aFileName);

		/**
		 * Commits the current transaction by deleting backup, temporary and
		 * journal files. The journal files are first refreshed so that
		 * operations shared between processes and spread across multiple
		 * drives are committed at the same time. If any journal file from this
		 * transaction is not present or has already been rolledback the
		 * commit will fail.
		 */
		IMPORT_C void CommitL();
		
		/**
		 * Starts the recovery process for all drives.
		 * Drive are rolled back independantly since removable media may be at
		 * a different state to internal drives (which may have already been 
		 * rolled back).
		 *
		 * @param aAllTransactions if ETrue all transactions in the current
		 * 						   path are rolled back otherwise only this
		 *						   transaction.
		 */
		IMPORT_C void RollBackL(TBool aAllTransactions);
		
		/**
		 * Returns the TransactionID
		 *
		 * @return a TInt64 representing the transaction
		 */
		inline TInt64 TransactionId() const;


		/**
		 * Test if any journal files have started being rolled back.
		 *
		 * @return ETrue if any journal files have started being rolled back
		 *         or EFalse otherwise.
		 */
		inline TBool StartedJournalRollback() const;
	
		/**
		 * Failure types - indicate when to simulate power failure during
		 * testing
		 */
		enum TFailType
			{
			EFailNone,
			EFailAddingNewFile,
			EFailRemovingFile,
			EFailAddingTempFile,
			EFailRestoringFile,
			EFailDeletingFile,
			EFailInstallComplete,
			EFailNewFilesRemoved,
			EFailOldFilesRestored,
			EFailTempFilesRemoved,
			EFailBackupFilesRemoved,
			};
		
		/**
		 * Failure position - indicate when to simulate power failure during
		 * testing
		 */	
		enum TFailPosition
			{
			EBeforeJournal,
			EAfterJournal,
			EBeforeAction,
			EAfterAction
			};
		
	protected:
		/**
		 * Constructor for CIntegrityServices
		 *
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 */
		IMPORT_C CIntegrityServices(TInt64 aTransactionID);
		
		/**
		 * Second phase constructor for CIntegrityServices
		 *
		 * @param aPath			 The path in which to read and write journal
		 *						 files. eg "\\private\\SID\\"
		 */
		IMPORT_C void ConstructL(const TDesC& aPath);
		
		/**
		 * Function only implemented in derived test class.
		 *
		 * @param aFailType		 The operation on which to fail
		 * @param aFailPosition	 The position at which to fail
		 * @param aFailFileName	 The filename on which to fail
		 */
		virtual void SimulatePowerFailureL(TFailType aFailType, TFailPosition aPosition, const TDesC& aFailFileName);
				
	private:

		/**
		 * Recovers the specified journal by either rolling back a failed
		 * installation or completing it if past the point of no return
		 * (ie backup files deleted). Processes the journal file on each drive
		 * used by this transaction one after the other.
		 *
		 * @param aJournal	The journal to rollback
		 *
		 */
		void RollBackJournalL(CJournal& aJournal);

  		/**
  		 * Performs recovery of the journal for a particular drive.
  		 * 
  		 * @param aJournal	The journal to rollback
  		 * @param aDrive	The drive to rollback
  		 */
  		void RollBackDriveL(CJournal& aJournal, TInt aDrive);

		/**
		 * Restores backup files to their original location
		 *
		 * @param aJournal	The journal for which files are to be restored
		 */
		void RestoreFilesL(CJournal& aJournal, TInt aDrive = -1);

		/**
		 * Function deletes all files in the list but does NOT fail 
		 * if a file cannot be found
		 *
		 * @param aJournal	The journal for which files are to be deleted
		 * @param aEvent	Files corresponding to this event are deleted.
		 */
		void DeleteFilesL(CJournal& aJournal, TIntegrityServicesEvent aEvent, TInt aDrive = -1);

		/**
		 * Removes a trailing slash from directory name, if needed.
		 * 
		 * @param aFileName the filename to modify. If the filename does not represent a directory, it is not modified
		 */
		static void NormalizeDirectoryName(TDes& aFileName);
		
		/**
		Creates a backup file by copying the source to a defined backup name. This MUST be used for executables.
		The source files will then be deleted by invoking RLoader::Delete
		@param aSource	the file to backup
		@param aBackup	the name of the backup file
		*/
		void CopyToBackupL(const TDesC& aSource, const TDesC& aBackup);

	private:
	
	    /**
		 * Pointer to the journal - uses log file(s) for persistant storage
		 * A log file is created on each drive involved so that they can be
		 * recovered independantly.
		 */
		CJournal* iJournal;
		
		/**
		 * Provided by the client to identify this transaction.
		 */
		TInt64 iTransactionID;
		
		/**
		 * The supplied path in which to read and write journal files.
		 */
		TPath iJournalPath;

		/**
		 * True if rollback has been started on at least one journal file.
		 */
		 TBool iStartedJournalRollback;
		 
		 /**
		 The drive number for the system drive.
		 */
		 TDriveNumber iSystemDrive;
		 
	protected:
	
		RFs iFs;
		
		RLoader iLoader;
			
	    /** 
		 * Failure type (used only by test code)
		 */
		TFailType iFailType;
		
		/** 
		 * Failure position (used only by test code)
		 */
		TFailPosition iFailPosition;
		
		/** 
		 * Specify the name of the file to fail on (used only in test code)
		 */
		TFileName iFailFileName;
		
		friend class CIntegrityTreeNode;
	};

inline TInt64 CIntegrityServices::TransactionId() const
	{
	return iTransactionID;
	}

inline TBool CIntegrityServices::StartedJournalRollback() const
	{
	return iStartedJournalRollback;
	}

} //namespace
#endif
