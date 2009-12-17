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
* Description:   CNcdConfigurationProtocolQueryParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_parser_cp_serverdetails.h"
#include "ncd_parser_cp_detail.h"
#include "ncd_parser_cp_capabilities.h"
#include "ncd_cp_serverdetailsimpl.h"
#include "catalogsdebug.h"

CNcdConfigurationProtocolServerDetailsParser* 
CNcdConfigurationProtocolServerDetailsParser::NewL( MNcdParserObserverBundle& aObservers,
                                                    MNcdSubParserObserver& aSubParserObserver,
                                                    TInt aDepth,
                                                    const Xml::RTagInfo& aElement,
                                                    const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolServerDetailsParser* self 
        = new(ELeave) CNcdConfigurationProtocolServerDetailsParser( aObservers,
                                                                    aSubParserObserver,
                                                                    aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolServerDetailsParser::CNcdConfigurationProtocolServerDetailsParser( MNcdParserObserverBundle& aObservers,
                                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolServerDetailsParser::~CNcdConfigurationProtocolServerDetailsParser()
    {
    delete iServerDetails;
    iServerDetails = 0;
    }

void CNcdConfigurationProtocolServerDetailsParser::ConstructL( const Xml::RTagInfo& aElement,
                                                               const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );
    
    iServerDetails = new(ELeave) CNcdConfigurationProtocolServerDetailsImpl();
    iServerDetails->ConstructL();

    // version
    NcdProtocolUtils::AssignDesL( iServerDetails->iVersion, AttributeValue( KAttrVersion, aAttributes ) );
    
    }

void CNcdConfigurationProtocolServerDetailsParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                                    const Xml::RAttributeArray& aAttributes, 
                                                                    TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    TPtrC8 tag( aElement.LocalName().DesC() );

    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if( tag == KTagDetail )
        {
        iSubParser = CNcdConfigurationProtocolDetailParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    else if( tag == KTagCapabilities )
        {
        iSubParser = CNcdConfigurationProtocolCapabilitiesParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }    
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth + 1, aElement, aAttributes );
        }
    }

void CNcdConfigurationProtocolServerDetailsParser::OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));

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

void CNcdConfigurationProtocolServerDetailsParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                       TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));

    if( aTag == KTagDetail )
        {
        CNcdConfigurationProtocolDetailParser* detailParser = 
            static_cast<CNcdConfigurationProtocolDetailParser*>( iSubParser );
        MNcdConfigurationProtocolDetail* detail = detailParser->Detail();
        CleanupDeletePushL( detail );
        ( iServerDetails->iDetails ).AppendL( detail );
        CleanupStack::Pop( detail );
        }
    else if( aTag == KTagCapabilities )
        {
        CNcdConfigurationProtocolCapabilitiesParser* capabilitiesParser = 
            static_cast<CNcdConfigurationProtocolCapabilitiesParser*>( iSubParser );
        iServerDetails->iCapabilities = capabilitiesParser->Capabilities();
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdConfigurationProtocolServerDetails* CNcdConfigurationProtocolServerDetailsParser::ServerDetails()
{
    MNcdConfigurationProtocolServerDetails* tmpPtr = iServerDetails;
    iServerDetails = 0;
    return tmpPtr;
}


