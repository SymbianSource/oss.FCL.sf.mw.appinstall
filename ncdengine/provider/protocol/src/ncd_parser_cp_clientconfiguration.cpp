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
* Description:   CNcdConfigurationProtocolClientConfigurationParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"

#include "ncd_parser_cp_clientconfiguration.h"
#include "ncd_parser_cp_cookie.h"
#include "ncd_parser_cp_details.h"
#include "ncd_cp_clientconfigurationimpl.h"

#include "catalogsdebug.h"

CNcdConfigurationProtocolClientConfigurationParser* 
CNcdConfigurationProtocolClientConfigurationParser::NewL( MNcdParserObserverBundle& aObservers,
                                                          MNcdSubParserObserver& aSubParserObserver,
                                                          TInt aDepth,
                                                          const Xml::RTagInfo& aElement,
                                                          const Xml::RAttributeArray& aAttributes,
                                                          const TDesC& aParentId )
    {
    CNcdConfigurationProtocolClientConfigurationParser* self 
        = new(ELeave) CNcdConfigurationProtocolClientConfigurationParser( aObservers,
                                                                          aSubParserObserver,
                                                                          aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }


CNcdConfigurationProtocolClientConfigurationParser* 
CNcdConfigurationProtocolClientConfigurationParser::NewL( MNcdParserObserverBundle& aObservers,
                                                          MNcdSubParserObserver& aSubParserObserver,
                                                          TInt aDepth )
    {
    CNcdConfigurationProtocolClientConfigurationParser* self 
        = new(ELeave) CNcdConfigurationProtocolClientConfigurationParser( aObservers,
                                                                          aSubParserObserver,
                                                                          aDepth );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolClientConfigurationParser::CNcdConfigurationProtocolClientConfigurationParser( 
                                                                            MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolClientConfigurationParser::~CNcdConfigurationProtocolClientConfigurationParser()
    {
    delete iClientConfiguration;
    iClientConfiguration = 0;
    }

void CNcdConfigurationProtocolClientConfigurationParser::ConstructL( const Xml::RTagInfo& aElement,
                                                                     const Xml::RAttributeArray& aAttributes,
                                                                     const TDesC& aParentId )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    DLINFO((_L("parent=%S"),&aParentId));
    (void) aParentId; // suppresses compiler warning

    iClientConfiguration = CNcdConfigurationProtocolClientConfigurationImpl::NewL();

    // attributes

    // expirationDelta
    TPtrC8 expDelta = AttributeValue( KAttrConfExpirationDelta, aAttributes );
    if( expDelta != KNullDesC8 )
        {
        iClientConfiguration->iExpirationDelta = NcdProtocolUtils::DesDecToIntL( expDelta );
        }
    }


void CNcdConfigurationProtocolClientConfigurationParser::ConstructL()
    {
    DLTRACEIN((""));    

    iClientConfiguration = CNcdConfigurationProtocolClientConfigurationImpl::NewL();

    }


void CNcdConfigurationProtocolClientConfigurationParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                                          const Xml::RAttributeArray& aAttributes, 
                                                                          TInt aErrorCode) 
    {
    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
        
        TPtrC8 tag( aElement.LocalName().DesC() );
        DLTRACE(("client configuration start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

        if( tag == KTagCookies )
            {
            iSubParser = CNcdConfigurationProtocolCookieParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagDetails )
            {
            iSubParser = CNcdConfigurationProtocolDetailsParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    }


void CNcdConfigurationProtocolClientConfigurationParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }

    else if( iSubParser )
        {
        iSubParser->OnEndElementL( aElement, aErrorCode );
        }
    
    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    }

void CNcdConfigurationProtocolClientConfigurationParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                             TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));


    if( aTag == KTagCookies && iSubParser )
        {
        CNcdConfigurationProtocolCookieParser* cookieParser = 
            static_cast<CNcdConfigurationProtocolCookieParser*>( iSubParser );
        iClientConfiguration->iCookies = cookieParser->Cookies();
        }
    else if( aTag == KTagDetails && iSubParser )
        {
        CNcdConfigurationProtocolDetailsParser* detailParser = 
            static_cast<CNcdConfigurationProtocolDetailsParser*>( iSubParser );
        iClientConfiguration->iDetails = detailParser->Details();
        }
    
    delete iSubParser;
    iSubParser = 0;
    }


MNcdConfigurationProtocolClientConfiguration* CNcdConfigurationProtocolClientConfigurationParser::ClientConfiguration()
    {
    MNcdConfigurationProtocolClientConfiguration* returnPtr = iClientConfiguration;
    iClientConfiguration = 0;
    return returnPtr;
    }

