/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdProviderProxy class implementation
*
*/


#include "ncdproviderproxy.h"
#include "catalogsclientserver.h"
#include "ncdnodemanagerproxy.h"
#include "ncdpurchasehistoryproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdproviderimpl.h"
#include "ncdnodefolderproxy.h"
#include "ncdoperation.h"
#include "ncdnodeidentifier.h"
#include "catalogsdebug.h"
#include "ncdkeyvaluepair.h"
#include "ncdkeyvaluemap.h"
#include "catalogsutils.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdrootnodeproxy.h"
#include "ncdutils.h"
#include "ncdfiledownloadoperationproxy.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "ncdrightsobjectoperationproxy.h"
#include "ncdproviderdefines.h"
#include "ncdproviderobserver.h"
#include "ncdfavoritemanagerproxy.h"
#include "ncdschemenodetype.h"
#include "ncdpanics.h"
#include "catalogsasyncmediator.h"
#include "ncdcreateaccesspointoperationproxy.h"
#include "ncdsendhttprequestoperationproxy.h"
#include "ncdconnectionmethod.h"
#include "ncdserverreportmanagerproxy.h"
#include "catalogsconstants.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdProviderProxy* CNcdProviderProxy::NewL( MCatalogsClientServer& aSession, 
                                            TInt aHandle  )
    {
    CNcdProviderProxy* self =  
        CNcdProviderProxy::NewLC( aSession, aHandle );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdProviderProxy* CNcdProviderProxy::NewLC( MCatalogsClientServer& aSession, 
                                             TInt aHandle  )
    {
    CNcdProviderProxy* self = 
        new( ELeave ) CNcdProviderProxy( aSession, aHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdProviderProxy::~CNcdProviderProxy()
    {
    DLTRACEIN((""));
    
    
    if ( iOperationManager ) 
        {
        // Release downloads that had been restored
        // from a previous session but not used by any clients.
        //
        // This has to be done before the Node manager is deleted because
        // operations can have references to nodes which have to be released
        // first
        iOperationManager->ReleaseUnusedOperations();
        }

    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdProvider::KInterfaceUid );

    DLTRACE(("Observers reset"));
    
    // Delete managers
    delete iServerReportManager;
    iServerReportManager = NULL;
    
    delete iNodeManager;
    iNodeManager = NULL;

    delete iPurchaseHistory;
    iPurchaseHistory = NULL;
    
    delete iOperationManager;
    iOperationManager = NULL;
    
    delete iSubscriptionManager;
    iSubscriptionManager = NULL;
    
    delete iFavoriteManager;
    iFavoriteManager = NULL;

    delete iAsyncMediator;
    iAsyncMediator = NULL;
    // Do not delete observers here.

    DLTRACEOUT(("Operation manager released"));
    }

// ---------------------------------------------------------------------------
// ExpirationCallback
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::ExpirationCallback( RCatalogsArray< MNcdNode >& aExpiredNodes )
    {
    DLTRACEIN((""));
    if( iObserver )
        {
        iObserver->ForceExpirationInformationReceived( aExpiredNodes );
        }
    }


// ---------------------------------------------------------------------------
// OperationManager 
// ---------------------------------------------------------------------------
//
CNcdOperationManagerProxy& CNcdProviderProxy::OperationManager()
    {
    return *iOperationManager;    
    }


// ---------------------------------------------------------------------------
// ServerReportManager
// ---------------------------------------------------------------------------
//
CNcdServerReportManagerProxy& CNcdProviderProxy::ServerReportManager()
    {
    return *iServerReportManager;
    }

    
// ---------------------------------------------------------------------------
// SetObserver 
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::SetObserver( MNcdProviderObserver* aObserver )
    {
    // No need to delete old observer. It is owned elsewhere.
    iObserver = aObserver;    
    }


// ---------------------------------------------------------------------------
// RootNodeL 
// ---------------------------------------------------------------------------
//
MNcdNode* CNcdProviderProxy::RootNodeL()
    {
    DLTRACEIN( ("") );
    
    DASSERT( iNodeManager );
        
    DLTRACE( ("Getting root node from the node manager") );

    // Node manager will handle the creation of the root.
    CNcdRootNodeProxy* root( &iNodeManager->RootNodeL() );
    
    DLTRACE(("Root: %X", root));

    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    root->AddRef();
    
    DLTRACEOUT((""));
    
    return root;    
    }
    

// ---------------------------------------------------------------------------
// OperationsL
// ---------------------------------------------------------------------------
//    
RCatalogsArray<MNcdOperation> CNcdProviderProxy::OperationsL() const
    {
    DLTRACEIN( ( "" ) );
    DASSERT( iOperationManager );
        
    RCatalogsArray<MNcdOperation> operations;
    CleanupClosePushL( operations );
    
    // Get the original array and insert its content to catalogs array.
    // Also, increase the reference counter for the items.
    const RPointerArray<MNcdOperation>& origArray = iOperationManager->Operations();
    MNcdOperation* oper( NULL );
    operations.ReserveL( origArray.Count() );
    for ( TInt i = 0; i < origArray.Count(); ++i )
        {
        oper = origArray[ i ];
        operations.AppendL( oper );
        oper->AddRef();
        }

    DLTRACEOUT( ( "" ) );

    CleanupStack::Pop( &operations );
    
    return operations;
    }
        

// ---------------------------------------------------------------------------
// PurchaseHistoryL 
// ---------------------------------------------------------------------------
//    
MNcdPurchaseHistory* CNcdProviderProxy::PurchaseHistoryL() const
    {
    DLTRACEIN( ("") );
    DASSERT( iPurchaseHistory );
        
    DLTRACE( ("Getting purchase history") );
    // Get purchase history interface.
    MNcdPurchaseHistory* history( iPurchaseHistory );
    
    // Increase also the reference counter by one here.
    history->AddRef();
    
    
    DLTRACEOUT( ("Purchase history: %X", history ) );
    return history;    
    }
    
// ---------------------------------------------------------------------------
// SubscriptionsL
// ---------------------------------------------------------------------------
//    
MNcdSubscriptionManager* CNcdProviderProxy::SubscriptionsL() const
    {
    DLTRACEIN( ( "" ) );
    DASSERT( iSubscriptionManager );

    iSubscriptionManager->AddRef();        
    return iSubscriptionManager;
    }


// ---------------------------------------------------------------------------
// AddConfigurationL 
// ---------------------------------------------------------------------------
//    
void CNcdProviderProxy::AddConfigurationL( const MNcdKeyValuePair& aConfiguration )
    {
    DLTRACEIN(( _L("Key: %S, value: %S"), &aConfiguration.Key(),
        &aConfiguration.Value() ));
    
    // Externalize the config.
    CNcdKeyValuePair* config = CNcdKeyValuePair::NewLC( aConfiguration );
    HBufC8* data = config->ExternalizeToDesLC();
    
    TBuf8<1> err;
    
    // Send the config
    User::LeaveIfError( ClientServerSession().SendSync( 
        CNcdProvider::ENcdProviderAddConfiguration,
        *data,
        err,
        Handle() ) );

    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( config );
    
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// RemoveConfigurationL 
// ---------------------------------------------------------------------------
//    
void CNcdProviderProxy::RemoveConfigurationL( const TDesC& aKey )
    {
    DLTRACEIN(( _L("Key: %S"), &aKey ) );
    
    RCatalogsBufferWriter source;
    source.OpenLC();
    ExternalizeDesL( aKey, source() );
    
    
    TBuf8<1> buf;
    
    // Send the config
    User::LeaveIfError(
            ClientServerSession().
                SendSync( CNcdProvider::ENcdProviderRemoveConfiguration,
                          source.PtrL(),
                          buf,
                          Handle() ) );
    
    CleanupStack::PopAndDestroy( &source );
    TInt err = buf[0];
    
    DLTRACEOUT( ( "Provider returned err: %d", err ) );
    
    User::LeaveIfError( err );
    }


// ---------------------------------------------------------------------------
// ConfigurationsL
// ---------------------------------------------------------------------------
//    
RPointerArray<CNcdKeyValuePair> CNcdProviderProxy::ConfigurationsL() const
    {
    DLTRACEIN( ( "" ) );        

    HBufC8* buf = NULL;
    
    // Ask for the configs
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( CNcdProvider::ENcdProviderRetrieveConfigurations,
                          KNullDesC8,
                          buf,
                          Handle(),
                          0 ) );
    
    User::LeaveIfNull( buf );
    DLINFO(("Received %d bytes", buf->Length() ));
    
    CleanupStack::PushL( buf );
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    CNcdKeyValueMap* map = CNcdKeyValueMap::NewLC();
    map->InternalizeL( stream );
        

    // Copy the pointers. RPointerArray's copy constructor doesn't 
    // work correctly
    RPointerArray<CNcdKeyValuePair> pairs;    
    pairs.ReserveL( map->Pairs().Count() );
    for ( TInt i = 0; i < map->Pairs().Count(); ++i ) 
        {
        pairs.Append( map->Pairs()[i] );
        }
    
    // Reset the map so that the actual pairs are not deleted with
    // the map    
    map->Reset();    
    CleanupStack::PopAndDestroy( 3 ); // map, stream, buf

    DLTRACEOUT(("Config count: %d", pairs.Count() ));
    return pairs;
    }
    

// ---------------------------------------------------------------------------
// NodeL
// ---------------------------------------------------------------------------
//    
MNcdNode* CNcdProviderProxy::NodeL( const TDesC& aNameSpaceId, 
                                    const TDesC& aNodeId ) const
    {
    DLTRACEIN( ( "namespace: %S, id: %S", &aNameSpaceId, &aNodeId ) );
    
    DASSERT( iNodeManager );
        
    DLTRACE( ( "Getting node from the node manager" ) );
    
    // NOTE!!! That the identifier is set here NULL! This is because
    // we do not know the actual UID here. But this uid value will be
    // replaced by the correct value when the server actually creates the
    // node.
    // NOTE ALSO!!! The server uri has not been set here for the node. So,
    // the server side should not use the uri when comparing nodes in this case.
    CNcdNodeIdentifier* identifier( 
        CNcdNodeIdentifier::NewLC( aNameSpaceId, aNodeId, TUid::Null() ) );
            
    // Node manager will handle the creation of the root.
    MNcdNode* node( &iNodeManager->NodeL( *identifier ) );
    
    CleanupStack::PopAndDestroy( identifier );
    
    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    DASSERT( node );
    node->AddRef();
    
    DLTRACEOUT( ( "" ) );
    return node;    
    }
    

// ---------------------------------------------------------------------------
// NodeL
// ---------------------------------------------------------------------------
//        
MNcdNode* CNcdProviderProxy::NodeL( const MNcdPurchaseDetails& aDetails ) const
    {
    DLTRACEIN((""));
    
    // This function call will create the temporary node according to the
    // purchase details. Even if the node and its metadata does not exist
    // in the RAM cache or in the db, the server will create a metadata and
    // a temporary node according to the info gotten from the purchase details.
    MNcdNode* node( &iNodeManager->CreateTemporaryNodeL( aDetails ) );

    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    DASSERT( node );
    node->AddRef();

    DLTRACEOUT((""));

    return node;
    }


// ---------------------------------------------------------------------------
// SchemeNodeL
// ---------------------------------------------------------------------------
//    
MNcdNode* CNcdProviderProxy::SchemeNodeL( const TDesC& aNameSpaceId, 
                                          const TDesC& aEntityId,
                                          const TDesC& aServerUri,
                                          TNcdSchemeNodeType aType,
                                          TBool aRemoveOnDisconnect,
                                          TBool aForceCreate ) const
    {
    DLTRACEIN((_L("namespace: %S, id: %S"), &aNameSpaceId, &aEntityId));
    
    DASSERT( iNodeManager );
        
    DLTRACE( ( "Getting node from the node manager" ) );
    
    // NOTE!!! That the identifier is set here NULL! This is because
    // we do not know the actual UID here. But this uid value will be
    // replaced by the correct value when the server actually creates the
    // node.

    CNcdNodeIdentifier* identifier( 
        CNcdNodeIdentifier::NewLC(
            aNameSpaceId,
            aEntityId,
            aServerUri,
            TUid::Null() ) );
            
    if ( identifier->ContainsEmptyFields( ETrue, ETrue, ETrue, EFalse ) ) 
        {
        DLINFO(("Empty identifier fields, leave KErrArgument"));
        User::Leave( KErrArgument );
        }
            
    // Node manager will handle the creation of the scheme node.
    MNcdNode* node( NULL );
    node = &iNodeManager->CreateSchemeNodeL(
            *identifier, aType, aRemoveOnDisconnect, aForceCreate );
                
    CleanupStack::PopAndDestroy( identifier );
    
    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    DASSERT( node );
    
    node->AddRef();
    DLINFO(("node state: %d", node->State() ));                
    
    DLTRACEOUT((""));    
    return node;    
    }


// ---------------------------------------------------------------------------
// SetStringLocalizer
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::SetStringLocalizer( MNcdClientLocalizer& aLocalizer )
    {
    iClientLocalizer = &aLocalizer;
    iOperationManager->SetClientLocalizer( aLocalizer );
    }


// ---------------------------------------------------------------------------
// File download creator
// ---------------------------------------------------------------------------
//
MNcdFileDownloadOperation* CNcdProviderProxy::DownloadFileL( const TDesC& aUri,
    const TDesC& aTargetFileName,
    MNcdFileDownloadOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    DASSERT( iOperationManager );
    
    MNcdFileDownloadOperation* dl = 
        iOperationManager->CreateFileDownloadOperationL(
            ENcdGenericFileDownload,
            aUri, 
            aTargetFileName, 
            &aObserver );
    
    DLTRACEOUT((""));
    return dl;
    }
   

// ---------------------------------------------------------------------------
// Default access point setter
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::SetDefaultConnectionMethodL( 
    const TNcdConnectionMethod& aAccessPointId )
    {
    DLTRACEIN(( "aAccessPointId type=%u, id=%u", 
        aAccessPointId.iType,
        aAccessPointId.iId ));
    
#ifdef __SERIES60_31__    
    if ( aAccessPointId.iType == ENcdConnectionMethodTypeDestination )
        {
        DLERROR(("SNAPs are not supported in 3.1"));
        User::Leave( KErrNotSupported );
        }
#endif
    
    TPtrC8 input( reinterpret_cast< const TUint8*>( &aAccessPointId ), 
        sizeof( aAccessPointId ) );
    TInt err;   // note: not used?
    
    // Send the config
    User::LeaveIfError( ClientServerSession().SendSync( 
        CNcdProvider::ENcdProviderSetDefaultAccessPoint,
        input,
        err,
        Handle() ) );

    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// Clears search results
// ---------------------------------------------------------------------------
//        
void CNcdProviderProxy::ClearSearchResultsL()
    {
    iNodeManager->ClearSearchResultsL();
    }
    

// ---------------------------------------------------------------------------
// Clears client cache
// ---------------------------------------------------------------------------
//        
void CNcdProviderProxy::ClearCacheL( TRequestStatus& aStatus )
    {
    DLTRACEIN((""));
    
    iOperationManager->CancelAllOperations();
    
    // AsyncMediator is used to internalize favorites after cache cleanup
    // because they may have changed due to database corruption
    delete iAsyncMediator;
    iAsyncMediator = NULL;
    
    iAsyncMediator = new( ELeave ) CCatalogsAsyncMediator( 
        aStatus, 
        TCallBack( &CNcdProviderProxy::InternalizeFavoriteManager, this ) );        
    
    // Notice that the proxy side node ram cache should be automatically cleared
    // when the UI has released the nodes.
    // This function is meant for the db cache clearing.
    ClientServerSession().SendAsync(
        CNcdProvider::ENcdProviderClearCache,
        KNullDesC8,
        iReturnBuf,
        Handle(),
        iAsyncMediator->iStatus );

    iAsyncMediator->Activate();
    DLTRACEOUT((""));    
    }


// ---------------------------------------------------------------------------
// Tells whether the SIM was changed since previous startup
// ---------------------------------------------------------------------------
//        
TBool CNcdProviderProxy::IsSimChangedL() 
    {
    DLTRACEIN((""));    
    
    TInt isChanged;
    User::LeaveIfError( ClientServerSession().SendSync(
        CNcdProvider::ENcdProviderIsSimChanged,
        KNullDesC8,
        isChanged,
        Handle()));
        
    return isChanged;
    }
    
    
// ---------------------------------------------------------------------------
// Tells whether engine uses fixed access point
// ---------------------------------------------------------------------------
//        
TBool CNcdProviderProxy::IsFixedApL() 
    {
    DLTRACEIN((""));
    
    TInt isFixedAp;
    User::LeaveIfError( ClientServerSession().SendSync(
        CNcdProvider::ENcdProviderIsFixedAp,
        KNullDesC8,
        isFixedAp,
        Handle() ) );
        
    return isFixedAp;
    }
        

// ---------------------------------------------------------------------------
// Downloads and installa a rights object
// ---------------------------------------------------------------------------
//        
MNcdRightsObjectOperation* CNcdProviderProxy::DownloadAndInstallRightsObjectL(
    MNcdRightsObjectOperationObserver& aObserver,
    const TDesC& aDownloadUri, 
    const TDesC& aMimeType, 
    const TNcdConnectionMethod& aConnectionMethod )
    {
    DLTRACEIN(( _L("aDownloadUri=%S, aMimeType=%S"), 
        &aDownloadUri, &aMimeType ));
    DASSERT( iOperationManager );

    CNcdRightsObjectOperationProxy* op = 
        iOperationManager->CreateRightsObjectOperationL( 
            aDownloadUri, aMimeType, aConnectionMethod, aObserver );

    return op;
    }


// ---------------------------------------------------------------------------
// Creates a new accesspoint
// ---------------------------------------------------------------------------
//        
MNcdCreateAccessPointOperation* CNcdProviderProxy::CreateAccessPointL( 
    const TDesC& aAccessPointData,
    MNcdCreateAccessPointOperationObserver& aObserver )
    {
    DLTRACEIN((""));
        
    CNcdCreateAccessPointOperationProxy* op = 
        iOperationManager->CreateCreateAccessPointOperationL( 
            aAccessPointData, 
            aObserver );
    
    return op;
    }


// ---------------------------------------------------------------------------
// Send a HTTP request
// ---------------------------------------------------------------------------
//        
MNcdSendHttpRequestOperation* CNcdProviderProxy::SendHttpRequestL(
    const TDesC8& aUri,
    const TDesC8& aRequest, 
    const TNcdConnectionMethod& aConnectionMethod, 
    MNcdSendHttpRequestOperationObserver& aObserver )
    {
    DLTRACEIN((""));
    CNcdSendHttpRequestOperationProxy* op = 
        iOperationManager->CreateSendHttpRequestOperationL( 
            aUri,
            aRequest, 
            aConnectionMethod,
            aObserver );
    return op;
    
    }


// ---------------------------------------------------------------------------
// Cliend ID getter
// ---------------------------------------------------------------------------
//        
HBufC* CNcdProviderProxy::ClientIdL() 
    {
    DLTRACEIN((""));
    
    
    RBuf clientId;
    clientId.CleanupClosePushL();
    clientId.CreateL( NcdProviderDefines::KClientIdMaxLength );
    User::LeaveIfError( ClientServerSession().SendSync(
        CNcdProvider::ENcdProviderClientId,
        KNullDesC,
        clientId,
        Handle() ) );
    
    HBufC* copy = clientId.AllocL();
    CleanupStack::PopAndDestroy( &clientId );
    return copy;
    }
    
        
// ---------------------------------------------------------------------------
// Engine type getter
// ---------------------------------------------------------------------------
//    
HBufC* CNcdProviderProxy::EngineTypeL()
    {
    return GetInfoFromProviderL( CNcdProvider::ENcdProviderInfoType );
    }

// ---------------------------------------------------------------------------
// Engine version getter
// ---------------------------------------------------------------------------
//    
HBufC* CNcdProviderProxy::EngineVersionL()
    {
    return GetInfoFromProviderL( CNcdProvider::ENcdProviderInfoVersion );
    }


// ---------------------------------------------------------------------------
// Engine provisioning getter
// ---------------------------------------------------------------------------
//    
HBufC* CNcdProviderProxy::EngineProvisioningL()
    {
    return GetInfoFromProviderL( CNcdProvider::ENcdProviderInfoProvisioning );
    }


// ---------------------------------------------------------------------------
// Engine UID getter
// ---------------------------------------------------------------------------
//    
HBufC* CNcdProviderProxy::EngineUidL()
    {
    return GetInfoFromProviderL( CNcdProvider::ENcdProviderInfoUid );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CNcdProviderProxy::CNcdProviderProxy( MCatalogsClientServer& aSession, 
                                      TInt aHandle )  
: CNcdInterfaceBaseProxy( aSession, aHandle, NULL )
    {
    SetReleaseId( CNcdProvider::ENcdProviderRelease );
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::ConstructL()
    {
    DLTRACEIN((""));

    // Register the interfaces
    MNcdProvider* provider( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( provider, this,
                                            MNcdProvider::KInterfaceUid ) );

    MNcdProviderPcClientSupport* support( this );
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL( support, this,
                                            MNcdProviderPcClientSupport::KInterfaceUid ) );

    MNcdDebugInformation* debugInfo( this );
    AddInterfaceL(
        CCatalogsInterfaceIdentifier::NewL( debugInfo, this,
                                            MNcdDebugInformation::KInterfaceUid ) );
                                            
    DLTRACE(("Base construct done"));

    // Create managers
    // First operation manager because node manager uses it.
    CreateOperationManagerL();
    
    DLTRACE(("Operationmanager done"));
    
    CreateSubscriptionManagerL();

    CreateNodeManagerL();
    
    DLTRACE(("Nodemanager done"));

    iSubscriptionManager->SetNodeManager( iNodeManager );
    DLTRACE(("Subscriptionmanager done"));

    CreatePurchaseHistoryL();
    
    DLTRACE(("Purchase history done"));

    CreateFavoriteManagerL();
    
    iNodeManager->SetFavoriteManager( *iFavoriteManager );

    CreateServerReportManagerL();
    
    DLTRACE(("Favoritemanager done"));

    // This restores downloads from previous sessions
    // NOTE: loads nodes and does other crazy stuff so keep it last    
    iOperationManager->RestoreContentDownloadsL();
    
    }
 
 
// ---------------------------------------------------------------------------
// CreateNodeManagerL
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::CreateNodeManagerL()
    {
    DLTRACEIN((""));
    // Get the handle for the manager from the provider
    TInt nodeManagerHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync( CNcdProvider::ENcdProviderNodeManagerHandle,
                          KNullDesC,
                          nodeManagerHandle,
                          Handle() ) );
    
    // Use the same session for the manager as for the provider
    iNodeManager = 
        CNcdNodeManagerProxy::NewL( ClientServerSession(), 
                                    nodeManagerHandle,
                                    *iOperationManager,
                                    *iSubscriptionManager,
                                    *this );    
    DLTRACEOUT((""));                                    
    }


// ---------------------------------------------------------------------------
// CreatePurchaseHistoryL
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::CreatePurchaseHistoryL()
    {
    DLTRACEIN((""));
    // Get the handle for the manager from the provider
    TInt purchaseHistoryHandle( 0 );
    User::LeaveIfError(
        ClientServerSession().SendSync(
            CNcdProvider::ENcdProviderPurchaseHistoryHandle,
            KNullDesC,
            purchaseHistoryHandle,
            Handle() ) );
    
    // Use the same session for the manager as for the provider
    iPurchaseHistory = 
        CNcdPurchaseHistoryProxy::NewL(
            ClientServerSession(),
            purchaseHistoryHandle,
            this,
            *iOperationManager );
    DLTRACEOUT((""));             
    }


// ---------------------------------------------------------------------------
// CreateOperationManagerL
// ---------------------------------------------------------------------------
//
void CNcdProviderProxy::CreateOperationManagerL()
    {
    
    DLTRACEIN((""));
    
    // Get the handle for the manager from the provider
    TInt operationManagerHandle( 0 );

    DLTRACE(("Before session"));
    MCatalogsClientServer& session = ClientServerSession();
    DLTRACE(("After session: %X", &session));

    User::LeaveIfError(
            ClientServerSession().
                SendSync( CNcdProvider::ENcdProviderOperationManagerHandle,
                          KNullDesC,
                          operationManagerHandle,
                          Handle() ) );
    
    DLTRACE(("Send done"));

    // Use the same session for the manager as for the provider
    iOperationManager = 
        CNcdOperationManagerProxy::NewL( ClientServerSession(), 
                                         operationManagerHandle );    
    DLTRACE(("Opmanager done"));
    
    }

// ---------------------------------------------------------------------------
// CreateSubscriptionManagerL
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::CreateSubscriptionManagerL()
    {
    DLTRACEIN((""));
    // Get the handle for the manager from the provider
    TInt subscriptionManagerHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync(
                    CNcdProvider::ENcdProviderSubscriptionManagerHandle,
                    KNullDesC,
                    subscriptionManagerHandle,
                    Handle() ) );
    
    // Use the same session for the manager as for the provider
    iSubscriptionManager = 
        CNcdSubscriptionManagerProxy::NewL( ClientServerSession(), 
                                            subscriptionManagerHandle,
                                            this,
                                            *iOperationManager );    
    DLTRACEOUT((""));                                    
    }
    

// ---------------------------------------------------------------------------
// CreateFavoriteManagerL
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::CreateFavoriteManagerL()
    {
    DLTRACEIN((""));
    // Get the handle for the manager from the provider
    TInt favoriteManagerHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync(
                    CNcdProvider::ENcdProviderFavoriteManagerHandle,
                    KNullDesC,
                    favoriteManagerHandle,
                    Handle() ) );
    
    // Use the same session for the manager as for the provider
    iFavoriteManager = 
        CNcdFavoriteManagerProxy::NewL( ClientServerSession(), 
                                        favoriteManagerHandle,
                                        this,
                                        *iNodeManager );                                          
    DLTRACEOUT((""));                                    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::CreateServerReportManagerL()
    {
    DLTRACEIN((""));
    // Get the handle for the manager from the provider
    TInt managerHandle( 0 );
    User::LeaveIfError(
            ClientServerSession().
                SendSync(
                    CNcdProvider::ENcdProviderServerReportManagerHandle,
                    KNullDesC,
                    managerHandle,
                    Handle() ) );
    
    // Use the same session for the manager as for the provider
    iServerReportManager = 
        CNcdServerReportManagerProxy::NewL( ClientServerSession(), 
                                            managerHandle,
                                            *this );                                          
    DLTRACEOUT((""));                                        
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
HBufC* CNcdProviderProxy::GetInfoFromProviderL( 
    TInt aInfoIndex )
    {
    // Write aInfo to a descriptor
    TBuf<1> info;
    info.SetLength( 1 );
    info[0] = aInfoIndex;
    
    HBufC* data( NULL );
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( CNcdProvider::ENcdGetProviderInfo,
                               info,
                               data,
                               Handle(),
                               0 ) );

    if ( data == NULL )
        {        
        User::Leave( KErrNotFound );
        }
    return data;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CNcdProviderProxy::InternalizeFavoriteManagerL()
    {
    DLTRACEIN((""));
    iFavoriteManager->InternalizeL();
    }
    
    
// ---------------------------------------------------------------------------
// Given to TCallBack when clearing cache
// ---------------------------------------------------------------------------
//   
TInt CNcdProviderProxy::InternalizeFavoriteManager( TAny* aProvider )
    {
    DLTRACEIN((""));
    CNcdProviderProxy* provider = 
        static_cast<CNcdProviderProxy*>( aProvider );
    DASSERT( provider );
    TRAPD( err, provider->InternalizeFavoriteManagerL() );
    return err;
    }
