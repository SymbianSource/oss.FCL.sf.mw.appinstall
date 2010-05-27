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

#include "ncd_parser_pp_information.h"
#include "ncd_parser_cp_cookie.h"
#include "ncd_parser_pp_messages.h"
#include "ncd_parser_cp_details.h"
#include "ncd_parser_pp_expiredcacheddata.h"
#include "ncd_parser_cp_serverdetails.h"
#include "ncd_pp_informationimpl.h"

#include "catalogsdebug.h"

CNcdPreminetProtocolInformationParser* 
CNcdPreminetProtocolInformationParser::NewL( MNcdParserObserverBundle& aObservers,
                                             MNcdSubParserObserver& aSubParserObserver,
                                             TInt aDepth,
                                             const Xml::RTagInfo& aElement,
                                             const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolInformationParser* self 
        = new(ELeave) CNcdPreminetProtocolInformationParser( aObservers,
                                                             aSubParserObserver,
                                                             aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolInformationParser::CNcdPreminetProtocolInformationParser( 
                                                                            MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolInformationParser::~CNcdPreminetProtocolInformationParser()
    {
    delete iInformation;
    iInformation = 0;
    }

void CNcdPreminetProtocolInformationParser::ConstructL( 
    const Xml::RTagInfo& aElement, const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    iInformation = new(ELeave) CNcdPreminetProtocolInformationImpl();
    iInformation->ConstructL();

    }

void CNcdPreminetProtocolInformationParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                             const Xml::RAttributeArray& aAttributes, 
                                                             TInt aErrorCode) 
    {
    DLTRACE(("start tag=%S",&aElement.LocalName().DesC()));
    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
        
        TPtrC8 tag( aElement.LocalName().DesC() );
        
        if( tag == KTagCookies )
            {
            iSubParser = CNcdConfigurationProtocolCookieParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagMessages )
            {
            iSubParser = CNcdPreminetProtocolMessagesParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagDetails )
            {
            iSubParser = CNcdConfigurationProtocolDetailsParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }

        else if( tag == KTagResendAfter )
            {
            TPtrC8 resendAfter = AttributeValue( KAttrSeconds, aAttributes );
            if( resendAfter != KNullDesC8 )
                {
                iInformation->iResendAfter = 
                    NcdProtocolUtils::DesDecToIntL( resendAfter );
                }
            }

        else if( tag == KTagServerDetails )
            {
            iSubParser = CNcdConfigurationProtocolServerDetailsParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagExpiredCachedData )
            {
            iSubParser = CNcdPreminetProtocolExpiredCachedDataParser::NewL(
                    *iObservers, *this, iDepth+1, aElement, aAttributes );
            }        
        
        else
            {
            iSubParser = CNcdUnknownParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    }


void CNcdPreminetProtocolInformationParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
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

void CNcdPreminetProtocolInformationParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));


    if( aTag == KTagCookies && iSubParser )
        {
        CNcdConfigurationProtocolCookieParser* cookieParser = 
            static_cast<CNcdConfigurationProtocolCookieParser*>( iSubParser );
        iInformation->iCookies = cookieParser->Cookies();
        }
    else if( aTag == KTagMessages && iSubParser )
        {
        CNcdPreminetProtocolMessagesParser* messageParser = 
            static_cast<CNcdPreminetProtocolMessagesParser*>( iSubParser );
        iInformation->iMessages = messageParser->Messages();
        }
    else if( aTag == KTagDetails && iSubParser )
        {
        CNcdConfigurationProtocolDetailsParser* detailsParser = 
            static_cast<CNcdConfigurationProtocolDetailsParser*>( iSubParser );
        iInformation->iDetails = detailsParser->Details();
        }
    else if( aTag == KTagExpiredCachedData && iSubParser )
        {
        CNcdPreminetProtocolExpiredCachedDataParser* expiredCachedDataParser = 
            static_cast<CNcdPreminetProtocolExpiredCachedDataParser*>( iSubParser );
        iInformation->iExpiredCachedData = expiredCachedDataParser->ExpiredCachedData();
        }
    else if( aTag == KTagServerDetails && iSubParser )
        {
        DLTRACE(("Server details parsed"));
        CNcdConfigurationProtocolServerDetailsParser* serverDetailsParser = 
            static_cast<CNcdConfigurationProtocolServerDetailsParser*>( iSubParser );
        iInformation->iServerDetails = serverDetailsParser->ServerDetails();
        }
    
    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolInformation* CNcdPreminetProtocolInformationParser::Information()
    {
    MNcdPreminetProtocolInformation* returnPtr = iInformation;
    iInformation = 0;
    return returnPtr;
    }

