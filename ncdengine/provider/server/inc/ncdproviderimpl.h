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
* Description:   Contains CNcdProvider class
*
*/


#ifndef NCD_PROVIDER_H
#define NCD_PROVIDER_H


#include "catalogscommunicable.h"
#include "catalogshttpconnectionmanager.h"


class CNcdNodeManager;
class CNcdFavoriteManager;
class CNcdPurchaseHistoryDb;
class CNcdOperationManager;
class CNcdSubscriptionManager;
class CNcdStorageManager;
class CNcdProtocol;
class CCatalogsTransport;
class MNcdConfigurationManager;
class MCatalogsHttpSession;
class MCatalogsSmsSession;
class MCatalogsContext;
class MCatalogsAccessPointManager;
class CNcdProviderUtils;
class MCatalogsHttpSession;
class RFs;
class CNcdReportManager;
class TCatalogsConnectionMethod;
class CNcdHttpUtils;
class CNcdServerReportManager;
class MNcdProviderCloseObserver;
class CNcdGeneralManager;

struct TNcdProviderContext
    {
    MCatalogsHttpSession* iHttpSession;
    MCatalogsSmsSession* iSmsSession;
    CNcdReportManager* iReportManager;
    
    TNcdProviderContext() : 
        iHttpSession( NULL ),
        iSmsSession( NULL ),
        iReportManager( NULL )
        {
        }
    };
    
/**
 *  CNcdProvider provides functions to ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdProvider : public CCatalogsCommunicable,
    public MCatalogsHttpConnectionConfirmationObserver,
    public MCatalogsHttpConnectionErrorObserver
    {

public:

    /**
     * TNcdProviderFunctionNumber should be used when proxies are requesting 
     * services by calling ReceiveMessageL function of this class.
     */
    enum TNcdProviderFunctionNumber
        {
        /**
         *
         */
         ENcdProviderNodeManagerHandle,       

        /**
         *
         */
         ENcdProviderPurchaseHistoryHandle,       

        /**
         *
         */
         ENcdProviderOperationManagerHandle,       

        /**
         *
         */
         ENcdProviderSubscriptionManagerHandle, 
         
        /**
         *
         */
         ENcdProviderFavoriteManagerHandle,
         
         /**
          *
          */
        ENcdProviderServerReportManagerHandle,
        
        /**
         *
         */
         ENcdProviderRelease,
         
         /**
          * 
          */
         ENcdProviderAddConfiguration,

         /**
          * 
          */         
         ENcdProviderRemoveConfiguration,


         /**
          * 
          */         
         ENcdProviderRetrieveConfigurations,


         /**
          * 
          */         
         ENcdProviderSetDefaultAccessPoint,

         
         /**
          *
          */
         ENcdProviderClientId,         
         
         /**
          *
          */
         ENcdProviderClearCache,
         
         
         /**
          */
         ENcdGetProviderInfo,
         
         /**
          *
          */
         ENcdProviderIsSimChanged,
         
         /**
          *
          */
         ENcdProviderIsFixedAp,
         
         /**
          *
          */
         ENcdProviderSyncSeenInfo
         
        };


    enum TNcdProviderInfo
        {
        ENcdProviderInfoType,
        ENcdProviderInfoVersion,
        ENcdProviderInfoUid,
        ENcdProviderInfoProvisioning
        };



    /**
     * NewLC
     *
     * @param aProviderIndex Local index of the provider.
     * @return CNcdProvider* Pointer to the created object 
     * of this class.
     */
    static CNcdProvider* NewLC( 
        const TUid& aFamilyId,
        MNcdProviderCloseObserver& aCloseObserver,
        const TDesC& aEngineRoot,
        CCatalogsTransport& aTransport,
        CNcdStorageManager& aStorageManager );


    /**
     * Destructor
     */
    virtual ~CNcdProvider();

public:

    void PrepareSessionL( MCatalogsSession& aSession, TUint32 aOptions );

    /**
     * Informs objects of session removal. They should not use
     * given session in anyway anymore
     *
     * @since S60 ?S60_version
     * @param aSession Session that is not in use anymore.
     */   
    void HandleSessionRemoval( MCatalogsSession& aSession );

    void GetProviderContextL( MCatalogsContext& aContext, TNcdProviderContext& aProviderContext );

    CNcdNodeManager& NodeManager();

    MCatalogsAccessPointManager& AccessPointManager();
    
    const TUid& FamilyId() const;
    
    const TDesC& FamilyName() const;
    
    TInt DatabaseClearingStatus() const;

