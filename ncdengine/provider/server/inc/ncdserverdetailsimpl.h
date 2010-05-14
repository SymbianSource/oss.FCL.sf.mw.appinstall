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
* Description:   CNcdServerDetails declaration
*
*/


#include <e32base.h>
#include <badesca.h>
#include "ncdserverdetails.h"
#include "ncdstoragedataitem.h"
#include "ncdproviderdefines.h"

#ifndef C_NCDSERVERDETAILS_H
#define C_NCDSERVERDETAILS_H

class MNcdConfigurationProtocolCookie;
class CNcdConfigurationProtocolCookie;

/**
 * Cached server specific details
 */
class CNcdServerDetails : public CBase, 
                          public MNcdServerDetails
    {
public:
    // Creator
    static CNcdServerDetails* NewLC(
        const TDesC& aServerUri,
        const TDesC& aNamespace );
        
    static CNcdServerDetails* NewL(
        RReadStream& aStream );
    
    // Destructor
    ~CNcdServerDetails();
    
    void ExternalizeL( RWriteStream& aStream ) const;

public: // MNcdServerDetails            

    /**
     * @see MNcdServerDetails::AddCapabilityL()
     */
    void AddCapabilityL( const TDesC& aCapability );


    /**
     * @see MNcdServerDetails::ClearCapabilitiesL()
     */
    void ClearCapabilitiesL();
    
    
    /**
     * @see MNcdServerDetails::IsCapabilitySupported()
     */
    TBool IsCapabilitySupported( const TDesC& aCapability ) const;
    
    /**
     * @see MNcdServerDetails::ServerUri()
     */
    const TDesC& ServerUri() const;            
    
    /**
     * @see MNcdServerDetails::Namespace()
     */
    const TDesC& Namespace() const;
    
    
    /**
     * @see MNcdServerDetails::AddCookieL()
     */
    void AddCookieL( 
        const MNcdConfigurationProtocolCookie& aCookie,
        const TDesC8& aSim );
        
   
    /**
     * @see MNcdServerDetails::CookiesL()
     */        
    RPointerArray<const MNcdConfigurationProtocolCookie> CookiesL(
        const TDesC8& aSim ) const;
    
    
    /**
     * @see MNcdServerDetails::RemoveExpiredCookies()
     */
    TInt RemoveExpiredCookies();
    
    
private:

    // Constructor
    CNcdServerDetails();
    
    // 2nd-phase constructor
    void ConstructL( const TDesC& aServerUri, const TDesC& aNamespace );
    void ConstructL( RReadStream& aStream );
    
    TInt FindCookie( const
        MNcdConfigurationProtocolCookie& aCookie ) const;
    

private:

    // Server URI
    HBufC* iServerUri;
    HBufC* iNamespace;
    
    // Server's capabilities
    CDesCArrayFlat* iCapabilities;
    RPointerArray<CNcdConfigurationProtocolCookie> iCookies;
    
    };

#endif // C_NCDSERVERDETAILS_H
