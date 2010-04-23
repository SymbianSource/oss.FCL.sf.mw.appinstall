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


#ifndef C_CATALOGSHTTPREQUESTPARSEROBSERVER_H
#define C_CATALOGSHTTPREQUESTPARSEROBSERVER_H

class MCatalogsHttpRequestParserObserver
    {
public:

    virtual void ParsedHttpRequestLineL( 
        const TDesC8& aMethod, 
        const TDesC8& aUri,
        const TDesC8& aVersion ) = 0;

    virtual void ParsedHttpHeaderL( 
        const TDesC8& aHeader,
        const TDesC8& aHeaderData ) = 0;
        
    virtual void ParsedHttpBodyL(
        const TDesC8& aBody ) = 0;        
        
protected:

    virtual ~MCatalogsHttpRequestParserObserver()
        {
        }
    };
    

#endif // C_CATALOGSHTTPREQUESTPARSEROBSERVER_H