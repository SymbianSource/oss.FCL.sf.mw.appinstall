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
* Description:   Contains CNcdDbMaxSizeMap class
*
*/


#include "ncddbmaxsizemap.h"
#include "catalogsdebug.h"



CNcdDbMaxSizeMap* CNcdDbMaxSizeMap::NewL( const TInt aDefaultMaxSize )
    {
    CNcdDbMaxSizeMap* self = 
        CNcdDbMaxSizeMap::NewLC( aDefaultMaxSize );
    CleanupStack::Pop( self );
    return self;
    }

CNcdDbMaxSizeMap* CNcdDbMaxSizeMap::NewLC( const TInt aDefaultMaxSize )
    {
    CNcdDbMaxSizeMap* self =
        new(ELeave) CNcdDbMaxSizeMap( aDefaultMaxSize);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdDbMaxSizeMap::CNcdDbMaxSizeMap(  const TInt aDefaultMaxSize )
: KElementWasNotFound( -1 ),
  iDefaultMaxSize( aDefaultMaxSize )
    {
    // Nothing to do here
    }

void CNcdDbMaxSizeMap::ConstructL()
    {
    // Nothing to do here
    }
    
CNcdDbMaxSizeMap::~CNcdDbMaxSizeMap()
    {
    // Release the arrays
    iKeyArray.Reset();
    iValueArray.Reset();
    }


TInt CNcdDbMaxSizeMap::SetValue( const TUid& aKey,
                                 TInt aValue )
    {
    DLTRACEIN((""));
    
    TInt error( KErrNone );
    TInt keyIndex( KeyIndex( aKey ) );
    
    if ( keyIndex != KElementWasNotFound )
        {
        DLINFO(("Key element already exists."))
        // Because the key was already in the array
        // we also got the value index. So, just replace
        // the old value by the new one.
        iValueArray[ keyIndex ] = aValue;
        }
    else
        {
        DLINFO(("Key element was not found. Create new pair."));
        // The key was not found. So new key and corresponding
        // value has to be set.
        error = iKeyArray.Append( aKey );
        if ( error != KErrNone )
            {
            // Do not do anything else because appending
            // could not be done.
            DLERROR(("Key array append error: %d", error));
            DLTRACEOUT((""));
            return error;
            }

        // Key was appended succesfully
        error = iValueArray.Append( aValue );            
        if ( error != KErrNone )
            {
            DLERROR(("value array appenderror: %d", error));
            
            // Value could not be appended
            // Remove the key that was appended above
            // from the end of thekey array 
            // because we could not append the corresponding 
            // value. This way the key and value pairs will
            // be in sync.
            iKeyArray.Remove( iKeyArray.Count() - 1 );
            
            // Leave with the given value.
            DLTRACEOUT((""));
            return error;
            }
        }

    DLTRACEOUT((""));
        
    return error;
    }


TInt CNcdDbMaxSizeMap::Value( const TUid& aKey ) const
    {
    DLTRACEIN((""));
    
    TInt index( KeyIndex( aKey ) );
    
    if ( index != KElementWasNotFound )
        {
        // Index was found. So, return the value.
        // Notice that key value has always the corresponding
        // value in the value array. The indexes are same for
        // the key and the value.
        DLTRACEOUT(("Value index found: %d", index));
        return iValueArray[ index ];
        }
    else
        {
        // The key was not found. So, return the default.
        DLTRACEOUT(("Value index not found. Use default: %d", iDefaultMaxSize));
        return iDefaultMaxSize;
        }
    }


TInt CNcdDbMaxSizeMap::KeyIndex( const TUid& aKey ) const
    {
    for ( TInt i = 0; i < iKeyArray.Count(); ++i )
        {
        if ( iKeyArray[ i ] == aKey )
            {
            // Correct key was found.
            // So, return the index.
            return i;
            }
        }
        
    // Key was not found.
    return KElementWasNotFound;
    }
