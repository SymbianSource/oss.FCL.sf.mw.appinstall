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


#ifndef C_CATALOGSHTTPREQUESTADAPTER_H
#define C_CATALOGSHTTPREQUESTADAPTER_H

#include <e32base.h>
#include "catalogshttprequestparserobserver.h"

class CCatalogsHttpRequestParser;
class MCatalogsHttpSession;
class MCatalogsHttpObserver;
class MCatalogsHttpOperation;

class CCatalogsHttpRequestAdapter : 
    public CBase,
    public MCatalogsHttpRequestParserObserver
    {
public:

    static CCatalogsHttpRequestAdapter* NewL(
        MCatalogsHttpSession& aHttpSession );
        
    virtual ~CCatalogsHttpRequestAdapter();

public:

    /**
     * Creates a HTTP transaction
     *
     * Request-URI and HTTP-version can be omitted from the request. 
     * Even if they exist they won't affect anything. Also Host-header
     * is ignored if it exists.
     *
     * @param aUri Destination URI
     * @param aRequest A valid HTTP/1.1 request
     * @param aObserver Observer
     * @return HTTP operation
     */
    MCatalogsHttpOperation* CreateTransactionL(
        const TDesC8& aUri,
        const TDesC8& aRequest,
        MCatalogsHttpObserver& aObserver );


public: // From MCatalogsHttpRequestParserObserver

    void ParsedHttpRequestLineL( 
        const TDesC8& aMethod, 
        const TDesC8& aUri,
        const TDesC8& aVersion );

    void ParsedHttpHeaderL( 
        const TDesC8& aHeader,
        const TDesC8& aHeaderData );
        
    void ParsedHttpBodyL(
        const TDesC8& aBody );

protected:

    CCatalogsHttpRequestAdapter( MCatalogsHttpSession& aHttpSession );
    void ConstructL();
    
    CCatalogsHttpRequestAdapter( const CCatalogsHttpRequestAdapter& );
    CCatalogsHttpRequestAdapter& operator=( const CCatalogsHttpRequestAdapter& );

private:                

    MCatalogsHttpSession& iHttpSession;
    CCatalogsHttpRequestParser* iParser;
    MCatalogsHttpOperation* iTransaction;
    MCatalogsHttpObserver* iHttpObserver;
    TPtrC8 iUri;
    
    };


#endif // C_CATALOGSHTTPREQUESTADAPTER_H
