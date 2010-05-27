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
* Description:   Implements CNcdProvider class
*
*/


// For streams
#include <s32mem.h>

// RFs
#include <f32file.h>
#include <bautils.h>

#include <es_sock.h> // KErrConnectionTerminated
#include <hash.h>

#include "ncdproviderimpl.h"
#include "catalogsbasemessage.h"
#include "catalogssession.h"
#include "ncdnodemanager.h"
#include "ncdfavoritemanagerimpl.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdoperationmanager.h"
#include "ncdnodeimpl.h"
#include "ncdstoragemanagerimpl.h"
#include "ncdprotocolimpl.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "catalogstransportimpl.h"
#include "catalogstransporttypes.h"
#include "ncdkeyvaluepair.h"
#include "ncdkeyvaluemap.h"
#include "ncdproviderdefines.h"
#include "ncdconfigurationmanagerimpl.h"
#include "ncdconfigurationkeys.h"
#include "catalogsuids.h"
#include "ncdutils.h"
#include "catalogscontext.h"
#include "catalogshttpsession.h"
#include "catalogssmssession.h"
#include "catalogshttpconfig.h"
#include "catalogsaccesspointmanagerimpl.h"
#include "ncdsubscriptionmanagerimpl.h"
#include "ncdpreviewmanager.h"
#include "ncdproviderutils.h"
#include "catalogsuids.h"
#include "ncdengineconfiguration.h"
#include "catalogsconstants.h"
#include "ncddatabaseversions.h"
#include "ncdsessionhandler.h"
#include "ncderrors.h"
#include "ncdreportmanager.h"
#include "ncdnodeseeninfo.h"
#include "ncdconnectionmethod.h"
#include "catalogsconnectionmethod.h"
#include "ncdhttputils.h"
#include "ncdserverreportmanagerimpl.h"
#include "ncdnodecachecleanermanager.h"
#include "ncdnodecachecleaner.h"
#include "ncdprovideroptions.h"
#include "ncdprovidercloseobserver.h"
#include "ncdgeneralmanager.h"

#ifdef __WINS__
// Fake SSID for emulator
_LIT8( KEmulatorFakeSsid, "emulatorssid" );
#endif 


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//   
CNcdProvider* CNcdProvider::NewLC( 
    const TUid& aFamilyId,
    MNcdProviderCloseObserver& aCloseObserver,
    const TDesC& aEngineRoot,
    CCatalogsTransport& aTransport,
    CNcdStorageManager& aStorageManager )
    {
    CNcdProvider* self = new( ELeave ) CNcdProvider( 
        aFamilyId, 
        aCloseObserver,
        aEngineRoot,
        aTransport,
        aStorageManager );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//   
CNcdProvider::~CNcdProvider()
    {
    DLTRACEIN((""));
    iCloseObserver.ProviderClosed( *this );    
    
    if ( iServerReportManager )
        {
        iServerReportManager->Close();        
        }
        
    if ( iNodeManager ) 
        {
        iNodeManager->Close();
        }    
    
    if ( iPurchaseHistory )
        {
        iPurchaseHistory->Close();
        }
    
    if ( iOperationManager ) 
        {        
        iOperationManager->Close();
        }

    if ( iSubscriptionManager ) 
        {        
        iSubscriptionManager->Close();
        }
        
    if ( iFavoriteManager ) 
        {
        iFavoriteManager->Close();
        }
        
    delete iProtocolHandler;        
    delete iConfigurationManager;
    
    delete iHttpUtils;
    delete iAccessPointManager;    	

	TRAP_IGNORE( HandleShutdownFileL() );
	delete iGeneralManager;
	
	// Close provider's storage resources
	iStorageManager.CloseClient( FamilyName() );
    DLTRACEOUT((""));
    }
        

// ---------------------------------------------------------------------------
// ReceiveMessage
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                   TInt aFunctionNumber )
    {
    DLTRACEIN(("Function: %d", aFunctionNumber ));
    DASSERT( aMessage );
            
    TRAPD( trapError, ReceiveMessageL( *aMessage, aFunctionNumber) );    
    
    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        aMessage->CompleteAndRelease( trapError );
        }
    }