public: // CCatalogsCommunicable

    /**
     * TNcdNodeFunctionNumber describes the functionality that
     * may be required using this function.
     *
     * @see CCatalogsCommuniCable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    void ReceiveMessageL( 
        MCatalogsBaseMessage& aMessage,
        TInt aFunctionNumber  );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );

public: // MCatalogsHttpConnectionConfirmationObserver

    virtual TCatalogsHttpConnectionConfirmationState 
        HandleConnectionConfirmationRequestL( 
            MCatalogsHttpSession& aSession, 
            const TCatalogsConnectionMethod& aMethod );


public: // MCatalogsHttpConnectionErrorObserver


    virtual void HandleConnectionErrorL( 
        MCatalogsHttpSession& aSession,
        const TCatalogsConnectionMethod& aMethod, 
        TInt aError );


protected:

    /**
     * Constructor
     */
    CNcdProvider( 
        const TUid& aFamilyId,
        MNcdProviderCloseObserver& aCloseObserver,
        const TDesC& aEngineRoot,
        CCatalogsTransport& aTransport,
        CNcdStorageManager& aStorageManager );

    /**
     * Creates the nodemanager and the data storer.
     */
    void ConstructL();


    // Here are all the functions that are used when ReceiveMessageL is called
    // and the functionality is delegated according to the given function 
    // number.

    // Gives the operationmanager handle to the proxy 
    void NodeManagerHandleRequestL( 
        MCatalogsBaseMessage& aMessage ) const;

    // Gives the Purchase History handle to the proxy.
    void PurchaseHistoryHandleRequestL( MCatalogsBaseMessage& aMessage ) const;

    // Gives the operationmanager handle to the proxy 
    void OperationManagerHandleRequestL( 
        MCatalogsBaseMessage& aMessage ) const;

    // Gives the subscriptionmanager handle to the proxy 
    void SubscriptionManagerHandleRequestL( 
        MCatalogsBaseMessage& aMessage ) const;

    // Gives the favoritemanager handle to the proxy 
    void FavoriteManagerHandleRequestL( 
        MCatalogsBaseMessage& aMessage ) const;

    void ServerReportManagerHandleRequestL(
        MCatalogsBaseMessage& aMessage ) const;

    // Removes one reference of this object from the session.
    // ( There may be multiple references, but we take care of our own. )
    void ReleaseRequestL( MCatalogsBaseMessage& aMessage ) const;


    void AddConfigurationRequestL( MCatalogsBaseMessage& aMessage );
    
    void RemoveConfigurationRequestL( MCatalogsBaseMessage& aMessage );
    
    void RetrieveConfigurationsRequestL( 
        MCatalogsBaseMessage& aMessage );

    void SetDefaultAccessPointRequestL( 
        MCatalogsBaseMessage& aMessage );
        
    void CreateClientIdRequestL(
        MCatalogsBaseMessage& aMessage );
        
    // Clears client cache
    void ClearCacheRequestL(
        MCatalogsBaseMessage& aMessage );

    void GetInfoRequestL(
        MCatalogsBaseMessage& aMessage );
        
    void IsSimChangedRequestL( MCatalogsBaseMessage& aMessage );
    
    void IsFixedApRequestL( MCatalogsBaseMessage& aMessage );
    
    void SyncSeenInfoRequestL( MCatalogsBaseMessage& aMessage );

    void AllowCacheCleaningL( 
        const MCatalogsContext& aContext, 
        TBool aAllow );
    
    TBool StartupBeginL( const TDesC& aEnginePath );

    void CheckDatabaseVersionsL( const TDesC& aEnginePath );

    
    void StartupEndL();        
        
    void ClearProviderFilesL( const TDesC& aEnginePath );
    
    void ClearDatabasesL( const TDesC& aEnginePath );
    void ClearPurchaseHistoryL();
    void ClearTempFilesL(); 
   
    void HandleShutdownFileL();
    
    // Manages the storages if SIM card is changed.
    // aClearStorages determines whether storages are cleared or not
    void ManageStoragesL( 
        const MCatalogsContext& aContext, 
        TBool aClearStorages );
    
    // Hashes IMSI with SHA-1
    HBufC8* HashImsiLC();

    HBufC8* ConvertDataToTextL( const TDesC8& aData ) const;
    
    void PurchaseHistoryPathLC( RBuf& aPath ) const;
    
    void FamilyPathLC( RBuf& aPath ) const;
    
    void HandleProviderOptionsL( 
        const MCatalogsContext& aContext, 
        TUint32 aOptions,
        MCatalogsHttpSession& aHttpSession );
    
private:

    // Prevent these two if they are not implemented
    CNcdProvider( const CNcdProvider& aObject );
    CNcdProvider& operator =( const CNcdProvider& aObject );
      

private: // data

    // ID of the family this provider instance belongs to
    TUid iFamilyId;
    
    TUidName iFamilyName;
    
    MNcdProviderCloseObserver& iCloseObserver;
    
    const TDesC& iEngineRoot;
    
    // Local provider index number of this provider. Used to index context provider fields,
    // for example.
    TInt iProviderIndex;

    // Node manager is used to manage the creation of nodes
    CNcdNodeManager* iNodeManager;
    
    // Purchase history
    CNcdPurchaseHistoryDb* iPurchaseHistory;

    // Operation manager is used to manage the creation of operations
    CNcdOperationManager* iOperationManager;


    // Subscription manager manages subscription info
    CNcdSubscriptionManager* iSubscriptionManager;


    // Storage manager is used to manage all storages in the provider    
    CNcdStorageManager& iStorageManager;
    
    // Protocol handler manages protocol sessions and provides access
    // to response parser and request processing
    CNcdProtocol* iProtocolHandler;
    
    // Transport
    CCatalogsTransport& iTransport;
    
    MNcdConfigurationManager* iConfigurationManager;
    
    // Access point manager is used to manage creation of access points
    MCatalogsAccessPointManager* iAccessPointManager;
    
    // Favorite manager is used to manage favorite node list.
    CNcdFavoriteManager* iFavoriteManager;
    
    // Server report manager is used to manage server report sending.
    CNcdServerReportManager* iServerReportManager;
       
    // ETrue if the SIM has been changed since previous startup    
    TBool iSimChanged;
    
    // ETrue while provider is starting up
    TBool iProviderStartingUp;
    
    CNcdHttpUtils* iHttpUtils;
    
    // ETrue if the engine was not shutdown properly last time
    TBool iShutdownFailed;
      
    CNcdGeneralManager* iGeneralManager;
        
    TInt iDatabaseClearingStatus;
    };


#endif // NCD_PROVIDER_H
