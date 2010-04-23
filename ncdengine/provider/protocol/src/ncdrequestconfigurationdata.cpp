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
* Description:   CNcdRequestConfigurationData implementation
*
*/


#include "ncdrequestconfigurationdata.h"
#include "ncdrequestconstants.h"
#include "ncdprotocolutils.h"
#include "ncdprotocolwords.h"
#include "ncd_cp_cookie.h"
#include "ncd_cp_detail.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"

void CloseStringArray( 
    RArray< TXmlEngString >& aArray )
    {
    TInt count = aArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        aArray[ i ].Free();
        }
    aArray.Close();
    }



TXmlEngElement GetDetailsElementL(
    RXmlEngDocument& aDocument, 
    const MNcdConfigurationProtocolDetail& aDetails,
    const TXmlEngString& aPrefix )
    {
    DLTRACEIN((""));
    TXmlEngElement detail = NcdProtocolUtils::NewElementL( 
        aDocument, KTagDetail, aPrefix );

    if ( aDetails.GroupId() != KNullDesC )
        NcdProtocolUtils::NewAttributeL( detail, KAttrGroupId, 
            aDetails.GroupId() );

    if ( aDetails.Label() != KNullDesC )
        NcdProtocolUtils::NewAttributeL( detail, KAttrLabel, 
            aDetails.Label() );
    
    if ( aDetails.Id() != KNullDesC )    
        NcdProtocolUtils::NewAttributeL( detail, KAttrId, 
            aDetails.Id() );

    if ( aDetails.Value() != KNullDesC )
        NcdProtocolUtils::NewAttributeL( detail, KAttrValue, 
            aDetails.Value() );
        
    for ( TInt j = 0; j < aDetails.Contents().Count(); ++j ) 
        {
        MNcdConfigurationProtocolContent* cont = aDetails.Contents()[j];
        TXmlEngElement content = NcdProtocolUtils::NewElementL( 
            aDocument, detail, KTagContent );
        NcdProtocolUtils::NewAttributeL( content, KAttrKey, cont->Key() );
        NcdProtocolUtils::NewAttributeL( content, KAttrValue, cont->Value() );
        }

    for ( TInt j = 0; j < aDetails.Details().Count(); ++j ) 
        {
        // get elements recursively
        TXmlEngElement child = GetDetailsElementL( aDocument, 
            *(aDetails.Details()[j]), aPrefix );
        detail.AppendChildL(child);
        }
    /*
    for ( TInt j = 0; j < aDetails.xmlFragment.Count(); ++j ) 
        {
        TNcdRequestCustomXmlFragment xmlFrag = aDetails.xmlFragment[j];
        TXmlEngElement xmlFragment = detail.AddNewElementL(
            NcdProtocolUtils::DesToStringL( KTagXmlFragment ) );
        }
    */
    DLTRACEOUT((""));
    return detail;
    }


