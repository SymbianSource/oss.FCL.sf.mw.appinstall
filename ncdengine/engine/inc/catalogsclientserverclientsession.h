/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Declaration of RCatalogsClientServerClientSession
*
*/


#ifndef R_CATALOGS_CLIENT_SERVER_CLIENT_SESSION_H
#define R_CATALOGS_CLIENT_SERVER_CLIENT_SESSION_H

#include <e32std.h>

#include "catalogsclientserver.h"
#include "catalogsserverdefines.h"

class CCatalogsClientServerAsyncTask;


/**
 *  A class implementing MCatalogsClientServer interface
 *
 *  This class offers message sending services for clients
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class RCatalogsClientServerClientSession : public RSessionBase,
                                           public MCatalogsClientServer
    {

public:

    /**
     * Constructor
     */
    RCatalogsClientServerClientSession();

    /**
     * Connect to the server and create a session. If connection
     * cannot be made, tries for a few more times. There is also a
     * sleep between the tries which halts the current thread.
     * 
     * @since S60 ?S60_version
     * @param aClientUid A uid identifying the client.
     * @return System wide error code.
     */
    TInt Connect( TUid aClientUid );

    /**
     * Disconnect the connection to the server
     * Notice: If there are pending tasks when this function
     *         is called they will not get completed. Try to
     *         complete the tasks before a call to this function.
     */
    void Disconnect();

    /**
     * Tells the server to create a new provider. The
     * function is executed asynchronously.
     *
     * @since S60 ?S60_version
     * @param aUid Uid of the provider that should be created.
     *             (integer containing a hexadecimal number)
     * @param aStatus A request status which indicates the 
     *          completion status of the asynchronous call
     * @param aHandle a reference to an integer that holds
     *              the provider message handle after the
     *              asynchronous function call completes
     *              (i.e. used to return the handle.) 
     * @param aOptions Provider options
     * 
     */
    void CreateProvider( TInt aUid,
                         TRequestStatus& aStatus,
                         TInt& aHandle,
                         TUint32 aOptions );

    /**
     * Sends a normal asynchronous message to server-side.
     *
     * @since S60 ?S60_version
     * @param aMessageType Internal messagetype used in
     *                     ClientServer.
     * @param aArgs Parameters for message encased in TIpcArgs.
     * @param aStatus A request status which indicates the 
     *                completion status of the asynchronous call
     * 
     */    
    void SendAsync( TCatalogsServerFunction aMessageType,
                    const TIpcArgs& aArgs,
                    TRequestStatus& aStatus );

    /**
     * Sends a synchronous message to server-side.
     *
     * @since S60 ?S60_version
     * @param aMessageType Internal messagetype used in
     *                     ClientServer.
     * @param aArgs Parameters for message encased in TIpcArgs.
     * @return Symbian error code
     * 
     */
    TInt SendSync( TCatalogsServerFunction aMessageType,
                   const TIpcArgs& aArgs );                    
                    
    /**
     * Informs that session side task has been completed
     * and that it can be removed.
     *
     * @since S60 ?S60_version
     * @param aCompletedTask Task that has been completed.
     * 
     */     
    void TaskCompleted( CCatalogsClientServerAsyncTask* aCompletedTask );

    /**
     * Deletes incomplete message from the server side by
     * sending a synchronous delete-message.
     *
     * @since S60 ?S60_version
     * @param aHandle Handle to incomplete message.
     */    
    TInt DeleteIncompleteMessage( TInt aHandle );    
        
    /**
     * Get the version number of the server.
     * 
     * @return The version number.
     */
    TVersion Version() const;
    

