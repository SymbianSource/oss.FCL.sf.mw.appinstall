/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_CATALOGSHTTPSESSION_H
#define M_CATALOGSHTTPSESSION_H

#include <e32base.h>
#include <e32cmn.h>

#include "catalogstransporttypes.h"
#include "catalogshttptypes.h"
#include "catalogstransportsession.h"
#include "catalogstransportoperationid.h"


class MCatalogsHttpOperation;   // HTTP operation
class MCatalogsHttpObserver;    // HTTP operation observer
class MCatalogsHttpConfig;
class CCatalogsHttpConnectionManager;
class TCatalogsConnectionMethod;

/**
* Interface for HTTP sessions
*/
class MCatalogsHttpSession : public MCatalogsTransportSession
    {
public:

    /**
    * Creates a new download operation
    * 
    * By default, the session owns the operation. If the client calls
    * AddRef() for the operation, then the ownership is shared between
    * the client and the session but this is rarely necessary. If the
    * the client calls AddRef() then it must also call Release() after 
    * calling Cancel()
    *
    * @param aUrl URL to download
    * @param aStart If true, download is started automatically. Otherwise
    * the caller must start it explicitly. Default: true
    * @param aObserver Observer for the download. Must be given if the 
    * default observer in the session configuration is NULL. Default: NULL
    * 
    * @return A new download operation
    */
    virtual MCatalogsHttpOperation* CreateDownloadL( const TDesC16& aUrl, 
        TBool aStart = ETrue, MCatalogsHttpObserver* aObserver = NULL ) = 0;

    virtual MCatalogsHttpOperation* CreateDownloadL( const TDesC8& aUrl, 
        TBool aStart = ETrue, MCatalogsHttpObserver* aObserver = NULL ) = 0;


    /**
    * Creates a new transaction operation
    *
    * By default, the session owns the operation. If the client calls
    * AddRef() for the operation, then the ownership is shared between
    * the client and the session but this is rarely necessary. If the
    * the client calls AddRef() then it must also call Release() after 
    * calling Cancel()
    *
    * 
    * @param aUrl Destination URL
    * @param aObserver Observer for the transaction
    *
    * @return A new transaction operation
    */
    virtual MCatalogsHttpOperation* CreateTransactionL( const TDesC16& aUrl, 
        MCatalogsHttpObserver* aObserver ) = 0;

    virtual MCatalogsHttpOperation* CreateTransactionL( const TDesC8& aUrl, 
        MCatalogsHttpObserver* aObserver ) = 0;


    /**
    * Download getter. 
    *
    * Returns the download that matches the id or NULL
    * @param aId Download ID
    * @return Download or NULL
    */
    virtual MCatalogsHttpOperation* Download( 
        const TCatalogsTransportOperationId& aId ) const = 0;


    /**
    * Transaction getter. 
    *
    * Returns the transaction that matches the id or NULL
    * @param aId Transaction ID
    * @return Transaction or NULL
    */            
    virtual MCatalogsHttpOperation* Transaction( 
        const TCatalogsTransportOperationId& aId ) const = 0;


    /**
    * Returns an array of current downloads. Array includes the downloads
    * that were paused or left executing when the client stopped the
    * last time.
    *
    * @return Array of downloads
    * @note Doesn't show downloads that were completed during the time
    * the client was not running
    */
    virtual const RCatalogsHttpOperationArray& 
        CurrentDownloads() const = 0;
    
    
    /**
    * Returns an array of downloads restored from the previous session. 
    *
    * The client should always call this method and either delete the 
    * downloads or restore them after setting the observer for them.
    *
    * @note Both observer and access point must be set for the restored
    * downloads by the client before they can be resumed. 
    *
    * @return Array of restored downloads
    */
    virtual const RCatalogsHttpOperationArray&
        RestoredDownloads() const = 0;
    

    /**
     * Moves a restored download to current downloads
     *
     * @param aDownload Download to move
     * @return KErrNone or other Symbian error code
     * @note Restored downloads are automatically moved to current 
     * downloads when they are started.
     */
    virtual TInt MoveRestoredDlToCurrentDls( 
        MCatalogsHttpOperation& aDownload ) = 0;
    
    
    /**
    * Returns an array of current transactions.
    *
    * @return Array of transactions
    */        
    virtual const RCatalogsHttpOperationArray& 
        CurrentTransactions() const = 0;



    /**
    * Cancels all current transactions, downloads and uploads
    * and closes all connections opened by this session
    */        
    virtual void CancelAllOperations() = 0;
    
    
    /**
     *
     */
    virtual void NotifyCancelAllOperations() = 0;
    
    /**
    * Cancels all current downloads. 
    */
    virtual void CancelAllDownloads() = 0;
    
    
    /**
    * Cancels all current transactions
    */
    virtual void CancelAllTransactions() = 0;
    
    
    /**
     * Deletes the restored downloads
     */
    virtual void DeleteRestoredDownloads() = 0;
    
    /**
    * Returns the current default configuration for transactions.
    *
    * The changes made to the configuration will affect the next
    * new transaction operation.
    *
    * @return Transaction configuration
    */
    virtual MCatalogsHttpConfig& DefaultTransactionConfig() const = 0;


    /**
    * Returns the current default configuration for downloads.
    *
    * The changes made to the configuration will affect the next
    * new download operation.
    *
    * @return Download configuration
    */        
    virtual MCatalogsHttpConfig& DefaultDownloadConfig() const = 0;
    
            
    /**
     * Sets default connection method for downloads and transactions
     *
     * @param aMethod Connection method
     */
    virtual void SetDefaultConnectionMethod( 
        const TCatalogsConnectionMethod& aMethod ) = 0;
    
    
    /**
     * Connection manager getter
     * @return Connection Manager
     */
    virtual CCatalogsHttpConnectionManager& ConnectionManager() = 0;
    
    /**
     * Session options setter
     * 
     * @param aOptions Options
     */
    virtual void SetOptions( TUint32 aOptions ) = 0;
            
    };

#endif // M_CATALOGSHTTPSESSION_H
