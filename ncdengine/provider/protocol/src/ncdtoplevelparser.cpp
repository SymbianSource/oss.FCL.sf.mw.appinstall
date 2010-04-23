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
* Description:   CNcdTopLevelParser implementation
*
*/


#include <e32debug.h>
#include "ncdtoplevelparser.h"
#include "ncdparserobserver.h"
#include "ncdunknownparser.h"
#include "catalogsdebug.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"

#include "ncd_parser_pp_response.h"
#include "ncd_parser_cp_response.h"

CNcdTopLevelParser* CNcdTopLevelParser::NewL( MNcdParserObserverBundle& aObservers,
                                              MNcdSubParserObserver& aSubParserObserver,
                                              TInt aDepth )
    {
    DLTRACE(("depth=%d",aDepth));
    CNcdTopLevelParser* self = new(ELeave) CNcdTopLevelParser( aObservers,
                                                               aSubParserObserver,
                                                               aDepth );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdTopLevelParser::CNcdTopLevelParser( MNcdParserObserverBundle& aObservers,
                                        MNcdSubParserObserver& aSubParserObserver,
                                        TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdTopLevelParser::~CNcdTopLevelParser()
    {
    }

void CNcdTopLevelParser::ConstructL()
    {
    
    }

void CNcdTopLevelParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                          const Xml::RAttributeArray& aAttributes, 
                                          TInt aErrorCode ) 
    {

    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    DLTRACEIN((""));

    // Error code checks need only be done in the top level parser,
    // all calls come through it.
    if( aErrorCode != KErrNone )
        {
        DLERROR(("%d",aErrorCode));
        iObservers->ParserObserver()->ParseError( aErrorCode );
        return;
        }

    TPtrC8 tag( aElement.LocalName().DesC() );
    // Here we handle the tag. If we don't know how to do it, try to create a subparser.

    if( iSubParser == 0 )
        {
        DLTRACE(("start tag=%S",&tag));
        if ( tag == KTagPreminetResponse )
            {
            iSubParser = CNcdPreminetProtocolResponseParser::NewL( *iObservers, *this, iDepth+1, 
                                                                   aElement, aAttributes );
            }
        else if ( tag == KTagConfigurationResponse )
            {
            iSubParser = CNcdConfigurationProtocolResponseParser::NewL( *iObservers, *this, iDepth+1, 
                                                                        aElement, aAttributes );
            }
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, 
                                                  aElement, aAttributes );
            }
        } 

    else
        {
        // Using old subparser
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }

    }

void CNcdTopLevelParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACE((""));

    // Error code checks need only be done in the top level parser,
    // all calls come through it.
    if( aErrorCode != KErrNone )
        {
        DLERROR(("%d",aErrorCode));
        iObservers->ParserObserver()->ParseError( aErrorCode );
        return;
        }
   
    CNcdSubParser::OnEndElementL( aElement, aErrorCode );
    }

void CNcdTopLevelParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    DLTRACEIN((""));
    // Error code checks need only be done in the top level parser,
    // all calls come through it.
    if( aErrorCode != KErrNone )
        {
        DLERROR(("%d",aErrorCode));
        iObservers->ParserObserver()->ParseError( aErrorCode );
        return;
        }

    if( iSubParser )
        {
        // Is this correct? if the content comes in chunks and a middle chunck
        // is dismissed due to this check, then an error probably occurs.
        if( /*NcdProtocolUtils::IsWhitespace( aBytes )*/false )
            {
//             DLINFO(("Skipping %db whitespace content",aBytes.Length()));
            }
        else if( iSubParser )
            {
            iSubParser->OnContentL( aBytes, aErrorCode );
            }
        }
    else
        {
        DLWARNING(("Ignoring %d bytes content",aBytes.Length()));
        }
    }



void CNcdTopLevelParser::StartL( const Xml::RTagInfo& /*aElement*/, 
                                 const Xml::RAttributeArray& /*aAttributes*/, 
                                 TInt /*aErrorCode*/ )
    {
    }

// void CNcdTopLevelParser::EndL( const Xml::RTagInfo& aElement, 
//                           TInt aErrorCode )
//     {
//     }

// void CNcdTopLevelParser::SubParserFinishedL( const TDesC8& aTag,
//                                              TInt aErrorCode )
//     {
//     DLTRACE(("SubParserFinishedL"));

//     delete iSubParser;
//     iSubParser = 0;
//     }
