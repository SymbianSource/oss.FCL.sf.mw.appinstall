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
* Description:   MNcdServerDetails declaration
*
*/

 
#ifndef M_NCDSERVERDETAILS_H
#define M_NCDSERVERDETAILS_H 
 
class MCatalogsContext;
class MNcdConfigurationProtocolCookie;
 
/**
 * Interface for server details manipulation
 */
class MNcdServerDetails
    {        
public:    
    /**
     * Adds a new capability to details.
     *
     * @param aCapability Capability
     * @note Does not check for duplicates
     */
    virtual void AddCapabilityL( const TDesC& aCapability ) = 0;
       
    
    /**
     * Clears server's capabilities
     */
    virtual void ClearCapabilitiesL() = 0;
    
    /**
     */
    virtual TBool IsCapabilitySupported( const TDesC& aCapability ) const = 0;
    
    
    /**
     * Server URI getter
     */
    virtual const TDesC& ServerUri() const = 0;
        
    
    /**
     * Namespace getter
     */
    virtual const TDesC& Namespace() const = 0;
    
    
    /**
     * Cookie adder
     */
    virtual void AddCookieL( 
        const MNcdConfigurationProtocolCookie& aCookie,
        const TDesC8& aSim ) = 0;
            
       
    /**
     * Cookie getter
     *
     * @param aSim SIM identification
     * @return Array of cookies. Ownership of the cookies is not transferred
     */
    virtual RPointerArray<const MNcdConfigurationProtocolCookie>
        CookiesL( const TDesC8& aSim ) const = 0;
    
    
    /**
     * Removes expired cookies
     *
     * @return Number of removed cookies
     */
    virtual TInt RemoveExpiredCookies() = 0;    
protected:
    
    virtual ~MNcdServerDetails()
        {
        }
        
    };
    
    
#endif // M_NCDSERVERDETAILS_H
