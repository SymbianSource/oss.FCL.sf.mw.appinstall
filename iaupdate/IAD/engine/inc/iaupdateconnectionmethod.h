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



#ifndef IA_UPDATE_CONNECTION_METHOD_H
#define IA_UPDATE_CONNECTION_METHOD_H


/**
 * Connection method 
 *
 * @since S60 v3.2
 */
class TIAUpdateConnectionMethod
    {
public:    

    /**
     * Connection types
     */
    enum TConnectionMethodType 
        {    
        /**
         * Always ask
         */
        EConnectionMethodTypeAlwaysAsk = 0,
        
        /**
         * Destination aka SNAP
         */
        EConnectionMethodTypeDestination,
        
        /**
         * Accesspoint
         */
        EConnectionMethodTypeAccessPoint,
        
        /**
         * Device default
         */
        EConnectionMethodTypeDefault,        
        };


    TIAUpdateConnectionMethod() : 
        iId( 0 ), 
        iType( EConnectionMethodTypeAlwaysAsk )
        {
        }


    TIAUpdateConnectionMethod( TUint32 aId, TConnectionMethodType aType ) : 
        iId( aId ), 
        iType( aType )
        {
        }


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
    TConnectionMethodType iType;    

    };
                   
    
#endif // IA_UPDATE_CONNECTION_METHOD_H    
