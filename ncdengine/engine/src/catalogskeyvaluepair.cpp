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
* Description:  
*
*/


#include <s32strm.h>
#include <utf.h>

#include "catalogskeyvaluepair.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewL( const TDesC16& aKey, 
    const TDesC16& aValue )
    {
    CCatalogsKeyValuePair* self = NewLC( aKey, aValue );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewLC( const TDesC16& aKey, 
    const TDesC16& aValue )
    {
    CCatalogsKeyValuePair* self = new ( ELeave ) CCatalogsKeyValuePair();
    CleanupStack::PushL( self );
    self->ConstructL( aKey, aValue );

    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewL( const TDesC8& aKey, 
    const TDesC8& aValue )
    {
    CCatalogsKeyValuePair* self = NewLC( aKey, aValue );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewLC( const TDesC8& aKey, 
    const TDesC8& aValue )
    {
    CCatalogsKeyValuePair* self = new ( ELeave ) CCatalogsKeyValuePair();
    CleanupStack::PushL( self );
    self->ConstructL( aKey, aValue );

    return self;
    }


// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewL( 
    const CCatalogsKeyValuePair& aOther )
    {
    CCatalogsKeyValuePair* self = NewLC( aOther );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewLC( 
    const CCatalogsKeyValuePair& aOther )
    {
    // Using normal constructor
    CCatalogsKeyValuePair* self = new ( ELeave ) CCatalogsKeyValuePair();
    CleanupStack::PushL( self );
    self->ConstructL( aOther.Key(), aOther.Value() );
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewL( 
    RReadStream& aStream )
    {
    CCatalogsKeyValuePair* self = CCatalogsKeyValuePair::NewLC( aStream );
    CleanupStack::Pop( self );
    return self;    
    }
    
// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair* CCatalogsKeyValuePair::NewLC( 
    RReadStream& aStream )
    {
    CCatalogsKeyValuePair* self = new ( ELeave ) CCatalogsKeyValuePair();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;    
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair::CCatalogsKeyValuePair()
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsKeyValuePair::~CCatalogsKeyValuePair()
    {
    delete iKey;
    delete iValue;    
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsKeyValuePair::ConstructL( const TDesC16& aKey, 
    const TDesC16& aValue )
    {
    iKey = ConvertUnicodeToUtf8L( aKey );
    iValue = ConvertUnicodeToUtf8L( aValue );
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsKeyValuePair::ConstructL( const TDesC8& aKey, 
    const TDesC8& aValue )
    {
    iKey = aKey.AllocL();
    iValue = aValue.AllocL();    
    }


// ---------------------------------------------------------------------------
// Key getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsKeyValuePair::Key() const
    {
    if ( iKey )
        {
        return *iKey;
        }

    return KNullDesC8;
    }


// ---------------------------------------------------------------------------
// Value getter
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsKeyValuePair::Value() const
    {
    if ( iValue )
        {
        return *iValue;
        }

    return KNullDesC8;
    }


// ---------------------------------------------------------------------------
// Key getter
// ---------------------------------------------------------------------------
//	
HBufC* CCatalogsKeyValuePair::KeyLC() const
    {    
    if( iKey ) 
        {            
        #ifdef _UNICODE
            HBufC* key = ConvertUtf8ToUnicodeL( *iKey );
            CleanupStack::PushL( key );
        #else
            HBufC* key = iKey->AllocLC();
        #endif
        
        return key;
        }
    return KNullDesC().AllocLC();
    }


// ---------------------------------------------------------------------------
// Key getter
// ---------------------------------------------------------------------------
//	
HBufC* CCatalogsKeyValuePair::KeyL() const
    {    
    if( iKey ) 
        {            
        #ifdef _UNICODE
            HBufC* key = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *iKey );
        #else
            HBufC* key = iKey->AllocL();
        #endif
        
        return key;
        }
    return KNullDesC().AllocL();
    }


// ---------------------------------------------------------------------------
// Value getter
// ---------------------------------------------------------------------------
//	
HBufC* CCatalogsKeyValuePair::ValueLC() const
    {
    if ( iValue )
        {
        #ifdef _UNICODE
            HBufC* value = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *iValue );
            CleanupStack::PushL( value );
        #else
            HBufC* value = iValue->AllocLC();
        #endif
        
        return value;
        }

    return KNullDesC().AllocLC();
    }


// ---------------------------------------------------------------------------
// Value getter
// ---------------------------------------------------------------------------
//	
HBufC* CCatalogsKeyValuePair::ValueL() const
    {
    if ( iValue )
        {
        #ifdef _UNICODE
            HBufC* value = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *iValue );
        #else
            HBufC* value = iValue->AllocL();
        #endif
        
        return value;
        }

    return KNullDesC().AllocL();
    }


// ---------------------------------------------------------------------------
// Matcher
// ---------------------------------------------------------------------------
//	
TBool CCatalogsKeyValuePair::MatchByKey( const CCatalogsKeyValuePair& aFirst, 
    const CCatalogsKeyValuePair& aSecond )
    {
    return ( aFirst.Key().Compare( aSecond.Key() ) == 0 );
    }



// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//	
void CCatalogsKeyValuePair::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeDesL( *iKey, aStream );
    ExternalizeDesL( *iValue, aStream );
    }

// ---------------------------------------------------------------------------
// Internalize
// ---------------------------------------------------------------------------
//	
void CCatalogsKeyValuePair::InternalizeL( RReadStream& aStream )
    {
    HBufC8* key = NULL;
    InternalizeDesL( key, aStream );
    CleanupStack::PushL( key );
    
    HBufC8* value = NULL;
    InternalizeDesL( value, aStream );
    
    CleanupStack::Pop( key );
    delete iKey;
    iKey = key;
    
    delete iValue;
    iValue = value;
    }
