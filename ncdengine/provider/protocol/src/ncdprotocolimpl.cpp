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
* Description:   Implementation of CNcdProtocol
*
*/


#include "ncdprotocolimpl.h"
#include "ncdparserfactory.h"
#include "ncdparser.h"
#include "ncdsessionhandler.h"
#include "catalogsdebug.h"
#include "ncdrequestbase.h"
#include "ncddeviceinteractionfactory.h"
#include "ncddeviceservice.h"
#include "ncdrequestconfiguration.h"
#include "ncdrequestconfigurationdata.h"
#include "catalogssmsutils.h"
#include "catalogscontext.h"
#include "ncdkeyvaluemap.h"
#include "ncdutils.h"
#include "ncdstoragemanager.h"
#include "ncdproviderdefines.h"
#include "ncdstorage.h"
#include "ncdconfigurationmanager.h"
#include "ncdprotocoldefaultobserverimpl.h"
#include "catalogsutils.h"
#include "ncd_cp_cookieimpl.h"
#include "ncd_cp_detailimpl.h"
#include "ncdinternalconfigurationkeys.h"
#include "ncdcapabilities.h"
#include "ncdserverdetails.h"
#include "catalogsuids.h"
#include "ncdproviderutils.h"
#include "ncdengineconfiguration.h"
#include "ncdinstallationserviceimpl.h"
#include "ncdprotocolstrings.h"
#include "catalogs_device_config.h"

 // Hardcoded platforminfo

_LIT( KPlatformInfo, "Series60" );

// ---------------------------------------------------------------------------
// CContextData::CContextData
// ---------------------------------------------------------------------------
//
CNcdProtocol::CContextData::CContextData( const MCatalogsContext& aContext, 
    CNcdSessionHandler* aSessionHandler ) : CBase(),
        iFamilyId( aContext.FamilyId() ),                
        iSessionHandler( aSessionHandler )
    {
    }

// ---------------------------------------------------------------------------
// CContextData::~CContextData
// ---------------------------------------------------------------------------
//
CNcdProtocol::CContextData::~CContextData()
    {    
    delete iSessionHandler;
    }
    


// ---------------------------------------------------------------------------
// CContextData::SessionHandler
// ---------------------------------------------------------------------------
//
MNcdSessionHandler& CNcdProtocol::CContextData::SessionHandler() const
    {
    DASSERT( iSessionHandler );
    return *iSessionHandler;
    }


// ---------------------------------------------------------------------------
// CContextData::Context
// ---------------------------------------------------------------------------
//
TUid CNcdProtocol::CContextData::FamilyId() const
    {
    return iFamilyId;
    }

