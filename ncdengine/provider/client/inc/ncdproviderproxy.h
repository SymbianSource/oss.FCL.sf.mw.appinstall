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
* Description:   Contains CNcdProviderProxy class
*
*/


#ifndef NCD_PROVIDER_PROXY_H
#define NCD_PROVIDER_PROXY_H


#include <e32base.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdprovider.h"
#include "ncdproviderpcclientsupport.h"
#include "ncddebuginformation.h"
#include "catalogsbaseimpl.h"

class MCatalogsClientServer;
class CNcdNodeManagerProxy;
class CNcdPurchaseHistoryProxy;
class CNcdOperationManagerProxy;
class MNcdFileDownloadOperation;
class MNcdFileDownloadOperationObserver;
class CNcdSubscriptionManagerProxy;
class CNcdFavoriteManagerProxy;
class CCatalogsAsyncMediator;
class CNcdCreateAccessPointOperationProxy;
class CNcdServerReportManagerProxy;

/**
 *  Class to used in testing of provider
 *
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdProviderProxy : public CNcdInterfaceBaseProxy,
                          public MNcdProvider,
                          public MNcdProviderPcClientSupport,
                          public MNcdDebugInformation
    {
public:

    static CNcdProviderProxy* NewL( MCatalogsClientServer& aSession,
                                    TInt aHandle );

    static CNcdProviderProxy* NewLC( MCatalogsClientServer& aSession,
                                     TInt aHandle );

    /**
     * Destructor
     * Releases node manager and operation manager proxies until they
     * are destroyed.
     */
    virtual ~CNcdProviderProxy();
    
    /**
     * Initiates callback to MNcdProviderObserver::ExpirationInformationReceived
     */
    void ExpirationCallback( RCatalogsArray< MNcdNode >& aExpiredNodes );

    /**
     * @return CNcdOperationManagerProxy&
     */
    CNcdOperationManagerProxy& OperationManager();

    /**
     * @return CNcdServerReportManagerProxy&
     */
    CNcdServerReportManagerProxy& ServerReportManager();
    

public: // MNcdProvider

    /**
     * @see MNcdProvider::SetObserver
     */
    virtual void SetObserver( MNcdProviderObserver* aObserver );
    
    /**
     * @see MNcdProvider::RootNodeL
     */
    virtual MNcdNode* RootNodeL();
    
    /**
     * @see MNcdProvider::Operations
     */
    virtual RCatalogsArray<MNcdOperation> OperationsL() const;

    /**
     * @see MNcdProvider::PurchaseHistoryL
     */
    virtual MNcdPurchaseHistory* PurchaseHistoryL() const;
    
    /**
     * @see MNcdProvider::SubscriptionsL
     */
    virtual MNcdSubscriptionManager* SubscriptionsL() const;

    /**
     * @see MNcdProvider::AddConfigurationL
     */
    virtual void AddConfigurationL( const MNcdKeyValuePair& aConfiguration );
    
    /**
     * @see MNcdProvider::RemoveConfigurationL
     */
    virtual void RemoveConfigurationL( const TDesC& aKey );

    /**
     * @see MNcdProvider::ConfigurationsL
     */
    virtual RPointerArray< CNcdKeyValuePair > ConfigurationsL() const;

    /**
     * @see MNcdProvider::NodeL
     */
    virtual MNcdNode* NodeL( const TDesC& aNameSpaceId, 
                             const TDesC& aNodeId ) const;

    /**
     * @see MNcdProvider::NodeL( const MNcdPurchaseDetails& aDetails )
     */
    virtual MNcdNode* NodeL( const MNcdPurchaseDetails& aDetails ) const;
        
    /**
     * @see MNcdProvider::SchemeNodeL
     */
    virtual MNcdNode* SchemeNodeL( const TDesC& aNameSpaceId, 
                                   const TDesC& aEntityId,
                                   const TDesC& aServerUri,
                                   TNcdSchemeNodeType aType,
                                   TBool aRemoveOnDisconnect,
                                   TBool aForceCreate ) const;


    /**
     * @see MNcdProvider::SetStringLocalizer
     */
    virtual void SetStringLocalizer( MNcdClientLocalizer& aLocalizer );


    /**
     * @see MNcdProvider::DownloadFileL
     */
    virtual MNcdFileDownloadOperation* DownloadFileL( const TDesC& aUri,
        const TDesC& aTargetFileName,
        MNcdFileDownloadOperationObserver& aObserver );

    
    /**
     * @see MNcdProvider::SetDefaultConnectionMethodL()
     */
    virtual void SetDefaultConnectionMethodL( 
        const TNcdConnectionMethod& aAccessPointId );
    
    /**
     * @see MNcdProvider::ClearSearchResultsL()
     */
    virtual void ClearSearchResultsL();
        
    /**
     * @see MNCdProvider::ClearCacheL()
     */
    virtual void ClearCacheL( TRequestStatus& aStatus );

    
    /**
     * @see MNcdProvider::IsSimChangedL()
     */
    virtual TBool IsSimChangedL();
    
    /**
     * @see MNcdProvider::IsFixedAp()
     */
    virtual TBool IsFixedApL();


