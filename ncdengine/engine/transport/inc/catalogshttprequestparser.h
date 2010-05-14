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
* Description:   
*
*/

	
#ifndef C_CATALOGSHTTPREQUESTPARSER_H
#define C_CATALOGSHTTPREQUESTPARSER_H

#include <e32base.h>

class MCatalogsHttpRequestParserObserver;

/**
 * HTTP request parser
 */
class CCatalogsHttpRequestParser : public CBase
    {
public:

    static CCatalogsHttpRequestParser* NewL();
    virtual ~CCatalogsHttpRequestParser();

    /**
     * Parses the request and sends the parsed pieces to the observer
     *
     * The request should be a valid HTTP/1.1 request. However,
     * Request-URI and HTTP-version can be omitted from the Status-line
     *
     * @param aRequest HTTP request
     * @param aObserver Parser observer
     */
    void ParseRequestL( 
        const TDesC8& aRequest,
        MCatalogsHttpRequestParserObserver& aObserver );
        
protected:

    enum TRequestParserState 
        {
        EParseStart,
        EParseHeaders,
        EParseBody,
        EParseEnd
        };
            
private:

    CCatalogsHttpRequestParser();
    
    
    CCatalogsHttpRequestParser( const CCatalogsHttpRequestParser& );
    CCatalogsHttpRequestParser& operator=( const CCatalogsHttpRequestParser& );
    
    void HandleStateL();
    void ParseStartLineL();
    void ParseHeadersL();
    void ParseBodyL();
    
    /**
     * Parses one continuous word
     */
    void ParseWordL( TPtrC8& aParsedWord );
    
    /**
     * Parses a header's value. The value can span multiple lines
     */
    void ParseHeaderValueL( TPtrC8& aParsedHeaderValue );
    
    /**
     * Checks if the current index is the start of the end of the line.
     * If the end of the line (CRLF) is found, it is skipped and ETrue is
     * returned. Otherwise returns EFalse and doesn't change the current index
     */
    TBool SkipEndOfLine();
    
    /**
     * Skips over a series of CRLF's and returns the number of skipped
     * pairs.
     * This is used to see where the headers end and the body starts
     */
    TInt SkipCrLfL();
    
private:    

    TRequestParserState iState;
    
    MCatalogsHttpRequestParserObserver* iObserver; // Not owned
    TLex8 iLex;
    HBufC8* iRequest; // Copy of the request. Owned
    };


#endif // C_CATALOGSHTTPREQUESTPARSER_H