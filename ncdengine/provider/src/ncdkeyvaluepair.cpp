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
* Description:   Implementation of CNcdKeyValuePair
*
*/


#include <s32strm.h> 
#include <s32mem.h>

#include "ncdutils.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//    
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewL( const TDesC& aKey,
    const TDesC& aValue )
    {
    CNcdKeyValuePair* self = CNcdKeyValuePair::NewLC( aKey, aValue );
    CleanupStack::Pop();
    return self;
    }
    

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewLC( const TDesC& aKey, 
    const TDesC& aValue )
    {
    CNcdKeyValuePair* self = new( ELeave ) CNcdKeyValuePair;
    CleanupStack::PushL( self );
    self->ConstructL( aKey, aValue );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewL( 
    const MNcdKeyValuePair& aOther )
    {
    // Using normal constructor
    CNcdKeyValuePair* self = CNcdKeyValuePair::NewLC( aOther );
    CleanupStack::Pop( self );
    return self;
    }    

// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewLC( 
    const MNcdKeyValuePair& aOther )
    {
    // Using normal constructor
    CNcdKeyValuePair* self = new ( ELeave ) CNcdKeyValuePair();
    CleanupStack::PushL( self );
    self->ConstructL( aOther.Key(), aOther.Value() );
    return self;
    }    


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//	
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewL( RReadStream& aStream )
    {
    CNcdKeyValuePair* self = CNcdKeyValuePair::NewLC( aStream );
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//	
EXPORT_C CNcdKeyValuePair* CNcdKeyValuePair::NewLC( RReadStream& aStream )
    {
    CNcdKeyValuePair* self = new( ELeave ) CNcdKeyValuePair;
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdKeyValuePair::~CNcdKeyValuePair()
    {
    delete iKey;
    delete iValue;
    }
    

// ---------------------------------------------------------------------------
// Key
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdKeyValuePair::Key() const
    {
    DASSERT( iKey );
    return *iKey;
    }


// ---------------------------------------------------------------------------
// SetKeyL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CNcdKeyValuePair::SetKeyL( const TDesC& aKey )
    {
    if ( !aKey.Length() ) 
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aKey.AllocL();
    delete iKey;
    iKey = temp;
    }


// ---------------------------------------------------------------------------
// Value
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdKeyValuePair::Value() const
    {
    DASSERT( iValue );
    return *iValue;        
    }


// ---------------------------------------------------------------------------
// SetValueL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CNcdKeyValuePair::SetValueL( const TDesC& aValue )
    {
    HBufC* temp = aValue.AllocL();
    delete iValue;
    iValue = temp;
    }


// ---------------------------------------------------------------------------
// Matcher
// ---------------------------------------------------------------------------
//	
EXPORT_C TBool CNcdKeyValuePair::MatchByKey( const CNcdKeyValuePair& aFirst, 
    const CNcdKeyValuePair& aSecond )
    {
    return ( aFirst.Key().Compare( aSecond.Key() ) == 0 );
    }


// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CNcdKeyValuePair::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeDesL( *iKey, aStream );
    ExternalizeDesL( *iValue, aStream );
    }

// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C HBufC8* CNcdKeyValuePair::ExternalizeToDesLC() const
    {
    HBufC8* ext = HBufC8::NewLC( 2 * ( sizeof(TInt) + iKey->Length() + 
        iValue->Length() ) );
    TPtr8 ptr( ext->Des() );
    RDesWriteStream stream( ptr );        
    CleanupClosePushL( stream );
    ExternalizeDesL( *iKey, stream );
    ExternalizeDesL( *iValue, stream );    
    CleanupStack::PopAndDestroy( &stream );
    
    return ext;
    }
    
// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CNcdKeyValuePair::InternalizeL( RReadStream& aStream )
    {
    InternalizeDesL( iKey, aStream );
    InternalizeDesL( iValue, aStream );
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CNcdKeyValuePair::ConstructL( const TDesC& aKey, const TDesC& aValue )
    {
    if ( !aKey.Length() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iKey = aKey.AllocL();
    iValue = aValue.AllocL();
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CNcdKeyValuePair::CNcdKeyValuePair()
    {
    }



