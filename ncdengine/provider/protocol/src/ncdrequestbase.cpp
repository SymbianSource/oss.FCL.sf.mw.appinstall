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
* Description:   CNcdRequestBase declaration
*
*/

 
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_cp_detail.h"
#include "ncd_cp_cookie.h"
#include "ncd_cp_queryresponse.h"
#include "ncdqueryitem.h"
#include "catalogsdebug.h"


CNcdRequestBase* CNcdRequestBase::NewL()
    {    
    CNcdRequestBase* self = 
        CNcdRequestBase::NewLC();
    CleanupStack::Pop();    
    return self;
    }

CNcdRequestBase* CNcdRequestBase::NewLC()
    {    
    CNcdRequestBase* self = new (ELeave) CNcdRequestBase();
    CleanupStack::PushL(self);
    self->ConstructL( KTagPreminetRequest );
    return self;
    }

void CNcdRequestBase::ConstructL( const TDesC8& aRootName ) 
    {
    DLTRACEIN(( "aRootName=%S", &aRootName ));
    iRootName = aRootName.AllocL();
    
    iPrefix = TXmlEngString();
    iId = TXmlEngString();
    iNamespace = TXmlEngString();
    iVersion = TXmlEngString();
    iSession = TXmlEngString();
    
#ifdef RD_XML_ENGINE_API_CHANGE
    iDomImplementation.OpenL();
#endif
    InitializeDocumentL();
    DLTRACEOUT((""));
    }

CNcdRequestBase::CNcdRequestBase()
    {
    }
    
CNcdRequestBase::~CNcdRequestBase()
    {
    DLTRACEIN((""));
    delete iConfiguration;
    
    iCookies.ResetAndDestroy();
    iQueryResponses.ResetAndDestroy();
      
    iDocument.Close();    // NOTE: leaks memory (S60 3.2 wk38 emulator)
#ifdef RD_XML_ENGINE_API_CHANGE
    iDomImplementation.Close();
#endif

    iId.Free();
    iVersion.Free();
    iSession.Free();
    iNamespace.Free();
    iPrefix.Free();

    delete iRootName;
    DLTRACEOUT((""));
    }
    
HBufC8* CNcdRequestBase::ExternalizeL() 
    {
    DLTRACEIN((""));
    
    const TXmlEngSerializationOptions KSerializationOptions = 
        TXmlEngSerializationOptions( 0x09, KSerializationOptionUtf8 );
    
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString str;
    iDocument.SaveL(str, iRoot, KSerializationOptions);
    
    str.PushL();
    
    HBufC8* buf = HBufC8::NewL( str.Size() );
    *buf = str.PtrC8();
   
    // Free the original C-string
    CleanupStack::PopAndDestroy();        
#else
    
    RBuf8 rbuf;
    iDocument.SaveL( rbuf, iRoot, KSerializationOptions );

    CleanupClosePushL( rbuf );
    HBufC8* buf = rbuf.AllocL();
    CleanupStack::PopAndDestroy();  // rbuf

#endif
    
    DLTRACEOUT((""));
    return buf;
    }
    
HBufC8* CNcdRequestBase::CreateRequestL() 
    {
    DLTRACEIN((""));
    // check required attributes
    if (iNamespace.IsNull() || iVersion.IsNull()) 
        {
        DLERROR(("not creating request"));
        DASSERT( 0 );
        // do not create request if required attributes are not set
        DLTRACEOUT(( "NULL" ));
        return NULL;
        }
        
    // namespace, required
    NcdProtocolUtils::NewAttributeL( iRoot, KAttrNamespace, iNamespace);
    
    // id, optional
    if (iId.NotNull()) 
        {
        NcdProtocolUtils::NewAttributeL( iRoot, KAttrId, iId );
        }
    
    // version, required
    NcdProtocolUtils::NewAttributeL( iRoot, KAttrVersion, iVersion);
    
    // session, optional
    if (iSession.NotNull())
        {
        NcdProtocolUtils::NewAttributeL( iRoot, KAttrSession, iSession);
        }

    // configuration, optional    
    if (iConfiguration) 
        {
        TXmlEngElement configuration = 
            iConfiguration->GetConfigurationElementL(iDocument);
        if (configuration.NotNull())
            iRoot.AppendChildL(configuration);
                
        TXmlEngElement cookies = 
            iConfiguration->GetCookiesElementL(iDocument);
        if (cookies.NotNull())
            iRoot.AppendChildL(cookies);
        
        TXmlEngElement queryResponse = 
            iConfiguration->GetQueryResponseElementL(iDocument);
        if (queryResponse.NotNull())
            iRoot.AppendChildL(queryResponse);
        }
    
    // cookies, optional
    TXmlEngElement cookies = GetCookiesElementL(KCdpNamespacePrefix);
    if ( cookies.NotNull() ) 
        {
        iRoot.AppendChildL(cookies);
        }
    
    TXmlEngElement queryResponses = GetQueryResponsesElementL(KCdpNamespacePrefix);
    if ( queryResponses.NotNull() ) 
        {
        iRoot.AppendChildL(queryResponses);
        }        

    // add element generated in the inheriting class, one of the following:
    // browse / search / manageSubscriptions / purchase / 
    //   installationReport / diffQuery
    iRoot.AppendChildL(iRequestElement);
    
        
    HBufC8* result = ExternalizeL();

    DLTRACEOUT((""));
    return result;
    }
    
