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
* Description:   CNcdProtocol declaration
*
*/


#ifndef C_NCDPROTOCOL_H
#define C_NCDPROTOCOL_H

#include <e32base.h>

#include "ncdparserobserver.h"
#include "ncdprotocol.h"
#include "ncddeviceservice.h"
#include "ncduserconfiguration.h"
#include "ncdprotocoldefaultobserver.h"

class CNcdSessionHandler;
class MNcdParser;
class CNcdRequestBase;
class CNcdRequestConfigurationData;
class CNcdRequestConfigurationClient;
class CNcdRequestConfigurationSoftware;
class CNcdRequestConfigurationHardware;
class CCatalogsSmsUtils;
class CNcdKeyValuePair;
class CNcdKeyValueMap;
class MCatalogsContext;
class MNcdDatabaseStorage;
class MNcdStorageManager;
class MNcdConfigurationManager;
class CNcdProtocolDefaultObserverImpl;
class MNcdProtocolDefaultObserver;
class MNcdSessionHandler;
class MNcdConfigurationProtocolCookie;
class CNcdSubscriptionManager;
class CNcdConfigurationProtocolDetailImpl;
class TCatalogsVersion;

/**
 * Protocol entry point.
 *
 * @note Requests are created with NcdRequestGenerator
 *
 */
