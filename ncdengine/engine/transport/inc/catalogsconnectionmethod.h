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


#ifndef T_CATALOGSCONNECTIONMETHOD_H
#define T_CATALOGSCONNECTIONMETHOD_H

#include "catalogsdebug.h"

#define DLMETHOD( method ) DLTRACE(("method: %d, %u, %u", (method).iType, (method).iId, (method).iApnId ))

class RWriteStream;
class RReadStream;

/**
 * Connection types
 *
 * @note ECatalogsConnectionMethodTypeDestination and 
 * ECatalogsConnectionMethodTypeDefault are supported 
 * only from 3.2 platform onwards.
 */
enum TCatalogsConnectionMethodType 
    {
    ECatalogsConnectionMethodTypeAlwaysAsk = 0,
    ECatalogsConnectionMethodTypeDestination,
    ECatalogsConnectionMethodTypeAccessPoint,
    ECatalogsConnectionMethodTypeDeviceDefault
    };

/**
 * Connection method 
 */
class TCatalogsConnectionMethod
    {
public:   

    TCatalogsConnectionMethod() : 
        iId( 0 ), 
        iType( ECatalogsConnectionMethodTypeDeviceDefault ),
        iApnId( 0 )
        {
        }


    TCatalogsConnectionMethod( TUint32 aId, TCatalogsConnectionMethodType aType ) : 
        iId( aId ), 
        iType( aType ),
        iApnId( 0 )
        {
        }
    
    TBool operator==( const TCatalogsConnectionMethod& aMethod ) const;
    
    TBool operator!=( const TCatalogsConnectionMethod& aMethod ) const;
    
    /**
     * Returns ETrue if just iType and iId matches
     */
    TBool Match( const TCatalogsConnectionMethod& aMethod ) const;
    
    /**
     * Returns iApnId if it's set, otherwise iId
     */
    TUint32 CurrentApnId() const;
        

    void ExternalizeL( RWriteStream& aStream ) const;
    void InternalizeL( RReadStream& aStream );
        
    /**
     * ID of the connection method. 
     *
     * This is access point ID if iType is ECatalogsConnectionMethodTypeAccessPoint
     * and destination/SNAP ID if iType is ECatalogsConnectionMethodTypeDestination.
     * Otherwise this should be 0
     */
    TUint32 iId;
    
    
    /**
     * Type of the connection
     */
    TCatalogsConnectionMethodType iType;    
    
    /**
     * Actual APN id if the connection has been made. 
     */
    TUint32 iApnId;
    };

    
#include "catalogsconnectionmethod.inl"    
       
    
#endif // _CATALOGSCONNECTIOMETHOD_H
