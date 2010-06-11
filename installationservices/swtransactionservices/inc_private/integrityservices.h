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
#include <usif/sts/stsdefs.h>
#include "integrityservicesevent.h"


class RFs;

namespace Usif
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
		 */
		 static CIntegrityServices* NewL(TStsTransactionId aTransactionID);
		
		/**
		 * Constructs a new CIntegrityServices object specifying a path for
		 * the journal files and puts it on the cleanup stack
		 * @param aTransactionID A unique ID provided by the client to
		 *						 identify this transaction. It is suggested
		 *						 that the client use the current time as the
		 *						 unique ID. This value can then be shared
		 *						 between different processes so that they use
		 *						 the same journal.
		 */
		 static CIntegrityServices* NewLC(TStsTransactionId aTransactionID);
		
		 ~CIntegrityServices();
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * so that it can be removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		 void RegisterNewL(const TDesC& aFileName);

		/**
		 * Instructs Integrity Services to create a new file. 
		 * It is removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 * @param newFile - Reference of a file handle that is set to the newly created file
		 * @param aFileMode - File creation mode of the new file (see TFileMode documentation
		 *                    for the possible mode or combination of modes)
		 */
		 void CreateNewL(const TDesC& aFileName, RFile &newFile, TUint aFileMode);
		
		/**
		 * Removes the specified file or directory, first backing it up before
		 * deleting it. A record is created in the journal file on the
		 * appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		 void RemoveL(const TDesC& aFileName);
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * that must later be removed. A record is created in the journal file
		 * on the appropriate drive.
		 *
		 * @param aFileName - Name of file including path
		 */
		 void RegisterTemporaryL(const TDesC& aFileName);

		/**
		 * Instructs Integrity Services to create a new temporary file. 
		 * It is removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file including path
		 * @param newFile - Reference of a file handle that is set to the newly created file
		 * @param aFileMode - File creation mode of the new file (see TFileMode documentation
		 *                    for the possible mode or combination of modes)
		 */
		 void CreateTemporaryL(const TDesC& aFileName, RFile &newFile, TUint aFileMode);

		/**
		 * Instructs Integrity Services to create a new file.
		 * If the file already exists it's removed to a backup location first before the new file is created. 
		 * The newly created file is removed if a rollback occurs and the old one is restored. 
		 * A record is created in the journal file on the appropriate drive.
		 * @param aFileName - Name of file or directory including path
		 * @param newFile - Reference of a file handle that is set to the newly created file
		 * @param aFileMode - File creation mode of the new file (see TFileMode documentation
		 *                    for the possible mode or combination of modes)
		 */
		 void OverwriteL(const TDesC& aFileName, RFile &newFile, TUint aFileMode);

		/**
		 * Commits the current transaction by deleting backup, temporary and
		 * journal files. The journal files are first refreshed so that
		 * operations shared between processes and spread across multiple
		 * drives are committed at the same time. If any journal file from this
		 * transaction is not present or has already been rolledback the
		 * commit will fail.
		 */
		 void CommitL();
		
		/**
		 * Starts the recovery process for all drives.
		 * Drive are rolled back independantly since removable media may be at
		 * a different state to internal drives (which may have already been 
		 * rolled back).
		 *
		 * @param aRecordAllRollbackEvents- This parameter specifies whether we should record events during the rollback.
		 *  This allows continuing the rollback in case it has been interrupted. In most cases, this flag should be "on", however
		 *   if we failed a previous roll back due to low space or low memory, it can prevent any roll back due to lack of resources for recording
		 *   the rollback events. In these cases, it should be set to "off".
		 *
		 */
		 void RollBackL(TBool aRecordAllRollbackEvents = ETrue);

		/**
		 * Returns the TransactionID
		 *
		 * @return a TStsTransactionId representing the transaction
		 */
		inline TStsTransactionId TransactionId() const;

		/**
		 * Scans through the transaction path and returns a list if transaction ids that have been found.
		 *  
		 * @param idArray - an array of TStsTransactionId in which the found ids are returned to the caller
		 */
		static void GetListOfTransactionsL(RArray<TStsTransactionId>& aIdArray);

		/**
		 * Roll back all transactions that it can find in the transaction path in the filesystem
		 * Important note: The function tries the best effort to roll back all transactions found. 
		 *                 Failing to roll back one transaction doesn't influence the roll back of others,
		 *                 however if any of the transactions fail to roll back properly the function finally
		 *                 will leave with the latest error encountered. (After it has tried to roll back all!)    
		 */
		static void RollbackAllL();

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
			
	private:
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
		 CIntegrityServices(TStsTransactionId aTransactionID);
		
		/**
		 * Second phase constructor for CIntegrityServices
		 *
		 */
		 void ConstructL();
						
		/**
		 * Removes a trailing slash from directory name, if needed.
		 * 
		 * @param aFileName the filename to modify. If the filename does not represent a directory, it is not modified
		 */
		static void NormalizeDirectoryName(TDes& aFileName);

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
		TStsTransactionId iTransactionID;
		
		/**
		 * The supplied path in which to read and write journal files.
		 */
		TPath iJournalPath;

		 /**
		 The drive number for the system drive.
		 */
		 TDriveNumber iSystemDrive;
		 
	private:
	
		RFs iFs;
		
		RLoader iLoader;
	
	public:
	    /** 
		 * Failure type (used only by test code)
		 */
		static TFailType iFailType;
		
		/** 
		 * Failure position (used only by test code)
		 */
		static TFailPosition iFailPosition;
		
		/** 
		 * Specify the name of the file to fail on (used only in test code)
		 */
		static TFileName iFailFileName;
		static TBool iIsFailureTestingEnabled;
		
		/**
		 * Test utility function
		 *
		 * @param aFailType		 The operation on which to fail
		 * @param aFailPosition	 The position at which to fail
		 * @param aFailFileName	 The filename on which to fail
		 */
		static void SimulatePowerFailureL(TFailType aFailType, TFailPosition aPosition, const TDesC& aFailFileName);
	};

inline TStsTransactionId CIntegrityServices::TransactionId() const
	{
	return iTransactionID;
	}

} //namespace
#endif
