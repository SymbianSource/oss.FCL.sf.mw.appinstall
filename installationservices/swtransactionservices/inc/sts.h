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
*
*/


/**
 @file
 @publishedAll
 @released
*/


#ifndef __STSSESSION_H__
#define __STSSESSION_H__

#include <scs/scsclient.h>
#include <f32file.h>
#include <usif/sts/stsdefs.h>

namespace Usif
{
/**
	The RStsSession class provides synchronous interface to the STS server.
	This class wraps all the details of client-server communication with the STS server
	residing in a separate process.

	This class is intended to be used by STS client applications.

        RStsSession class uses a unique transaction ID to identify operations belonging to different FS management transactions.
        A transaction is a logical set of file system operations to be executed or rolled back atomically so that either all 
        operations fail or all operations succeed together.

        Closing the session through RStsSession::Close() rolls back the transaction unless it was explicitly rolled back or committed
        using the respective methods.
	
	@publishedAll
	@released	
 */

    class RStsSession : public RScsClientBase
		{
	public:
		IMPORT_C RStsSession();

		/**
		 Opens a session to the STS server and creates a new transaction
		 @pre Neither CreateTransactionL nor OpenTransactionL have been invoked previously on this STS session.
		 @leave system wide error codes
		 @return the newly created transaction's unique ID
		 */
		IMPORT_C TStsTransactionId CreateTransactionL();

		/**
		 Opens a session to the STS server and joins an existing transaction.
		 This API should not be called throughout a valid session to the STS server
		 (i.e: CreateTransactionL() >> OpenTransactionL() or OpenTransactionL() >> OpenTransactionL())
		 @pre There is an active transaction with the supplied id.
		 @pre Neither CreateTransactionL nor OpenTransactionL have been invoked previously on this STS session. 
		 @leave system wide error codes
		 @param aTransactionID the unique TInt32 transaction ID
		 */
		IMPORT_C void OpenTransactionL(TStsTransactionId aTransactionID);

	public:
		//journalling related functions

		/**
		 Notifies the STS server that a file or directory is being added to the file system.
		 Important note: the actual file operation is not executed. It has to be done 
		 by the client code AFTER calling this API.
		 On commit the file is kept, on rollback it is removed.                
		 @leave system wide error codes
		 @param aFileName - Fully qualified name of file or directory
		 */
		IMPORT_C void RegisterNewL(const TDesC& aFileName);

		/**
		 Instructs the STS server to create a new file.
		 The server creates a file and returns a handle to it. 
		 On commit the file is kept, on rollback it is removed.
		 Important note: ownership of the newly created file is transfered from the server
		 to the client, so it is the client's responsibility to make sure that the file is
		 closed properly.
		 On creation the new file is empty.
		 @leave system wide error codes
		 @leave KErrAlreadyExist If the file already exists
		 @param aFileName - Fully qualified name of file
		 @param aFile - Reference of a file handle that is set to the new created file
		 @param aFileMode - File creation mode of the new file (see TFileMode documentation
		                    for the possible mode or combination of modes)
		 */
		IMPORT_C void CreateNewL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode);

		/**
		 Instructs the STS server to remove the specified file or directory.
		 Important note: the file is removed by the STS server. The client doesn't have to delete it
		 after a call to this API.
		 If the file does not exist, this function does nothing.
		 On commit the file remains permanently deleted, on rollback it is restored.
		 @leave system wide error codes
		 @param aFileName - Fully qualified name of file or directory
		 */
		IMPORT_C void RemoveL(const TDesC& aFileName);

		/**
		 Notifies the STS server that a temporary file is being added to the file system.
		 Important note: the actual file operation (i.e: creating the file at the target location) is not executed. 
		 It has to be done by the client code AFTER calling this API.
		 The temporary file is deleted both on commit and on rollback.
		 @leave system wide error codes
		 @param aFileName - Fully qualified name of file or directory
		 */
		IMPORT_C void RegisterTemporaryL(const TDesC& aFileName);