// from base class MCatalogsClientServer

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send a synchronous message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Descriptor for data returning purpose.
     * @param aHandle An integer that identifies receiver.
     * @return System wide error code. 
     */
    TInt SendSync( TInt aFunction,
                   const TDesC8& aInput,
                   TDes8& aOutput,
                   TInt aHandle );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send a synchronous message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Descriptor for data returning purpose.
     * @param aHandle An integer that identifies receiver.
     * @return System wide error code. 
     */
    TInt SendSync( TInt aFunction,
                   const TDesC16& aInput,
                   TDes16& aOutput,
                   TInt aHandle );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send a synchronous message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutputInt Integer for data returning purpose.
     * @param aHandle An integer that identifies receiver.
     * @return System wide error code. 
     */
    TInt SendSync( TInt aFunction,
                   const TDesC16& aInput,
                   TInt& aOutputInt,
                   TInt aHandle );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send a synchronous message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutputInt Integer for data returning purpose.
     * @param aHandle An integer that identifies receiver.
     * @return System wide error code. 
     */
    TInt SendSync( TInt aFunction,
                   const TDesC8& aInput,
                   TInt& aOutputInt,
                   TInt aHandle );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send asynchronous message to a receiver on the
     * the server-side so that ClientServer allocates needed
     * buffer for return message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Reference to descriptor pointer which is used to
     *                return a pointer to allocated return buffer.
     * @param aHandle An integer that identifies receiver. 
     * @param aLength Expected length of the return message.
     * @return KErrNone if successful, otherwise an error code
     */
    TInt SendSyncAlloc( TInt aFunction,
                        const TDesC8& aInput,
                        HBufC8*& aOutput,
                        TInt aHandle,
                        TInt aLength );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send asynchronous message to a receiver on the
     * the server-side so that ClientServer allocates needed
     * buffer for return message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Reference to descriptor pointer which is used to
     *                return a pointer to allocated return buffer.
     * @param aHandle An integer that identifies receiver. 
     * @param aLength Expected length of the return message.
     * @return KErrNone if successful, otherwise an error code
     */
    TInt SendSyncAlloc( TInt aFunction,
                        const TDesC16& aInput,
                        HBufC16*& aOutput,
                        TInt aHandle,
                        TInt aLength );


    /**
     * Sends a file open message to a server side object. The server side object
     * should return an open RFile handle
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     *                
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @return An open file handle
     */
    RFile SendSyncFileOpenL( TInt aFunction,
                             const TDesC8& aInput,                           
                             TInt aHandle );    
                        
    
    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send basic asynchronous message to a
     * receiver on the server-side.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Descriptor for data returning purpose.
     * @param aHandle An integer that identifies receiver. 
     * @param aStatus A request status which indicates the 
     *                completion status of the asynchronous call.
     */
    void SendAsync( TInt aFunction,
                    const TDesC8& aInput,
                    TDes8& aOutput,
                    TInt aHandle,
                    TRequestStatus& aStatus );

    
    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to send asynchronous message to a receiver on the
     * the server-side so that ClientServer allocates needed
     * buffer for return message.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Reference to descriptor pointer which is used to
     *                return a pointer to allocated return buffer.
     * @param aHandle An integer that identifies receiver. 
     * @param aStatus A request status which indicates the 
     *                completion status of the asynchronous call.
     * @param aLength Expected length of the return message.
     */
    void SendAsyncAlloc( TInt aFunction,
                          const TDesC8& aInput,
                          HBufC8*& aOutput,
                          TInt aHandle,
                          TRequestStatus& aStatus,
                          TInt aLength );

    /**
     * From MCatalogsClientServer
     * @see MCatalogsClientServer
     *
     * Function to inform that sender is going down. This means that
     * all asyncTasks that are handling message transfer of the
     * sender will complete with KErrCancel. The asyncTasks handling
     * message sending of the identified sender will not be deleted
     * at this point. Instead they continue to wait for completion
     * of the messages from the server side. When message from
     * the server side is received, corresponding asyncTask will be
     * silently deleted.
     *
     * @since S60 ?S60_version
     * @param aStatus TRequestStatus that identifies the sender whose
     *                messages should be completed immediately with
     *                KErrCancel.
     */                          
    void AsyncMessageSenderDown( TRequestStatus& aStatus );

private:

    /**
     * Function to get a task that handles async message send/receive
     *
     * @since S60 ?S60_version
     * @param aTask Reference to a pointer, which is used to return
     *              a task that handles single asynchronous message
     *              send and receive task
     */
    void GetAsyncTaskL( CCatalogsClientServerAsyncTask*& aTask );


    /**
     * Assignment operator. Private with no implementation to prevent
     * usage.
     *
     * @since S60 ?S60_version
     * @param aOther Object that should be assigned to this.
     */
    RCatalogsClientServerClientSession&  operator=(
        const RCatalogsClientServerClientSession& aOther );

    /**
     * Copy constructor. Private with no implementation to prevent
     * usage.
     *
     * @since S60 ?S60_version
     * @param aOther Object that should be copied.
     */
    RCatalogsClientServerClientSession(
        const RCatalogsClientServerClientSession& aOther );

private: // data

    /**
     * Array that holds pending tasks.
     */
    RPointerArray<CCatalogsClientServerAsyncTask> iTasks;

    /**
     * Variable to keep track whether session has been opened
     * or not.
     */
    enum
        {
        EOpen,
        EClosed
        } iSessionStatus;

    };

#endif // R_CATALOGS_CLIENT_SERVER_CLIENT_SESSION_H