// ---------------------------------------------------------------------------
// CContextData::SessionL
// ---------------------------------------------------------------------------
//
void CNcdProtocol::CContextData::SessionL( const TDesC& aSessionId, 
    const TDesC& aServerUri, const TDesC& aNameSpace )
    {
    DLTRACEIN(( _L("Session Id: %S, server URI: %S, name space: %S"), 
        &aSessionId, &aServerUri, &aNameSpace ));            
    
    // Remove old session if any
    iSessionHandler->RemoveSession( aServerUri, aNameSpace );

    // Create a new session    
    DLTRACE( ( "Creating a new session" ) );      
    iSessionHandler->CreateSessionL( aServerUri, aNameSpace, aSessionId );

    DLTRACEOUT(( "" ));
    }
    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdProtocol::CNcdProtocol( MNcdConfigurationManager& aConfigurationManager,
    CNcdSubscriptionManager& aSubscriptionManager ) : 
    iConfigurationManager( aConfigurationManager ),
    iSubscriptionManager( aSubscriptionManager )
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdProtocol::~CNcdProtocol()
    {
    DLTRACEIN((""));    
    delete iSmsUtils;
    
    iContexts.ResetAndDestroy();    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdProtocol* CNcdProtocol::NewL( MNcdConfigurationManager& aConfigurationManager,
    CNcdSubscriptionManager& aSubscriptionManager )
    {
    CNcdProtocol* self = new(ELeave) CNcdProtocol( aConfigurationManager,
        aSubscriptionManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdProtocol::ConstructL()
    {    
    DLTRACEIN((""));
    iSmsUtils = CCatalogsSmsUtils::NewL();
    iDeviceService = &CNcdProviderUtils::DeviceService();
    
    // This gets the firmware string and appends it with the variant
    // string
    iDeviceService->AppendVariantToFirmwareL( 
        CNcdProviderUtils::FileSession() );
        
    // Get home MCC and MNC so they don't have to be acquired separately
    // for every session
    iDeviceService->HomeNetworkInfoL( iHomeMcc, iHomeMnc );
    }


// ---------------------------------------------------------------------------
// CreateParserL
// ---------------------------------------------------------------------------
//
MNcdParser* CNcdProtocol::CreateParserL( MCatalogsContext& aContext,
    const TDesC& aServerUri )
    {    
    CContextData& data = CreateContextDataL( aContext );
    
    // Create default observer
    CNcdProtocolDefaultObserver* protocolDefaultObserver = 
        CNcdProtocolDefaultObserver::NewL( aContext,  
        iConfigurationManager,
        iSubscriptionManager,
        aServerUri );
    
    CleanupStack::PushL( protocolDefaultObserver );
    
    // Ownership of the default observer is transferred
    MNcdParser* parser = NcdParserFactory::CreateParserL( 
        protocolDefaultObserver );
        
    CleanupStack::Pop( protocolDefaultObserver );
    
    
    // Set all default observers here! Clients may override them.
    parser->Observers().SetSessionObserver( &data );
    
    CleanupDeletePushL( parser );
    // The following is used in session observer callback, 
    // and entityreference building:
    parser->SetOriginL( aServerUri );
    
    CleanupStack::Pop( parser );
    return parser;
    }

// ---------------------------------------------------------------------------
// Preminet protocol request processor
// ---------------------------------------------------------------------------
// 
HBufC8* CNcdProtocol::ProcessPreminetRequestL( 
    const MCatalogsContext& aContext, 
    CNcdRequestBase& aRequest, const TDesC& aServerUri,
    TBool aForceConfigurationData )
    {
    DLTRACEIN((""));
    
    // Get or create context data for the context
    CContextData& contextData( CreateContextDataL( aContext ) );
    HBufC* nameSpace = aRequest.NamespaceLC();
    TBool sessionExists = 
        contextData.SessionHandler().DoesSessionExist( aServerUri, *nameSpace );
    
    if ( aForceConfigurationData || !sessionExists ) 
        {
        DLTRACE(( _L("Session starts for server: %S, ns: %S"), &aServerUri,
            nameSpace ));
        CNcdRequestConfigurationData* config =
            CNcdRequestConfigurationData::NewLC();
    
        aRequest.SetConfigurationL( config );
        CleanupStack::Pop( config );

        // Common common processing for the request: add client-info etc.
        AddClientInfoToRequestL( aContext, *config );
        
        // Add engine's client-info
        AddEngineClientInfoToRequestL( *config );
        
        // Array for the cookies that are sent to the server
        RPointerArray<MNcdConfigurationProtocolCookie> cookies;
        CleanupResetAndDestroyPushL( cookies );
        
        // Add CGW cookies if there are any
        TBool cookiesChanged = AddCookiesL( 
            aContext, 
            cookies, 
            aServerUri,
            *nameSpace );
                
        // Add CDB cookies, these override existing server cookies        
        cookiesChanged |= AddCookiesL( 
            aContext,             
            cookies,             
            iConfigurationManager.MasterServerAddressL( aContext ),
            NcdProviderDefines::KConfigNamespace );
        
        // Finally add the cookies to the request        
        AddCookiesToRequestL( aRequest, cookies );
        
        CleanupStack::PopAndDestroy( &cookies );        
        
        // Remove expired cookies from db
        if ( cookiesChanged ) 
            {
            DLTRACE(("Cookies had expired, update the config to db"));
            iConfigurationManager.SaveConfigurationToDbL( aContext );
            }        
        }
    
    if ( sessionExists )    
        {        
        // Set session id for a continuing session
        aRequest.SetSessionL( contextData.SessionHandler().Session( 
            aServerUri, *nameSpace ) );
        }
    CleanupStack::PopAndDestroy( nameSpace );
    HBufC8* req = aRequest.CreateRequestL();
    DLINFO(("request=%S",req));
    DLTRACEOUT((""));
    return req;    
    }


// ---------------------------------------------------------------------------
// Configuration protocol request processor
// ---------------------------------------------------------------------------
// 
HBufC8* CNcdProtocol::ProcessConfigurationRequestL( 
    const MCatalogsContext& aContext, 
    CNcdRequestConfiguration& aRequest )
    {
    DLTRACEIN((""));
    // Get or create context data for the context
    CContextData& contextData( CreateContextDataL( aContext ) );

    CNcdRequestConfigurationData* config =
        CNcdRequestConfigurationData::NewLC();

    aRequest.SetConfigurationL( config );
    CleanupStack::Pop( config );

        
    // Common common processing for the request: add client-info etc.
    AddClientInfoToRequestL( aContext, *config );
    
    // Add engine's client-info
    AddEngineClientInfoToRequestL( *config );

    // Array for the cookies that are sent to the server
    RPointerArray<MNcdConfigurationProtocolCookie> cookies;
    CleanupResetAndDestroyPushL( cookies );

    // Add cookies if there any
    TBool cookiesChanged = AddCookiesL( aContext, 
        cookies,
        iConfigurationManager.MasterServerAddressL( aContext ),
        NcdProviderDefines::KConfigNamespace );
        
    AddCookiesToConfigRequestL( *config, cookies );
    CleanupStack::PopAndDestroy( &cookies );
    
    // Remove expired cookies from db
    if ( cookiesChanged ) 
        {
        DLTRACE(("Cookies had expired, update the config to db"));
        iConfigurationManager.SaveConfigurationToDbL( aContext );
        }
    
    HBufC8* req = aRequest.CreateRequestL();
    //DLINFO(("request=%S",req));
    DLTRACEOUT((""));
    return req;
    }


// ---------------------------------------------------------------------------
// Session handler getter
// ---------------------------------------------------------------------------
//
MNcdSessionHandler& CNcdProtocol::SessionHandlerL( 
    const MCatalogsContext& aContext ) const
    {
    TInt index = FindContextData( aContext );
    if ( index == KErrNotFound ) 
        {
        User::Leave( KErrNotFound );
        }
    return iContexts[index]->SessionHandler();
    }
 

// ---------------------------------------------------------------------------
// Protocol options setter
// ---------------------------------------------------------------------------
//
void CNcdProtocol::SetProtocolOptions( TUint32 aOptions )
    {
    DLTRACEIN(("aOptions: %d", aOptions ));
    iProtocolOptions = aOptions;
    }

// ---------------------------------------------------------------------------
// Add client-info to the request configuration
// ---------------------------------------------------------------------------
// 
void CNcdProtocol::AddClientInfoToRequestL( 
    const MCatalogsContext& aContext, 
    CNcdRequestConfigurationData& aConfig )
    {
    DLTRACEIN((""));

    // Connect to telephony services so it won't have to be done in each
    // deviceinteraction method that require such services
    iDeviceService->ConnectL();
    
    CNcdKeyValueMap *userConfig = NULL;
     // Search for the user configuration
    TRAPD( err, 
        userConfig = &iConfigurationManager.ConfigurationsL( aContext,
            MNcdUserConfiguration::ENcdConfigurationClientInfo ) );
    if ( err != KErrNone ) 
        {
        // @ Error handling!
        }
        
    RPointerArray<CNcdKeyValuePair> configPairs;
    
    // Using close since configPairs won't own the data it points to
    CleanupClosePushL( configPairs );
    if ( userConfig )
        {           
        configPairs.ReserveL( userConfig->Pairs().Count() );
         
        for ( TInt i = 0; i < userConfig->Pairs().Count(); ++i )
            {                
            configPairs.AppendL( userConfig->Pairs()[i] );
            }            
        }
        
    // Add network info to the request
    SetNetworkInfoL( aConfig );
    
    CNcdRequestConfigurationClient* client = 
        CNcdRequestConfigurationClient::NewLC();

    // Add hardware configuration info to the request
    SetHardwareConfigurationL( client->Hardware(), configPairs );
        
    // Add software configuration info to the request
    AddSoftwareConfigurationL( aContext, *client, configPairs );
    
    
    DLTRACE(("Add client-info to request"));
    aConfig.AddClientL( client );
    CleanupStack::Pop( client );
    
    CleanupStack::PopAndDestroy( &configPairs );
    
    // Close telephony services
    User::LeaveIfError( iDeviceService->Close() );    
    }


// ---------------------------------------------------------------------------
// AddEngineClientInfoToRequestL
// Adds a client info element describing the engine
// ---------------------------------------------------------------------------
//
void CNcdProtocol::AddEngineClientInfoToRequestL( 
    CNcdRequestConfigurationData& aConfig )
    {
    DLTRACEIN((""));
    
    // Create a new client-element
    CNcdRequestConfigurationClient* client = 
        CNcdRequestConfigurationClient::NewLC();

    // Create a new software element
    CNcdRequestConfigurationSoftware* software = 
        CNcdRequestConfigurationSoftware::NewLC();
            

    // Set type, version and UID    
    software->SetSoftwareTypeL( 
        CNcdProviderUtils::EngineConfig().EngineType() );
    
    software->SetSoftwareVersionL( 
        CNcdProviderUtils::EngineConfig().EngineVersion() );
    
    software->SetSoftwareIdsL( 
        CNcdProviderUtils::EngineConfig().EngineUid(), 
        KNullDesC, 
        KNullDesC8 );

    
    //Set provisioning for the engine     
    if ( CNcdProviderUtils::EngineConfig().EngineProvisioning().Length() ) 
        {
        CNcdConfigurationProtocolDetailImpl* detail =
            CreateDetailLC( NcdConfigurationKeys::KProvisioning, 
                CNcdProviderUtils::EngineConfig().EngineProvisioning() );
        software->AddSoftwareDetailsL( detail );
        CleanupStack::Pop( detail );        
        }

    // engine install drive
    CNcdConfigurationProtocolDetailImpl* drive =
        CreateDetailLC( NcdConfigurationKeys::KInstallDrive, 
            CNcdProviderUtils::EngineConfig().EngineInstallDrive() );
    software->AddSoftwareDetailsL( drive );
    CleanupStack::Pop( drive );        

        
    client->AddSoftwareL( software );
    CleanupStack::Pop( software );
    
    aConfig.AddClientL( client );
    CleanupStack::Pop( client );    

    }

// ---------------------------------------------------------------------------
// SetNetworkInfoL
// Fills the network information 
// ---------------------------------------------------------------------------
//
void CNcdProtocol::SetNetworkInfoL( CNcdRequestConfigurationData& aConfig )
    {
    DLTRACEIN(( "" ));
        {        
        DLTRACE(( "Setting MCC & MNC" ));
        // Get & set current MCC and MNC
        aConfig.SetNetworkMccL( iHomeMcc, iDeviceService->CurrentMccL() );
        aConfig.SetNetworkMncL( iHomeMnc, iDeviceService->CurrentMncL() );
        }
        
        // Set service provider name
        {        
        DLTRACE(( "Setting service provider name" ));
        TBuf<MNcdDeviceService::KServiceProviderMaxLength> spName;
        iDeviceService->ServiceProviderL( spName );
        DLINFO(( _L("SP: %S"), &spName ));
        aConfig.SetNetworkProviderL( spName );        
        }
        
     // IMSI not set on purpose     
     
     
     // Set SMS center number
     HBufC* smsc = iSmsUtils->SmsCenterNumberLC();
     aConfig.SetNetworkSmscL( *smsc );
     CleanupStack::PopAndDestroy( smsc );   
     
    /**
     * @ Add: gid1/2, msisdn, operatorname
     */
     
        
    DLTRACEOUT(( "" ));
    }
    

// ---------------------------------------------------------------------------
// AddSoftwareConfigurationL
// Fills the client software information 
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddSoftwareConfigurationL( const MCatalogsContext& aContext,
    CNcdRequestConfigurationClient& aClient,
    RPointerArray<CNcdKeyValuePair>& aConfigPairs )
    {
    DLTRACEIN((""));    
    
/*

    RArray<TString> testmodes;
   
    RArray<TNcdRequestCustomDetails> details;
    KInstallDrive
    
Done:    
    TString uid;
    // Quasi-unique client id.
    TString id;

    //SIM-sensitive client id obtained by hashing the client id and IMSI code.
    TString sid; 
    TString language;
    TString type;
    TString version;
    RArray<TString> capabilities;

*/    
        
    DLTRACE(("Hashing client ID with IMSI"));
    const TDesC& clientId( iConfigurationManager.ClientIdL( aContext ) );
    
    DLTRACE(("Getting SSID"));
    TPtrC8 ssidPtr( iConfigurationManager.SsidL( aContext ) );
    DLTRACE(( "SSID: %S, length: %d", &ssidPtr, ssidPtr.Length() ));
    
    // Generate the SSID if it doesn't already exist
/*
    if ( !ssidPtr.Length() ) 
        {        
        DLTRACE(("Creating a new SSID"));
        HBufC8* ssid = NULL;
        //ssid = HashImsiAndIdLC( clientId );
        ssid = HashImsiLC();
       
        DLTRACE(("Setting uid, client id and hash to request"));

        // Ownership is transferred to the manager
        iConfigurationManager.SetSsidL( aContext, ssid );
        
        CleanupStack::Pop( ssid );
        ssidPtr.Set( *ssid );
        
        DLTRACE(( "Hashed SSID: %S", &ssidPtr ));
    
        }
*/        
        
    CNcdRequestConfigurationSoftware* software = 
        CNcdRequestConfigurationSoftware::NewLC();

    // Use secureId as the client UID
    software->SetSoftwareIdsL( CleanUidName( aContext.SecureId() ), 
        clientId, ssidPtr );

    DLTRACE(( _L("Client ID: %S"), &clientId ));
    
    
    // Add client & engine capabilities (must be done before 
    // AddConfigurationsToSoftwareDetailsL)
    AddCapabilitiesL( *software, aConfigPairs );
    
    
    if ( FindKey( NcdConfigurationKeys::KSoftwareType(), aConfigPairs ) 
         != KErrNotFound )
        {
        DLTRACE(("Front-end overrides type"));
        // Add software type
        AddSoftwareUserConfigurationL( *software, 
            &CNcdRequestConfigurationSoftware::SetSoftwareTypeL, aConfigPairs, 
            NcdConfigurationKeys::KSoftwareType() );
        }
    else 
        {
        DLTRACE(("Using type from engine"));
        software->SetSoftwareTypeL( 
            CNcdProviderUtils::EngineConfig().EngineType() );
        }


    // Add software language
    AddSoftwareUserConfigurationL( *software, 
        &CNcdRequestConfigurationSoftware::SetSoftwareLanguageL, aConfigPairs, 
        NcdConfigurationKeys::KSoftwareLanguage() );


#ifdef CATALOGS_OVERRIDE_LANGUAGE
    DLINFO(("Overriding software language"));
    software->SetSoftwareLanguageL( KCatalogsOverrideSoftwareLanguage() );
#endif

    // Add software version
    AddSoftwareUserConfigurationL( *software, 
        &CNcdRequestConfigurationSoftware::SetSoftwareVersionL, aConfigPairs, 
        NcdConfigurationKeys::KSoftwareVersion() );


    // Add unused configurations as details
    // Note: This MUST be done only after all AddUserConfigurationL calls
    AddConfigurationsToSoftwareDetailsL( *software, aConfigPairs );
    
    aClient.AddSoftwareL( software );
    CleanupStack::Pop( software );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// SetHardwareConfigurationL
// Fills the client hardware information 
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::SetHardwareConfigurationL( 
    CNcdRequestConfigurationHardware& aConfig, 
    RPointerArray<CNcdKeyValuePair>& aConfigPairs )
    {
    DLTRACEIN((""));    
/*
    TString uaProfileUri;

    TString manufacturer;
    TString model;
    RArray<TNcdConfigurationDisplay> displays;
    RArray<TNcdRequestCustomDetails> details;
    KFreeStorage

Done:    
    //TString identification;    
    //TString language;
    //TString platform;
    //TString firmwareVersion;

*/    
    // Set device identification
    aConfig.SetHardwareIdentificationL( 
        *iDeviceService->DeviceIdentificationLC() );
    CleanupStack::PopAndDestroy();
    
    // Set device language
    aConfig.SetHardwareLanguageL( *iDeviceService->DeviceLanguageLC() );
    CleanupStack::PopAndDestroy(); // Device language

    // Set platform
    aConfig.SetHardwarePlatformL( KPlatformInfo );

    // Set firmware
    aConfig.SetHardwareFirmwareL( iDeviceService->FirmwareL() );

    // Set device model and manufacturer
    aConfig.SetHardwareModelL( iDeviceService->DeviceModelL() );    
    aConfig.SetHardwareManufacturerL( iDeviceService->DeviceManufacturerL() );
    
    // Get free space for the temp drive used for downloads
    TInt64 freeSpace = FreeDiskSpaceL( CNcdProviderUtils::FileSession(), 
        CNcdProviderUtils::EngineConfig().EngineTempDrive() );

    // Format the integer and add to hardware details
    RBuf formatBuffer;
    CleanupClosePushL( formatBuffer );
    formatBuffer.CreateL( 128 );
    _LIT( KInt64, "%li" );
    
    formatBuffer.Format( KInt64, freeSpace );
    DLTRACE(("Adding free space to hardware details"));
    CNcdConfigurationProtocolDetailImpl* freeSpaceDetail = CreateDetailLC(
        NcdInternalConfigurationKeys::KFreeStorage(), formatBuffer );
    aConfig.AddHardwareDetailsL( freeSpaceDetail );
    CleanupStack::Pop( freeSpaceDetail );        
    CleanupStack::PopAndDestroy( &formatBuffer );
    
    // Add flash-player details
    DLTRACE(("Adding flash-player to hardware details"));
    TUid flashUid;
    TCatalogsVersion flashVersion;
    TBool flashInstalled = FlashPlayerDataL( flashUid, flashVersion );
    if ( flashInstalled ) 
        {        
        CNcdConfigurationProtocolDetailImpl* flashPlayerDetail = CreateDetailLC(        
            NcdInternalConfigurationKeys::KFlashPlayer(), KNullDesC() );
        CNcdConfigurationProtocolContentImpl* fpVersion = 
            CNcdConfigurationProtocolContentImpl::NewLC();
        NcdProtocolUtils::AssignDesL(
            fpVersion->iKey, KFlashPlayerVersion );
        NcdProtocolUtils::AssignDesL(
            fpVersion->iValue, *TCatalogsVersion::ConvertLC( flashVersion ) );
        CleanupStack::PopAndDestroy(); 
        flashPlayerDetail->iContents.AppendL( fpVersion );
        CleanupStack::Pop( fpVersion );
                   
        CNcdConfigurationProtocolContentImpl* fpUid = 
            CNcdConfigurationProtocolContentImpl::NewLC();
        NcdProtocolUtils::AssignDesL( fpUid->iKey, KFlashPlayerUid );
        TUidName fpUidName = CleanUidName( flashUid );        
        NcdProtocolUtils::AssignDesL( fpUid->iValue, fpUidName );
        flashPlayerDetail->iContents.AppendL( fpUid );
        CleanupStack::Pop( fpUid );
            
        aConfig.AddHardwareDetailsL( flashPlayerDetail );
        CleanupStack::Pop( flashPlayerDetail );
        }

    // Add IMEI if necessary
    if ( iProtocolOptions & ESendImei ) 
        {
        const TDesC& imei = iDeviceService->ImeiL();    
        DLINFO(( _L("Adding IMEI: %S"), &imei ));
        CNcdConfigurationProtocolDetailImpl* imeiDetail = 
            CreateDetailLC( NcdInternalConfigurationKeys::KImei(), imei );
        aConfig.AddHardwareDetailsL( imeiDetail );
        CleanupStack::Pop( imeiDetail );    
        }
    
    // Add all displays
    while ( AddDisplayDetailsL( aConfig, aConfigPairs ) == KErrNone )
        {        
        // Empty on purpose
        }

    AddProductCodeToHardwareDetailsL( aConfig, aConfigPairs );
    AddFirmwareDetailsToHardwareDetailsL( aConfig );  
    DLTRACEOUT((""));
    }



// ---------------------------------------------------------------------------
// Adds a configuration from the user to the request configuration
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddConfigurationsToSoftwareDetailsL( 
    CNcdRequestConfigurationSoftware& aConfig,
    RPointerArray<CNcdKeyValuePair>& aUserConfig ) const
    {
    DLTRACEIN(("Config count: %d", aUserConfig.Count() ));

    // All leftover key-value -pairs are added as details    
    for ( TInt i = 0; i < aUserConfig.Count(); ++i ) 
        {
        CNcdConfigurationProtocolDetailImpl* detail =
            CreateDetailLC( aUserConfig[i]->Key(), aUserConfig[i]->Value() );
        aConfig.AddSoftwareDetailsL( detail );
        CleanupStack::Pop( detail );
        }
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Creates a detail object
// ---------------------------------------------------------------------------
//    
CNcdConfigurationProtocolDetailImpl* CNcdProtocol::CreateDetailLC( 
    const TDesC& aKey, const TDesC& aValue ) const
    {
    CNcdConfigurationProtocolDetailImpl* details = 
        CNcdConfigurationProtocolDetailImpl::NewLC();
    
    delete details->iId;
    details->iId = NULL;    
    details->iId = aKey.AllocL();
    
    delete details->iValue;
    details->iValue = NULL;
    details->iValue = aValue.AllocL();            
    return details;
    }
    
// ---------------------------------------------------------------------------
// Adds a configuration from the user to the request configuration
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddSoftwareUserConfigurationL( 
    CNcdRequestConfigurationSoftware& aConfig,
    void (CNcdRequestConfigurationSoftware::*aValueSetter)( const TDesC& ),
    RPointerArray<CNcdKeyValuePair>& aUserConfig, 
    const TDesC& aKey ) const
    {
    DLTRACEIN(( _L("Count: %d, Key: %S"), aUserConfig.Count(), &aKey ));
    DASSERT( aValueSetter );

    TInt index = FindKey( aKey, aUserConfig );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(( _L("Calling the setter with value: %S"), 
            &aUserConfig[index]->Value() ));

        const TDesC& value = aUserConfig[index]->Value();
        (aConfig.*aValueSetter )( value );    

        aUserConfig.Remove( index );        
        }
      
    DLTRACEOUT((""));    
    }
    
    
// ---------------------------------------------------------------------------
// Adds client capabilities to the request
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddCapabilitiesL( 
    CNcdRequestConfigurationSoftware& aConfig,
    RPointerArray<CNcdKeyValuePair>& aUserConfig ) const    
    {
    DLTRACEIN((""));
    TInt i = aUserConfig.Count() - 1;
    while( i >= 0 ) 
        {
        // Check if the configuration is actually a capability
        if ( aUserConfig[i]->Key().Compare( 
            NcdConfigurationKeys::KCapability ) == 0 ) 
            {
            // Checks that the capability is supported by the engine
            if ( IsCapabilitySupported( aUserConfig[i]->Value() ) ) 
                {
                DLTRACE(( _L("Adding capability: %S"), &aUserConfig[i]->Value() ));
                aConfig.AddSoftwareCapabilityL( aUserConfig[i]->Value() );
                }
            
            // Just remove since the pairs are not owned by the array
            aUserConfig.Remove( i );
            }
        --i;
        }
      
    DLTRACEOUT((""));
    }
  

// ---------------------------------------------------------------------------
// Checks if the engine supports the given capability or not
// ---------------------------------------------------------------------------
//            
TBool CNcdProtocol::IsCapabilitySupported( const TDesC& aCapability ) const
    {
    /**
     * @ Update engine/provider's capabilities here
     */
    DLTRACEIN((""));
    /** 
     * Check that engine supports the capability
     */
    if ( NcdCapabilities::KSearch().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KSubscriptions().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
/*        
    else if ( NcdCapabilities::KEmbeddedSessions().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/        
/*
    else if ( NcdCapabilities::KActivities().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/
/*        
    else if ( NcdCapabilities::KLegacySubscriptions().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/        
/*    
    else if ( NcdCapabilities::KUpload().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/    
/*
    else if ( NcdCapabilities::KReDownload().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/        
/*
    else if ( NcdCapabilities::KGpsLocation().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/
/*        
    else if ( NcdCapabilities::KDelayedTransfer().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/      
/* 
    else if ( NcdCapabilities::KAutoDownload().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        } 
*/        
/*           
    else if ( NcdCapabilities::KXmlResponseFiltering().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
*/        
    else if ( NcdCapabilities::KInstallationReport().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
/*        
    else if ( NcdCapabilities::KBrowseFiltering().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    
    else if ( NcdCapabilities::KClientReview().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    
    else if ( NcdCapabilities::KPromotionalEntities().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/    
    else if ( NcdCapabilities::KDirectSchemeLinks().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }    
    else if ( NcdCapabilities::KIndirectSchemeLinks().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
/*        
    else if ( NcdCapabilities::KEntityRequest().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
       
    else if ( NcdCapabilities::KDiffQuery().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KUpLevel().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KRemoteContent().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KPredefinedSearch().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }        
    else if ( NcdCapabilities::KPredefinedPurchase().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/        
    else if ( NcdCapabilities::KCacheExpiration().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
/*        
    else if ( NcdCapabilities::KMultiPurchase().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/        
    else if ( NcdCapabilities::KMultiDownload().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KBasicQueries().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KSourceScopeCookies().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KSimScopeCookies().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
/*        
    else if ( NcdCapabilities::KCancelPurchase().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
        
    else if ( NcdCapabilities::KSmsAction().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/        
    else if ( NcdCapabilities::KDrmClientDownload().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
/*        
    else if ( NcdCapabilities::KGiftPurchase().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
*/
    else if ( NcdCapabilities::KUniversalSubscriptions().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
    else if ( NcdCapabilities::KDownloadReport().Compare( aCapability ) == 0 ) 
        {
        return ETrue;
        }
        
    DLTRACEOUT((""));    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// AddCookiesL
// ---------------------------------------------------------------------------
//    
TBool CNcdProtocol::AddCookiesL( const MCatalogsContext& aContext,
    RPointerArray<MNcdConfigurationProtocolCookie>& aCookies,
    const TDesC& aServerUri,
    const TDesC& aNamespace )
    {
    DLTRACEIN((""));

    TBool removedCookies = EFalse;
    
    TInt index = FindContextData( aContext );
    if ( index != KErrNotFound ) 
        {
        
        // Get SSID
        const TDesC8& ssid( iConfigurationManager.SsidL( aContext ) );


        // Get server details
        MNcdServerDetails& details( 
            iConfigurationManager.ServerDetailsL( aContext, 
            aServerUri,
            aNamespace ));
        
        // Remove expired cookies
        removedCookies = details.RemoveExpiredCookies();
        
        // If the cookie's scope is "sim" it has to match the given SSID
        RPointerArray<const MNcdConfigurationProtocolCookie> cookies( 
            details.CookiesL( ssid ) );
            
        CleanupClosePushL( cookies );                
        
        TIdentityRelation<MNcdConfigurationProtocolCookie> identity(
            CNcdConfigurationProtocolCookie::Identity );
        
        DLTRACE(( "Adding %i cookies to the request", cookies.Count() ));
        for ( TInt i = 0; i < cookies.Count(); ++i ) 
            {
            
            // Only add "saveAndSend" cookies
            if ( cookies[i]->Type().Compare( 
                NcdConfigurationProtocolCookieTypes::KSaveAndSend ) == 0 )
                {
                DLTRACE(( _L("Adding cookie with key: %S"), &cookies[i]->Key() ));
                CNcdConfigurationProtocolCookie* copy = 
                    CNcdConfigurationProtocolCookie::NewLC( *cookies[i] );
                
                
                // Ensure that a matching cookie doesn't already exist
                TInt oldCookie = aCookies.Find( copy, identity );
                if ( oldCookie != KErrNotFound )
                    {
                    delete aCookies[oldCookie];
                    aCookies[oldCookie] = copy;
                    }
                else
                    {
                    // Add cookie to request, ownership is transferred
                    aCookies.AppendL( copy );
                    }
 
                CleanupStack::Pop( copy );
                }
            }        
        DLTRACE(( "Cookies added" ));
        CleanupStack::PopAndDestroy( &cookies );
        
        }
    DLTRACEOUT((""));
    return removedCookies;
    }


// ---------------------------------------------------------------------------
// Add cookies to the request
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddCookiesToRequestL( 
    CNcdRequestBase& aRequest,
    RPointerArray<MNcdConfigurationProtocolCookie>& aCookies )
    {
    for ( TInt i = 0; i < aCookies.Count(); ++i )
        {
        aRequest.AddCookieL( aCookies[i] );
        aCookies[i] = NULL;
        }
    aCookies.Reset();
    }


// ---------------------------------------------------------------------------
// Add cookies to the config request
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddCookiesToConfigRequestL( 
    CNcdRequestConfigurationData& aConfig,
    RPointerArray<MNcdConfigurationProtocolCookie>& aCookies )
    {
    for ( TInt i = 0; i < aCookies.Count(); ++i )
        {
        aConfig.AddCookieL( aCookies[i] );
        aCookies[i] = NULL;
        }
    aCookies.Reset();
    }
    

// ---------------------------------------------------------------------------
// Searches for the given key in the array
// ---------------------------------------------------------------------------
//    
TInt CNcdProtocol::FindKey( const TDesC& aKey, 
    const RPointerArray<CNcdKeyValuePair>& aArray ) const
    {    
    for ( TInt i = 0; i < aArray.Count(); ++i ) 
        {     
        if ( aArray[i]->Key().Compare( aKey ) == 0 ) 
            {    
            return i;
            }
        }
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Reads the value of the given key as TInt and removes the key
// ---------------------------------------------------------------------------
//    
TInt CNcdProtocol::AddDisplayDetailsL(     
    CNcdRequestConfigurationHardware& aConfig,
    RPointerArray<CNcdKeyValuePair>& aConfigPairs )
    {
    DLTRACEIN((""));
    TInt index = FindKey( NcdConfigurationKeys::KDisplay, aConfigPairs );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(("Found the key"));
        TInt disp = 0;
        TInt width = 0;
        TInt height = 0;
        TInt colors = 0;
        
        // Interpret the value as TInts
        TInt err = InterpretDisplayValue( aConfigPairs[index]->Value(), 
            disp, width, height, colors );
            
        // Remove the pair (doesn't matter whether the conversion was 
        // successful or not
        aConfigPairs.Remove( index );
        if ( err == KErrNone ) 
            {            
            DLTRACE(("Adding display details to the config"));
            aConfig.AddHardwareDisplayL( colors, height, width );    
            }
        return err;        
        }
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Interprets a display value
// ---------------------------------------------------------------------------
//    
TInt CNcdProtocol::InterpretDisplayValue( 
    const TDesC& aValue, TInt& aDispNumber, TInt& aWidth, 
    TInt& aHeight, TInt& aColors ) const
    {
    DLTRACEIN((""));
    TLex lex( aValue );
    
    TInt err = lex.Val( aDispNumber );
    if ( err == KErrNone ) 
        {        
        lex.SkipSpaceAndMark();
        err = lex.Val( aWidth );
        if ( err == KErrNone ) 
            {            
            lex.SkipSpaceAndMark();
            err = lex.Val( aHeight );
            if ( err == KErrNone ) 
                {
                lex.SkipSpaceAndMark();
                lex.Val( aColors );            
                }
            }
        }
    
    DLTRACEOUT(("disp: %d, width: %d, height: %d, colors: %d", 
        aDispNumber, aWidth, aHeight, aColors ));
    return err;
    }


// ---------------------------------------------------------------------------
// Reads the value of the given key and and removes the config pair
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddProductCodeToHardwareDetailsL(     
    CNcdRequestConfigurationHardware& aConfig,
    RPointerArray<CNcdKeyValuePair>& aConfigPairs )
    {
    DLTRACEIN((""));

    // First try to get the product code from the device service.
    // If device service gives the product code, then always use it.
    // If device service does not give the product code, then check
    // if product code is given through the NCD API and use it if 
    // available.

    // Get possible product code from the device service.
    HBufC* productCode( iDeviceService->ProductCodeLC() );

    // Get the index value for the possible product code in
    // config pair array.
    TInt index( 
        FindKey( NcdConfigurationKeys::KDeviceProductCode, 
                 aConfigPairs ) );

    // Check if the config pair array contains the product code.
    if ( index != KErrNotFound )
        {
        DLINFO(("Product code given in config pair."));
        if ( !productCode )
            {
            DLINFO(("Use config pair product code"));

            // Product code was not gotten from device service.
            // So, use the config pair value.
            productCode = aConfigPairs[ index ]->Value().AllocLC();            
            }

        // Remove the pair is required. This way the config pair will not
        // be included into the software details element in the server requests.
        aConfigPairs.Remove( index );
        }

    // Check if product code was gotten from device service or from
    // the config pair.
    if ( productCode )
        {
        DLINFO(( _L("Adding productCode: %S"), productCode ));

        // Add the hardware detail because value exists.
        CNcdConfigurationProtocolDetailImpl* productCodeDetail( 
            CreateDetailLC( NcdConfigurationKeys::KDeviceProductCode(), 
                            *productCode ) );
        aConfig.AddHardwareDetailsL( productCodeDetail );
        CleanupStack::Pop( productCodeDetail );

        CleanupStack::PopAndDestroy( productCode );        
        }
    }

    
// ---------------------------------------------------------------------------
// Creates a new context data object or returns an existing one
// ---------------------------------------------------------------------------
//    
CNcdProtocol::CContextData& CNcdProtocol::CreateContextDataL( 
    const MCatalogsContext& aContext )
    {
    DLTRACEIN((""));
    TInt index = FindContextData( aContext );
    if ( index == KErrNotFound )
        {
        // Create a new session handler
        CNcdSessionHandler* sessionHandler = CNcdSessionHandler::NewL();
        
        CleanupStack::PushL( sessionHandler );
        
        
        // Create a new context data
        // owns session and cookie handlers
        CContextData* newData = new( ELeave ) CContextData( aContext, 
            sessionHandler );
            
        CleanupStack::Pop( sessionHandler );
        CleanupStack::PushL( newData );
        
        iContexts.AppendL( newData );
        CleanupStack::Pop( newData );
        index = iContexts.Count() - 1;
        }
    DLTRACEOUT(( "index: %d", index ));
    return *iContexts[index];
    }


// ---------------------------------------------------------------------------
// Searches for the given context in contexts
// ---------------------------------------------------------------------------
//    
TInt CNcdProtocol::FindContextData( const MCatalogsContext& aContext ) const
    {
    TIdentityRelation<CContextData> match( CNcdProtocol::MatchContextDatas );
    CContextData data( aContext, NULL );
    
    return iContexts.Find( &data, match );
    }
    

// ---------------------------------------------------------------------------
// Compares two contexts
// ---------------------------------------------------------------------------
//    
TBool CNcdProtocol::MatchContextDatas( const CContextData& aFirst, 
    const CContextData& aSecond )
    {
    return aFirst.FamilyId() == aSecond.FamilyId();
    }


// ---------------------------------------------------------------------------
// Gets flash player data
// ---------------------------------------------------------------------------
//    
TBool CNcdProtocol::FlashPlayerDataL( TUid& aUid, TCatalogsVersion& aVersion ) 
    {
    DLTRACEIN((""));
    
    TUid flashUid = iDeviceService->FlashPlayerUidL();
    
    if( flashUid.iUid == NULL )
        {
        return EFalse;
        }
        
    TCatalogsVersion flashVersion;
    MNcdInstallationService& installationService = 
        CNcdProviderUtils::InstallationServiceL();
    TInt err = installationService.ApplicationVersion( flashUid, flashVersion );
    
    if ( err != KErrNone ) 
        {
        DLTRACEOUT(("Error occured while retrieving flash player version"))
        return EFalse;
        }
    
    aUid = flashUid;
    aVersion = flashVersion;
    
    DLTRACEOUT(("Flash player uid: %d, version: %d.%d.%d", 
        aUid.iUid, 
        aVersion.iMajor,
        aVersion.iMinor,
        aVersion.iBuild));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Add additional firmware information
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddFirmwareDetailsToHardwareDetailsL(     
    CNcdRequestConfigurationHardware& aConfig )
    {
    DLTRACEIN((""));
    RBuf buf;
    buf.CreateL( MNcdDeviceService::KFirmwareDetailMaxLength );
    CleanupClosePushL( buf );

    RFs& fs = CNcdProviderUtils::FileSession();
    
    iDeviceService->GetProductType( buf, fs );
    AddHardwareDetailL( aConfig, KProductType, buf );    
    buf.Zero();

    iDeviceService->GetFirmwareId( buf, fs );
    AddHardwareDetailL( aConfig, KFirmwareId, buf );    
    buf.Zero();    
    
    iDeviceService->GetFirmwareVersion1( buf );
    AddHardwareDetailL( aConfig, KFirmwareVersion1, buf );    
    buf.Zero();
    
    iDeviceService->GetFirmwareVersion2( buf );
    AddHardwareDetailL( aConfig, KFirmwareVersion2, buf );    
    buf.Zero();

    iDeviceService->GetFirmwareVersion3( buf, fs );
    AddHardwareDetailL( aConfig, KFirmwareVersion3, buf );    
    buf.Zero();
    
    AddHardwareDetailL( aConfig, KSimLockStatus, KFalseAsZero );

    CleanupStack::PopAndDestroy( &buf );
    }


// ---------------------------------------------------------------------------
// Helper method for adding a detail to hardware details
// ---------------------------------------------------------------------------
//    
void CNcdProtocol::AddHardwareDetailL( 
    CNcdRequestConfigurationHardware& aConfig,
    const TDesC& aKey,
    const TDesC& aValue )
    {
    if ( aValue.Length() )
        {
        CNcdConfigurationProtocolDetailImpl* detail( 
            CreateDetailLC( aKey, 
                            aValue ) );
        aConfig.AddHardwareDetailsL( detail );
        CleanupStack::Pop( detail );
        }
    }