/**
 * Software-element for client-elements
 */
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CNcdRequestConfigurationSoftware* CNcdRequestConfigurationSoftware::NewLC()
    {
    CNcdRequestConfigurationSoftware* self = new(ELeave) 
        CNcdRequestConfigurationSoftware;
    CleanupStack::PushL( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CNcdRequestConfigurationSoftware::~CNcdRequestConfigurationSoftware()
    {
    iSoftware.type.Free();
    iSoftware.version.Free();
    iSoftware.uid.Free();
    iSoftware.id.Free();
    iSoftware.ssid.Free();
    iSoftware.language.Free();    
    CloseStringArray( iSoftware.capabilities );
    iSoftware.details.ResetAndDestroy();
        
    }

void CNcdRequestConfigurationSoftware::SetSoftwareTypeL( 
    const TDesC& aType )
    {
    iSoftware.type.SetL(aType);
    }

void CNcdRequestConfigurationSoftware::SetSoftwareVersionL( 
    const TDesC& aVersion )
    {
    DLTRACEIN((""));
    iSoftware.version.SetL(aVersion);
    DLTRACEOUT((""));
    }

void CNcdRequestConfigurationSoftware::SetSoftwareIdsL( 
    const TDesC& aUid, const TDesC& aId, const TDesC8& aSsid )
    {
    if (aUid != KNullDesC)
        iSoftware.uid.SetL(aUid);
    if (aId != KNullDesC)
        iSoftware.id.SetL(aId);
    if (aSsid != KNullDesC8)
        iSoftware.ssid.SetL(aSsid);
    }

void CNcdRequestConfigurationSoftware::SetSoftwareLanguageL( 
    const TDesC& aLanguage )
    {
    iSoftware.language.SetL(aLanguage);
    }


void CNcdRequestConfigurationSoftware::AddSoftwareCapabilityL( 
    const TDesC& aCapability )
    {
    TXmlEngString capability;
    capability.SetL(aCapability);
    if ( iSoftware.capabilities.Append(capability) != KErrNone )
        {
        capability.Free();
        }
    }

void CNcdRequestConfigurationSoftware::AddSoftwareDetailsL( 
    MNcdConfigurationProtocolDetail* aDetail)
    {
    iSoftware.details.AppendL(aDetail);
    }


TXmlEngElement CNcdRequestConfigurationSoftware::GetSoftwareConfigurationElementL(
    RXmlEngDocument& aDocument, const TXmlEngString& aPrefix )
    {
    DLTRACEIN((""));
    TXmlEngElement software = NcdProtocolUtils::NewElementL( aDocument, 
        KTagSoftware, aPrefix );
        
    // software/@version, required
    NcdProtocolUtils::NewAttributeL(software, KAttrVersion, iSoftware.version);

    NcdProtocolUtils::NewAttributeL(software, KAttrType, iSoftware.type);

    if (iSoftware.uid.NotNull())
        NcdProtocolUtils::NewAttributeL(software, KAttrUid, iSoftware.uid);

    if (iSoftware.id.NotNull())
        NcdProtocolUtils::NewAttributeL(software, KAttrId, iSoftware.id);

    if (iSoftware.ssid.NotNull())
        NcdProtocolUtils::NewAttributeL(software, KAttrSsid, iSoftware.ssid);
        
    if ( iSoftware.language.NotNull() ) 
        {
        TXmlEngElement language = NcdProtocolUtils::NewElementL( aDocument, 
            software, KTagLanguage, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        language.SetTextL( iSoftware.language );
#else
        language.SetTextL( iSoftware.language.PtrC8() );
#endif
        }


    if ( iSoftware.capabilities.Count() > 0 ) 
        {
        TXmlEngElement capabilities = NcdProtocolUtils::NewElementL( 
            aDocument, software, KTagCapabilities, aPrefix );
            
        for (TInt i = 0; i < iSoftware.capabilities.Count(); ++i)
            {            
            TXmlEngElement capability = NcdProtocolUtils::NewElementL( 
                aDocument, capabilities, KTagCapability, aPrefix );

#ifndef RD_XML_ENGINE_API_CHANGE            
            capability.SetTextL( iSoftware.capabilities[i] );            
#else
            capability.SetTextL( iSoftware.capabilities[i].PtrC8() );
#endif            
            }
        }

    // details
    if ( iSoftware.details.Count() > 0 ) 
        {
        TXmlEngElement details = 
            NcdProtocolUtils::NewElementL( aDocument, software, 
                KTagDetails, aPrefix );
            
        for ( TInt i = 0; i < iSoftware.details.Count(); ++i ) 
            {
            MNcdConfigurationProtocolDetail* det = iSoftware.details[i];
            details.AppendChildL( GetDetailsElementL( aDocument, 
                *det, aPrefix ) );
            }
        }

    DLTRACEOUT((""));
    return software;
    }
    

/**
 * Hardware-element for client-elements
 */

CNcdRequestConfigurationHardware* CNcdRequestConfigurationHardware::NewL()
    {
    CNcdRequestConfigurationHardware* self = new(ELeave) 
        CNcdRequestConfigurationHardware;
    return self;
    }
    
    
CNcdRequestConfigurationHardware::~CNcdRequestConfigurationHardware()
    {
    // Clean iHardware
    iHardware.uaProfileUri.Free();
    iHardware.identification.Free();
    iHardware.manufacturer.Free();
    iHardware.model.Free();
    iHardware.language.Free();
    iHardware.platform.Free();
    iHardware.firmwareVersion.Free();
    iHardware.displays.ResetAndDestroy();
    iHardware.details.ResetAndDestroy();
    
    }


void CNcdRequestConfigurationHardware::SetHardwareUaProfileUriL( 
    const TDesC& aUaProfileUri )
    {
    iHardware.uaProfileUri.SetL(aUaProfileUri);
    }

void CNcdRequestConfigurationHardware::SetHardwareIdentificationL( 
    const TDesC& aIdentification )
    {
    iHardware.identification.SetL(aIdentification);
    }

void CNcdRequestConfigurationHardware::SetHardwareManufacturerL( 
    const TDesC& aManufacturer )
    {
    iHardware.manufacturer.SetL(aManufacturer);
    }

void CNcdRequestConfigurationHardware::SetHardwareModelL( 
    const TDesC& aModel )
    {
    iHardware.model.SetL(aModel);
    }

void CNcdRequestConfigurationHardware::SetHardwareLanguageL( 
    const TDesC& aLanguage )
    {
    iHardware.language.SetL(aLanguage);
    }

void CNcdRequestConfigurationHardware::SetHardwarePlatformL( 
    const TDesC& aPlatform )
    {
    iHardware.platform.SetL(aPlatform);
    }

void CNcdRequestConfigurationHardware::SetHardwareFirmwareL( 
    const TDesC& aFirmwareVersion )
    {
    iHardware.firmwareVersion.SetL(aFirmwareVersion);
    }

void CNcdRequestConfigurationHardware::AddHardwareDisplayL( 
    TInt aColors, TInt aHeight, TInt aWidth )
    {
    CNcdConfigurationDisplay* disp = new (ELeave) CNcdConfigurationDisplay;
    disp->iColors = aColors;
    disp->iHeight = aHeight;
    disp->iWidth = aWidth;
    if ( iHardware.displays.Append(disp) != KErrNone ) 
        {
        delete disp;
        }
    }
    

void CNcdRequestConfigurationHardware::AddHardwareDetailsL( 
    MNcdConfigurationProtocolDetail* aDetail )
    {
    iHardware.details.AppendL(aDetail);
    }


TXmlEngElement CNcdRequestConfigurationHardware::GetHardwareConfigurationElementL(
    RXmlEngDocument& aDocument, const TXmlEngString& aPrefix )
    {
    DLTRACEIN((""));
    TXmlEngElement hardware = NcdProtocolUtils::NewElementL( aDocument, 
        KTagHardware, aPrefix );
    
    if (iHardware.uaProfileUri.NotNull()) 
        {
        TXmlEngElement uaProfileUri = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagUaProfileUri, aPrefix );
            
#ifndef RD_XML_ENGINE_API_CHANGE
        uaProfileUri.SetTextL(iHardware.uaProfileUri);
#else
        uaProfileUri.SetTextL( iHardware.uaProfileUri.PtrC8() );
#endif
        }

    if (iHardware.identification.NotNull())
        {
        TXmlEngElement identification = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagIdentification, aPrefix );
            
#ifndef RD_XML_ENGINE_API_CHANGE
        identification.SetTextL(iHardware.identification);
#else
        identification.SetTextL( iHardware.identification.PtrC8() );
#endif
        }
        
    if (iHardware.manufacturer.NotNull())
        {
        TXmlEngElement manufacturer = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagManufacturer, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        manufacturer.SetTextL(iHardware.manufacturer);
#else
        manufacturer.SetTextL( iHardware.manufacturer.PtrC8() );
#endif
        }
    
    if (iHardware.model.NotNull())
        {
        TXmlEngElement model = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagModel, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        model.SetTextL(iHardware.model);
#else
        model.SetTextL( iHardware.model.PtrC8() );
#endif
        }

    if (iHardware.language.NotNull())
        {
        TXmlEngElement language = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagLanguage, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        language.SetTextL(iHardware.language);
#else
        language.SetTextL( iHardware.language.PtrC8() );
#endif
        }

    if (iHardware.platform.NotNull())
        {
        TXmlEngElement platform = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagPlatform, aPrefix );
            
#ifndef RD_XML_ENGINE_API_CHANGE
        platform.SetTextL(iHardware.platform);
#else
        platform.SetTextL( iHardware.platform.PtrC8() );
#endif
        }
        
    if (iHardware.firmwareVersion.NotNull())
        {
        TXmlEngElement firmwareVersion = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagFirmwareVersion, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        firmwareVersion.SetTextL(iHardware.firmwareVersion);
#else
        firmwareVersion.SetTextL( iHardware.firmwareVersion.PtrC8() );
#endif
        }
        
    for (TInt i = 0; i < iHardware.displays.Count(); ++i)
        {
        CNcdConfigurationDisplay* disp = iHardware.displays[i];
        TXmlEngElement display = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagDisplay, aPrefix );
        NcdProtocolUtils::NewAttributeL( display, KAttrWidth, disp->iWidth );
        NcdProtocolUtils::NewAttributeL( display, KAttrHeight, disp->iHeight );
        NcdProtocolUtils::NewAttributeL( display, KAttrColors, disp->iColors );
        }

    // details
    if ( iHardware.details.Count() > 0 ) 
        {
        TXmlEngElement details = NcdProtocolUtils::NewElementL( 
            aDocument, hardware, KTagDetails, aPrefix );
            
        for ( TInt i = 0; i < iHardware.details.Count(); ++i ) 
            {
            MNcdConfigurationProtocolDetail* det = iHardware.details[i];
            details.AppendChildL( GetDetailsElementL( aDocument, 
                *det, aPrefix ) );
            }
        }

    if (hardware.HasChildNodes())
        {
        DLTRACEOUT((""));
        return hardware;        
        }
    DLTRACEOUT(("return null"));
    return NULL;        
    }



