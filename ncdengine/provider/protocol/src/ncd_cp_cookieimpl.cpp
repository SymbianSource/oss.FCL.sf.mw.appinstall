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
* Description:   Implementation of CNcdCookieHandler
*
*/


#include "ncd_cp_cookieimpl.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdConfigurationProtocolCookie::CNcdConfigurationProtocolCookie()
    {
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdConfigurationProtocolCookie* 
    CNcdConfigurationProtocolCookie::NewL()
    {
    CNcdConfigurationProtocolCookie* self = new(ELeave) 
        CNcdConfigurationProtocolCookie();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdConfigurationProtocolCookie* 
    CNcdConfigurationProtocolCookie::NewLC()
    {
    CNcdConfigurationProtocolCookie* self = 
        new (ELeave) CNcdConfigurationProtocolCookie();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------
//
CNcdConfigurationProtocolCookie* 
    CNcdConfigurationProtocolCookie::NewLC( 
    const MNcdConfigurationProtocolCookie& aCookie )
    {
    CNcdConfigurationProtocolCookie* self = 
        new (ELeave) CNcdConfigurationProtocolCookie();
    CleanupStack::PushL( self );
    self->ConstructL( aCookie );
    return self;    
    }

                                             
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdConfigurationProtocolCookie::ConstructL()
    {
    NcdProtocolUtils::AssignEmptyDesL( iKey );
    NcdProtocolUtils::AssignDesL( iType, 
        NcdConfigurationProtocolCookieTypes::KSaveAndSend );
    NcdProtocolUtils::AssignDesL( iScope, 
        NcdConfigurationProtocolCookieScopes::KClient );
    NcdProtocolUtils::AssignEmptyDesL( iSim );
    }


// ---------------------------------------------------------------------------
// ConstructL 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationProtocolCookie::ConstructL( 
    const MNcdConfigurationProtocolCookie & aCookie )
    {
    NcdProtocolUtils::AssignDesL( iKey, aCookie.Key() );
    NcdProtocolUtils::AssignDesL( iType, aCookie.Type() );
    NcdProtocolUtils::AssignDesL( iScope, aCookie.Scope() );
    iExpirationDelta = aCookie.ExpirationDelta();
    iValues.ReserveL( aCookie.ValueCount() );
    for ( TInt i = 0; i < aCookie.ValueCount(); ++i )
        {
        HBufC* value = aCookie.Value( i ).AllocL();
        iValues.Append( value );
        }
    NcdProtocolUtils::AssignDesL( iSim, aCookie.Sim() );
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdConfigurationProtocolCookie::~CNcdConfigurationProtocolCookie()
    {
    delete iKey;
    delete iType;
    delete iScope;
    iValues.ResetAndDestroy();
    delete iSim;
    }

// ---------------------------------------------------------------------------
// Key
// ---------------------------------------------------------------------------
//
const TDesC& CNcdConfigurationProtocolCookie::Key() const
    {
    DASSERT( iKey );
    return *iKey;
    }

// ---------------------------------------------------------------------------
// Type
// ---------------------------------------------------------------------------
//
const TDesC& CNcdConfigurationProtocolCookie::Type() const
    {
    DASSERT( iType );
    return *iType;
    }


// ---------------------------------------------------------------------------
// Scope
// ---------------------------------------------------------------------------
//
const TDesC& CNcdConfigurationProtocolCookie::Scope() const
    {
    DASSERT( iScope );
    return *iScope;
    }

// ---------------------------------------------------------------------------
// ExpirationDelta
// ---------------------------------------------------------------------------
//
TInt CNcdConfigurationProtocolCookie::ExpirationDelta() const
    {
    return iExpirationDelta;
    }


// ---------------------------------------------------------------------------
// ValueCount
// ---------------------------------------------------------------------------
//
TInt CNcdConfigurationProtocolCookie::ValueCount() const
    {
    return iValues.Count();
    }

// ---------------------------------------------------------------------------
// Value
// ---------------------------------------------------------------------------
//
const TDesC& CNcdConfigurationProtocolCookie::Value( TInt aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < ValueCount() );
    return *( iValues[aIndex]);
    }


// ---------------------------------------------------------------------------
// Sim
// ---------------------------------------------------------------------------
//
const TDesC8& CNcdConfigurationProtocolCookie::Sim() const
    {
    return *iSim;
    }


// ---------------------------------------------------------------------------
// Expiration time getter
// ---------------------------------------------------------------------------
//
TTime CNcdConfigurationProtocolCookie::ExpirationTime() const
    {
    return iExpirationTime;
    }
    
    
// ---------------------------------------------------------------------------
// Identity
// ---------------------------------------------------------------------------
//
TBool CNcdConfigurationProtocolCookie::Identity( 
    const MNcdConfigurationProtocolCookie& aFirst,
    const MNcdConfigurationProtocolCookie& aSecond )
    {
    return aFirst.Key() == aSecond.Key();
    }

// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//

void CNcdConfigurationProtocolCookie::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    DASSERT( iKey );    
    DASSERT( iType );
    DASSERT( iScope );
    ExternalizeDesL( *iKey, aStream ); 
    
    DLTRACE(("Externalizing cookie values"));
    // Externalize values
    aStream.WriteInt32L( ValueCount() );
    for ( TInt i = 0; i < ValueCount(); ++i ) 
        {
        ExternalizeDesL( Value( i ), aStream );
        }
    
    DLTRACE(("Cookie values externalized"));
    ExternalizeDesL( *iType, aStream );
    ExternalizeDesL( *iScope, aStream );
    ExternalizeDesL( *iSim, aStream );
    aStream.WriteInt32L( iExpirationDelta );
    aStream << iExpirationTime.Int64();
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//
void CNcdConfigurationProtocolCookie::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    InternalizeDesL( iKey, aStream );
    
    TInt valueCount = aStream.ReadInt32L();
    iValues.ResetAndDestroy();
    iValues.ReserveL( valueCount );
    
    DLTRACE(( "Internalizing cookie values, count: %d", valueCount ));
    for ( TInt i = 0; i < valueCount; ++i )
        {
        HBufC* value = NULL;
        InternalizeDesL( value, aStream );
        // "Can't" fail
        iValues.Append( value );        
        }
    DLTRACE(( "Cookies values internalized" ));
    
    InternalizeDesL( iType, aStream );
    InternalizeDesL( iScope, aStream );
    InternalizeDesL( iSim, aStream );
    iExpirationDelta = aStream.ReadInt32L();
    
    // Read expiration time
    TInt64 time( 0 );
    aStream >> time;
    iExpirationTime = TTime( time );
    DLTRACEOUT((""));
    }

