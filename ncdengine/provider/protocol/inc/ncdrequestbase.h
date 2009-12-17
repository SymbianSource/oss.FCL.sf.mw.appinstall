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
* Description:   CNcdRequest declaration
*
*/


#ifndef CNCDREQUESTBASE_HH
#define CNCDREQUESTBASE_HH

#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include "ncdrequestconstants.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolutils.h"
#include "ncdprotocolwords.h"

class CNcdRequestConfigurationData;
class MNcdConfigurationProtocolDetail;
class MNcdConfigurationProtocolCookie;
class MNcdConfigurationProtocolQueryResponse;

class CNcdRequestBase : public CBase
    {

public:
    /**
     * Request body generation.
     * Returns NULL if all required fields are not set.
     * At least namespace must be set explicitly.
     * @return The request serialized into 8-bit buffer or NULL.
     */
    virtual HBufC8* CreateRequestL();
        
    /**
     * Configuration object getter.
     * The ownership is NOT transferred outside RequestBase
     * @return Configuration object.
     */
    CNcdRequestConfigurationData* Configuration();

    /**
     * Configuration object setter.
     * RequestBase takes ownership of the object.
     * @param aConfiguration Configuration object
     */
    void SetConfigurationL( 
        CNcdRequestConfigurationData* configuration);
    
    /**
     * Adds a query response to the request.
     * @param aResponse The response object to add.
     */
    void AddQueryResponseL(
        MNcdConfigurationProtocolQueryResponse* aResponse);

    
    /**
     * Adds cookie element to the request.
     * RequestBase takes ownership of the object.
     * @param aCookie Pointer to the cookie object
     */
    void AddCookieL(
        MNcdConfigurationProtocolCookie* aCookie);

    // response root element attribute setters
    void SetRequestIdL(TInt aId);
    void SetProtocolVersionL(const TDesC8& aVersion);
    void SetSessionL(const TDesC& aSession);
    void SetNamespaceL(const TDesC& aNamespace);

    HBufC* IdLC() const;
    HBufC* SessionLC() const;
    HBufC* NamespaceLC() const;
    
public:
    ~CNcdRequestBase();

protected:
    static CNcdRequestBase* NewL();
    static CNcdRequestBase* NewLC();

    CNcdRequestBase();

    virtual void ConstructL( const TDesC8& aRootName );

    TXmlEngElement GetCookiesElementL( const TDesC8& aPrefix );
    TXmlEngElement GetQueryResponsesElementL( const TDesC8& aPrefix );
    TXmlEngElement GetDetailsElementL( const TDesC8& aPrefix );

    HBufC8* ExternalizeL();

private:
    void InitializeDocumentL();

protected:
#ifdef RD_XML_ENGINE_API_CHANGE
    RXmlEngDOMImplementation iDomImplementation;
#endif
    RXmlEngDocument iDocument;
    TXmlEngElement  iRoot;

    TXmlEngNamespace iCdpNamespace;
    TXmlEngNamespace iXsNamespace;
    TXmlEngNamespace iXsiNamespace;

    // element used for the actual request
    TXmlEngElement  iRequestElement;

    // these can change according to the request type
    // (preminetRequest / configurationRequest)    
    TXmlEngString iId;
    TXmlEngString iVersion;
    HBufC8* iRootName;
    
    // response root attributes
    TXmlEngString iSession;
    TXmlEngString iNamespace;

    // owned: client and network configuration data
    CNcdRequestConfigurationData* iConfiguration;
    
private:
    
    TXmlEngString iPrefix;    
    
    RPointerArray<MNcdConfigurationProtocolCookie> iCookies;
    RPointerArray<MNcdConfigurationProtocolQueryResponse> iQueryResponses;
    };

#endif //CNCDREQUESTBASE_HH
    