		/**
		 Instructs the STS server to create a new temporary file.
		 The server creates a file and returns a handle to it. 
		 The temporary file is deleted both on commit and on rollback.
		 Important note: ownership of the newly created file is transfered from the server
		 to the client, so it is the client's responsibility to make sure that the file is
		 closed properly.                
		 On creation the new file is empty.

		 @leave system wide error codes
		 @leave KErrAlreadyExists  A file with this name already exists
		 @param aFileName - Fully qualified name of file
		 @param aFile - Reference of a file handle that is set to the new created file
		 @param aFileMode - File creation mode of the new file (see TFileMode documentation
		                    for the possible mode or combination of modes)
		 */
		IMPORT_C void CreateTemporaryL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode);

		/**
		 Instructs the STS server to overwrite the target file.
		 If the file doesn't exist, an empty new file is created. If the file already exists, it is overwritten with an empty one. 
		 A file handle to the new file is returned to the client in both cases.		 
		 Important note: ownership of the newly created file is transfered from the server
		 to the client, so it is the client's responsibility to make sure that the file is
		 closed properly.        
		 On creation the new file is empty.
		 On commit the new file is kept. On rollback, the new file is removed and the original one is restored.
		 
		 Please note that this function is recommended whenever the client generates the content for the new file.
		 If the new file's content is not generated (e.g. it is copied), it is recommended to use RemoveL followed by RegisterNewL.
		 
		 @leave system wide error codes
		 @param aFileName - Fully qualified name of file or directory
		 @param aFile - Reference of a file handle that is set to the new created file
		 @param aFileMode - File creation mode of the new file (see TFileMode documentation
		                    for the possible mode or combination of modes)
		 */
		IMPORT_C void OverwriteL(const TDesC& aFileName, RFile &aFile, TUint aCreateMode);

		/**
		 Commits the current file system transaction. After commiting a transaction its file 
		 operations cannot be reverted.
		 Important note: this function implies closing the session to the server.
		 @leave system wide error codes
		 */
		IMPORT_C void CommitL();

		/**
		 Starts the recovery process for the transaction. All operations are reverted during roll back restoring
		 the file system to a state that it was in right before starting the transaction.
		 Important note: This function implies closing the session to the server;
		                 any affected directories(including pre-existing) which were found empty will be deleted.
		 @leave system wide error codes
		 */
		IMPORT_C void RollBackL();

		/**
		 Returns the currently active transcation's id.
		 @leave system wide error codes
		 @leave KErrArgument If no transaction is active in this session - either it was not opened, or it has been already committed or rolled back.
		 */
		IMPORT_C TStsTransactionId TransactionIdL();

		/**
		 Closes this session to the STS server. Also rolls back the transaction in case neither CommitL not RollBackL
		 were invoked previously.
		 NOTE: Any affected directories(including pre-existing) which were found empty will be deleted.
		 */
		IMPORT_C void Close();

	private:
		/**
		 Connects to the STS server and starts it if necessary
		 @leave system wide error codes
		 */
		void ConnectL();

		};//class RStsSession
   

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
   /**
	 This class provides provides synchronous interface to the STS server for rolling back all pending transactions.

	 @internalTechnology
	 @released
	 */
	class RStsRecoverySession : public RScsClientBase
		{
	public:
		IMPORT_C RStsRecoverySession();

		/**
		 Connects to the STS server and instructs it to roll back all pending transactions. 
		 Does nothing if there are no pending transactions.
		 @leave system wide error codes
		 */
		IMPORT_C void RollbackAllPendingL();

		/**
		 Closes this session to the STS server.
		 */
		IMPORT_C void Close();

	private:
		/**
		 Connects to the STS server and starts it if necessary
		 @leave system wide error codes
		 */
		void ConnectL();

		};//class RStsRecoverySession   
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	}////namespace Usif

#endif
