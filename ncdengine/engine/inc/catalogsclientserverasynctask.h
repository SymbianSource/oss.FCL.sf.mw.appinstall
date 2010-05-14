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
* Description:  
*
*/


#ifndef C_CATALOGS_CLIENT_SERVER_ASYNC_TASK_H
#define C_CATALOGS_CLIENT_SERVER_ASYNC_TASK_H

#include <e32base.h>

class RCatalogsClientServerClientSession;

/**
 *  Class which handles tasks related to transfer of a asynchronous
 *  message. The most important function related to this is to
 *  keep a package-object alive when a return message is expected.
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsClientServerAsyncTask : public CActive
    {


public:


    /**
     * @enumtype TClientServerAsyncTaskState
     *
     * @description The state of the asynchronous task that handles
     *              asynchronous message sending and receiving within
     *              ClientServer.
     * @value ENoRequestsPending No task set.
     * @value ECreateProviderRequest Task is handling provider creation.
     * @value EAsyncMessageSendRequest Task is handling asynchronous message
     *                                 sending.
     * @value EAsyncAllocMessageSendRequest Task is handling asynchronous
     *                                      message sending with return 
     *                                      buffer's automatic allocation.
     * @value EAsyncAllocCompleteMessageRequest Task is currently handling
     *                                          retrieval of incomplete
     *                                          message from the server side
     *                                          because originally allocated
     *                                          return buffer was too small
     *                                          and it had to be reallocated.
     */ 
    enum TClientServerAsyncTaskState
        {
        ENoRequestsPending,
        ECreateProviderRequest,
        EAsyncMessageSendRequest,
        EAsyncAllocMessageSendRequest,
        EAsyncAllocCompleteMessageRequest
        };

    static CCatalogsClientServerAsyncTask* NewL( 
        RCatalogsClientServerClientSession& aSession );

    static CCatalogsClientServerAsyncTask* NewLC(
        RCatalogsClientServerClientSession& aSession );

    virtual ~CCatalogsClientServerAsyncTask();

    /**
     * Function to handle asynchronous provider creation
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
     * @return Error code to tell whether the test succeeded or not.
     */     
    void CreateProvider( 
        TInt aUid, 
        TRequestStatus& aStatus, 
        TInt& aHandle,
        TUint32 aOptions );

    /**
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
     * Function to send asynchronous message to a receiver on the
     * server-side. Needed buffer for return message is allocated.
     *
     * @since S60 ?S60_version
     * @param aFunction Integer for receiver usage.
     * @param aInput Input data that should be sent.
     * @param aOutput Reference to descriptor pointer which is used to
     *                return a pointer to allocated return buffer.
     * @param aHandle An integer that identifies receiver. 
     * @param aStatus A request status which indicates the 
     *                completion status of the asynchronous call.
     * @param aLength   Expected length of the return message.
     */                    
    void SendAsyncAlloc( TInt aFunction,
                         const TDesC8& aInput,
                         HBufC8*& aOutput,
                         TInt aHandle,
                         TRequestStatus& aStatus,
                         TInt aLength );

    /**
     * Getter for pointer to the sender's TRequestStatus.
     *
     * @since S60 ?S60_version
     * @return TRequestStatus of the sender. NULL if the sender info
     *         is not available anymore. (for example SenderDown has
     *         been called)
     */
    TRequestStatus* SendersRequestStatus() const;
    
    /**
     * Informs this task that the sender is going down.
     * Message send (with aStatus given in send functions)
     * is completed immediately with KErrCancel. After this
     * the asyncTask waits for the completion of the server side
     * message. When the task receives message completion the
     * task is silently deleted.
     *
     * @since S60 ?S60_version
     */ 
    void SenderDown();

protected:

// from base class CActive

    /**
     * From CActive.
     * Handles request completion.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void RunL();

    /**
     * From CActive.
     * Cancels outstanding request.
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */    
    void DoCancel();


private:

    CCatalogsClientServerAsyncTask(
        RCatalogsClientServerClientSession& aSession );
    void ConstructL();
        
    /**
     * Function that cleans up some generally used member variables
     * and informs that this task is not used anymore.
     *
     * @since S60 ?S60_version
     */    
    void GeneralCleanAndComplete();

    /**
     * Function that cleans up member variables needed in
     * asyncalloc. Also does GeneralCleanAndComplete.
     * Completes with given error.
     *
     * @since S60 ?S60_version
     * @param aError Error code that is used when completing send.
     */    
    void CompleteAllocWithError( TInt aError );

    /**
     * Function that does needed tasks after asynchronous message
     * with return message allocation has been sent and completed.
     *
     * @since S60 ?S60_version
     */     
    void HandleAllocMessageComplete();
    
    /**
     * Function that does needed tasks after asynchronous message
     * with return message allocation has been reallocated,
     * sent and completed.
     *
     * @since S60 ?S60_version
     */       
    void HandleReallocMessageComplete();

    /**
     * Function that sets the given pointer to be the return pointer
     * used to return messages from the asyncAlloc message transfer.
     * In other words writes into pointer reference given by sender
     * the pointer received as a parameter. If the sender is already
     * down does not write the return pointer.
     *
     * @since S60 ?S60_version
     * @param aBufferForReturn Pointer to buffer that should be used
     *                         for returning purposes.
     */
    void SetAllocReturnPointer( HBufC8* aBufferForReturn );

private: // data

        
        /**
         *  Variable to keep track of the state of this object
         */
        TClientServerAsyncTaskState iState;

        /**
         *  Tells whether the original sender of the message still
         *  exists and message sending can be completed.
         */
        TBool iSenderDown;

        /**
         *  Session that is used to perform needed tasks
         */
        RCatalogsClientServerClientSession& iSession;
        
        /**
         *  This is used to inform client that the message transfer has
         *  been completed.
         *  Not own.
         */
        TRequestStatus* iClientStatus;
        
        /**
         *  Pointer for handle returning purpose
         *  Not own.
         */
        TInt* iReturnHandle;
        /**
         *  Package that is used to send to and receive handle
         *  value from the server. Used in external and internal
         *  messages of ClientServer. So this does not always
         *  contain handle of the original receiver object.
         */
        TPckgBuf<TInt> iIntegerHandleBuf;
        
        /**
         *  Package that is used to send function-number
         */
        TPckgBuf<TInt> iIntegerFunctionBuf;
        
        /**
         *  TIpcArgs for message transportation needs.
         */        
        TIpcArgs iArgs;
        
        
        // Variables for message returning purposes

        /**
         *  Pointer to a descriptor that is given by the sender.
         *  This is used to return message when using asynchronous
         *  messaging.
         *  Not own.
         */          
        TDes8* iReturnPointer;

        /**
         *  Pointer to a descriptor pointer that is given by the sender.
         *  This is used to return allocated return message when using
         *  asynchronous messaging with return buffer allocation.
         *  Not own.
         */          
        HBufC8** iAllocReturnPointer;


        // Temporary variables which are used in asynchronous communication
        // and message returning from the server side.
        
        /**
         *  Pointer to created buffer which is sent to server-side
         *  to retrieve a return message.
         *  Own.
         */         
        HBufC8* iTempReturnBuf;
        /**
         *  Modifiable version of iTempReturnBuf.
         */
        TPtr8 iTempWritableReturnBuf;
        
    };


#endif // C_CATALOGS_CLIENT_SERVER_ASYNC_TASK_H
