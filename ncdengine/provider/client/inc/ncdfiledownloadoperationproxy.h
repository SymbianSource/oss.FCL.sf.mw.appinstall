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


#ifndef C_NCDFILEDOWNLOADOPERATIONPROXY_H	
#define C_NCDFILEDOWNLOADOPERATIONPROXY_H

#include "ncdbaseoperationproxy.h"
#include "ncdfiledownloadoperation.h"
#include "ncdoperationimpl.h"

class MNcdFileDownloadOperationObserver;
class CNcdNodeProxy;

class CNcdFileDownloadOperationProxy : 
    public CNcdOperation< MNcdFileDownloadOperation >
    {
public:

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
    static CNcdFileDownloadOperationProxy* NewL( MCatalogsClientServer& aSession,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        const TDesC& aTargetFilename,
                        MNcdFileDownloadOperationObserver* aObserver,
                        TNcdDownloadDataType aDataType );
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
    static CNcdFileDownloadOperationProxy* NewLC(  MCatalogsClientServer& aSession,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        const TDesC& aTargetFilename,
                        MNcdFileDownloadOperationObserver* aObserver,
                        TNcdDownloadDataType aDataType );

        
public: // From MNcdFileDownloadOperation
    
    /**
     * @see MNcdFileDownloadOperation::TargetFileName()
     */    
    const TDesC& TargetFileName() const;
        
    /**
     * @see MNcdFileDownloadOperation::PauseL()
     */
    void PauseL();


    /**
     * @see MNcdFileDownloadOperation::ResumeL()
     */
    void ResumeL();


    /**
     * @see MNcdFileDownloadOperation::IsPaused()
     */
    TBool IsPaused();


    /**
     * @see MNcdDownloadOperation::IsPausableL()
     */
    TBool IsPausableL();
  
  
    /**
     * @see MNcdFileDownloadOperation::DownloadDataType()
     */
    TNcdDownloadDataType DownloadDataType() const;


public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;
    

protected: // Constructor and destructor

    /**
     * Constructor
     *
     */
    CNcdFileDownloadOperationProxy( TNcdDownloadDataType aDataType );
    
    virtual ~CNcdFileDownloadOperationProxy();


    /**
     * ConstructL
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */         
    void ConstructL( MCatalogsClientServer& aSession,
        TInt aHandle,
        MNcdOperationProxyRemoveHandler* aRemoveHandler,
        CNcdNodeProxy* aNode,
        CNcdNodeManagerProxy* aNodeManager,
        MNcdFileDownloadOperationObserver* aObserver,
        const TDesC& aTargetFilename );
    
    
protected: // From CNcdBaseOperationProxy

    /**
     * @see CNcdBaseOperationProxy::ProgressCallback()
     */
    void ProgressCallback();
    
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback()
     */
    void QueryReceivedCallback( CNcdQuery* aQuery );
    
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback()
     */
    void CompleteCallback( TInt aError );
    

    /**
     * @see CNcdBaseOperationProxy::CreateInitializationBufferL()
     */    
    HBufC8* CreateInitializationBufferL();
    
    
    /**
     * @see CNcdBaseOperationProxy::InitializationCallback()    
     */
    void InitializationCallback( RReadStream& aReadStream, 
        TInt aDataLength );    
    
    
private: // data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    MNcdFileDownloadOperationObserver* iObserver;
    
    // Name of the target file      
    HBufC* iTargetFilename;
    
    // Pause-state of the download
    TBool iIsPaused;
    
    TNcdDownloadDataType iDataType;

    };

#endif // C_NCDFILEDOWNLOADOPERATIONPROXY_H