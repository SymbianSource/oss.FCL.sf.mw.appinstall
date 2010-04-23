/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _CATALOGSHTTPUTILS_H
#define _CATALOGSHTTPUTILS_H

#include <e32base.h>
#include <es_enum.h>

#include "catalogsconnectionmethod.h"

class RConnection;


/**
 * Utility functions for handling connections
 */
namespace CatalogsHttpUtils 
    {  

    /**
     * Checks if the given connection method actually exists in the device.
     *
     * Note that ECatalogsConnectionMethodTypeDeviceDefault always returns ETrue and
     * ECatalogsConnectionMethodTypeAlwaysAsk always returns EFalse
     * 
     * @param aMethod
     */
    TBool ConnectionMethodExistsL( const TCatalogsConnectionMethod& aMethod );
    
    /**
     * Gets the id of the active APN from the connection
     *
     * @param aConnection Connection
     * @param aApn Active APN
     * @return Symbian error code
     */
    TInt ApnFromConnection( RConnection& aConnection, TUint32& aApn ); 

    
    /**
     * Checks if the given connectionmethod is open in the connection.
     * 
     * @note Errors are ignored
     * @param aConnection Connection
     * @param aConnectionCount Open connections in the connection (EnumerateConnections)
     * @param aMethod Connectionmethod
     * @param aConnectedMethod Info about the connected method if this function returns ETrue
     * @return ETrue if the connectionmethod is open
     */
    TBool IsConnectionMethodOpen( 
        RConnection& aConnection, 
        TUint32 aConnectionCount,
        const TCatalogsConnectionMethod& aMethod,
        TConnectionInfoBuf& aConnectedMethod );
    
    
    /**
     * Encodes an UTF8 URI
     */
    HBufC8* EncodeUriL( const TDesC8& aUri );

    }

/**
 * Utility class for parsing HTTP Content-Disposition header.
 */
class TCatalogsContentDispositionParser
    {
public:

    /**
     * Constructor
     */
    TCatalogsContentDispositionParser( const TDesC8& aContentDisposition );
    
    /**
     * Filename getter
     */
    HBufC* FilenameLC() const;

private:
    const TDesC8& iContentDisposition;
    
    };




#endif // C_CATALOGSHTTPUTILS_H
