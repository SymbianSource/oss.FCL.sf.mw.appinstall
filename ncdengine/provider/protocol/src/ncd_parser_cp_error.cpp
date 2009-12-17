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
* Description:   CNcdConfigurationProtocolErrorParser implementation
*
*/


#include "catalogsdebug.h"
#include "ncd_cp_errorimpl.h"
#include "ncd_pp_errorimpl.h"
#include "ncd_parser_cp_error.h"
#include "ncd_parser_cp_query.h"
#include "ncdprotocolutils.h"
#include "ncdprotocolwords.h"
#include "ncdunknownparser.h"

CNcdConfigurationProtocolErrorParser* 
CNcdConfigurationProtocolErrorParser::NewL( MNcdParserObserverBundle& aObservers,
                                            MNcdSubParserObserver& aSubParserObserver,
                                            TInt aDepth,
                                            const Xml::RTagInfo& aElement,
                                            const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolErrorParser* self 
        = new(ELeave) CNcdConfigurationProtocolErrorParser( aObservers,
                                                            aSubParserObserver,
                                                            aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolErrorParser::CNcdConfigurationProtocolErrorParser( MNcdParserObserverBundle& aObservers,
                                                                              MNcdSubParserObserver& aSubParserObserver,
                                                                              TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolErrorParser::~CNcdConfigurationProtocolErrorParser()
    {
    delete iError;
    iError = 0;
    }

void CNcdConfigurationProtocolErrorParser::ConstructL( const Xml::RTagInfo& aElement,
                                                       const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );
    iError = new(ELeave) CNcdPreminetProtocolErrorImpl();

    // attributes
    // code
    TPtrC8 code = AttributeValue( KAttrCode, aAttributes );
    if( code != KNullDesC8 )
        {
        iError->iCode = NcdProtocolUtils::DesDecToIntL( code );
        }
    // id
    NcdProtocolUtils::AssignDesL( iError->iId, AttributeValue( KAttrId, aAttributes ) );

    }

void CNcdConfigurationProtocolErrorParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                            const Xml::RAttributeArray& aAttributes, 
                                                            TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("error start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

    if ( iSubParser ) 
        {
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if( tag == KTagMessage )
        {
        iSubParser = CNcdConfigurationProtocolQueryParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }

void CNcdConfigurationProtocolErrorParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));

    if ( iSubParser ) 
        {
        iSubParser->OnEndElementL( aElement, aErrorCode );
        }
    else if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    }

void CNcdConfigurationProtocolErrorParser::SubParserFinishedL( const TDesC8& aTag, 
                                                               TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning


    if( iSubParser->Tag() == KTagMessage )
        {
        CNcdConfigurationProtocolQueryParser* messageParser = 
            static_cast<CNcdConfigurationProtocolQueryParser*>( iSubParser );
        iError->iMessage = messageParser->Query();
        }
    
    delete iSubParser;
    iSubParser = 0;
    }

MNcdConfigurationProtocolError* CNcdConfigurationProtocolErrorParser::ConfigurationProtocolError()
{
    MNcdConfigurationProtocolError* returnPtr = iError;
    iError = 0;
    return returnPtr;
}

MNcdPreminetProtocolError* CNcdConfigurationProtocolErrorParser::PreminetProtocolError()
{
    MNcdPreminetProtocolError* returnPtr = iError;
    iError = 0;
    return returnPtr;
}

