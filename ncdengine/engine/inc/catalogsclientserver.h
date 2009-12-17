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
* Description:   Interface MCatalogsClientServer declaration 
*
*/


#ifndef M_CATALOGS_CLIENT_SERVER_H
#define M_CATALOGS_CLIENT_SERVER_H

#include <f32file.h>

class TRequestStatus;
class TDesC8;
class TDes8;
class HBufC8;

/**
 *  An interface including message sending methods
 *
 *  This interface is used on the client side
 *  to send messages to the server side objects 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class MCatalogsClientServer
    {

public:

    /**
     * Sends a message to a server side object
     * and waits synchronously for the reply
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a descriptor where the output data can be written
     *                  The maximum length of the output data must be known and
     *                  the descriptor allocated accordingly
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSync( TInt aFunction,
                           const TDesC8& aInput,
                           TDes8& aOutput,
                           TInt aHandle ) = 0;
                           
     /**
     * Sends a message to a server side object
     * and waits synchronously for the reply
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a descriptor where the output data can be written
     *                  The maximum length of the output data must be known and
     *                  the descriptor allocated accordingly
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSync( TInt aFunction,
                           const TDesC16& aInput,
                           TDes16& aOutput,
                           TInt aHandle ) = 0;

     /**
     * Sends a message to a server side object
     * and waits synchronously for the reply
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutputInt a integer where the output data can be written
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSync( TInt aFunction,
                           const TDesC16& aInput,
                           TInt& aOutputInt,
                           TInt aHandle ) = 0;    

     /**
     * Sends a message to a server side object
     * and waits synchronously for the reply
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutputInt a integer where the output data can be written
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSync( TInt aFunction,
                           const TDesC8& aInput,
                           TInt& aOutputInt,
                           TInt aHandle ) = 0;  

    /**
     * Sends a message to a server side object
     * and waits synchronously for the reply. The maximum length
     * of the output data does not have to be known beforehand. The 
     * implementing class allocates an output buffer of adequate length.
     * NOTICE: If user sends a message from the server-side with some
     *         other outcome than KErrNone, it passes through (when it is
     *         not KCatalogsErrorTooSmallDescriptor) and the
     *         output message has to be deleted after usage.
     *         If outcome is KCatalogsErrorTooSmallDescriptor, a null-pointer
     *         is received.
     * NOTICE: aOutput should be initially a NULL-pointer
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a reference to a pointer that points to a descriptor
     *                  that includes the output data when after the reply
     *                  is received from the server
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @param aLength estimated maximum length of the output data. This length
     *              does not have to be known for sure when sending a message
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSyncAlloc( TInt aFunction,
                                const TDesC8& aInput,
                                HBufC8*& aOutput,
                                TInt aHandle,
                                TInt aLength ) = 0;


    /**
     * Sends a message to a server side object
     * and waits synchronously for the reply. The maximum length
     * of the output data does not have to be known beforehand. The 
     * implementing class allocates an output buffer of adequate length.
     * NOTICE: If user sends a message from the server-side with some
     *         other outcome than KErrNone, it passes through (when it is
     *         not KCatalogsErrorTooSmallDescriptor) and the
     *         output message has to be deleted after usage.
     *         If outcome is KCatalogsErrorTooSmallDescriptor, a null-pointer
     *         is received.
     * NOTICE: aOutput should be initially a NULL-pointer
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a reference to a pointer that points to a descriptor
     *                  that includes the output data when after the reply
     *                  is received from the server
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @param aLength estimated maximum length of the output data. This length
     *              does not have to be known for sure when sending a message
     * @return KErrNone if successful, otherwise an error code
     */
    virtual TInt SendSyncAlloc( TInt aFunction,
                                const TDesC16& aInput,
                                HBufC16*& aOutput,
                                TInt aHandle,
                                TInt aLength ) = 0;
                                  
    
    /**
     * Sends a message to a server side object
     * and waits asynchronously for the reply. 
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a descriptor where the output data can be written.
     *                  The maximum length of the output data must be known and
     *                  the descriptor allocated accordingly
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @param aStatus a request status which indicates the completion 
     *                  status of the asynchronous request.
     */
    virtual void SendAsync( TInt aFunction,
                            const TDesC8& aInput,
                            TDes8& aOutput,
                            TInt aHandle,
                            TRequestStatus& aStatus ) = 0;
    

    /**
     * Sends a file open message to a server side object. The server side object
     * should return an open RFile handle
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput Open RFile
     *                
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @param aStatus a request status which indicates the completion 
     *                  status of the asynchronous request.
     */
    virtual RFile SendSyncFileOpenL( TInt aFunction,
                                     const TDesC8& aInput,
                                     TInt aHandle ) = 0;    

    /**
     * Sends a message to a server side object
     * and waits asynchronously for the reply. The maximum length
     * of the output data does not have to be known beforehand. The 
     * implementing class allocates an output buffer of adequate length.
     * NOTICE: If user sends a message from the server-side with some
     *         other outcome than KErrNone, it passes through (when it is
     *         not KCatalogsErrorTooSmallDescriptor) and the
     *         output message has to be deleted after usage.
     *         If outcome is KCatalogsErrorTooSmallDescriptor, a null-pointer
     *         is received.
     * NOTICE: aOutput should be initially a NULL-pointer
     *
     * @since S60 ?S60_version
     * @param aFunction the opcode specifying the requested service
     * @param aInput input data to be sent
     * @param aOutput a reference to a pointer that points to a descriptor
     *                  that includes the output data when after the reply
     *                  is received from the server
     * @param aHandle a handle that is used to specify the receiving
     *                  object on the server side
     * @param aStatus a request status which indicates the completion 
     *                  status of the asynchronous request.
     * @param aLength estimated maximum length of the output data. This length
     *              does not have to be known for sure when sending a message
     */
    virtual void SendAsyncAlloc( TInt aFunction,
                                  const TDesC8& aInput,
                                  HBufC8*& aOutput,
                                  TInt aHandle,
                                  TRequestStatus& aStatus,
                                  TInt aLength ) = 0;
    
    /**
     * Informs ClientServer that sender has gone down and no messages
     * related to that specific sender should be completed anymore by
     * ClientServer. ClientServer still expects that the messages that
     * were sent from the sender are completed by the receiver on the
     * server side.
     *
     * @notice aStatus has to be the same TRequestStatus that was given
     *         when sending the message. Comparation is done simply by
     *         comparing the addresses of TRequestStatus given now and
     *         TRequestStatuses given during message sends.
     *
     * @since S60 ?S60_version
     * @param aStatus TRequestStatus that identifies the sender whose
     *                messages should not be completed anymore.
     *
     */
    virtual void AsyncMessageSenderDown( TRequestStatus& aStatus ) = 0;

    };


#endif // M_CATALOGS_CLIENT_SERVER_H
