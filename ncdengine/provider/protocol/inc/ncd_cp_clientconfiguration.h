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
* Description:   MNcdConfigurationProtocolClientConfiguration declaration
*
*/
 

#ifndef M_NCDCONFIGURATIONPROTOCOLCLIENTCONFIGURATION_H
#define M_NCDCONFIGURATIONPROTOCOLCLIENTCONFIGURATION_H

#include <e32base.h>

class MNcdConfigurationProtocolCookie;
class MNcdConfigurationProtocolDetail;

class MNcdConfigurationProtocolClientConfiguration
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolClientConfiguration() {}

    /**
     * Returns when configuration expires.
     * @return Expiration delta
     */
    virtual TInt ExpirationDelta() const = 0;

    /**
     * Returns the amount of cookies in configuration
     * @return Cookie count
     */
    virtual TInt CookieCount() const = 0;

    /**
     * Get a cookie element.
     * @param aIndex Cookie index. Leaves if index is out of bounds.
     * @return Cookie reference.
     */    
    virtual MNcdConfigurationProtocolCookie& CookieL( TInt aIndex ) const = 0;
    
    /**
     * Returns the amount of details in configuration
     * @return Cookie count
     */
    virtual TInt DetailCount() const = 0;

    /**
     * Get a details element.
     * @param aIndex Details index. Leaves if index is out of bounds.
     * @return Details reference.
     */    
    virtual const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const = 0;
    };

#endif
