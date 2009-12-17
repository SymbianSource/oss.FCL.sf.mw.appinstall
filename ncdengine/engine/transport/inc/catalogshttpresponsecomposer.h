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


#ifndef T_CATALOGSHTTPRESPONSECOMPOSER_H
#define T_CATALOGSHTTPRESPONSECOMPOSER_H

#include <e32base.h>

class MCatalogsHttpOperation;
class RWriteStream;
class CCatalogsKeyValuePair;

/**
 */
class TCatalogsHttpResponseComposer
    {
public:   

    /**
     * Composes a standard HTTP response from the operation's response headers
     * and the given body
     *
     * @param aOperation A successfully finished HTTP transaction
     * @param aBody Response body
     * @return HTTP response
     *
     * @note HTTP version is always HTTP/1.1
     */
    HBufC8* ComposeResponseL( 
        const MCatalogsHttpOperation& aOperation,
        const TDesC8& aBody ) const;
    
private:
    

    void WriteStatusLineL(   
        const MCatalogsHttpOperation& aOperation,
        RWriteStream& aStream ) const;

    void WriteHeadersL(   
        const MCatalogsHttpOperation& aOperation,
        RWriteStream& aStream ) const;


    void WriteHeaderL(   
        const CCatalogsKeyValuePair& aHeader,
        RWriteStream& aStream ) const;
    };

#endif // C_CATALOGSHTTPRESPONSECOMPOSER_H
