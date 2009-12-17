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
* Description:   CNcdKeyValueMap implementation
*
*/


#include <s32strm.h>
#include <s32mem.h>

#include "ncdkeyvaluemap.h"
#include "ncdkeyvaluepair.h"

#include "catalogsdebug.h"
#include "ncdutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap* CNcdKeyValueMap::NewL()
    {
    CNcdKeyValueMap* self = CNcdKeyValueMap::NewLC();
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap* CNcdKeyValueMap::NewLC()
    {
    CNcdKeyValueMap* self = new ( ELeave ) CNcdKeyValueMap();
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Copy creator
// ---------------------------------------------------------------------------
//	    
CNcdKeyValueMap* CNcdKeyValueMap::NewL( const 
    CNcdKeyValueMap& aOther )
    {
    CNcdKeyValueMap* self = new ( ELeave )
        CNcdKeyValueMap( aOther );
    CleanupStack::PushL( self );
    self->ConstructL( aOther );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Cloner
// ---------------------------------------------------------------------------
//	        
CNcdKeyValueMap* CNcdKeyValueMap::CloneL() const
    {
    return NewL( *this );    
    }
  

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap::~CNcdKeyValueMap()
    { 
    iPairs.ResetAndDestroy();
    }
    
    
// ---------------------------------------------------------------------------
// Pair adder
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::AddL( const TDesC& aKey, const TDesC& aValue )
    {    
    DLTRACEIN( ( "" ) );
    
    // Add the replacement
    CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC( 
        aKey, aValue );
    User::LeaveIfError( iPairs.Append( pair ) );
    CleanupStack::Pop( pair );    
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Pair adder
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::AddL( CNcdKeyValuePair* aPair )
    {    
    DLTRACEIN((""));
        
    // Add the replacement
    iPairs.AppendL( aPair );    
    }



// ---------------------------------------------------------------------------
// Pair replacer
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::ReplaceL( CNcdKeyValuePair* aPair )
    {
    DLTRACEIN((""));    
    // Remove existing pair with the same key if any
    TInt err = Remove( aPair->Key() );
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }
    AddL( aPair );    
    }
    

// ---------------------------------------------------------------------------
// Pair remover
// ---------------------------------------------------------------------------
//	
TInt CNcdKeyValueMap::Remove( const TDesC& aKey )
    {
    DLTRACEIN((""));
    TRAPD(err,
        {        
        err = KErrNotFound;
        TIdentityRelation<CNcdKeyValuePair> match( 
            CNcdKeyValuePair::MatchByKey );

        CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC( aKey, 
            KNullDesC );    

        TInt index = iPairs.Find( pair, match );

        if ( index != KErrNotFound )
            {
            CNcdKeyValuePair* removed = iPairs[index];
            iPairs.Remove( index );
            delete removed;

            err = KErrNone;
            }

        CleanupStack::PopAndDestroy( pair );
        
        } ); //TRAPD    
    return err;        
    }
           

// ---------------------------------------------------------------------------
// Pair list getter
// ---------------------------------------------------------------------------
//	            
RPointerArray<CNcdKeyValuePair>& CNcdKeyValueMap::Pairs()
    {
    return iPairs;
    }


// ---------------------------------------------------------------------------
// Pair list getter
// ---------------------------------------------------------------------------
//	            
const RPointerArray<CNcdKeyValuePair>& CNcdKeyValueMap::Pairs() 
    const
    {
    return iPairs;
    }


// ---------------------------------------------------------------------------
// Search for a value by key
// ---------------------------------------------------------------------------
//	            
const TDesC& CNcdKeyValueMap::ValueByKeyL( const TDesC& aKey ) const
    {
    CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC( aKey, 
        KNullDesC );
    return FindValueL( pair );
    }


// ---------------------------------------------------------------------------
// Value exists
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap::KeyValueIndex CNcdKeyValueMap::KeyExists( 
    const TDesC& aKey ) const
    {
    DLTRACEIN(( _L("Key: %S"), &aKey ));
    
    for ( TInt i = 0; i < iPairs.Count(); ++i ) 
        {
        if ( iPairs[i]->Key().Compare( aKey ) == 0 ) 
            {
            DLTRACEOUT(("index: %d", i));
            return i;
            }
        }
        
    DLTRACEOUT(("Key not found"));
    return KErrNotFound;    
    }

// ---------------------------------------------------------------------------
// Returns a value by index
// ---------------------------------------------------------------------------
//	
const TDesC& CNcdKeyValueMap::ValueByIndex( const KeyValueIndex& aIndex ) const
    {
    DASSERT( aIndex >= 0 && aIndex < iPairs.Count() );
    return iPairs[aIndex]->Value();
    }
        

// ---------------------------------------------------------------------------
// Pair exists
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap::KeyValueIndex CNcdKeyValueMap::PairExists( 
    const CNcdKeyValuePair& aPair ) const
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iPairs.Count(); ++i )
        {
        if ( iPairs[i]->Key().Compare( aPair.Key() ) == 0 && 
             iPairs[i]->Value().Compare( aPair.Value() ) == 0 )
            {
            return i;
            }        
        }
    
    DLTRACEOUT(("Key not found"));
    return KErrNotFound;            
    }


// ---------------------------------------------------------------------------
// Reset and destroy
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::ResetAndDestroy()
    {
    iPairs.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::Reset()
    {
    iPairs.Reset();
    }


// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::AppendL( const CNcdKeyValueMap& aMap )
    {
    DLTRACEIN((""));
    const RPointerArray<CNcdKeyValuePair>& pairs( aMap.Pairs() );
    TInt count = pairs.Count();
    iPairs.ReserveL( iPairs.Count() + count );
    for ( TInt i = 0; i < count; ++i )
        {
        CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC( 
            *aMap.iPairs[i] );        
        iPairs.Append( pair );
        CleanupStack::Pop( pair );        
        }
    }


// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iPairs.Count() );
    for ( TInt i = 0; i < iPairs.Count(); ++i )
        {
        iPairs[i]->ExternalizeL( aStream );
        }
    
    }


// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//	    
void CNcdKeyValueMap::ExternalizeL( RBuf8& aTarget ) const
    {
    DLTRACEIN((""));
    TInt totalLength = 0;
    
    // Calculate total needed memory    
    for ( TInt i = 0; i < iPairs.Count(); ++i ) 
        {
        totalLength += iPairs[i]->Key().Length() + iPairs[i]->Value().Length() 
            + 2*sizeof(TInt32);
        }
    
    // 16bits = 2 * 8bits
    totalLength *= 2;
    aTarget.Close();
    
    // Total length of pairs + length of pair count
    aTarget.CreateL( totalLength + sizeof(TInt32) );
    
    // Open a write stream to the buffer
    RDesWriteStream stream( aTarget );
    CleanupClosePushL( stream );
    
    // Write count
    stream.WriteInt32L( iPairs.Count() );
    
    DLTRACE(("Externalize pairs"));
    for ( TInt i = 0; i < iPairs.Count(); ++i ) 
        {
        iPairs[i]->ExternalizeL( stream );
        }
    CleanupStack::PopAndDestroy( &stream );
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//	    
void CNcdKeyValueMap::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    TInt count = aStream.ReadInt32L();
    DLTRACE(("Reading %d pairs", count));
    
    iPairs.ResetAndDestroy();
    if ( count ) 
        {   
        DLTRACE(("Internalizing pairs"));             
        iPairs.ReserveL( count );
        CNcdKeyValuePair* pair = NULL;
        
        // Internalize pairs
        for ( TInt i = 0; i < count; ++i ) 
            {            
            pair = CNcdKeyValuePair::NewL( aStream );
            DLTRACE( ( _L("Key: %S, value: %S"), &pair->Key(), &pair->Value() ));
            // Append cannot fail
            iPairs.Append( pair );
            }
        }
                    
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap::CNcdKeyValueMap()
    {
    }

// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------
//	
CNcdKeyValueMap::CNcdKeyValueMap( 
    const CNcdKeyValueMap& /*aOther*/ )
    {
    // The actual copying is done in ConstructL( aOther )
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CNcdKeyValueMap::ConstructL( const CNcdKeyValueMap& aOther )
    {
    DLTRACEIN( ( _L("") ) );
    for( TInt i = 0; i < aOther.iPairs.Count(); ++i ) 
        {
        // Create a copy of the header and add it to the new header list
        CNcdKeyValuePair* pair = CNcdKeyValuePair::NewLC( 
            *aOther.iPairs[i] );        
        iPairs.AppendL( pair );
        CleanupStack::Pop( pair );
        }
    DLTRACEOUT( ( _L("") ) );
    }


// ---------------------------------------------------------------------------
// Searches for the given key and returns the matching value
// ---------------------------------------------------------------------------
//	
const TDesC& CNcdKeyValueMap::FindValueL( 
    CNcdKeyValuePair* aPair ) const
    {
    TIdentityRelation<CNcdKeyValuePair> match( 
        CNcdKeyValuePair::MatchByKey );
    
    TInt index = iPairs.Find( aPair, match );

    if ( index == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( aPair );
    return iPairs[index]->Value();        
    }
