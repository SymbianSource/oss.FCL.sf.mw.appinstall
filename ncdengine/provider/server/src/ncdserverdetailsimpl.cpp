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
* Description:   Implementation of CNcdServerDetails
*
*/


#include <e32std.h>

#include "ncdserverdetailsimpl.h"
#include "catalogsdebug.h"
#include "ncd_cp_cookieimpl.h"
#include "ncddatabasestorage.h"
#include "ncdstorageitem.h"
#include "catalogsutils.h"
#include "ncd_cp_cookie.h"
#include "ncd_cp_cookieimpl.h"
#include "catalogsconstants.h"

// ---------------------------------------------------------------------------
// CNcdServerDetails
// ---------------------------------------------------------------------------
//    

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//         
CNcdServerDetails* 
    CNcdServerDetails::NewLC(
            const TDesC& aServerUri, const TDesC& aNamespace )
    {
    CNcdServerDetails* self = new( ELeave ) CNcdServerDetails();
    CleanupStack::PushL( self );
    self->ConstructL( aServerUri, aNamespace );
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CNcdServerDetails* CNcdServerDetails::NewL( RReadStream& aStream ) 
    {
    CNcdServerDetails* self = new( ELeave ) CNcdServerDetails();
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }
        

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//         
CNcdServerDetails::~CNcdServerDetails()
    {
    delete iServerUri;    
    delete iNamespace;
    delete iCapabilities;
    iCookies.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// Externalizes the object
// ---------------------------------------------------------------------------
//
void CNcdServerDetails::ExternalizeL( RWriteStream& aStream ) const 
    {
    DLTRACEIN(( _L("ServerURI: %S, Namespace: %S"), iServerUri,
        iNamespace ));
    ExternalizeDesL( *iServerUri, aStream );
    ExternalizeDesL( *iNamespace, aStream );
    
    TInt count = iCapabilities->MdcaCount();
    DLTRACE(("Externalizing %d capabilities", count ));
    aStream.WriteInt32L( count );
    for ( TInt i = 0 ; i < count; i++ ) 
        {
        DLTRACE(( _L("Cap: %S"), &iCapabilities->MdcaPoint( i ) ));
        ExternalizeDesL( iCapabilities->MdcaPoint( i ), aStream );
        }
    
    count = iCookies.Count();
    DLTRACE(("Externalizing %d cookies", count ));
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; ++i )
        {
        iCookies[i]->ExternalizeL( aStream );
        }
    }                       
        
// ---------------------------------------------------------------------------
// Adds a new capability
// ---------------------------------------------------------------------------
//         
void CNcdServerDetails::AddCapabilityL( 
    const TDesC& aCapability )
    {
    DLTRACEIN(( _L("Server cap: %S to server: %S"), &aCapability,
        iServerUri ));
        
    iCapabilities->AppendL( aCapability );
    }


// ---------------------------------------------------------------------------
// Clears capabilities
// ---------------------------------------------------------------------------
//         
void CNcdServerDetails::ClearCapabilitiesL()
    {
    DLTRACEIN((""));
    delete iCapabilities;
    iCapabilities = NULL;
    iCapabilities = new( ELeave ) CDesCArrayFlat( KListGranularity );    
    DLTRACEOUT(("Capabilities cleared"));
    }
        
        
// ---------------------------------------------------------------------------
// Check if the server supports the given capability
// ---------------------------------------------------------------------------
//         
TBool CNcdServerDetails::IsCapabilitySupported( 
    const TDesC& aCapability ) const
    {
    TInt pos = 0;
    return ( iCapabilities->Find( aCapability, pos, ECmpNormal ) == 0 );
    }


// ---------------------------------------------------------------------------
// Server URI getter
// ---------------------------------------------------------------------------
//         
const TDesC& CNcdServerDetails::ServerUri() const
    {
    DASSERT( iServerUri );
    return *iServerUri;
    }


// ---------------------------------------------------------------------------
// Namespace getter
// ---------------------------------------------------------------------------
//         
const TDesC& CNcdServerDetails::Namespace() const
    {
    DASSERT( iNamespace );
    return *iNamespace;
    }


// ---------------------------------------------------------------------------
// AddCookieL
// ---------------------------------------------------------------------------
//         
void CNcdServerDetails::AddCookieL( 
    const MNcdConfigurationProtocolCookie& aCookie,
    const TDesC8& aSim )
    {
    DLTRACEIN(( _L("Key: %S, type: %S"), &aCookie.Key(), &aCookie.Type() ));
    TInt index = FindCookie( aCookie );
    
    if ( aCookie.Type() == NcdConfigurationProtocolCookieTypes::KRemove ) 
        {
        DLINFO(("Removing cookie"));
        if ( index != KErrNotFound )
            {
            delete iCookies[index];
            iCookies.Remove( index );            
            }
        return;
        }
        
    CNcdConfigurationProtocolCookie* cookie =
        CNcdConfigurationProtocolCookie::NewLC( aCookie );
    
    
    // Set SIM identification
    AssignDesL( cookie->iSim, aSim );
    
    if ( index == KErrNotFound )
        {
        DLINFO(("Adding a new cookie"));
        iCookies.AppendL( cookie );
        }
    else
        {
        DLINFO(("Replacing an existing cookie"));
        delete iCookies[index];
        iCookies[index] = cookie;
        }
    CleanupStack::Pop( cookie );    

    // Convert expiration delta to expiration time and put it to the cookie
    if ( cookie->ExpirationDelta() )
        {        
        DLINFO(("Expiration delta: %d", cookie->ExpirationDelta() ));
        TTimeIntervalMinutes delta( cookie->ExpirationDelta() );
        TTime currentTime;
        currentTime.HomeTime();
        currentTime += delta;
        cookie->iExpirationTime = currentTime;
        }
    }
    

// ---------------------------------------------------------------------------
// CookiesL
// ---------------------------------------------------------------------------
//         
RPointerArray<const MNcdConfigurationProtocolCookie> 
    CNcdServerDetails::CookiesL(
    const TDesC8& aSim ) const
    {
    DLTRACEIN((""));
    RPointerArray<const MNcdConfigurationProtocolCookie> array;
    CleanupClosePushL( array );        
    for ( TInt i = 0; i < iCookies.Count(); ++i )
        {
        if ( iCookies[i]->Scope() != 
            NcdConfigurationProtocolCookieScopes::KSim ||
            iCookies[i]->Sim() == aSim )
            {
            DLTRACE(("Found a suitable cookie"));
            array.AppendL( iCookies[i] );
            }
        }
    CleanupStack::Pop( &array );
    DLTRACEOUT(("Found %d suitable cookies", array.Count() ));
    return array;
    }


// ---------------------------------------------------------------------------
// Removes cookies that have expired
// ---------------------------------------------------------------------------
//         
TInt CNcdServerDetails::RemoveExpiredCookies()
    {
    DLTRACEIN((""));
    
    TInt removed = 0;
    TTime time;
    time.UniversalTime();
    
    TInt i = 0;
    while ( i < iCookies.Count() )
        {
        if ( iCookies[i]->ExpirationDelta() && 
             iCookies[i]->ExpirationTime() < time )
            {
            DLINFO(( _L("Cookie %S has expired"), &iCookies[i]->Key() ));
            delete iCookies[i];
            iCookies.Remove( i );
            removed++;
            }
        else
            {
            ++i;
            }
        }
    DLTRACEOUT(("Removed %d cookies", removed));
    return removed;
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//         
CNcdServerDetails::CNcdServerDetails()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//         
void CNcdServerDetails::ConstructL( 
    const TDesC& aServerUri,
    const TDesC& aNamespace )
    {
    iServerUri = aServerUri.AllocL();
    iNamespace = aNamespace.AllocL();
    iCapabilities = new( ELeave ) CDesCArrayFlat( KListGranularity );
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//         
void CNcdServerDetails::ConstructL( RReadStream& aStream ) 
    {
    DLTRACEIN((""));
    InternalizeDesL( iServerUri, aStream );
    InternalizeDesL( iNamespace, aStream );
    DLTRACE(( _L("ServerURI: %S, Namespace: %S"), iServerUri, iNamespace ));
    iCapabilities = new( ELeave ) CDesCArrayFlat( KListGranularity );
    
    TInt32 capabilityCount = aStream.ReadInt32L();
    DLTRACE(("Internalizing %d capabilities", capabilityCount ));
    
    HBufC* capability( NULL );
    for ( TInt i = 0; i < capabilityCount; i++ ) 
        {        
        InternalizeDesL( capability, aStream );
        CleanupStack::PushL( capability );
        DLTRACE(( _L("Cap: %S"), capability ));
        iCapabilities->AppendL( *capability );
        CleanupStack::PopAndDestroy( capability );
        capability = NULL;
        }
        
    TInt32 cookieCount = aStream.ReadInt32L();
    DLTRACE(("Internalizing %d cookies", cookieCount ));
    iCookies.ReserveL( cookieCount );
    for ( TInt i = 0; i < cookieCount; ++i )
        {
        CNcdConfigurationProtocolCookie* cookie = 
            CNcdConfigurationProtocolCookie::NewLC();
        cookie->InternalizeL( aStream );
        iCookies.AppendL( cookie );
        CleanupStack::Pop( cookie );        
        }
    DLTRACEOUT(("Internalization successful"));
    }


// ---------------------------------------------------------------------------
// Find cookie
// ---------------------------------------------------------------------------
//         
TInt CNcdServerDetails::FindCookie( const
    MNcdConfigurationProtocolCookie& aCookie ) const
    {
    for ( TInt i = 0; i < iCookies.Count(); ++i )
        {
        if ( aCookie.Key() == iCookies[i]->Key() ) 
            {
            return i;
            }
        }
    return KErrNotFound;
    }

