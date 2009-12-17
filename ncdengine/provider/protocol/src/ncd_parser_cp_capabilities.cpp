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
* Description:   CNcdConfigurationProtocolCapabilitiesParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_parser_cp_capabilities.h"
#include "catalogsdebug.h"

CNcdConfigurationProtocolCapabilitiesParser* 
CNcdConfigurationProtocolCapabilitiesParser::NewL( MNcdParserObserverBundle& aObservers,
                                                   MNcdSubParserObserver& aSubParserObserver,
                                                   TInt aDepth,
                                                   const Xml::RTagInfo& aElement,
                                                   const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolCapabilitiesParser* self 
        = new(ELeave) CNcdConfigurationProtocolCapabilitiesParser( aObservers,
                                                                   aSubParserObserver,
                                                                   aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolCapabilitiesParser::CNcdConfigurationProtocolCapabilitiesParser( MNcdParserObserverBundle& aObservers,
                                                                                          MNcdSubParserObserver& aSubParserObserver,
                                                                                          TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolCapabilitiesParser::~CNcdConfigurationProtocolCapabilitiesParser()
    {
    delete iCapabilities;
    iCapabilities = 0;
    }

void CNcdConfigurationProtocolCapabilitiesParser::ConstructL( 
    const Xml::RTagInfo& aElement, const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );
    iCapabilities = new(ELeave) CArrayPtrFlat<HBufC>( 4 );
    }

void CNcdConfigurationProtocolCapabilitiesParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                                   const Xml::RAttributeArray& aAttributes, 
                                                                   TInt aErrorCode ) 
    {
    if( iSubParser == 0 )
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

        TPtrC8 tag( aElement.LocalName().DesC() );
        
        if( tag == KTagCapability )
            {
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


void CNcdConfigurationProtocolCapabilitiesParser::OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    TPtrC8 tag( aElement.LocalName().DesC() );

    if( iSubParser == 0 && iTag && *iTag == tag )
        {
        DLTRACE(("end tag=%S",&tag));
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    else if( tag == KTagCapability && iBuffer )
        {        
        HBufC* capability = 0;
        NcdProtocolUtils::AssignDesL( capability, *iBuffer );
        CleanupDeletePushL( capability );
        iCapabilities->AppendL( capability );
        CleanupStack::Pop( capability );
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

void CNcdConfigurationProtocolCapabilitiesParser::SubParserFinishedL( const TDesC8& aTag, TInt /*aErrorCode*/ )                                                                  
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    delete iSubParser;
    iSubParser = 0;
    }

CArrayPtr<HBufC>* CNcdConfigurationProtocolCapabilitiesParser::Capabilities()
    {
    CArrayPtr<HBufC>* returnPtr = iCapabilities;
    iCapabilities = 0;
    return returnPtr;
    }

