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
* Description:   CNcdRequestBrowseSearch implementation
*
*/


#include "ncdrequestconfiguration.h"
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "ncdprotocolutils.h"
#include "ncdprotocolwords.h"
#include "catalogsdebug.h"

CNcdRequestConfiguration* CNcdRequestConfiguration::NewL()
    {
    //DLTRACEIN(( _L("NewL")));
    CNcdRequestConfiguration* self = 
        CNcdRequestConfiguration::NewLC( );
    CleanupStack::Pop();
    //DLTRACEOUT(( _L("NewL")));
    return self;
    }

CNcdRequestConfiguration* CNcdRequestConfiguration::NewLC()
    {
    //DLTRACEIN(( _L("NewLC")));
    CNcdRequestConfiguration* self = 
        new (ELeave) CNcdRequestConfiguration();
    CleanupStack::PushL( self );
    self->ConstructL();
    //DLTRACEOUT(( _L("NewLC")));
    return self;
    }

void CNcdRequestConfiguration::ConstructL()
    {
    DLTRACEIN((""));
//    iRootName.SetL();
    CNcdRequestBase::ConstructL( KTagConfigurationRequest );
    iNamespaceUri.SetL( KCdpNamespaceUri );
    iPrefix.SetL( KNullDesC8 );
    iVersion.SetL( KConfigurationReqVersion );
    iType.SetL( KDefaultRequestType );
    DLTRACEOUT((""));
    }
    
CNcdRequestConfiguration::CNcdRequestConfiguration()
    : CNcdRequestBase()
    {
    }

CNcdRequestConfiguration::~CNcdRequestConfiguration()
    {
    DLTRACEIN((""));
    iType.Free();
    iNamespaceUri.Free();
    iPrefix.Free();
    delete iBundles;
    DLTRACEOUT((""));
    }

void CNcdRequestConfiguration::AddCatalogBundleRequestL( TDesC8& aBundleId )
    {
    if ( !iBundles ) 
        {
        iBundles = new (ELeave) CDesC8ArrayFlat(1);
        }
    iBundles->AppendL( aBundleId );
    }

// generates the dom nodes
HBufC8* CNcdRequestConfiguration::CreateRequestL()
    {
    // generate browse part of the request
    DLTRACEIN((_L("entry")));

    // override request root element name and namespace
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString configurationRequest;
    NcdProtocolUtils::DesToStringL( configurationRequest, KTagConfigurationRequest );
    iRoot = iDocument.CreateDocumentElementL( configurationRequest );
    configurationRequest.Free();
    
    TXmlEngString cdpUri;
    NcdProtocolUtils::DesToStringL( cdpUri, KCdpNamespaceUri );
    TXmlEngString cdpPrefix;
    NcdProtocolUtils::DesToStringL( cdpPrefix, KCdpNamespacePrefix);
    iRoot.SetDefaultNamespaceL( cdpUri );
    iCdpNamespace = iRoot.AddNamespaceDeclarationL( cdpUri, cdpPrefix );
    cdpUri.Free();
    cdpPrefix.Free();

    TXmlEngString xsUri;
    TXmlEngString xsPrefix;
    NcdProtocolUtils::DesToStringL( xsUri, KXsNamespaceUri );
    NcdProtocolUtils::DesToStringL( xsPrefix, KXsNamespacePrefix );
    iXsNamespace = iRoot.AddNamespaceDeclarationL( xsUri, xsPrefix );
    xsUri.Free();
    xsPrefix.Free();
    
    TXmlEngString xsiUri;
    TXmlEngString xsiPrefix;
    NcdProtocolUtils::DesToStringL( xsiUri, KXsiNamespaceUri );
    NcdProtocolUtils::DesToStringL( xsiPrefix, KXsiNamespacePrefix );
    iXsiNamespace = iRoot.AddNamespaceDeclarationL( xsiUri, xsiPrefix );
    xsiUri.Free();
    xsiPrefix.Free();
    
#else
    iRoot = iDocument.CreateDocumentElementL( KTagConfigurationRequest );
    iRoot.SetDefaultNamespaceL( KCdpNamespaceUri );
    iCdpNamespace = iRoot.AddNamespaceDeclarationL( KCdpNamespaceUri, KCdpNamespacePrefix );
    iXsNamespace = iRoot.AddNamespaceDeclarationL( KXsNamespaceUri, KXsNamespacePrefix );
    iXsiNamespace = iRoot.AddNamespaceDeclarationL( KXsiNamespaceUri, KXsiNamespacePrefix );
#endif

    NcdProtocolUtils::NewAttributeL( iRoot, KAttrVersion, iVersion);
    NcdProtocolUtils::NewAttributeL( iRoot, KAttrType, iType);
    
    // no request if there is no configuration data
    if ( !iConfiguration )
        {
        DLTRACEOUT(( "exit with null" ));
        return NULL;
        }
    
    // prefix not needed in configuration request
    iConfiguration->SetNamespacePrefixL( KNullDesC8 );

    // network    
    TXmlEngElement network = 
        iConfiguration->GetNetworkConfigurationElementL( iDocument );
    if ( network.NotNull() )
        iRoot.AppendChildL( network );
    
    DLTRACE(("Adding %d client-infos", 
        iConfiguration->ClientConfigurationCount() ));
        
    for ( TInt clientIndex = 0; 
          clientIndex < iConfiguration->ClientConfigurationCount(); 
          ++clientIndex )
        {        
        // client
        TXmlEngElement client = 
            iConfiguration->GetClientConfigurationElementL( 
                iDocument, clientIndex );
                
        if ( client.NotNull() )
            iRoot.AppendChildL( client );
        }
        
    // cookies
    TXmlEngElement cookies = iConfiguration->GetCookiesElementL( iDocument );
    if ( cookies.NotNull() )
        iRoot.AppendChildL( cookies );
    

    // queryResponse
    TXmlEngElement queryResponses = GetQueryResponsesElementL( KNullDesC8 );
    if ( queryResponses.NotNull() )
        iRoot.AppendChildL( queryResponses );

    // details
    TXmlEngElement details = GetDetailsElementL( KNullDesC8 );
    if ( details.NotNull() ) 
        iRoot.AppendChildL( details );
    // catalog bundle requests
    if ( iBundles && iBundles->MdcaCount() > 0 ) 
        {
        if ( details.IsNull() ) 
            {
            details = 
                NcdProtocolUtils::NewElementL( iDocument, iRoot, KTagDetails );
            }
        for ( TInt i = 0; i < iBundles->MdcaCount(); ++i ) 
            {
            TPtrC8 ptr = iBundles->MdcaPoint( i );
            TXmlEngElement detail = 
                NcdProtocolUtils::NewElementL( iDocument, details, KTagDetail );
            NcdProtocolUtils::NewAttributeL( detail, KAttrId, KValueCatalogRequest );
            NcdProtocolUtils::NewAttributeL( detail, KAttrValue, ptr );
            }
        }
    
    DLTRACEOUT((_L("exit")));
    // base class CreateRequestL not needed
    return CNcdRequestBase::ExternalizeL();
    }
    
