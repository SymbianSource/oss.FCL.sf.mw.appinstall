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
* Description:   MNcdPreminetProtocolInformation declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLINFORMATION_H
#define M_NCDPREMINETPROTOCOLINFORMATION_H

#include <e32base.h>

class MNcdConfigurationProtocolCookie;
class MNcdConfigurationProtocolQuery;
class MNcdConfigurationProtocolDetail;
class MNcdPreminetProtocolExpiredCachedData;
class MNcdConfigurationProtocolServerDetails;

class MNcdPreminetProtocolInformation
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolInformation() {}

    /**
     * Amount of cookies
     * @return Cookie count
     */
    virtual TInt CookieCount() const = 0;

    /**
     * Get a cookie
     * @param aIndex Index of cookie. Leaves if index is out of bounds
     * @return Cookie object reference. 
     * @see CookieCount()
     */
    virtual const MNcdConfigurationProtocolCookie& CookieL( TInt aIndex ) const = 0;
    
    /**
     * Amount of message elements
     * @return Message element count
     */
    virtual TInt MessageCount() const = 0;

    /**
     * Get a message element
     * @param aIndex Index of message. Leaves if index is out of bounds
     * @return Message object reference. 
     * @see MessageCount()
     */
    virtual const MNcdConfigurationProtocolQuery& MessageL( TInt aIndex ) const = 0;
    
    /**
     * Amount of detail elements
     * @return Detail element count
     */
    virtual TInt DetailCount() const = 0;

    /**
     * Get a detail element
     * @param aIndex Index of detail element. Leaves if index is out of bounds
     * @return Detail object reference. 
     * @see DetailCount()
     */
    virtual const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const = 0;
    
    /**
     * Amount of seconds to use for resending delay
     * @return Resend delay in seconds
     */
    virtual TInt ResendAfter() const = 0;

    /**
     * Returns expired cached data object. Ownership is NOT transferred.
     * 
     * @return Pointer to the data or NULL if not available
     */
    virtual const MNcdPreminetProtocolExpiredCachedData* 
        ExpiredCachedData() const = 0; 
    
    /**
     * Returns the server details object. Ownership is NOT transferred.
     * 
     * @return Pointer to the data or NULL if not available
     */
    virtual const MNcdConfigurationProtocolServerDetails* 
        ServerDetails() const = 0;
    
    /**
     * Returns the namespace string. 
     * @return Namespace string or KNullDesC
     */
    virtual const TDesC& Namespace() const = 0;
    };

#endif
