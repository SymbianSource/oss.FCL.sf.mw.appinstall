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
* Description:   CNcdConfigurationProtocolDetailParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"

#include "ncd_parser_pp_moreinfo.h"
#include "ncd_parser_cp_query.h"
#include "ncd_cp_query.h"

#include "catalogsdebug.h"

CNcdPreminetProtocolMoreInfoParser* 
CNcdPreminetProtocolMoreInfoParser::NewL( MNcdParserObserverBundle& aObservers,
                                            MNcdSubParserObserver& aSubParserObserver,
                                            TInt aDepth,
                                            const Xml::RTagInfo& aElement,
                                            const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolMoreInfoParser* self 
        = new(ELeave) CNcdPreminetProtocolMoreInfoParser( aObservers,
                                                            aSubParserObserver,
                                                            aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolMoreInfoParser::CNcdPreminetProtocolMoreInfoParser( MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolMoreInfoParser::~CNcdPreminetProtocolMoreInfoParser()
    {
    delete iMoreInfo;
    iMoreInfo = 0;
    }

void CNcdPreminetProtocolMoreInfoParser::ConstructL( const Xml::RTagInfo& aElement,
                                                       const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );    
    }

void CNcdPreminetProtocolMoreInfoParser::OnStartElementL( const Xml::RTagInfo& aElement, 
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
    else if( tag == KTagMessage )
        {
        iSubParser = CNcdConfigurationProtocolQueryParser::NewL( 
            *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }


void CNcdPreminetProtocolMoreInfoParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
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

void CNcdPreminetProtocolMoreInfoParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if( iSubParser->Tag() == KTagMessage )
        {
        MNcdConfigurationProtocolQuery* moreInfo =
            static_cast<CNcdConfigurationProtocolQueryParser*>
            ( iSubParser )->Query();
        delete iMoreInfo;
        iMoreInfo = moreInfo;
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdConfigurationProtocolQuery* CNcdPreminetProtocolMoreInfoParser::MoreInfo()
    {
    MNcdConfigurationProtocolQuery* returnPtr = iMoreInfo;
    iMoreInfo = 0;
    return returnPtr;
    }