class CNcdProtocol : public CBase, public MNcdProtocol
    {  
public:
    
    /**
     * Protocol options are set with SetProtocolOptions
     */
    enum TNcdProtocolOptions
        {
        /**
         * Send IMEI in the client configuration
         */ 
        ESendImei = 1
        };
    
public:
    /**
     * Creator
     */
    static CNcdProtocol* NewL(
        MNcdConfigurationManager& aConfigurationManager,
        CNcdSubscriptionManager& aSubscriptionManager );
       
    /**
     * Destructor
     */ 
    ~CNcdProtocol();
    
private:

    /**
     * Constructor
     */
    CNcdProtocol(
        MNcdConfigurationManager& aConfigurationManager,
        CNcdSubscriptionManager& aSubscriptionManager );
    
    /**
     * ConstructL
     */
    void ConstructL();
    
public: // From MNcdProtocol

    /**
     * @see MNcdProtocol::CreateParserL()
     */
    MNcdParser* CreateParserL( MCatalogsContext& aContext,
        const TDesC& aServerUri );


    /**
     * @see MNcdProtocol::ProcessPreminetRequestL()
     */
    HBufC8* ProcessPreminetRequestL( const MCatalogsContext& aContext, 
        CNcdRequestBase& aRequest, 
        const TDesC& aServerUri,
        TBool aForceConfigurationData = EFalse );

    /**
     * @see MNcdProtocol::ProcessConfigurationRequestL()
     */
    HBufC8* ProcessConfigurationRequestL( const MCatalogsContext& aContext, 
        CNcdRequestConfiguration& aRequest );
    
    
    /**
     * @see MNcdProtocol::SessionHandler()
     */
    MNcdSessionHandler& SessionHandlerL( 
        const MCatalogsContext& aContext ) const;
        

public: // New methods
    
    /**
     * Sets protocol options
     * 
     * @param aOptions A combination of TNcdProtocolOptions
     */
    void SetProtocolOptions( TUint32 aOptions );

private: // New methods

    // Adds client-info to the given request configuration (except cookies)
    void AddClientInfoToRequestL( 
        const MCatalogsContext& aContext, 
        CNcdRequestConfigurationData& aConfig );


    // Adds engine's client-info the request
    void AddEngineClientInfoToRequestL( 
        CNcdRequestConfigurationData& aConfig );

    void SetNetworkInfoL( CNcdRequestConfigurationData& aConfig );   
    
    void AddSoftwareConfigurationL( const MCatalogsContext& aContext,
        CNcdRequestConfigurationClient& aClient,
        RPointerArray<CNcdKeyValuePair>& aConfigPairs );
        
    void SetHardwareConfigurationL( 
        CNcdRequestConfigurationHardware& aConfig,
        RPointerArray<CNcdKeyValuePair>& aConfigPairs );

    // Adds user configurations to the request config
    void AddConfigurationsToSoftwareDetailsL( 
        CNcdRequestConfigurationSoftware& aConfig,
        RPointerArray<CNcdKeyValuePair>& aUserConfig) const;

    // Adds a single protocol detail
    CNcdConfigurationProtocolDetailImpl* CreateDetailLC( 
        const TDesC& aKey, const TDesC& aValue ) const;

    void AddSoftwareUserConfigurationL( 
        CNcdRequestConfigurationSoftware& aConfig,
        void (CNcdRequestConfigurationSoftware::*aValueSetter)( const TDesC& ),
        RPointerArray<CNcdKeyValuePair>& aUserConfig, 
        const TDesC& aKey ) const;

    // Adds client's capabilities and engine's/provider's hardcoded capabilities
    void AddCapabilitiesL( 
        CNcdRequestConfigurationSoftware& aConfig,
        RPointerArray<CNcdKeyValuePair>& aUserConfig ) const;
        
    // Checks if the engine supports the given capability or not
    // Engine's caps are hardcoded in this method
    TBool IsCapabilitySupported( const TDesC& aCapability ) const;
        
    // Adds cookies to the array
    // Returns ETrue if cookies were removed due to expiration
    TBool AddCookiesL( const MCatalogsContext& aContext, 
        RPointerArray<MNcdConfigurationProtocolCookie>& aCookies,
        const TDesC& aServerUri,
        const TDesC& aNamespace );
        
    // Adds the cookies from the array to the request and removes them
    // from the array
    void AddCookiesToRequestL( 
        CNcdRequestBase& aRequest,
        RPointerArray<MNcdConfigurationProtocolCookie>& aCookies );

    // Adds the cookies from the array to the configuration request 
    // and removes them from the array
    void AddCookiesToConfigRequestL( 
        CNcdRequestConfigurationData& aConfig,
        RPointerArray<MNcdConfigurationProtocolCookie>& aCookies );

    // Searches for the given key
    TInt FindKey( const TDesC& aKey, 
        const RPointerArray<CNcdKeyValuePair>& aArray ) const;


    // Adds display details to aConfig from aConfigPairs
    TInt AddDisplayDetailsL( 
        CNcdRequestConfigurationHardware& aConfig,
        RPointerArray<CNcdKeyValuePair>& aConfigPairs );

    // Interprets the given display value
    TInt InterpretDisplayValue( 
        const TDesC& aValue, TInt& aDispNumber, TInt& aWidth, 
        TInt& aHeight, TInt& aColors ) const;

    // Adds the possible product code to the hardware details.
    void AddProductCodeToHardwareDetailsL(     
        CNcdRequestConfigurationHardware& aConfig,
        RPointerArray<CNcdKeyValuePair>& aConfigPairs );
                
    /**
     * Gets flash player UID and version.
     *
     * @param aUid On return, contains the UID of flash-player installed
     *             on the device.
     * @param aVersion On return, contains the version of flash-player
     *                 installed on the device.
     * @return ETrue If the flash player data was found, otherwise false.
     */
    TBool FlashPlayerDataL( TUid& aUid, TCatalogsVersion& aVersion );
    
    /**
     * Adds additional firmware details to hardware details
     */
    void AddFirmwareDetailsToHardwareDetailsL(     
        CNcdRequestConfigurationHardware& aConfig );

    /**
     * A helper method for adding a hardware detail
     */
    void AddHardwareDetailL( 
        CNcdRequestConfigurationHardware& aConfig,
        const TDesC& aKey,
        const TDesC& aValue );

private:

    // forward declaration
    class CContextData;

    CContextData& CreateContextDataL( const MCatalogsContext& aContext );
    TInt FindContextData( const MCatalogsContext& aContext ) const;

    static TBool MatchContextDatas( const CContextData& aFirst, 
        const CContextData& aSecond );


private: // Data types

    /**
     * Context-specific data
     */
    class CContextData : public CBase, 
        public MNcdParserSessionObserver
        {
        public:
            CContextData( const MCatalogsContext& aContext,                 
                CNcdSessionHandler* aSessionHandler );
                
            ~CContextData();
            
            /**
             * Session handler getter
             */
            MNcdSessionHandler& SessionHandler() const;
             
            
            TUid FamilyId() const;
             
        private: // From MNcdParserSessionObserver

            /**
             * @see MNcdParserSessionObserver::SessionL
             */
            void SessionL( const TDesC& aSessionId, const TDesC& aServerUri,
                const TDesC& aNameSpace );
                
        private:
            // Context's family id
            TUid iFamilyId;            
            
            // Session handler
            CNcdSessionHandler* iSessionHandler;
            
        };


private: // Data
            
    MNcdConfigurationManager& iConfigurationManager;
    CNcdSubscriptionManager& iSubscriptionManager;
        
    // Owned
    CCatalogsSmsUtils* iSmsUtils;    
    
    // Not owned
    MNcdDeviceService* iDeviceService;
    
    TBuf<MNcdDeviceService::KMccLength> iHomeMcc;
    TBuf<MNcdDeviceService::KMncLength> iHomeMnc;

    // Context-specific data, owned
    RPointerArray<CContextData> iContexts;
    
    TUint32 iProtocolOptions;
    };


#endif
