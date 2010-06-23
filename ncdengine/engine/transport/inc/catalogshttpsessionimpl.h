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


#ifndef C_CATALOGSHTTPSESSION_H
#define C_CATALOGSHTTPSESSION_H

#include <e32base.h>
#include "catalogshttpsession.h"

class CCatalogsHttpDownloadManager;
class CCatalogsHttpTransactionManager;
class CCatalogsTransport;
class MCatalogsHttpSessionManager;
class CCatalogsHttpConfig;
class CCatalogsHttpConnectionManager;
class CCatalogsConnectionEventSink;

/**
* HTTP session implementation
*/
class CCatalogsHttpSession : public CBase, 
    public MCatalogsHttpSession
    {
public: // Constructors and destructor

    /**
    * Creator
    *
    * @param aSessionId Session id
    * @param aOwner Owner object
    * @return A new HTTP session
    */
    static CCatalogsHttpSession* NewL( 
        TInt32 aSessionId,
        CCatalogsTransport& aOwner, 
        MCatalogsHttpSessionManager& aSessionManager,
        TBool aCleanupSession );
    
    /**
    * Destructor
    */
    virtual ~CCatalogsHttpSession();
    

public: // MCatalogsHttpSession
    
    /**
    * @see MCatalogsHttpSession::AddRef()
    */        
    TInt AddRef();		
    
    /**
    * @see MCatalogsHttpSession::Release()
    */        
    TInt Release();
      
    /**
    * @see MCatalogsHttpSession::RefCount()
    */                
    TInt RefCount() const;   

    /**
    * Returns session ID
    *
    * @return Session ID
    */
    TInt32 SessionId() const;
    
    
    /**
    * Returns the type of the session.
    *
    * This value is the same is the interface ID used to create the
    * session
    *
    * @return Session type
    */
    TInt SessionType() const;        

public:

    /**
    * Creates a new download operation
    * 
    * @param aUrl URL to download
    * @param aObserver Observer for the download
    * @param aStart If true, download is started automatically. Otherwise
    * the caller must start it explicitly. Default: true
    * 
    * @return A new download operation
    */
    MCatalogsHttpOperation* CreateDownloadL( const TDesC16& aUrl, 
        TBool aStart, MCatalogsHttpObserver* aObserver );

    MCatalogsHttpOperation* CreateDownloadL( const TDesC8& aUrl, 
        TBool aStart, MCatalogsHttpObserver* aObserver );


    /**
    * Creates a new transaction operation
    * 
    * @param aUrl Destination URL
    * @param aObserver Observer for the transaction
    *
    * @return A new transaction operation
    */
    MCatalogsHttpOperation* CreateTransactionL( const TDesC16& aUrl, 
        MCatalogsHttpObserver* aObserver );

    MCatalogsHttpOperation* CreateTransactionL( const TDesC8& aUrl, 
        MCatalogsHttpObserver* aObserver );


    MCatalogsHttpOperation* CreateDlTransactionL( const TDesC8& aUrl,
        MCatalogsHttpObserver& aObserver,
        const CCatalogsHttpConfig& aConfig );

   /**
    * Download getter. 
    *
    * Returns the download that matches the id or NULL
    * @param aId Download ID
    * @return Download or NULL
    */
    MCatalogsHttpOperation* Download( 
        const TCatalogsTransportOperationId& aId ) const;


    /**
    * Transaction getter. 
    *
    * Returns the transaction that matches the id or NULL
    * @param aId Transaction ID
    * @return Transaction or NULL
    */            
    MCatalogsHttpOperation* Transaction( 
        const TCatalogsTransportOperationId& aId ) const;


    /**
    * Returns an array of current downloads. 
    *
    * @return Array of downloads
    * @note Doesn't show downloads that were completed during the time
    * the client was not running
    */
    const RCatalogsHttpOperationArray& 
        CurrentDownloads() const;
    
    
    /**
     * Returns an array of downloads restored from the previous
     * session.
     *
     * @return Array of downloads
     */
    const RCatalogsHttpOperationArray&
        RestoredDownloads() const;


    /**
     * @see MCatalogsHttpSession::MoveRestoredDlToCurrentDls()
     */
    TInt MoveRestoredDlToCurrentDls( 
        MCatalogsHttpOperation& aDownload );
    
    
    /**
    * Returns an array of current transactions.
    *
    * @return Array of transactions
    */        
    const RCatalogsHttpOperationArray& 
        CurrentTransactions() const;



    /**
    * Cancels all current transactions, downloads and uploads
    * and closes all connections opened by this session
    */        
    void CancelAllOperations();


    /**
     * 
     */        
    void NotifyCancelAllOperations();
    

    /**
    * Cancels all current downloads. 
    */
    void CancelAllDownloads();
    
    
    /**
    * Cancels all current transactions
    */
    void CancelAllTransactions();        
    
    
    /**
     * @see MCatalogsHttpSession::DeleteRestoredDownloads()
     */
    void DeleteRestoredDownloads();
    
    /**
    * Returns the current default configuration for transactions.
    *
    * The changes made to the configuration will affect the next
    * new transaction operation.
    *
    * @return Transaction configuration
    */
    MCatalogsHttpConfig& DefaultTransactionConfig() const;


    /**
    * Returns the current default configuration for downloads.
    *
    * The changes made to the configuration will affect the next
    * new download operation.
    *
    * @return Download configuration
    */        
    MCatalogsHttpConfig& DefaultDownloadConfig() const;
       

    /**
     * @see MCatalogsHttpSession::SetDefaultConnectionMethod
     */
    void SetDefaultConnectionMethod( const TCatalogsConnectionMethod& aMethod );    
    
    
    /**
     * @see MCatalogsHttpSession::ConnectionManager()
     */
    CCatalogsHttpConnectionManager& ConnectionManager();           


    /**
     * @see MCatalogsHttpSession::SetOptions()
     */
    void SetOptions( TUint32 aOptions );
    
public: // 

    void SetOwner( CCatalogsTransport* aOwner );
    
    void ReportConnectionStatus( TBool aActive );
    
private:

    CCatalogsHttpSession( TInt32 aSessionId, CCatalogsTransport& aOwner,
        MCatalogsHttpSessionManager& aSessionManager );
        
    void ConstructL( MCatalogsHttpSessionManager& aManager, 
        TBool aCleanupSession );

private:
        
    CCatalogsTransport* iOwner;
    MCatalogsHttpSessionManager& iSessionManager;
    TInt32 iSessionId;
    CCatalogsHttpDownloadManager* iDownloadMgr;
    CCatalogsHttpTransactionManager* iTransactionMgr;    
    TInt iRefCount;    
    CCatalogsHttpConnectionManager* iConnectionMgr;
    CCatalogsConnectionEventSink* iConnectionEventSink;
    };

#endif // C_CATALOGSHTTPSESSION_H    
