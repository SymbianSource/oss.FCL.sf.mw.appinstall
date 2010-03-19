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
* Definition of CJournalFile
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __JOURNALFILE_H__
#define __JOURNALFILE_H__

#include <e32base.h>
#include <s32file.h>

#include "integrityservicesevent.h"
#include "integritytree.h"

namespace Usif
{
	
/**
 * The purpose of this class is to wrap a RFileWriteStream so that multiple
 * journal files can be held open by the client. This eliminates the need to 
 * constantly open and close files. 
 *
 * @released
 * @internalTechnology 
 */
class CJournalFile : public CBase
	{
	public:
		
		/**
		 * Constructs a new CJournalFile
		 *
		 * @param aFs reference to the supplied file system session
		 * @param aLoader reference to the RLoader session
		 * @param aFileName the filename of the journal
		 */
		static CJournalFile* NewL(RFs& aFs, RLoader& aLoader, const TDesC& aFileName, TInt aDrive);

		/**
		 * Constructs a new CJournalFile an places it on the cleanup stack
		 *
		 * @param aFs reference to the supplied file system session
		 * @param aLoader reference to the RLoader session
		 * @param aFileName the filename of the journal
		 */
		static CJournalFile* NewLC(RFs& aFs, RLoader& aLoader, const TDesC& aFileName, TInt aDrive);
		
		virtual ~CJournalFile();

		/**
		 * Records an event in this journal file.
		 *
		 * @param aEvent	the event to record
		 * @param aEvent	specifies whether the event should be serialized. If set to False, only the "in-memory" state of the journal is changed
		 This parameter is used in possible low memory or low disk space conditions.
		 */
		void EventL(TIntegrityServicesEvent aEvent, TBool aSerializeEventToJournal = ETrue);
		
		/**
		 * Notifies Integrity Services that a file or directory is being added
		 * so that it can be removed if a rollback occurs. A record is created
		 * in the journal file on the appropriate drive.
		 *
		 * @param aFileName - Name of file or directory including path
		 */
		void AddL(const TDesC& aFileName);
		
		/**
		 * Checks if the file being removed has already been journalled for
		 * adding in the same journal file.  If it has been, nothing is
		 * journalled and the backup file name is set to zero length. Otherwise
		 * a record is journalled for the removal and the backup filename is
		 * set to the appropriate next name to use.
		 *
		 * @param aFileName - Name of file or directory including path
		 * @param aBackupFileName - the generated backup filename to return or
		 *                          zero-length if the file doesn't need to
		 *                          be backed up.
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
		 * Performs an operation on the file tree structure
		 *
		 * @param aFunc A function to apply to all nodes
		 * @param aTypeFilter The type of node to apply the function to
		 * @param aFailType The type of failure to induce, used exclusively in testing.
		 *
		 */
		
		void JournalOperationL(TTreeWalkFunctionL aFunc, TIntegrityServicesEvent aTypeFilter,
			CIntegrityServices::TFailType aFailType);
		
		/**
		 * Returns the last event found in the journal, used to determine how far the 
		 * installation progressed
		 *
		 * @return TIntegrityServicesEvent representing the last event in the file
		 */
		TIntegrityServicesEvent LastEvent() const;
		
		/**
		 * Checks that the filename is valid and complete
		 *
		 * @param aFs a reference to an open file system session
		 * @param aFileName the filename to check
		 * @return TInt representing the drive specified in this filename
		 */
		static TInt CheckFileNameL(RFs& aFs, const TDesC& aFileName);
		
		/**
		 * Accessor method that tells us which drive this journal file is on
		 */
		
		TInt Drive();
				
		/**
		 * Closes any file handle held open by this object. The journal file object
		 * must not be used again after this call.
		 */
		 		
		void Close();
		
	private:
		/**
		 * Constructor for CJournalFile
		 *
		 * @param aFs a reference to the supplied file system session
		 * @param aLoader reference to the RLoader session
		 */
		CJournalFile(RFs& aFs, RLoader& aLoader, TInt aDrive);
		
		/**
		 * Second phase constructor for CJournalFile
		 *
		 * @param aFileName the filename of the journal
		 */
		void ConstructL(const TDesC& aFileName);
		
		/**
		 * Reads and verifies the journal file
		 */
		void ReadL();

		/**
		 * Reads and verifies a journal entry from the supplied stream
		 *
		 * @param aJournalStream the stream to read
		 * @param aDrive used to verify files referred to are only from that drive
		 */
		void ReadEntryL(RFileReadStream& aJournalStream, TInt aDrive);
		
		/**
		 * Prepares to write to the journal file by opening an RFileWriteStream
		 */
		void PrepareToWriteL();
		
		/**
		 * Generates the next backup filename for this journal file.
		 */
		void NextBackupFileNameL(TDes& aBackupFileName);

		/**
		 * Reference to the supplied file system session
		 */
		RFs& iFs;

		/**
		 * Reference to RLoader server session
		 */
		RLoader& iLoader;
		
		/**
		 * The write stream for this journal
		 */
		RFileWriteStream iWriteStream;
		
		/**
		 * The last journal event recorded in the journal
		 */
		TIntegrityServicesEvent iLastEvent;
		
		CIntegrityTreeNode* iTreeRoot;
		
		/**
		 * The count of backup files in this journal
		 */
		TInt iBackupFilesCount;
		
		/**
		 * The filename of this journal
		 */
		TFileName iJournalFileName;
		
		/**
		 * The drive number this journal file is on
		 */
		
		TInt iDrive;
	};

} //namespace
#endif
