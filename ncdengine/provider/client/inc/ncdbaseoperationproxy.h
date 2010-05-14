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
* Description:   ?Description
*
*/
	

#ifndef C_NCDBASEOPERATIONPROXY_H
#define C_NCDBASEOPERATIONPROXY_H

#include "catalogsbase.h"
#include "catalogsbaseimpl.h"
#include "ncdoperation.h"
#include "ncdprogress.h"
#include "ncdsendableprogress.h"
#include "ncdoperationfunctionids.h"

class MNcdSendable;
class MNcdOperationProxyRemoveHandler;
class CNcdQuery;
class MCatalogsClientServer;
class CNcdNodeProxy;
class MNcdClientLocalizer;
class CNcdNodeManagerProxy;

/**
 *  Operation proxy base class.
 *
 *  Implements common functionality for operation proxys. This class is
 *  abstract.
 */
class CNcdBaseOperationProxy : 
    public CCatalogsBase<CActive>    
    {
public: // from MNcdOperation

    /**
     * @see MNcdOperation::OperationStateL()
     */
    MNcdOperation::TState DoOperationStateL() const;
    
        
    /**
     * @see MNcdOperation::StartOperationL()
     */
    void DoStartOperationL();
    
    /**
     * @see MNcdOperation::CancelOperation()
     */
    void DoCancelOperation();

    /**
     * @see MNcdOperation::Progress()
     */
    TNcdProgress DoProgress() const;
    

    /**
     * @see MNcdOperation::CompleteQuery()
     */    
    void DoCompleteQueryL( MNcdQuery& aQuery );
    
    /**
     * @see MNcdOperation::Node()
     */
    MNcdNode* DoNode();
    
protected:

    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.     
     */
    CNcdBaseOperationProxy( MNcdClientLocalizer* aLocalizer);

    /**
     * ConstructL
     *
     * @param aNodeManager Pointer to node manager. Used to handle expired
     *                     nodes. Ownership is not transferred.
     *
     */
    virtual void ConstructL( MCatalogsClientServer& aSession, TInt aHandle, 
        MNcdOperationProxyRemoveHandler* aRemoveHandler, CNcdNodeProxy* aNode,
        CNcdNodeManagerProxy* aNodeManager );

    virtual ~CNcdBaseOperationProxy();

    
    /**
     * Handles messages completed by the server side operation.
     *
     * @return Symbian error code.
     */
    virtual void HandleCompletedMessage( 
        TNcdOperationMessageCompletionId aCompletionId,
        RReadStream& aReadStream,
        TInt aDataLength );
    
    /**
     * Sends a continue message.
     */
    virtual void SendContinueMessageL();
    
    
    /**
     * Sends a release message
     */
    virtual void SendReleaseMessage();
    
    
    /**
     * Inits send and receive buffers.
     *
     * @param aSendSize Size of the send buffer.
     * @param aSendSize Size of the receive buffer.
     */
    void InitBuffersL( TInt aSendSize, TInt aReceiveSize );
    
    /**
     * Initiates a progress callback.
     * Defined here so that the base class can initiate the callback.
     * Implemented in child classes.
     */
    virtual void ProgressCallback() = 0;
    
    /**
     * Initiates a query received callback.
     * Defined here so that the base class can initiate the callback.
     * Implemented in child classes.
     */
    virtual void QueryReceivedCallback( CNcdQuery* aQuery ) = 0;
    
    /**
     * Initiates a complete callback.
     * Defined here so that the base class can initiate the callback.
     * Implemented in child classes.
     *
     * @param aError System wide error code.
     */
    virtual void CompleteCallback( TInt aError ) = 0;

    
    /**
     * Used to initialize the operation during construction.
     *
     * If either the proxy or the server-side operation need some data from
     * each other during construction, this method should be called in
     * proxy's ConstructL after proxybase's ConstructL has been called.
     *
     * The proxy implementation should create a data buffer containing the
     * initialization data in CreateInitilizationBufferL.
     *
     * The proxy implementation should read initialization data received from
     * the server in InitializationCallback()
     */     
    virtual void InitializeOperationL();


    /**
     * Creates a buffer that contains initialization data that is sent to
     * the server-side operation during InitializeOperation()
     *
     * Creates an empty buffer by default
     *
     * This should be redefined in the implementation class if it uses
     * initialization
     */
    virtual HBufC8* CreateInitializationBufferL();
    
    
    /**
     * Initializes the proxy with data received from the server
     *
     * Does nothing by default.
     * @note aReadStream is closed by the base class     
     */
    virtual void InitializationCallback( RReadStream& aReadStream, 
        TInt aDataLength );

    
    /**
     * Setter for operation state
     *
     * @param aState Operation state.
     */
    void SetState( MNcdOperation::TState aState );
    
    
    /**
     * Getter for the progress variable.
     *
     * @return TSendableProgress&
     */    
    TNcdSendableProgress& SendableProgress();

    /**
     * @return CNcdNodeManagerProxy* Node manager.
     * Ownership is not transferred.
     */
    CNcdNodeManagerProxy* NodeManager() const;

    /**
     * @return CNcdNodeProxy* Node.
     * Ownership is not transferred.
     */
    CNcdNodeProxy* NodeProxy() const;    

    /**
     * Updates the purchase history with the latest operation error code
     * and sets the latest operation time to the current universal time.
     *
     * @param aErrorCode Error code of the last purchase related operation.
     */
    void UpdateOperationInfoToPurchaseHistoryL( TInt aErrorCode );
            
    
// from base class CActive

protected: // CActive

    /**
     * @see CActive::RunL
     */
    void RunL();

    /**
     * @see CActive::DoCancel
     */    
    void DoCancel();
    
    /**
     * @see CActive::RunError
     */
    TInt RunError( TInt aError );

public:

    /**
     * ClientServerSession
     *
     * @return MCatalogsClientServer& an object that contains
     * the client server session information
     */
    virtual MCatalogsClientServer& ClientServerSession() const;


    /**
     * Handle
     *
     * @return TInt a handle that identifies the server side object
     * that this proxy uses.
     */
    virtual TInt Handle() const;


    /**
     * Gives a non-modifiable descriptor for message sending.
     * @return TDesC8 The non-modifiable send buffer. Uses the
     * HBufC8 buffer that is owned by the object of this class.
     * @exception KErrNotFound if buffer does not exist.
     */
    virtual const TDesC8& SendBuf8L();
        
    /**
     * Changes the send buffer.
     * The ownership is transferred.
     * @param aBuffer is the new buffer that will replace the existing
     * 8-bit buffer. The old buffer is deleted.
     * The new buffer will be used for message sending.
     */
    virtual void SetSendBuf8( HBufC8* aBuffer );
           

protected:

    /**
     * Op proxy remove handler.
     */
    MNcdOperationProxyRemoveHandler* iRemoveHandler;    
        
    /**
     * Pending query object.
     */
    CNcdQuery* iQuery;    
    
    
private: // data

    /** 
     * Node manager for example for expired nodes handling.
     * Not own.
     */
    CNcdNodeManagerProxy* iNodeManager;

    /**
     * The node this operation was started from.
     */
    CNcdNodeProxy* iNode;
    
    /** 
     * State of the operation.
     */
    MNcdOperation::TState iState;
    
    /** 
     * Progress of the operation.
     */
    TNcdSendableProgress iProgress;
    
    
    HBufC8* iReceiveBuffer;

    /**
     * Interface for communicating to the server-side.
     */    
    MCatalogsClientServer* iSession;

    /**
     * Handle to identify the server-side counterpart for object of this
     * class.
     */
    TInt iHandle;

    
    // Variables for 8-bit transactions
        
    /**
     * Descriptor for message sending to the server-side.
     * Own.
     */     
    HBufC8* iSendHeapBuf8;
   
    
    CBufBase* iSendBuffer;
    TPtr8 iSendPtr;
    
    /**
     * String localizer, not own.
     */
    MNcdClientLocalizer* iStringLocalizer;
    };
	
	
#endif //  C_NCDBASEOPERATIONPROXY_H