// ---------------------------------------------------------------------------
// ReceiveMessageL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ReceiveMessageL( 
    MCatalogsBaseMessage& aMessage,
    TInt aFunctionNumber  )
    {
    switch( aFunctionNumber )
        {
        case ENcdProviderNodeManagerHandle:
            NodeManagerHandleRequestL( aMessage );
            break;
            
        case ENcdProviderPurchaseHistoryHandle:
            PurchaseHistoryHandleRequestL( aMessage );
            break;
            
        case ENcdProviderOperationManagerHandle:
            OperationManagerHandleRequestL( aMessage );
            break;

        case ENcdProviderSubscriptionManagerHandle:
            SubscriptionManagerHandleRequestL( aMessage );
            break;
            
        case ENcdProviderFavoriteManagerHandle:
            FavoriteManagerHandleRequestL( aMessage );
            break;

        case ENcdProviderServerReportManagerHandle:
            ServerReportManagerHandleRequestL( aMessage );
            break;

        case ENcdProviderRelease:
            ReleaseRequestL( aMessage );
            break;
            
        case ENcdProviderAddConfiguration:
            AddConfigurationRequestL( aMessage );
            break;
            
        case ENcdProviderRemoveConfiguration:
            RemoveConfigurationRequestL( aMessage );
            break;

        case ENcdProviderRetrieveConfigurations:
            RetrieveConfigurationsRequestL( aMessage );
            break;
            
        case ENcdProviderSetDefaultAccessPoint:
            SetDefaultAccessPointRequestL( aMessage );
            break;
                       
        case ENcdProviderClientId:
            CreateClientIdRequestL( aMessage );
            break;

        case ENcdProviderClearCache:
            ClearCacheRequestL( aMessage );
            break;
        
        case ENcdGetProviderInfo:
            GetInfoRequestL( aMessage );
            break;
           
        case ENcdProviderIsSimChanged:
            IsSimChangedRequestL( aMessage );
            break;
            
        case ENcdProviderIsFixedAp:
            IsFixedApRequestL( aMessage );
            break;
            
        case ENcdProviderSyncSeenInfo:
            SyncSeenInfoRequestL( aMessage );
            break;
            
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    
    }

// ---------------------------------------------------------------------------
// CounterPartLost
// ---------------------------------------------------------------------------
//       
void CNcdProvider::CounterPartLost( const MCatalogsSession& /* aSession */ )
    {
    /**
     * @ Implement this
     */
    }



// ---------------------------------------------------------------------------
// Handles connection confirmation requests
// ---------------------------------------------------------------------------
//           
TCatalogsHttpConnectionConfirmationState 
    CNcdProvider::HandleConnectionConfirmationRequestL( 
        MCatalogsHttpSession& /* aSession */, 
        const TCatalogsConnectionMethod& /* aMethod */  )
    {
    DLTRACEIN((""));
    /**
     * @ Handle connection confirmation requests
     */
    return ECatalogsHttpConnectionConfirmed;
    }


// ---------------------------------------------------------------------------
// Handles some errors in HTTP connections
// ---------------------------------------------------------------------------
//           
void CNcdProvider::HandleConnectionErrorL( 
    MCatalogsHttpSession& aSession,
    const TCatalogsConnectionMethod& /*aMethod*/, 
    TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    // Access point selection was cancelled so cancel all HTTP operations
    // Also connection termination by pressing the red phone button is handled
    // here
    if ( aError == KErrCancel ||
         aError == KErrConnectionTerminated )
        {
        aSession.NotifyCancelAllOperations();
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//           
CNcdProvider::CNcdProvider( 
    const TUid& aFamilyId,
    MNcdProviderCloseObserver& aCloseObserver,
    const TDesC& aEngineRoot,
    CCatalogsTransport& aTransport,
    CNcdStorageManager& aStorageManager ) : 
        CCatalogsCommunicable(), 
        iFamilyId( aFamilyId ),
        iFamilyName( aFamilyId.Name() ),
        iCloseObserver( aCloseObserver ),
        iEngineRoot( aEngineRoot ),
        iStorageManager( aStorageManager ),
        iTransport( aTransport )
        
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ConstructL()
    {
    DLTRACEIN((""));
    RBuf familyPath;
    FamilyPathLC( familyPath );
    
    BaflUtils::EnsurePathExistsL( CNcdProviderUtils::FileSession(), familyPath );
    // Begin startup procedure. If the previous startup failed, all of 
    // provider's data files are deleted    
    // StartupEndL is called in PrepareSessionL
    TBool startupFailed = StartupBeginL( familyPath );    
    CleanupStack::PopAndDestroy( &familyPath );
    
    RBuf path;
    PurchaseHistoryPathLC( path );
    iPurchaseHistory = CNcdPurchaseHistoryDb::NewL( path );
    CleanupStack::PopAndDestroy( &path );
                
    DLTRACE(("iPurchaseHistory ok"));
        
    iGeneralManager = new( ELeave ) CNcdGeneralManager( 
        iFamilyId, 
        FamilyName() );
    
    iGeneralManager->SetStorageManager( iStorageManager );
    iGeneralManager->SetPurchaseHistory( *iPurchaseHistory );
        
    // Create configuration manager for the provider    
    iConfigurationManager = 
        CNcdConfigurationManager::NewL( *iGeneralManager );
    
    iGeneralManager->SetConfigurationManager( *iConfigurationManager );
    
    DLTRACE(("iConfigurationManager ok"));

    iNodeManager = CNcdNodeManager::NewL( *iGeneralManager );
    iGeneralManager->SetNodeManager( *iNodeManager );
    DLTRACE(("iNodeManager ok"));


    iAccessPointManager = 
        CCatalogsAccessPointManager::NewL( *iGeneralManager );
    
    iGeneralManager->SetAccessPointManager( *iAccessPointManager );
    DLTRACE(("iAccessPointManager ok"));    


    iSubscriptionManager = CNcdSubscriptionManager::NewL( 
        iStorageManager, *iNodeManager );

    iProtocolHandler = CNcdProtocol::NewL(
        *iConfigurationManager, *iSubscriptionManager );

    iGeneralManager->SetProtocolManager( *iProtocolHandler );
    
    DLTRACE(("iProtocolHandler ok"));

    iHttpUtils = new( ELeave ) CNcdHttpUtils( *iAccessPointManager );
    iGeneralManager->SetHttpUtils( *iHttpUtils );    

    iOperationManager = CNcdOperationManager::NewL( 
        *this, 
        *iGeneralManager,        
        *iSubscriptionManager );

    DLTRACE(("iOperationManager ok"));

    iSubscriptionManager->SetOperationManager( iOperationManager );
    DLTRACE(("iSubscriptionManager ok"));
    
    iFavoriteManager = CNcdFavoriteManager::NewL( *iGeneralManager );
    DLTRACE(("iFavoriteManager ok"));
    
    iNodeManager->SetFavoriteManager( *iFavoriteManager );

    iServerReportManager = CNcdServerReportManager::NewL( *this );
    DLTRACE(("iServerReportManager ok"));

    // If fixed AP must be used, set it to AP manager.    
    MNcdEngineConfiguration& engineConfig = CNcdProviderUtils::EngineConfig();
    if ( engineConfig.UseFixedAp() ) 
        {
        const RPointerArray<CNcdKeyValuePair>& apDetails = 
            engineConfig.FixedApDetails();
        TRAPD( err, iAccessPointManager->SetFixedApL( apDetails ) );
        if ( err != KErrNone ) 
            {
            User::Leave( KNcdErrorApCreationFailed );
            }
        }
        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// NodeManagerHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::NodeManagerHandleRequestL( 
    MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the manager to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iNodeManager ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );    
    }


// ---------------------------------------------------------------------------
// PurchaseHistoryHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::PurchaseHistoryHandleRequestL(
    MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the purchase history to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iPurchaseHistory ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );        
    }


