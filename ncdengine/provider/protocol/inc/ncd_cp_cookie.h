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
* Description:   MNcdConfigurationProtocolCookie declaration
*
*/


#ifndef M_NCDCONFIGURATIONPROTOCOLCOOKIE_H
#define M_NCDCONFIGURATIONPROTOCOLCOOKIE_H

#include <e32base.h>
#include <e32std.h>

/**
 * Cookie scope definitions
 */
namespace NcdConfigurationProtocolCookieScopes
    {
    /**
     * Client-scope
     */
    _LIT( KClient, "client" );
    
    /**
     * SIM-scope
     */
    _LIT( KSim, "sim" );
    }


/**
 * Cookie type definitions
 */
namespace NcdConfigurationProtocolCookieTypes
    {
    /**
     * Save
     */
    _LIT( KSave, "save" );
    
    /**
     * Save and send
     */
    _LIT( KSaveAndSend, "saveAndSend" );
    
    /**
     * Remove
     */
    _LIT( KRemove, "remove" );
    }
    

/**
 * Configuration protocol cookie interface
 */
class MNcdConfigurationProtocolCookie
    {        
public:

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolCookie() {}

    /**
     * Returns the key.
     * @return Keys string or KNullDesC
     */
    virtual const TDesC& Key() const = 0;

    /**
     * Returns the type.
     * @return Type string or KNullDesC
     */
    virtual const TDesC& Type() const = 0;

    /**
     * Returns the cookie scope.
     * @return Scope string or KNullDesC
     */
    virtual const TDesC& Scope() const = 0;

    /**
     * Returns the expiration delta.
     * @return Delta value in minutes
     */
    virtual TInt ExpirationDelta() const = 0;

    /**
     * Returns the amount of value fields.
     * @return Value count
     */
    virtual TInt ValueCount() const = 0;

    /**
     * Get a value.
     * @param aIndex Value index. 
     * @return Value string.
     * @see ValueCount()
     */
    virtual const TDesC& Value( TInt aIndex ) const = 0;


    /**
     * Sim identification getter 
     */
    virtual const TDesC8& Sim() const = 0;
    
    /**
     * Expiration time getter     
     */
    virtual TTime ExpirationTime() const = 0;
    };

#endif
