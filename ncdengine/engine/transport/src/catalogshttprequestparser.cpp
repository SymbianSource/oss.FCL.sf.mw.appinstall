/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CCatalogsHttpRequestParser
*
*/


#include "catalogshttprequestparser.h"

#include "catalogshttprequestparserobserver.h"
#include "catalogshttpmessageconstants.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsHttpRequestParser* CCatalogsHttpRequestParser::NewL()
    {
    CCatalogsHttpRequestParser* self = new( ELeave ) CCatalogsHttpRequestParser;
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsHttpRequestParser::~CCatalogsHttpRequestParser()
    {
    DeletePtr( iRequest );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseRequestL( 
    const TDesC8& aRequest,
    MCatalogsHttpRequestParserObserver& aObserver )
    {
    DLTRACEIN((""));
    if ( !aRequest.Size() ) 
        {
        DLERROR(("Empty request, leaving"));
        User::Leave( KErrArgument );
        }
    
    AssignDesL( iRequest, aRequest );    
    iLex.Assign( *iRequest );
    iObserver = &aObserver;
    iState = EParseStart;
    HandleStateL();
    
    DeletePtr( iRequest );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsHttpRequestParser::CCatalogsHttpRequestParser()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::HandleStateL()
    {
    DLTRACEIN((""));
    while( iState != EParseEnd ) 
        {
        
        switch( iState ) 
            {
            case EParseStart:
                {
                ParseStartLineL();
                break;
                }
                
            case EParseHeaders:
                {
                ParseHeadersL();
                break;
                }
                
            case EParseBody:
                {
                ParseBodyL();
                break;
                }
                
            case EParseEnd:
                {
                DLTRACE(("End"));
                break;
                }
            
            default:
                {
                DASSERT( 0 );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseStartLineL()
    {
    DLTRACEIN((""));
    TPtrC8 method( KNullDesC8 );
    TPtrC8 uri( KNullDesC8 );
    TPtrC8 version( KNullDesC8 );
    
    ParseWordL( method );
    
    // Request-URI and HTTP-version are allowed to be missing
    TBool skippedEndOfLine = SkipEndOfLine();
    if ( !skippedEndOfLine )
        {        
        DLTRACE(("Parsing Request-URI"));
        ParseWordL( uri );
        
        skippedEndOfLine = SkipEndOfLine();
        if ( !skippedEndOfLine ) 
            {        
            DLTRACE(("Parsing HTTP-version"));
            ParseWordL( version );
            }
        }
    
    if ( skippedEndOfLine ) 
        {
        // Revert back to the end of the line so that CRLF counting in
        // ParseHeadersL works correctly
        iLex.UnGet();
        iLex.UnGet();
        }
    
    DASSERT( iObserver );
    iObserver->ParsedHttpRequestLineL( method, uri, version );
    iState = EParseHeaders;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseHeadersL()
    {
    DLTRACEIN((""));
    TPtrC8 header;
    TPtrC8 headerData;
    
    DASSERT( iObserver );
    
    while ( !iLex.Eos() && 
            // 1 CRLF means end of the line
            // 2 CRLFs means that headers ended and body begins
            SkipCrLfL() == 1 ) 
        {
        ParseWordL( header );
        TInt index = header.LocateReverse( 
            CatalogsHttpMessageConstants::KColon()[0] );
        if ( index == KErrNotFound ) 
            {
            DLERROR(("Invalid header"));
            User::Leave( KErrCorrupt );
            }
        ParseHeaderValueL( headerData );
        iObserver->ParsedHttpHeaderL( header.Left( index ), headerData );        
        }
    iState = EParseBody;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseBodyL()
    {
    DLTRACEIN((""));
    DASSERT( iObserver );
    
    iObserver->ParsedHttpBodyL( iLex.Remainder() );
    iState = EParseEnd;
    }    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseWordL( 
    TPtrC8& aParsedWord )
    {
    DLTRACEIN((""));

    if ( iLex.Eos() ) 
        {
        DLERROR(("Out of data, leaving"));
        User::Leave( KErrCorrupt );
        }
    
    // skip space at beginning if any    
    iLex.SkipSpaceAndMark();
    
    // read only next word
    aParsedWord.Set( iLex.NextToken() );

    DLTRACE(("Word: %S", &aParsedWord ));
    }


// ---------------------------------------------------------------------------
// Parse header value
// ---------------------------------------------------------------------------
//
void CCatalogsHttpRequestParser::ParseHeaderValueL( 
    TPtrC8& aParsedHeaderValue )
    {
    DLTRACEIN((""));

    if ( iLex.Eos() ) 
        {
        DLERROR(("Out of data, leaving"));
        User::Leave( KErrCorrupt );
        }
    
    // skip space at the beginning if any    
    iLex.SkipSpaceAndMark();
       
    while( !iLex.Eos() ) 
        {
        // If true, found end of line but we need to check if the header
        // continues on the next line
        if ( SkipEndOfLine() ) 
            {
            TChar peeked( iLex.Peek() );
            // headers span multiple lines if the next line starts with
            // at least one space or horizontal tab
            if ( peeked == CatalogsHttpMessageConstants::KSpace()[0] || 
                 peeked == CatalogsHttpMessageConstants::KHorizTab ) 
                {
                DLTRACE(("header spans on multiple lines"));
                // Remove CrLf because platform components 
                // will otherwise cut the header short
                TInt current = iLex.Offset();
                TPtr8 ptr( iRequest->Des() );

                TInt length = CatalogsHttpMessageConstants::KCrLf().Length();
                DLTRACE(("Length: %d", length));
                
                DLTRACE(("Deleting: %S", &ptr.Mid( current-length, length ) ));
                ptr.Delete( current - length, length );
                }
            else 
                {
                DLTRACE(("End of the header value"));
                // revert back to the end of the last line so that
                // we correctly identify the end of headers outside 
                // this function
                iLex.UnGet();
                iLex.UnGet();
                break;
                }
            }
        else
            {            
            iLex.Inc();
            }
        }
    aParsedHeaderValue.Set( iLex.MarkedToken() );

    DLTRACE(("Word: %S", &aParsedHeaderValue ));
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCatalogsHttpRequestParser::SkipEndOfLine()
    {    
    if ( iLex.Peek() == CatalogsHttpMessageConstants::KCr  ) 
        {
        iLex.Inc();
        if ( iLex.Peek() == CatalogsHttpMessageConstants::KLf ) 
            {
            iLex.Inc();
            return ETrue;
            }
        // revert back to original index
        iLex.UnGet();
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCatalogsHttpRequestParser::SkipCrLfL() 
    {
    TInt count = 0;

    while ( !iLex.Eos() ) 
        {
        if ( iLex.Peek() == CatalogsHttpMessageConstants::KCr ) 
            {
            iLex.Inc();

            if ( iLex.Peek() == CatalogsHttpMessageConstants::KLf ) 
                {
                iLex.Inc();
                }
            else 
                {
                DLERROR(("LF should have followed CR"));
                User::Leave( KErrCorrupt );
                }
            count++;
            }
        else // nothing to skip
            {
            break;
            }
        }
    return count;     
    }    