// ---------------------------------------------------------------------------
// OperationManagerHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::OperationManagerHandleRequestL( 
    MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the manager to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iOperationManager ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );        
    }

// ---------------------------------------------------------------------------
// SubscriptionManagerHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::SubscriptionManagerHandleRequestL( 
    MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the manager to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iSubscriptionManager ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );        
    }

// ---------------------------------------------------------------------------
// FavoriteManagerHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::FavoriteManagerHandleRequestL( 
    MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the manager to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iFavoriteManager ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );        
    }


// ---------------------------------------------------------------------------
// ServerReportHandleRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ServerReportManagerHandleRequestL(
        MCatalogsBaseMessage& aMessage ) const
    {
    // Get the session that will contain the handle
    MCatalogsSession& requestSession( aMessage.Session() );

    // Add the manager to the session and get the handle.
    TInt handle( requestSession.AddObjectL( iServerReportManager ) );

    // Sent the information to the client side            
    aMessage.CompleteAndReleaseL( handle, KErrNone );        
    }


// ---------------------------------------------------------------------------
// ReleaseRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ReleaseRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.    
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );    
    }


// ---------------------------------------------------------------------------
// AddConfigurationRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::AddConfigurationRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // Read input from the message
    RBuf8 data;
    data.CreateL( aMessage.InputLength() );
    CleanupClosePushL( data );
    
    User::LeaveIfError( aMessage.ReadInput( data ) );
    DLTRACE(("Msg len: %d", aMessage.InputLength() ));
    
    // open stream to the data
    RDesReadStream stream( data );
    CleanupClosePushL( stream );
    
    // internalize key-value pair from the stream
    CNcdKeyValuePair* pair = CNcdKeyValuePair::NewL( stream );
    
    CleanupStack::PopAndDestroy( &stream );    
    CleanupStack::PopAndDestroy( &data );
    
    DLINFO((_L("Key: %S, value: %S"), &pair->Key(), &pair->Value()));

    // Node manager has to be informed about the db size restrictions.
    // So, proper db cleaning methods may be started when required.
    if ( pair->Key() == NcdConfigurationKeys::KMaxStorageSize )
        {
        // The configuration value gives the max db size for the client.
        TInt maxDbSize( 0 );
        TLex lexPair( pair->Value() );
        
        DLINFO((_L("Convert max storage size string: %S"), &pair->Value()));
        User::LeaveIfError( lexPair.Val( maxDbSize ) );
        
        DLINFO(("Max storage size string converted correctly: %d", maxDbSize));
        // The value could be parsed correctly.
        // Set max db size value.
        // So, inform node manger about this.
        iNodeManager->DbSetMaxSizeL( aMessage.Session().Context().FamilyId(), 
                                    maxDbSize );            
        }    
        
    // AddConfigurationL deletes pair if a leave occurs
    iConfigurationManager->AddConfigurationL( aMessage.Session().Context(),
                                              pair );

    // Complete the message
    aMessage.CompleteAndRelease( KErrNone );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// RemoveConfigurationsRequestL