/**
 * Client-element for configuration requests
 */
CNcdRequestConfigurationClient* CNcdRequestConfigurationClient::NewLC()
    {
    CNcdRequestConfigurationClient* self = new(ELeave) 
        CNcdRequestConfigurationClient;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
    
CNcdRequestConfigurationClient::~CNcdRequestConfigurationClient()
    {
    iSoftwares.ResetAndDestroy();
    delete iHardware;
    }
    

void CNcdRequestConfigurationClient::AddSoftwareL( 
    CNcdRequestConfigurationSoftware* aSoftware )
    {
    iSoftwares.AppendL( aSoftware );
    }
    
TInt CNcdRequestConfigurationClient::SoftwareCount() const
    {
    return iSoftwares.Count();
    }
    
    
CNcdRequestConfigurationSoftware& 
    CNcdRequestConfigurationClient::Software( TInt aIndex )
    {
    return *(iSoftwares[ aIndex ]);
    }
            
                  
CNcdRequestConfigurationHardware& CNcdRequestConfigurationClient::Hardware()
    {
    DASSERT( iHardware );
    return *iHardware;
    }
     

CNcdRequestConfigurationClient::CNcdRequestConfigurationClient()
    {
    }
    
    
void CNcdRequestConfigurationClient::ConstructL()
    {
    iHardware = CNcdRequestConfigurationHardware::NewL();
    }
    

// ---------------------------------------------------------------------------
// CNcdRequestConfigurationData
// ---------------------------------------------------------------------------
//

CNcdRequestConfigurationData* CNcdRequestConfigurationData::NewLC()
    {
    CNcdRequestConfigurationData* self = 
        new (ELeave) CNcdRequestConfigurationData();
    CleanupStack::PushL(self);

    return self;
    }


CNcdRequestConfigurationData::CNcdRequestConfigurationData()
    {
    }
    

CNcdRequestConfigurationData::~CNcdRequestConfigurationData()
    {
    DLTRACEIN((""));
    /*
    * @ TXmlEngString members need to be Freed!
    */
    
    iQueryResponseId.Free();
    iPrefix.Free();

    iCookies.ResetAndDestroy();
    ResetAndCloseArray( iQueryResponses );

    // Clean iNetwork
    iNetwork.mcc.Free();
    iNetwork.mnc.Free();
    iNetwork.currentMcc.Free();
    iNetwork.currentMnc.Free();
    iNetwork.gid1.Free();
    iNetwork.gid2.Free();
    iNetwork.smsc.Free();
    iNetwork.imsi.Free();
    iNetwork.msisdn.Free();
    iNetwork.cellId.Free();
    iNetwork.serviceProviderName.Free();
    iNetwork.operatorName.Free();
    iNetwork.details.ResetAndDestroy();

    iClients.ResetAndDestroy();
    DLTRACEOUT((""));
    }


void CNcdRequestConfigurationData::SetNamespacePrefixL(
    const TDesC8& aPrefix) 
    {
    if ( aPrefix == KNullDesC8 ) 
        {
        iPrefix.Free();
        iPrefix = TXmlEngString();
        }
    else 
        {
        iPrefix.SetL(aPrefix);
        }
    }

void CNcdRequestConfigurationData::AddCookieL(
    MNcdConfigurationProtocolCookie* aCookie) 
    {
    DLTRACEIN((""));
    iCookies.AppendL(aCookie);
    }

void CNcdRequestConfigurationData::AddQueryResponseL(
    TNcdConfigurationQueryResponse aQueryResponse) 
    {
    iQueryResponses.AppendL(aQueryResponse);
    }


void CNcdRequestConfigurationData::AddClientL(
    CNcdRequestConfigurationClient* aClient )
    {
    DLTRACEIN((""));
    iClients.AppendL( aClient );
    }

TXmlEngElement CNcdRequestConfigurationData::GetConfigurationElementL(
    RXmlEngDocument& aDocument)
    {
    DLTRACEIN((""));
    TXmlEngElement configRoot = NcdProtocolUtils::NewElementL( aDocument, KTagConfiguration );
    
    TXmlEngElement network = GetNetworkConfigurationElementL( aDocument );
    if (network.NotNull())
        configRoot.AppendChildL(network);
    
    DLTRACE(("Add %d client-infos", iClients.Count() ));
    for ( TInt i = 0; i < iClients.Count(); ++i )
        {        
        TXmlEngElement client = GetClientConfigurationElementL( aDocument, i );
        if ( client.NotNull() )
            configRoot.AppendChildL( client );
        }
        
    if (configRoot.HasChildNodes())
        {
        DLTRACEOUT((""));
        return configRoot;
        }
    DLTRACEOUT(("return null"));
    return NULL;
    }
    
    
TXmlEngElement CNcdRequestConfigurationData::GetCookiesElementL(
    RXmlEngDocument& aDocument)
    {
    DLTRACEIN((""));
    if (iCookies.Count() == 0) 
        {
        DLTRACEOUT(("no cookies"));
        return NULL;
        }
    
    TXmlEngElement cookies = NcdProtocolUtils::NewElementL( aDocument, KTagCookies, iPrefix );
    
    for (TInt i = 0; i < iCookies.Count(); ++i)
        {
        MNcdConfigurationProtocolCookie* cookie = iCookies[i];
        TXmlEngElement cookieElem = 
            NcdProtocolUtils::NewElementL( aDocument, cookies, KTagCookie, iPrefix );
        
        NcdProtocolUtils::NewAttributeL( cookieElem, KAttrKey,
            cookie->Key() );
        
        // These are not supposed to be sent to the server
        /*
        NcdProtocolUtils::NewAttributeL( cookieElem, KAttrType,
            cookie->Type() );
        NcdProtocolUtils::NewAttributeL( cookieElem, KAttrExpirationDelta, cookie->ExpirationDelta() );
        NcdProtocolUtils::NewAttributeL( cookieElem, KAttrScope,
            cookie->Scope() );
        */
        
        // Add cookie values    
        for ( TInt j = 0; j < cookie->ValueCount(); ++j ) 
            {
            TXmlEngElement value = 
                NcdProtocolUtils::NewElementL( aDocument, cookieElem, KTagValue, iPrefix );    
#ifndef RD_XML_ENGINE_API_CHANGE
                value.SetTextL( cookie->Value(j) );
#else
                HBufC8* tempBuf = NcdProtocolUtils::ConvertUnicodeToUtf8L( cookie->Value(j) );
                CleanupStack::PushL( tempBuf );
                value.SetTextL( *tempBuf );
                CleanupStack::PopAndDestroy( tempBuf );
#endif
            }
        }
    
    DLTRACEOUT((""));
    return cookies;
    }

    
TXmlEngElement CNcdRequestConfigurationData::GetQueryResponseElementL(
    RXmlEngDocument& aDocument)
    {
    DLTRACEIN((""));
    if (iQueryResponses.Count() == 0) 
        {
        DLTRACEOUT(("return null"));
        return NULL;
        }
        
    TXmlEngElement queryResponse = NcdProtocolUtils::NewElementL( aDocument, KTagQueryResponse, iPrefix );
    NcdProtocolUtils::NewAttributeL( queryResponse, KAttrId,
        iQueryResponseId );
    
    for (TInt i = 0; i < iQueryResponses.Count(); ++i)
        {
        TNcdConfigurationQueryResponse resp = iQueryResponses[i];
        TXmlEngElement response = NcdProtocolUtils::NewElementL( aDocument, queryResponse, KTagResponse, iPrefix );
        NcdProtocolUtils::NewAttributeL( response, KAttrId, resp.id);
        TXmlEngElement value = NcdProtocolUtils::NewElementL( aDocument, response, KTagValue, iPrefix );            
#ifndef RD_XML_ENGINE_API_CHANGE
        value.SetTextL(resp.semantics);            
#else
        value.SetTextL( resp.semantics.PtrC8() );
#endif
        }
    
    DLTRACEOUT((""));
    return queryResponse;
    }
    
TXmlEngElement CNcdRequestConfigurationData::GetNetworkConfigurationElementL(
    RXmlEngDocument& aDocument)
    {
    DLTRACEIN(( "" ));
    TXmlEngElement network = NcdProtocolUtils::NewElementL( aDocument, KTagNetwork );
            
    // network configuration
    if ( iNetwork.mcc.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrMcc, iNetwork.mcc );
    
    if ( iNetwork.mnc.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrMnc, iNetwork.mnc );
    
    if ( iNetwork.currentMcc.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrCurrentMcc, iNetwork.currentMcc );
    
    if ( iNetwork.currentMnc.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrCurrentMnc, iNetwork.currentMnc );
    
    if ( iNetwork.gid1.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrGid1, iNetwork.gid1 );
    
    if ( iNetwork.gid2.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrGid2, iNetwork.gid2 );
    
    if ( iNetwork.smsc.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrSmsc, iNetwork.smsc );
    
    if ( iNetwork.imsi.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( network, KAttrImsi, iNetwork.imsi );
        
    if ( iNetwork.serviceProviderName.NotNull() ) 
        {
        TXmlEngElement serviceProviderName = 
            NcdProtocolUtils::NewElementL( aDocument, network, KTagServiceProviderName, iPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        serviceProviderName.SetTextL( iNetwork.serviceProviderName );
#else
        serviceProviderName.SetTextL( iNetwork.serviceProviderName.PtrC8() );
#endif
        }

    if ( iNetwork.operatorName.NotNull() )
        {
        TXmlEngElement operatorName = 
            NcdProtocolUtils::NewElementL( aDocument, network, KTagOperatorName, iPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
        operatorName.SetTextL( iNetwork.operatorName );
#else
        operatorName.SetTextL( iNetwork.operatorName.PtrC8() );
#endif
        }

    if ( iNetwork.details.Count() > 0 ) 
        {
        TXmlEngElement details = 
            NcdProtocolUtils::NewElementL( aDocument, network, KTagDetails, iPrefix );
            
        for ( TInt i = 0; i < iNetwork.details.Count(); ++i ) 
            {
            MNcdConfigurationProtocolDetail* det = iNetwork.details[i];
            details.AppendChildL( GetDetailsElementL( aDocument, 
                *det, iPrefix ) );
            }
        }

    if (network.HasChildNodes()) 
        {
        DLTRACEOUT(( "" ));
        return network;
        }
    DLTRACEOUT(( "return null" ));
    return NULL;
    }
    
    
TInt CNcdRequestConfigurationData::ClientConfigurationCount() const
    {
    return iClients.Count();
    }


TXmlEngElement CNcdRequestConfigurationData::GetClientConfigurationElementL(
    RXmlEngDocument& aDocument, TInt aIndex )
    {
    DLTRACEIN((""));
    // client, required
    TXmlEngElement client = NcdProtocolUtils::NewElementL( aDocument, KTagClient );

    CNcdRequestConfigurationClient& confClient( *iClients[aIndex] );
    
    DLTRACE(("Adding %d software-elements", confClient.SoftwareCount() ));
    
    for ( TInt i = 0; i < confClient.SoftwareCount(); ++i ) 
        {        
        TXmlEngElement software = confClient.Software( 
            i ).GetSoftwareConfigurationElementL( aDocument, iPrefix );
        client.AppendChildL(software);
        }
    
    DLTRACE(("Adding hardware element"));
    TXmlEngElement hardware = 
        confClient.Hardware().GetHardwareConfigurationElementL( aDocument, 
        iPrefix );
        
    if (hardware.NotNull())
        client.AppendChildL(hardware);
    
    if (client.HasChildNodes())
        {
        DLTRACEOUT((""));
        return client;
        }
    DLTRACEOUT(("return null"));
    return NULL;
    }
    
    
    
void CNcdRequestConfigurationData::SetNetworkMccL( 
    const TDesC& aMcc, const TDesC& aCurrentMcc )
    {
    if (aMcc != KNullDesC)
        iNetwork.mcc.SetL(aMcc);
    if (aCurrentMcc != KNullDesC)
        iNetwork.currentMcc.SetL(aCurrentMcc);
    }
    
void CNcdRequestConfigurationData::SetNetworkMncL( 
    const TDesC& aMnc, const TDesC& aCurrentMnc )
    {
    if (aMnc != KNullDesC)
        iNetwork.mnc.SetL(aMnc);
    if (aCurrentMnc != KNullDesC)
        iNetwork.currentMnc.SetL(aCurrentMnc);
    }
    
void CNcdRequestConfigurationData::SetNetworkGidL( 
    const TDesC& aGid1, const TDesC& aGid2 )
    {
    if (aGid1 != KNullDesC)
        iNetwork.gid1.SetL(aGid1);
    if (aGid2 != KNullDesC)
        iNetwork.gid2.SetL(aGid2);
    }
    
void CNcdRequestConfigurationData::SetNetworkSmscL( 
    const TDesC& aSmsc )
    {
    iNetwork.smsc.SetL(aSmsc);
    }
    
void CNcdRequestConfigurationData::SetNetworkImsiL( 
    const TDesC& aImsi )
    {
    iNetwork.imsi.SetL(aImsi);
    }
    
void CNcdRequestConfigurationData::SetNetworkMsisdnL( 
    const TDesC& aMsisdn )
    {
    iNetwork.msisdn.SetL(aMsisdn);
    }
    
void CNcdRequestConfigurationData::SetNetworkCellIdL( 
    const TDesC& aCellId )
    {
    iNetwork.cellId.SetL(aCellId);
    }
    
void CNcdRequestConfigurationData::SetNetworkProviderL( 
    const TDesC& aServiceProviderName )
    {
    iNetwork.serviceProviderName.SetL(aServiceProviderName);
    }
    
void CNcdRequestConfigurationData::SetNetworkOperatorL( 
    const TDesC& aOperatorName )
    {
    iNetwork.operatorName.SetL(aOperatorName);
    }

void CNcdRequestConfigurationData::AddNetworkDetailsL( 
    MNcdConfigurationProtocolDetail* aDetail )
    {
    iNetwork.details.AppendL(aDetail);
    }


