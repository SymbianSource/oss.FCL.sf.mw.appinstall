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

#include "catalogshttpheadersimpl.h"
#include "catalogsutils.h"
#include "catalogskeyvaluepair.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CCatalogsHttpHeaders* CCatalogsHttpHeaders::NewL()
    {
    return new ( ELeave ) CCatalogsHttpHeaders();
    }


// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	    
CCatalogsHttpHeaders* CCatalogsHttpHeaders::NewL( const 
    CCatalogsHttpHeaders& aOther )
    {
    // Constructor does not copy anything. 
    // All the copying is done in ConstructL
    CCatalogsHttpHeaders* self = new ( ELeave )
        CCatalogsHttpHeaders();
    CleanupStack::PushL( self );
    self->ConstructL( aOther );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	        
CCatalogsHttpHeaders* CCatalogsHttpHeaders::NewL( RReadStream& aStream )
    {
    CCatalogsHttpHeaders* self = new ( ELeave ) CCatalogsHttpHeaders();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Cloner
// ---------------------------------------------------------------------------
//	        
CCatalogsHttpHeaders* CCatalogsHttpHeaders::CloneL() const
    {
    return NewL( *this );    
    }
  

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpHeaders::~CCatalogsHttpHeaders()
    { 
    iHeaders.ResetAndDestroy();
    }
    
    
// ---------------------------------------------------------------------------
// Header setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpHeaders::AddHeaderL( const TDesC8& aHeader, 
    const TDesC8& aValue )
    {
    DLTRACEIN( ("") );
    // Remove existing header with the same key if any
    TInt err = RemoveHeader( aHeader );
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    // Add the replacement
    CCatalogsKeyValuePair* header = CCatalogsKeyValuePair::NewLC( 
        aHeader, aValue );
    User::LeaveIfError( iHeaders.Append( header ) );
    CleanupStack::Pop( header );
    DLTRACEOUT((""));
    }

        
// ---------------------------------------------------------------------------
// Header remover
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpHeaders::RemoveHeader( const TDesC8& aHeader )
    {
    DLTRACEIN((""));    
    if ( !iHeaders.Count() ) 
        {
        return KErrNotFound;
        }
        
    TRAPD(err,
        {                
        err = KErrNotFound;
        TIdentityRelation<CCatalogsKeyValuePair> match( 
            CCatalogsKeyValuePair::MatchByKey );

        CCatalogsKeyValuePair* pair = CCatalogsKeyValuePair::NewLC( aHeader, 
            KNullDesC8 );    

        TInt index = iHeaders.Find( pair, match );

        if ( index != KErrNotFound )
            {
            CCatalogsKeyValuePair* removed = iHeaders[index];
            iHeaders.Remove( index );
            delete removed;

            err = KErrNone;
            }

        CleanupStack::PopAndDestroy( pair );
        
        } ); //TRAPD    
    return err;    
    }


// ---------------------------------------------------------------------------
// Header setter
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpHeaders::AddHeaderL( const TDesC16& aHeader, 
    const TDesC16& aValue )
    {    
    DLTRACEIN((""));
    // Remove existing header with the same key if any
    TInt err = RemoveHeader( aHeader );
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    // Add the replacement
    CCatalogsKeyValuePair* header = CCatalogsKeyValuePair::NewLC( 
        aHeader, aValue );
    User::LeaveIfError( iHeaders.Append( header ) );
    CleanupStack::Pop( header );    
    }


// ---------------------------------------------------------------------------
// Header remover
// ---------------------------------------------------------------------------
//	
TInt CCatalogsHttpHeaders::RemoveHeader( const TDesC16& aHeader )
    {
    DLTRACEIN((""));    
    HBufC8* header = NULL;
    TRAPD( err, header = ConvertUnicodeToUtf8L( aHeader ) );  
    if ( err == KErrNone ) 
        {        
        err = RemoveHeader( *header );        
        }
    delete header;
    return err;
    }
           

// ---------------------------------------------------------------------------
// Header list getter
// ---------------------------------------------------------------------------
//	            
RPointerArray<CCatalogsKeyValuePair>& CCatalogsHttpHeaders::Headers()
    {
    return iHeaders;
    }


// ---------------------------------------------------------------------------
// Header list getter
// ---------------------------------------------------------------------------
//	            
const RPointerArray<CCatalogsKeyValuePair>& CCatalogsHttpHeaders::Headers() 
    const
    {
    return iHeaders;
    }


// ---------------------------------------------------------------------------
// Search for a header by key
// ---------------------------------------------------------------------------
//	            
const TDesC8& CCatalogsHttpHeaders::HeaderByKeyL( const TDesC8& aKey ) const
    {
    CCatalogsKeyValuePair* pair = CCatalogsKeyValuePair::NewLC( aKey, KNullDesC8 );
    const TDesC8& foundHeader = FindHeaderL( *pair );
    CleanupStack::PopAndDestroy( pair );
    return foundHeader;
    }


// ---------------------------------------------------------------------------
// Search for a header by key
// ---------------------------------------------------------------------------
//	            
const TDesC8& CCatalogsHttpHeaders::HeaderByKeyL( const TDesC16& aKey ) const
    {
    CCatalogsKeyValuePair* pair = CCatalogsKeyValuePair::NewLC( aKey, KNullDesC16 );
    const TDesC8& foundHeader = FindHeaderL( *pair );
    CleanupStack::PopAndDestroy( pair );
    return foundHeader;
    }


// ---------------------------------------------------------------------------
// Externalize
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpHeaders::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    aStream.WriteInt32L( iHeaders.Count() );
    for ( TInt i = 0; i < iHeaders.Count(); ++i )
        {
        iHeaders[i]->ExternalizeL( aStream );
        }
    }


// ---------------------------------------------------------------------------
// Internalize
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpHeaders::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    TInt32 count = aStream.ReadInt32L();
    iHeaders.ResetAndDestroy();
    
    iHeaders.ReserveL( count );
    for ( ; count > 0; --count )
        {
        CCatalogsKeyValuePair* pair = CCatalogsKeyValuePair::NewL( aStream );
        
        // Can't fail
        iHeaders.Append( pair );        
        }
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CCatalogsHttpHeaders::CCatalogsHttpHeaders()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CCatalogsHttpHeaders::ConstructL( const CCatalogsHttpHeaders& aOther )
    {
    DLTRACEIN( ("") );
    for( TInt i = 0; i < aOther.iHeaders.Count(); ++i ) 
        {
        // Create a copy of the header and add it to the new header list
        CCatalogsKeyValuePair* pair = CCatalogsKeyValuePair::NewLC( 
            *aOther.iHeaders[i] );        
        iHeaders.AppendL( pair );
        CleanupStack::Pop( pair );
        }
    DLTRACEOUT( ( "") );
    }


// ---------------------------------------------------------------------------
// Searches for the given key and returns the matching value
// ---------------------------------------------------------------------------
//	
const TDesC8& CCatalogsHttpHeaders::FindHeaderL( 
    CCatalogsKeyValuePair& aPair ) const
    {
    TIdentityRelation<CCatalogsKeyValuePair> match( 
        CCatalogsKeyValuePair::MatchByKey );
    
    TInt index = iHeaders.Find( &aPair, match );

    if ( index == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    return iHeaders[index]->Value();        
    }
