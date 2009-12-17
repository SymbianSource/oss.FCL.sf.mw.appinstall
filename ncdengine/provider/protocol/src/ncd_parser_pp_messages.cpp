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
* Description:   CNcdPreminetProtocolMessagesParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_cp_detailimpl.h"
#include "ncd_parser_pp_messages.h"
#include "ncd_parser_cp_query.h"
#include "catalogsdebug.h"
#include "ncd_cp_query.h"

CNcdPreminetProtocolMessagesParser* 
CNcdPreminetProtocolMessagesParser::NewL( MNcdParserObserverBundle& aObservers,
                                          MNcdSubParserObserver& aSubParserObserver,
                                          TInt aDepth,
                                          const Xml::RTagInfo& aElement,
                                          const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolMessagesParser* self 
        = new(ELeave) CNcdPreminetProtocolMessagesParser( aObservers,
                                                          aSubParserObserver,
                                                          aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolMessagesParser::CNcdPreminetProtocolMessagesParser( MNcdParserObserverBundle& aObservers,
                                                                        MNcdSubParserObserver& aSubParserObserver,
                                                                        TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolMessagesParser::~CNcdPreminetProtocolMessagesParser()
    {
    delete iMessages;
    iMessages = 0;
    }

void CNcdPreminetProtocolMessagesParser::ConstructL( 
    const Xml::RTagInfo& aElement, const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );
    iMessages = new(ELeave) CArrayPtrFlat<MNcdConfigurationProtocolQuery>( 4 );
    }

void CNcdPreminetProtocolMessagesParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                          const Xml::RAttributeArray& aAttributes, 
                                                          TInt aErrorCode ) 
    {
    if( iSubParser == 0 )
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

        TPtrC8 tag( aElement.LocalName().DesC() );
        
        if( tag == KTagMessage )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
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


void CNcdPreminetProtocolMessagesParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
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

void CNcdPreminetProtocolMessagesParser::SubParserFinishedL( const TDesC8& aTag, TInt /*aErrorCode*/ )                                                                  
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));

    if( aTag == KTagMessage )
        {
        MNcdConfigurationProtocolQuery* message =
            static_cast<CNcdConfigurationProtocolQueryParser*>( iSubParser )->Query();
        CleanupDeletePushL( message );
        iMessages->AppendL( message );
        CleanupStack::Pop( message );
        }
    
    delete iSubParser;
    iSubParser = 0;
    }

CArrayPtr<MNcdConfigurationProtocolQuery>* CNcdPreminetProtocolMessagesParser::Messages()
    {
    CArrayPtr<MNcdConfigurationProtocolQuery>* returnPtr = iMessages;
    iMessages = 0;
    return returnPtr;
    }