void CNcdRequestBase::InitializeDocumentL() 
    {
    DLTRACEIN((""));
    if (iDocument.NotNull()) 
        {
        iDocument.Close(); // ! this leaks!
        }

#ifndef RD_XML_ENGINE_API_CHANGE
    iDocument = RXmlEngDocument::NewL();
#else
    iDocument.OpenL( iDomImplementation );
#endif
    iRoot = iDocument.CreateDocumentElementL(*iRootName);
    
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString defNs;
    NcdProtocolUtils::DesToStringL(defNs, KDefaultNamespaceUri);
    iRoot.SetDefaultNamespaceL( defNs );
    defNs.Free();

    TXmlEngString cdpNs;
    NcdProtocolUtils::DesToStringL(cdpNs, KCdpNamespaceUri);
    TXmlEngString cdpNsPrefix;
    NcdProtocolUtils::DesToStringL(cdpNsPrefix, KCdpNamespacePrefix);
    iCdpNamespace = iRoot.AddNamespaceDeclarationL( cdpNs, cdpNsPrefix );
    cdpNs.Free();
    cdpNsPrefix.Free();
    
    TXmlEngString xsNs;
    NcdProtocolUtils::DesToStringL(xsNs, KXsNamespaceUri);
    TXmlEngString xsNsPrefix;
    NcdProtocolUtils::DesToStringL(xsNsPrefix, KXsNamespacePrefix);
    iXsNamespace = iRoot.AddNamespaceDeclarationL(xsNs, xsNsPrefix);
    xsNs.Free();
    xsNsPrefix.Free();
    
    TXmlEngString xsiNs;
    NcdProtocolUtils::DesToStringL(xsiNs, KXsiNamespaceUri);
    TXmlEngString xsiNsPrefix;
    NcdProtocolUtils::DesToStringL(xsiNsPrefix, KXsiNamespacePrefix);
    iXsiNamespace = iRoot.AddNamespaceDeclarationL(xsiNs, xsiNsPrefix);
    xsiNs.Free();
    xsiNsPrefix.Free();

#else
    iRoot.SetDefaultNamespaceL( KDefaultNamespaceUri );
    iCdpNamespace = iRoot.AddNamespaceDeclarationL( KCdpNamespaceUri, KCdpNamespacePrefix );
    iXsNamespace = iRoot.AddNamespaceDeclarationL( KXsNamespaceUri, KXsNamespacePrefix );
    iXsiNamespace = iRoot.AddNamespaceDeclarationL( KXsiNamespaceUri, KXsiNamespacePrefix );
#endif

    DLTRACEOUT((""));
    }
    
CNcdRequestConfigurationData* CNcdRequestBase::Configuration() 
    {
    return iConfiguration;
    }
    
void CNcdRequestBase::SetConfigurationL(
    CNcdRequestConfigurationData* aConfiguration) 
    {
    DLTRACEIN((""));
    delete iConfiguration;
    iConfiguration = aConfiguration;
    }
    
void CNcdRequestBase::SetRequestIdL(TInt aId) 
    {
    TBuf<32> buf;
    buf.AppendNum(aId);
    TXmlEngString id;
    id.SetL(buf);
    iId = id;
    }
    
void CNcdRequestBase::SetProtocolVersionL(const TDesC8& aVersion)
    {
    iVersion.SetL(aVersion);
    }
    
void CNcdRequestBase::SetSessionL(const TDesC& aSession)
    {
    iSession.SetL(aSession);
    }
    
void CNcdRequestBase::SetNamespaceL( const TDesC& aNamespace)
    {
    iNamespace.SetL(aNamespace);
    }
    
