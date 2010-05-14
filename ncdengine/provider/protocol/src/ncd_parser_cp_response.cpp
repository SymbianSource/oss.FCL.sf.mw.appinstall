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
* Description:  
*
*/


#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdunknownparser.h"
#include "ncdparserobserver.h"

#include "ncd_parser_cp_response.h"
#include "ncd_parser_cp_clientconfiguration.h"
#include "ncd_parser_cp_details.h"
#include "ncd_parser_cp_query.h"
#include "ncd_parser_cp_actionrequest.h"
#include "ncd_parser_cp_error.h"
#include "ncd_parser_cp_serverdetails.h"
#include "ncd_cp_query.h"

#include "catalogsdebug.h"


CNcdConfigurationProtocolResponseParser* 
CNcdConfigurationProtocolResponseParser::NewL( MNcdParserObserverBundle& aObservers,
                                               MNcdSubParserObserver& aSubParserObserver,
                                               TInt aDepth,
                                               const Xml::RTagInfo& aElement, 
                                               const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolResponseParser* self 
        = new(ELeave) CNcdConfigurationProtocolResponseParser( aObservers,
                                                               aSubParserObserver,
                                                               aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolResponseParser::CNcdConfigurationProtocolResponseParser( MNcdParserObserverBundle& aObservers,
                                                                                  MNcdSubParserObserver& aSubParserObserver,
                                                                                  TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolResponseParser::~CNcdConfigurationProtocolResponseParser()
    {
    delete iSubParser;
    iSubParser = 0;
    }

void CNcdConfigurationProtocolResponseParser::ConstructL( const Xml::RTagInfo& aElement, 
                                                          const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );

    // attributes

    // version
    HBufC* version = NcdProtocolUtils::ConvertUtf8ToUnicodeL( AttributeValue( KAttrVersion, aAttributes ) );
    CleanupStack::PushL( version );

    // expirationDelta
    TPtrC8 expirationDeltaDes = AttributeValue( KAttrExpirationDelta, aAttributes );
    TInt expirationDelta = 0;
    if( expirationDeltaDes != KNullDesC8 )
        {
        expirationDelta = NcdProtocolUtils::DesDecToIntL( expirationDeltaDes );
        }

    MNcdParserConfigurationProtocolObserver* observer = iObservers->ConfigurationProtocolObserver();
    if( observer )
        {
        observer->ConfigurationBeginL( *version, expirationDelta );
        }
    CleanupStack::PopAndDestroy( version );
    }


void CNcdConfigurationProtocolResponseParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                               const Xml::RAttributeArray& aAttributes, 
                                                               TInt aErrorCode) 
    {

    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("configuration start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

        if( tag == KTagClientConfiguration )
            {
            iSubParser = CNcdConfigurationProtocolClientConfigurationParser::NewL( *iObservers, *this, 
                                                                                   iDepth+1, aElement, aAttributes,
                                                                                   KNullDesC);
            }
        else if ( tag == KTagMessage )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( *iObservers, *this, 
                                                                iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagDetails )
            {
            iSubParser = CNcdConfigurationProtocolDetailsParser::NewL( *iObservers, *this, 
                                                                       iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagActionRequest )
            {
            iSubParser = CNcdConfigurationProtocolActionRequestParser::NewL( *iObservers, *this, 
                                                                             iDepth+1, aElement, aAttributes );
            }        
        else if( tag == KTagError )
            {
            iSubParser = CNcdConfigurationProtocolErrorParser::NewL( *iObservers, *this, 
                                                                     iDepth+1, aElement, aAttributes );
            }                
        else if( tag == KTagServerDetails )
            {
            iSubParser = CNcdConfigurationProtocolServerDetailsParser::NewL( *iObservers, *this, 
                                                                             iDepth+1, aElement, aAttributes );
            }
        
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    else
        {
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    }



void CNcdConfigurationProtocolResponseParser::OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode )
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


void CNcdConfigurationProtocolResponseParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                  TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));

    MNcdParserConfigurationProtocolObserver* observer = 
        iObservers->ConfigurationProtocolObserver();
    
    if( aTag == KTagClientConfiguration && observer )
        {
        MNcdConfigurationProtocolClientConfiguration* clientConfiguration =
            static_cast<CNcdConfigurationProtocolClientConfigurationParser*>( iSubParser )->ClientConfiguration();
        observer->ClientConfigurationL( clientConfiguration );
        }

    else if( aTag == KTagDetails && observer )
        {
        CArrayPtr<MNcdConfigurationProtocolDetail>* details =
            static_cast<CNcdConfigurationProtocolDetailsParser*>( iSubParser )->Details();
        observer->ConfigurationDetailsL( details );
        }

    else if( aTag == KTagMessage && observer )
        {
        MNcdConfigurationProtocolQuery* query =
            static_cast<CNcdConfigurationProtocolQueryParser*>( iSubParser )->Query();
        observer->ConfigurationQueryL( query );
        }
    
    else if( aTag == KTagActionRequest && observer )
        {
        MNcdConfigurationProtocolActionRequest* actionRequest =
            static_cast<CNcdConfigurationProtocolActionRequestParser*>( iSubParser )->ActionRequest();
        observer->ConfigurationActionRequestL( actionRequest );
        }
    
    else if( aTag == KTagServerDetails && observer )
        {
        MNcdConfigurationProtocolServerDetails* serverDetails =
            static_cast<CNcdConfigurationProtocolServerDetailsParser*>( iSubParser )->ServerDetails();
        observer->ConfigurationServerDetailsL( serverDetails );
        }
    
    else if( aTag == KTagError && observer )
        {
        MNcdConfigurationProtocolError* error =
            static_cast<CNcdConfigurationProtocolErrorParser*>( iSubParser )->ConfigurationProtocolError();
        observer->ConfigurationErrorL( error );
        }

    delete iSubParser;
    iSubParser = 0;
    }
