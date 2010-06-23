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
* Description:   CNcdRequestConfiguration declaration
*
*/


#ifndef C_NCD_REQUEST_CONFIGURATION_DATA_HH
#define C_NCD_REQUEST_CONFIGURATION_DATA_HH

#include <e32std.h>
#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolCookie;
class MNcdConfigurationProtocolDetail;


/**
 * Software-element for client-elements
 */
class CNcdRequestConfigurationSoftware : public CBase
    {
public:

    static CNcdRequestConfigurationSoftware* NewLC();
    ~CNcdRequestConfigurationSoftware();

public:

    void SetSoftwareTypeL( const TDesC& aType );
    void SetSoftwareVersionL( const TDesC& aVersion );
    void SetSoftwareIdsL( const TDesC& aUid, const TDesC& aId, 
        const TDesC8& aSsid );
    void SetSoftwareLanguageL( const TDesC& aLanguage );    
    void AddSoftwareCapabilityL( const TDesC& aCapability );
    void AddSoftwareDetailsL( MNcdConfigurationProtocolDetail* aDetail);
    
    
    TXmlEngElement GetSoftwareConfigurationElementL(
        RXmlEngDocument& aDocument, const TXmlEngString& aPrefix );
    
private:

    // client software configuration
    TNcdConfigurationSoftware iSoftware;
    
    
    };
    

/**
 * Hardware-element for client-elements
 */
class CNcdRequestConfigurationHardware : public CBase
    {
public:

    static CNcdRequestConfigurationHardware* NewL();
    ~CNcdRequestConfigurationHardware();

public:

    void SetHardwareUaProfileUriL( const TDesC& aUaProfileUri );
    void SetHardwareIdentificationL( const TDesC& aIdentification );
    void SetHardwareManufacturerL( const TDesC& aManufacturer );
    void SetHardwareModelL( const TDesC& aModel );
    void SetHardwareLanguageL( const TDesC& aLanguage );
    void SetHardwarePlatformL( const TDesC& aPlatform );
    void SetHardwareFirmwareL( const TDesC& aFirmwareVersion );
    void AddHardwareDisplayL( TInt aColors, TInt aHeight, TInt aWidth );
    void AddHardwareDetailsL( MNcdConfigurationProtocolDetail* aDetail );
    
    TXmlEngElement GetHardwareConfigurationElementL(
        RXmlEngDocument& aDocument,
        const TXmlEngString& aPrefix );
        
private:

    // client hardware configuration
    TNcdConfigurationHardware iHardware;
        
    };
    

/**
 * Client-element for configuration requests
 */
class CNcdRequestConfigurationClient : public CBase
    {
public:
    static CNcdRequestConfigurationClient* NewLC();
    ~CNcdRequestConfigurationClient();
    
public:

    void AddSoftwareL( CNcdRequestConfigurationSoftware* aSoftware );
    
    TInt SoftwareCount() const;
    CNcdRequestConfigurationSoftware& Software( TInt aIndex );
       
       
    CNcdRequestConfigurationHardware& Hardware();
     
protected:    

    CNcdRequestConfigurationClient();
    void ConstructL();    
    
private:

    RPointerArray<CNcdRequestConfigurationSoftware> iSoftwares;
    CNcdRequestConfigurationHardware* iHardware;
        
    };



class CNcdRequestConfigurationData : public CBase
    {
public:

    static CNcdRequestConfigurationData* NewLC();
    ~CNcdRequestConfigurationData();    

private:
    
    CNcdRequestConfigurationData();

public:
    void SetNamespacePrefixL( const TDesC8& aPrefix );
    
    void AddCookieL( MNcdConfigurationProtocolCookie* aCookie ); 

    void AddQueryResponseL(
        TNcdConfigurationQueryResponse aQueryResponse);

    /* Network configuration data */    
    void SetNetworkMccL( const TDesC& aMcc, const TDesC& aCurrentMcc );
    void SetNetworkMncL( const TDesC& aMnc, const TDesC& aCurrentMnc );
    void SetNetworkGidL( const TDesC& aGid1, const TDesC& aGid2 );
    void SetNetworkSmscL( const TDesC& aSmsc );
    void SetNetworkImsiL( const TDesC& aImsi );
    void SetNetworkMsisdnL( const TDesC& aMsisdn );
    void SetNetworkCellIdL( const TDesC& aCellId );
    void SetNetworkProviderL( const TDesC& aServiceProviderName );
    void SetNetworkOperatorL( const TDesC& aOperatorName );
    void AddNetworkDetailsL( MNcdConfigurationProtocolDetail* aDetail );
    
    /* Software configuration data */    
    
    /**
     * Adds a new client-info. Ownership is transferred
     */     
    void AddClientL( CNcdRequestConfigurationClient* aClient );
    
    // request generation methods,
    // used by request generator    
    TXmlEngElement GetConfigurationElementL(RXmlEngDocument& aDocument);
    TXmlEngElement GetCookiesElementL(RXmlEngDocument& aDocument);
    TXmlEngElement GetQueryResponseElementL(RXmlEngDocument& aDocument);
    TXmlEngElement GetNetworkConfigurationElementL(RXmlEngDocument& aDocument);
    
    /**
     * Number of client-elements
     */
    TInt ClientConfigurationCount() const;
    
    /**
     * Returns the client element for the given index
     */
    TXmlEngElement GetClientConfigurationElementL( RXmlEngDocument& aDocument,
        TInt aIndex );
  

public:
    // network configuration
    TNcdConfigurationNetwork iNetwork;

    TXmlEngString iQueryResponseId;

private:
    TXmlEngString iPrefix;
    
    TBool iNetworkConfigurationEnabled;
    TBool iSoftwareConfigurationEnabled;
    TBool iHardwareConfigurationEnabled;

    RPointerArray<CNcdRequestConfigurationClient> iClients;
    RPointerArray<MNcdConfigurationProtocolCookie> iCookies;
    RArray<TNcdConfigurationQueryResponse> iQueryResponses;
    
    };
           
    
#endif //C_NCD_REQUEST_CONFIGURATION_DATA_HH