public: // from MNcdProviderPcClientSupport

    /**
     * @see MNcdProviderPcClientSupport::DownloadAndInstallRightsObjectL()
     */
    virtual MNcdRightsObjectOperation* DownloadAndInstallRightsObjectL(
        MNcdRightsObjectOperationObserver& aObserver,
        const TDesC& aDownloadUri, 
        const TDesC& aMimeType, 
        const TNcdConnectionMethod& aConnectionMethod );

    /**
     * @see MNcdProviderPcClientSupport::CreateAccessPointL()
     */
    virtual MNcdCreateAccessPointOperation* CreateAccessPointL( 
        const TDesC& aAccessPointData,
        MNcdCreateAccessPointOperationObserver& aObserver );


    /**
     * @see MNcdProviderPcClientSupport::SendHttpRequestL()
     */
    virtual MNcdSendHttpRequestOperation* SendHttpRequestL(
        const TDesC8& aUri,
        const TDesC8& aRequest, 
        const TNcdConnectionMethod& aConnectionMethod, 
        MNcdSendHttpRequestOperationObserver& aObserver );
    
public: // from MNcdDebugInformation

    /**
     * @see MNcdDebugInformation::ClientId().
     */
    virtual HBufC* ClientIdL();


    /**
     * @see MNcdDebugInformation::EngineTypeL()
     */
    virtual HBufC* EngineTypeL();
    
    /**
     * @see MNcdDebugInformation::EngineVersionL()
     */
    virtual HBufC* EngineVersionL();        
    
    
    /**
     * @see MNcdDebugInformation::EngineProvisioningL()
     */
    virtual HBufC* EngineProvisioningL();
    
    
    /**
     * @see MNcdDebugInformation::EngineUidL()
     */
    virtual HBufC* EngineUidL();
    
protected:

    CNcdProviderProxy( MCatalogsClientServer& aSession,
                       TInt aHandle );
                       

    /**
     * Calls the ConstructL of the parent class.
     * Also, uses CreateNodeManagerL and CreateOperationManagerL
     * to create managers.
     */
    virtual void ConstructL();

    /**
     * CreateNodeManagerL
     * ConstructL will use this function to create the manager.
     * If a child class implements this function then it is called
     * instead.
     */
    virtual void CreateNodeManagerL();

    /**
     * CreatePurchaseHistoryL
     * ConstructL will use this function to create the purchase history.
     * If a child class implements this function then it is called
     * instead.
     */
    virtual void CreatePurchaseHistoryL();

    /**
     * CreateOperationManagerL
     * ConstructL will use this function to create the manager.
     * If a child class implements this function then it is called
     * instead.
     */
    virtual void CreateOperationManagerL();

    /**
     * CreateSubscriptionManagerL
     * ConstructL will use this function to create the manager.
     * If a child class implements this function then it is called
     * instead.
     */
    virtual void CreateSubscriptionManagerL();
    
    /**
     * CreateFavoriteManagerL
     * ConstructL will use this function to create the manager.
     * If a child class implements this function then it is called
     * instead.
     */    
    virtual void CreateFavoriteManagerL();

    /**
     * CreateServerReportManagerL
     * ConstructL will use this function to create the manager.
     * If a child class implements this function then it is called
     * instead.
     */    
    virtual void CreateServerReportManagerL();
    

    HBufC* GetInfoFromProviderL( TInt aInfoIndex );
    
        
    void InternalizeFavoriteManagerL();

    static TInt InternalizeFavoriteManager( TAny* aProvider );
    
private:

    // Prevent if not implemented
    CNcdProviderProxy( const CNcdProviderProxy& aObject );
    CNcdProviderProxy& operator =( const CNcdProviderProxy& aObject );


private: // data

    CNcdNodeManagerProxy* iNodeManager;
    CNcdPurchaseHistoryProxy* iPurchaseHistory;
    CNcdOperationManagerProxy* iOperationManager;
    CNcdSubscriptionManagerProxy* iSubscriptionManager;
    CNcdFavoriteManagerProxy* iFavoriteManager;
    CNcdServerReportManagerProxy* iServerReportManager;

    MNcdProviderObserver* iObserver;
    
    /**
     * Client localizer, not own.
     */
    MNcdClientLocalizer* iClientLocalizer;

         
    // Buffer for returning data from asynchronous requests
    TBuf8<1> iReturnBuf;
    
    CCatalogsAsyncMediator* iAsyncMediator;

    };

#endif // NCD_PROVIDER_PROXY_H
