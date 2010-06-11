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


#ifndef _NCDCONNECTIOMETHOD_H
#define _NCDCONNECTIOMETHOD_H

#include <e32base.h>

class RWriteStream;
class RReadStream;


/**
 * Connection types
 *
 * @note ENcdConnectionMethodTypeDestination is
 * supported from 3.2 platform onwards.
 */
enum TNcdConnectionMethodType 
    {    
    /**
     * Always ask
     */
    ENcdConnectionMethodTypeAlwaysAsk = 0,
    
    /**
     * Destination aka SNAP
     */
    ENcdConnectionMethodTypeDestination,
    
    /**
     * Accesspoint
     */
    ENcdConnectionMethodTypeAccessPoint,
    
    /**
     * Device default
     */
    ENcdConnectionMethodTypeDefault,        
    };

/**
 * Connection method 
 */
class TNcdConnectionMethod
    {
public:    
    TNcdConnectionMethod() : 
        iId( 0 ), 
        iType( ENcdConnectionMethodTypeAlwaysAsk )
        {
        }


    TNcdConnectionMethod( TUint32 aId, TNcdConnectionMethodType aType ) : 
        iId( aId ), 
        iType( aType )
        {
        }
  

    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;


    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    
    /**
     * ID of the connection method. 
     *
     * This is access point ID if iType is ENcdConnectionMethodTypeAccessPoint
     * and destination/SNAP ID if iType is ENcdConnectionMethodTypeDestination.
     * Otherwise this should be 0
     */
    TUint32 iId;
    
    
    /**
     * Type of the connection
     */
    TNcdConnectionMethodType iType;    
    };
                   
    
#endif // _NCDCONNECTIOMETHOD_H    