HBufC* CNcdRequestBase::IdLC() const
    {    
    return iId.AllocLC();
    }

HBufC* CNcdRequestBase::SessionLC() const
    {
    return iSession.AllocLC();;
    }

HBufC* CNcdRequestBase::NamespaceLC() const
    {
    return iNamespace.AllocLC();    
    }

void CNcdRequestBase::AddQueryResponseL(MNcdConfigurationProtocolQueryResponse* aResponse)
    {
    iQueryResponses.AppendL(aResponse);
    }

void CNcdRequestBase::AddCookieL(MNcdConfigurationProtocolCookie* aCookie)
    {
    iCookies.AppendL(aCookie);
    }

TXmlEngElement CNcdRequestBase::GetCookiesElementL( const TDesC8& aPrefix ) 
    {
    TXmlEngElement cookies;
    
    if ( iCookies.Count() > 0 ) 
        {
        cookies = NcdProtocolUtils::NewElementL( iDocument, iRoot, KTagCookies );
        for ( TInt i = 0; i < iCookies.Count(); ++i ) 
            {
            MNcdConfigurationProtocolCookie* cook = iCookies[i];
            TXmlEngElement cookie = 
                NcdProtocolUtils::NewElementL( iDocument, cookies, KTagCookie );
            NcdProtocolUtils::NewAttributeL( cookie, KAttrKey, cook->Key() );
            for ( TInt j = 0; j < cook->ValueCount(); ++j ) 
                {
                TXmlEngElement value = 
                    NcdProtocolUtils::NewElementL( iDocument, cookie, KTagValue, aPrefix );
#ifndef RD_XML_ENGINE_API_CHANGE
                value.SetTextL( cook->Value(j) );
#else
                HBufC8* tempBuf = NcdProtocolUtils::ConvertUnicodeToUtf8L( cook->Value(j) );
                CleanupStack::PushL( tempBuf );
                value.SetTextL( *tempBuf );
                CleanupStack::PopAndDestroy( tempBuf );
#endif
                }
            }
        }
    return cookies;
    }

TXmlEngElement CNcdRequestBase::GetQueryResponsesElementL( const TDesC8& aPrefix ) 
    {
    TXmlEngElement queryResponse;
    // queryResponses, optional
    if ( iQueryResponses.Count() > 0 )
        {
        for (TInt i = 0; i < iQueryResponses.Count(); ++i)
            {
            MNcdConfigurationProtocolQueryResponse* resp = iQueryResponses[i];
            queryResponse = NcdProtocolUtils::NewElementL(iDocument, iRoot, KTagQueryResponse);
            if ( resp->Id() != KNullDesC ) 
                NcdProtocolUtils::NewAttributeL(queryResponse, KAttrId, resp->Id());
            if ( resp->Semantics() != MNcdQueryItem::ESemanticsNone )
                NcdProtocolUtils::NewAttributeL(queryResponse, KAttrSemantics, resp->Semantics());
            if ( resp->Cancel() )
                NcdProtocolUtils::NewBoolAttributeL(queryResponse, KAttrCancel, resp->Cancel());

            for (TInt j = 0; j < resp->ResponseCount(); ++j)
                {
                const MNcdConfigurationProtocolQueryResponseValue& val = resp->ResponseL(j);
                TXmlEngElement response = NcdProtocolUtils::NewElementL(
                    iDocument, queryResponse, KTagResponse, aPrefix);
                NcdProtocolUtils::NewAttributeL(response, KAttrId, val.Id());
                NcdProtocolUtils::NewAttributeL(response, KAttrSemantics, val.Semantics());
                for (TInt k = 0; k < val.ValueCount(); ++k) 
                    {
                    TXmlEngElement value = NcdProtocolUtils::NewElementL(
                        iDocument, response, KTagValue, aPrefix);
#ifndef RD_XML_ENGINE_API_CHANGE
                        value.SetTextL( val.ValueL(k) );
#else
                        HBufC8* tempBuf = NcdProtocolUtils::ConvertUnicodeToUtf8L( val.ValueL(k) );
                        CleanupStack::PushL( tempBuf );
                        value.SetTextL( *tempBuf );
                        CleanupStack::PopAndDestroy( tempBuf );
#endif
                    }
                }
            }
        }
    return queryResponse;
    }
    

TXmlEngElement CNcdRequestBase::GetDetailsElementL( const TDesC8& /*aPrefix*/ ) 
    {
    DLTRACEIN((""));
    return TXmlEngElement();
    }