// ---------------------------------------------------------------------------
//           
void CNcdProvider::RemoveConfigurationRequestL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    // Read input from the message
    RBuf8 data;
    data.CreateL( aMessage.InputLength() );
    CleanupClosePushL( data );
    
    User::LeaveIfError( aMessage.ReadInput( data ) );

    RDesReadStream stream( data );
    CleanupClosePushL( stream );
    HBufC* key = NULL;
    
    // Read key from the data
    if ( InternalizeDesL( key, stream ) == 0 ) 
        {
        delete key;
        User::Leave( KErrArgument );
        }
        
    CleanupStack::PopAndDestroy( 2 ); // stream, data    
    CleanupStack::PushL( key );
    
    // Remove configuration
    TInt err = iConfigurationManager->RemoveConfigurationL( 
        aMessage.Session().Context(), *key );
    CleanupStack::PopAndDestroy( key );
    
    if ( err > KErrNone ) 
        {
        err = KErrNone;
        }
    
    TBuf8<1> errBuf;
    errBuf.SetLength( 1 );
    
    errBuf[0] = 0;

    aMessage.CompleteAndReleaseL( errBuf, err );
    DLTRACEOUT(("err: %d", err));
    }
    
    
// ---------------------------------------------------------------------------
// RetrieveConfigurationsRequestL
// ---------------------------------------------------------------------------
//           
void CNcdProvider::RetrieveConfigurationsRequestL( 
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // Get the configuration
    CNcdKeyValueMap* configuration = iConfigurationManager->ConfigurationsLC( 
        aMessage.Session().Context() );    
    
    if ( !configuration->Pairs().Count() ) 
        {
        DLERROR(("No configurations, leaving"));
        User::Leave( KErrNotFound );
        }
        
    // Externalize the configuration
    RBuf8 data;
    CleanupClosePushL( data );
    configuration->ExternalizeL( data );
    
    // Complete the message
    aMessage.CompleteAndReleaseL( data, KErrNone );
    
    CleanupStack::PopAndDestroy( 2, configuration ); // data
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void CNcdProvider::GetProviderContextL( 
    MCatalogsContext& aContext, 
    TNcdProviderContext& aProviderContext )
    {
    DLTRACEIN((""));
    RBuf8& providerContextBuf = aContext.ProviderDataL( iProviderIndex );
    
    // If the provider creation has failed it is not guaranteed that provider 
    // context has been created
    if ( providerContextBuf.Size() != sizeof( TNcdProviderContext ) ) 
        {
        DLERROR(("Size of the provider context is wrong, leaving with KErrCorrupt. Size is %d, should be %d",
            providerContextBuf.Size(), sizeof( TNcdProviderContext )));
        User::Leave( KErrCorrupt );
        }
        
    // Read the provider specific context data.
    TPtr8 des( (TUint8*)&aProviderContext, 
        sizeof( TNcdProviderContext ), 
        sizeof( TNcdProviderContext ) );
    des = providerContextBuf;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
CNcdNodeManager& CNcdProvider::NodeManager()
    {
    return *iNodeManager;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
MCatalogsAccessPointManager& CNcdProvider::AccessPointManager()
    {
    return *iAccessPointManager;    
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
const TUid& CNcdProvider::FamilyId() const
    {
    return iFamilyId;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
const TDesC& CNcdProvider::FamilyName() const
    {
    return iFamilyName;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
TInt CNcdProvider::DatabaseClearingStatus() const
    {
    return iDatabaseClearingStatus;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
void CNcdProvider::PrepareSessionL( 
    MCatalogsSession& aSession, 
    TUint32 aOptions )
    {
    DLTRACEIN(("aOptions: %u", aOptions ));
    // Prepare provider specific context structure.
    TNcdProviderContext context;
    
    // Create own HTTP session for the client. Use secure id -> different sessions for clients
    // in the same family.
    // DatabaseClearingStatus() is used to determine wheter download manager
    // should be cleared or not
    context.iHttpSession = static_cast<MCatalogsHttpSession*>( 
        iTransport.QueryInterfaceL( aSession.Context().SecureId().iId, 
                                    KCatalogsTransportHttpInterface,
                                    DatabaseClearingStatus() != 0 ) );    

    context.iSmsSession = static_cast<MCatalogsSmsSession*>(     
        iTransport.QueryInterfaceL( aSession.Context().SecureId().iId,
                                    KCatalogsTransportSmsInterface ) );

    context.iHttpSession->ConnectionManager().SetConnectionConfirmationObserver( this );
    context.iHttpSession->ConnectionManager().SetConnectionErrorObserver( this );

    context.iReportManager = 
        CNcdReportManager::NewL( 
            aSession.Context(),
            *iGeneralManager,            
            *context.iHttpSession,
            iShutdownFailed );

    // Save the session interface pointer in provider's context slot.
    TPtrC8 contextDes( (const TUint8*)&context, sizeof( TNcdProviderContext ) );
    aSession.Context().ProviderDataL( iProviderIndex ).CreateL( contextDes );
    
    HandleProviderOptionsL( 
        aSession.Context(), 
        aOptions, 
        *context.iHttpSession );
        
    // Startup sequence ends.
    StartupEndL();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
void CNcdProvider::HandleSessionRemoval( MCatalogsSession& aSession )
    {
    DLTRACEIN((""));
    TRAP_IGNORE( 
        {
        TNcdProviderContext context;        
        GetProviderContextL( aSession.Context(), context );

        // Report manager must be deleted before HTTP session is released
        DLTRACE(("Deleting report manager"));
        delete context.iReportManager;

        // Release the HTTP and SMS sessions and clear the context data.
        ReleasePtr( context.iHttpSession );
        ReleasePtr( context.iSmsSession );
        
        aSession.Context().ProviderDataL( iProviderIndex ).Close();        
        }); // TRAP_IGNORE
    }

// ---------------------------------------------------------------------------
// SetDefaultAccessPointRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::SetDefaultAccessPointRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    // Read input from the message
    DASSERT( aMessage.InputLength() == sizeof( TNcdConnectionMethod ) );
    
    RBuf8 data;
    data.CreateL( aMessage.InputLength() );
    CleanupClosePushL( data );
    
    User::LeaveIfError( aMessage.ReadInput( data ) );
    DLINFO(("Msg len: %d", aMessage.InputLength() ));
    
    // Read the TUint32 inside the input data.
    const TNcdConnectionMethod* accessPointId = 
        reinterpret_cast< const TNcdConnectionMethod* >( data.Ptr() );
    DLINFO(( "Access point type: %d, id %u", 
        accessPointId->iType, 
        accessPointId->iId ));

    // We have the HTTP session interface pointer in context data.
    TNcdProviderContext context;
    GetProviderContextL( aMessage.Session().Context(), context );
    DASSERT( context.iHttpSession != NULL );

    TCatalogsConnectionMethod method;
    
    iHttpUtils->ConvertConnectionMethod(
        *accessPointId,
        method );
             
    // Set the default access points to the HTTP session.
    context.iHttpSession->SetDefaultConnectionMethod( method );    

    CleanupStack::PopAndDestroy();  // data

    // Complete the message
    aMessage.CompleteAndRelease( KErrNone );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// CreateClientIdRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::CreateClientIdRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    
    const TDesC& clientId = iConfigurationManager->ClientIdL(
        aMessage.Session().Context() );
    aMessage.CompleteAndReleaseL( clientId, KErrNone );
    }


// ---------------------------------------------------------------------------
// ClearCacheRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ClearCacheRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    MCatalogsContext& context( aMessage.Session().Context() );
    
    CNcdReportManager& reportManager = 
        iOperationManager->ReportManagerL( context );
    
    reportManager.CloseStorage();
    
    // Clears cache and deletes incomplete downloads 
    iNodeManager->ClearClientCacheL( context, ETrue );
  
    CNcdProviderUtils::EngineConfig().ClearClientDataL( FamilyName(), EFalse );
    
    DLTRACE(( "Cache cleared" ));
    iConfigurationManager->ClearServerCapabilitiesL( context );
    
    DLTRACE(( "Server capabilities cleared" ));
    
    // Because capabilities are removed, we need to also remove server sessions
    // so that we get the capabilities again on next response.
    TRAPD( err,
        {        
        iProtocolHandler->SessionHandlerL( aMessage.Session().Context() )
            .RemoveAllSessions();
        });
    LeaveIfNotErrorL( err, KErrNotFound );
    
    DLTRACE(( "Server sessions removed" ));
    
    aMessage.CompleteAndReleaseL( KNullDesC8(), KErrNone );
    DLTRACEOUT(( "Message completed" ));    
    }


// ---------------------------------------------------------------------------
// AllowCacheCleaningRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::AllowCacheCleaningL( 
    const MCatalogsContext& aContext,
    TBool aAllow )
    {
    DLTRACEIN(("aAllow: %d", aAllow));
    NodeManager().
        NodeCacheCleanerManager().
            CacheCleanerL( aContext.FamilyId() ).
                SetAllowCleaning( aAllow );

    }


// ---------------------------------------------------------------------------
// GetInfoRequestL
// ---------------------------------------------------------------------------
//       
void CNcdProvider::GetInfoRequestL(
    MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    DASSERT( aMessage.InputLength() == 1 );
    TBuf<1> data;
    
    aMessage.ReadInput( data );
    TNcdProviderInfo info( static_cast<TNcdProviderInfo>( data[0] ) );
    
    const MNcdEngineConfiguration& config( 
        CNcdProviderUtils::EngineConfig() );
    
    HBufC* response = NULL;
    switch( info )
        {
        case ENcdProviderInfoType: 
            {
            DLTRACE(("Engine type"));
            response = config.EngineType().AllocLC();
            break;
            }
            
        case ENcdProviderInfoVersion:
            {
            DLTRACE(("Engine version"));
            response = config.EngineVersion().AllocLC();
            break;
            }
        
        case ENcdProviderInfoUid:
            {
            DLTRACE(("Engine uid"));
            response = config.EngineUid().AllocLC();
            break;
            }
        
        case ENcdProviderInfoProvisioning:
            {
            DLTRACE(("Engine provisioning"));
            response = config.EngineProvisioning().AllocLC();
            break;
            }

        default:
            {
            DASSERT( 0 );
            }

        }
    if (response)
        {
             aMessage.CompleteAndReleaseL( *response, KErrNone );
             CleanupStack::PopAndDestroy( response );
        }
    
    DLTRACEOUT(("Response sent"));
    }
    

void CNcdProvider::IsSimChangedRequestL( MCatalogsBaseMessage& aMessage ) 
    {
    DLTRACEIN((""));    
    aMessage.CompleteAndReleaseL( iSimChanged, KErrNone );
    DLTRACEOUT(("Response sent"));
    } 
    
    
void CNcdProvider::IsFixedApRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    MNcdEngineConfiguration& engineConfig = CNcdProviderUtils::EngineConfig();
    aMessage.CompleteAndReleaseL( engineConfig.UseFixedAp(), KErrNone );
    }
    

void CNcdProvider::SyncSeenInfoRequestL( MCatalogsBaseMessage& aMessage )
    {
    DLTRACEIN((""));
    DASSERT( iNodeManager );
    
    // Read the syncronization depth
    RCatalogsMessageReader reader;
    reader.OpenLC( aMessage );
    TInt depth = reader().ReadInt32L();
    CleanupStack::PopAndDestroy( &reader );
    
    // Sync seen info.
    // Remove syncseen if not needed
    /*iNodeManager->SeenInfo().SyncSeenInfoL(
        aMessage.Session().Context().FamilyId(), depth, *iNodeManager );*/
        
    aMessage.CompleteAndRelease( KErrNone );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//       
TBool CNcdProvider::StartupBeginL( const TDesC& aEnginePath )
    {
    DLTRACEIN((""));
    iProviderStartingUp = ETrue;
    
    // Clears family's temp files 
    ClearTempFilesL();
    
    RBuf path;
    AppendPathsLC( 
        path, 
        aEnginePath, 
        NcdProviderDefines::KNcdProviderStartupFile() );
    
    // Check if "ncdstartup" exists. If it does, databases are deleted.
    // Also database versions are checked. If they don't match, db's are deleted
    
    TBool startupFailed = EFalse;
    DLTRACE(("Checking if the startup file exists and db versions match"));
    
    TInt failedShutdowns = 0;
    TRAPD( err, failedShutdowns = 
        CNcdProviderUtils::UpdateShutdownFileL( aEnginePath ) );
    
    iShutdownFailed = failedShutdowns > 0;        
    
    if ( err != KErrNone || 
         failedShutdowns > NcdProviderDefines::KNcdMaxFailedShutdowns || 
         BaflUtils::FileExists( CNcdProviderUtils::FileSession(), path ) )
        {
        DLERROR(("Last startup failed. Cleaning files"));
        iDatabaseClearingStatus = KNcdDatabasesClearedAfterCrash;
        ClearProviderFilesL( aEnginePath );
        
        // Delete the shutdown file because updating it failed
        if ( err != KErrNone ) 
            {
            DLTRACE(("Deleting shutdown file because it wasn't updated successfully"));
            TRAP_IGNORE( CNcdProviderUtils::RemoveShutdownFileL( 
                aEnginePath ) );
            }
        startupFailed = ETrue;
        }
    else 
        {
        CheckDatabaseVersionsL( aEnginePath );
        }

    // Startup file is created 
    DLTRACE(( _L("Creating/replacing startup file %S"), &path ));
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Replace( 
        CNcdProviderUtils::FileSession(), path, EFileWrite ) );
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( &path );
    DLTRACEOUT(("Startup begun"));  
    return startupFailed;  
    }


// ---------------------------------------------------------------------------
// Checks that the database version numbers match with the supported versions
// 
// ---------------------------------------------------------------------------
//       
void CNcdProvider::CheckDatabaseVersionsL(
    const TDesC& aEnginePath )
    {
    DLTRACEIN(( _L("Engine path: %S"), &aEnginePath ));
    
    TUint32 generalVersion = 0;
    TUint32 purchaseVersion = 0;
    TRAPD( err, CNcdProviderUtils::ReadDatabaseVersionsL( 
        aEnginePath, generalVersion, purchaseVersion ) );
    
    DLINFO(("Supported versions, general: %d, purchase history: %d",
        KNcdGeneralDatabaseVersion, KNcdPurchaseHistoryVersion ));
    
    if ( err == KErrNone ) 
        {
        if ( generalVersion != KNcdGeneralDatabaseVersion )
            {
            DLERROR(("General database version mismatch. Deleting existing dbs"));
            iDatabaseClearingStatus = KNcdGeneralDatabaseVersionMismatch;
            ClearDatabasesL( aEnginePath );
            }
            
        if ( purchaseVersion != KNcdPurchaseHistoryVersion )
            {
            DLERROR(("Purchase history version mismatch. Deleting purchase history"));
            iDatabaseClearingStatus |= KNcdPurchaseHistoryVersionMismatch;
            ClearPurchaseHistoryL();            
            }
        }
    else
        {
        DLERROR(("Couldn't read database versions. Deleting dbs just to be sure."));
        iDatabaseClearingStatus = 
            KNcdPurchaseHistoryVersionMismatch |
            KNcdGeneralDatabaseVersionMismatch;
        ClearProviderFilesL( aEnginePath );
        }
    
    DLTRACEOUT(("Database versions checked, clearing status: %d", 
        iDatabaseClearingStatus ));        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//       
void CNcdProvider::StartupEndL()
    {
    DLTRACEIN((""));
    
    if ( !iProviderStartingUp ) 
        {
        DLTRACEOUT(("Provider already up"));
        return;
        }
    
    RBuf path;
    FamilyPathLC( path );
    
    // Ensure that database versions file contains the correct versions
    CNcdProviderUtils::WriteDatabaseVersionsL( 
        path, 
        KNcdGeneralDatabaseVersion,
        KNcdPurchaseHistoryVersion );
    
    path.Append( NcdProviderDefines::KNcdProviderStartupFile );
    
    DLTRACE(( _L("Deleting the startup file after successful startup, path: %S"), 
        &path ));
    CNcdProviderUtils::FileSession().Delete( path );
    
    CleanupStack::PopAndDestroy( &path );
        
    iProviderStartingUp = EFalse;
    DLTRACEOUT(("Startup file deleted"));
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ClearProviderFilesL( const TDesC& aEnginePath )
    {
    DLTRACEIN((""));
    ClearDatabasesL( aEnginePath );
    ClearPurchaseHistoryL();
    // Clear downloaded files
    CNcdProviderUtils::EngineConfig().ClearClientDataL( FamilyName(), EFalse );
    }


// ---------------------------------------------------------------------------
// Clears all databases except purchase history
// Also doesn't delete startupfile nor shutdownfile
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ClearDatabasesL( const TDesC& aEnginePath )
    {
    DLTRACEIN((""));
    RFs& fs( CNcdProviderUtils::FileSession() );    
        
    CFileMan* fileman = CFileMan::NewL( fs );
    CleanupStack::PushL( fileman );
    
    CDir* fileList;
    CDir* dirList;
    
    User::LeaveIfError( fs.GetDir( aEnginePath,
        KEntryAttDir, ESortNone, fileList, dirList) );
    
    delete fileList;
    fileList = NULL;        
    
    // Minimize stack usage by creating TParse on heap
    TParse* parse = NULL;
    parse = new (ELeave) TParse;
    CleanupDeletePushL( parse );
    parse->Set( aEnginePath, NULL, NULL );
    
    // Delete directories
    for ( TInt i = 0; i < dirList->Count(); i++ )
        {        
        DLTRACE(( _L("Deleting %S"), &(*dirList)[i].iName ));
        parse->AddDir( (*dirList)[i].iName );
        fileman->RmDir( parse->DriveAndPath() );
        parse->PopDir();
        }
        
    delete dirList;
    dirList = NULL;        
    
    CleanupStack::PopAndDestroy( 2, fileman ); // parse, fileman
    
    DLTRACEOUT(("Databases successfully deleted"));    
    }

// ---------------------------------------------------------------------------
// Deletes the purchase history database
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ClearPurchaseHistoryL()
    {
    DLTRACEIN((""));

    RBuf path;
    PurchaseHistoryPathLC( path );
    
    DLTRACE(( _L("Deleting purchase history, %S"), &path ));
    TInt err = CNcdProviderUtils::FileSession().Delete( path );
    CleanupStack::PopAndDestroy( &path );
    
    DLTRACEOUT(( "Purchase history deleted, err = %d", err ));
    LeaveIfNotErrorL( err, KErrNotFound, KErrPathNotFound );
    }


// ---------------------------------------------------------------------------
// Clears family's temporary files
// ---------------------------------------------------------------------------
//       
void CNcdProvider::ClearTempFilesL()
    {
    DLTRACEIN((""));
    CNcdProviderUtils::EngineConfig().ClearClientDataL( FamilyName(), ETrue );
    }


// ---------------------------------------------------------------------------
// Handles the shutdown file
// ---------------------------------------------------------------------------
//       
void CNcdProvider::HandleShutdownFileL()
    {
    DLTRACEIN((""));
    RBuf path;
    FamilyPathLC( path );
    CNcdProviderUtils::RemoveShutdownFileL( path );
    CleanupStack::PopAndDestroy( &path );
    DLTRACEOUT(("Shutdown file removed"));
    }

// ---------------------------------------------------------------------------
// Manages the storages if SIM card is changed
// ---------------------------------------------------------------------------
//
void CNcdProvider::ManageStoragesL( 
    const MCatalogsContext& aContext, 
    TBool aClearStorages ) 
    {
    DLTRACEIN(("aClearStorages: %d", aClearStorages ));
    const TDesC8& previousSsid = iConfigurationManager->SsidL( aContext );
    HBufC8* currentSsid = HashImsiLC();
    
    if ( previousSsid != *currentSsid ) 
        {
        // SIM card was changed or removed, manage the storages
        iConfigurationManager->SetSsidL( aContext, currentSsid );
        CleanupStack::Pop( currentSsid );
        
        if ( aClearStorages ) 
            {
            DLTRACE(("Clearing storages"));
            // Clear node cache (NOTE! FAVORITE NODES MUST NOT BE CLEARED!!).
            iNodeManager->ClearClientCacheL( aContext, EFalse );
            
            // Clear subscriptions
            iSubscriptionManager->ClearSubscriptionDbL( aContext );
            
            // Clear server capabilities
            iConfigurationManager->ClearServerCapabilitiesL( aContext );
            }
        
        iSimChanged = ETrue;
        }
    else 
        {
        CleanupStack::PopAndDestroy( currentSsid );
        }
    }
    
// ---------------------------------------------------------------------------
// Hash IMSI
// ---------------------------------------------------------------------------
//        
HBufC8* CNcdProvider::HashImsiLC()
    {    
    DLTRACEIN((""));
    CSHA1* sha1 = CSHA1::NewL(); 
    CleanupStack::PushL( sha1 );
    
    #ifdef __WINS__

    // emulator uses a fake ssid
    CleanupStack::PopAndDestroy( sha1 );  // sha1
    DLINFO(("Emulator, fake SSID used"));
    return KEmulatorFakeSsid().AllocLC();

    #else

    MNcdDeviceService& deviceService = CNcdProviderUtils::DeviceService();

    HBufC8* temp = Des16ToDes8L( deviceService.ImsiL() );
    CleanupStack::PushL( temp );

    // No SIM card causes null IMSI
    if ( *temp == KNullDesC8 ) 
        {
        // null IMSI is KNullDesC in device 
        CleanupStack::PopAndDestroy( 2, sha1 );  // temp, sha1
        DLINFO(("Null IMSI!"));
        return KNullDesC8().AllocLC();
        }
    
    // Finalize hash
    HBufC8* hash = sha1->Final( *temp ).AllocL();
    
    CleanupStack::PopAndDestroy( 2, sha1 );  // temp, sha1
    CleanupStack::PushL( hash );    
    
    HBufC8* final = ConvertDataToTextL( *hash );
    CleanupStack::PopAndDestroy( hash );
    CleanupStack::PushL( final );
    
    DLTRACEOUT(("Hash: %S, length: %d", final, final->Length() ));
    return final;

    #endif
    }
    
// ---------------------------------------------------------------------------
// Converts binary data to text
// ---------------------------------------------------------------------------
//    
HBufC8* CNcdProvider::ConvertDataToTextL( const TDesC8& aData ) const
    {
    DLTRACEIN(("length: %i", aData.Length() ));
    _LIT8( KFormatString, "%02x" );       
    
    HBufC8* target = HBufC8::NewL( 2 * aData.Length() );
    TPtr8 targetPtr( target->Des() );
    TBuf8<2> buf;
    
    for ( TInt i = 0; i < aData.Length(); i++ ) 
        {        
        buf.Format( KFormatString, aData[i] );
        targetPtr.Append( buf );
        }
     
    DLTRACEOUT(("target: %S", target));
    return target;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProvider::PurchaseHistoryPathLC( RBuf& aPath ) const
    {    
    CleanupClosePushL( aPath );
    
    aPath.CreateL( 
        iEngineRoot.Length() + 
        NcdProviderDefines::KPurchaseHistoryDirectory().Length() +
        FamilyName().Length() + 
        NcdProviderDefines::KDatabaseExtension().Length() );
    aPath.Append( iEngineRoot );
    aPath.Append( NcdProviderDefines::KPurchaseHistoryDirectory() );
    aPath.Append( FamilyName() );
    aPath.Append( NcdProviderDefines::KDatabaseExtension );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProvider::FamilyPathLC( RBuf& aPath ) const
    {
    CleanupClosePushL( aPath );
    aPath.CreateL( KMaxPath );    
    aPath.Append( iEngineRoot );
    aPath.Append( FamilyName() );
    aPath.Append( KDirectorySeparator );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProvider::HandleProviderOptionsL( 
    const MCatalogsContext& aContext, 
    TUint32 aOptions,
    MCatalogsHttpSession& aHttpSession )
    {
    DLTRACEIN(("Options: %d", aOptions ));
    
    // Manage the client's storages if SIM card is changed
    ManageStoragesL( 
        aContext, 
        aOptions & ENcdProviderEnableSimChangeCacheCleaning );
    
    AllowCacheCleaningL( 
        aContext,
        // We actually enable cache cleaning so we have to invert the flag
        !( aOptions & ENcdProviderDisableNodeCacheCleaner ) );

    TUint32 protocolOptions = 0;
    if ( aOptions & ENcdProviderSendImei ) 
        {
        protocolOptions = CNcdProtocol::ESendImei;
        }
    iProtocolHandler->SetProtocolOptions( protocolOptions );
    
    TUint32 httpOptions = 0;
    if ( aOptions & ENcdProviderDisableHttpHeadRequest ) 
        {
        httpOptions = ECatalogsHttpDisableHeadRequest;
        }
    aHttpSession.SetOptions( httpOptions );
    }
