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


#ifndef C_NCDDOWNLOADOPERATIONPROXY_H	
#define C_NCDDOWNLOADOPERATIONPROXY_H

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncddownloadoperation.h"
#include "ncdoperationdatatypes.h"

class MNcdDownloadOperationObserver;
class CNcdNodeProxy;

class CNcdDownloadOperationProxy : 
    public CNcdOperation< MNcdDownloadOperation >
    {
public:

    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aDownloadType Download type
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */    
    static CNcdDownloadOperationProxy* NewL( MCatalogsClientServer& aSession,
                        TNcdDownloadDataType aDownloadType,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode, 
                        MNcdDownloadOperationObserver* aObserver,
                        CNcdNodeManagerProxy* aNodeManager,
                        TInt aDownloadIndex = 0,
                        TBool aIsRunning = EFalse );
    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aDownloadType Download type
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */
    static CNcdDownloadOperationProxy* NewLC(  MCatalogsClientServer& aSession,
                        TNcdDownloadDataType aDownloadType,
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        MNcdDownloadOperationObserver* aObserver,
                        CNcdNodeManagerProxy* aNodeManager,
                        TInt aDownloadIndex = 0,
                        TBool aIsRunning = EFalse );


        
public: // From MNcdDownloadOperation
    
    /**
     * @see MNcdDownloadOperation::FileCount()
     */
    TInt FileCount();
    
    /**
     * @see MNcdDownloadOperation::CurrentFile()
     */
    TInt CurrentFile();    

    /**
     * @see MNcdDownloadOperation::PauseL()
     */
    void PauseL();


    /**
     * @see MNcdDownloadOperation::ResumeL()
     */
    void ResumeL();


    /**
     * @see MNcdDownloadOperation::IsPaused()
     */
    TBool IsPaused();


    /**
     * @see MNcdDownloadOperation::IsPausableL()
     */
    TBool IsPausableL();
  
  
    /**
     * @see MNcdDownloadOperation::AddObserverL()
     */
    void AddObserverL( MNcdDownloadOperationObserver& aObserver );

    /**
     * @see MNcdDownloadOperation::RemoveObserver()
     */
    TBool RemoveObserver( MNcdDownloadOperationObserver& aObserver );


    /**
     * @see MNcdDownloadOperation::DownloadDataType()
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
    CNcdDownloadOperationProxy( TNcdDownloadDataType aDownloadType,
        TInt aDownloadIndex );
    
    virtual ~CNcdDownloadOperationProxy();


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
                    MNcdDownloadOperationObserver* aObserver,
                    CNcdNodeManagerProxy* aNodeManager,
                    TBool aIsRunning );
    
    
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

protected: // CActive

    /**
     * @see CActive::DoCancel
     */    
    void DoCancel();
    
    
private: // data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    RPointerArray<MNcdDownloadOperationObserver> iObservers;
    
    TNcdDownloadDataType iDownloadType;
    
    // Total number of files      
    TInt iFileCount;
    
    // Number of the currently downloaded file
    TInt iCurrentFile;

    
    // Pause-state of the download
    TBool iIsPaused;
    
    };

#endif // C_NCDDOWNLOADOPERATIONPROXY_H