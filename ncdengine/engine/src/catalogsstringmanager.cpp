/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CCatalogsStringManager class
*
*/


#include "catalogsstringmanager.h"
#include "catalogsrefstring.h"
#include "catalogsconstants.h"

#include "catalogsdebug.h"


CCatalogsStringManager* CCatalogsStringManager::iStringManager = NULL;



// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCatalogsStringManager* CCatalogsStringManager::NewL()
    {
    CCatalogsStringManager* self = new( ELeave ) CCatalogsStringManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Delete
// ---------------------------------------------------------------------------
//
void CCatalogsStringManager::Delete() 
    {
    delete iStringManager;
    iStringManager = NULL;
    }


// ---------------------------------------------------------------------------
// Sets new string manager
// ---------------------------------------------------------------------------
//
void CCatalogsStringManager::SetStringManager( 
    CCatalogsStringManager* aManager ) 
    {
    delete iStringManager;
    iStringManager = aManager;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsStringManager::~CCatalogsStringManager()
    {
    TInt count = iStrings.Count();
    while ( count-- ) 
        {
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
        // Check that every string has refcount 1
        RStringArray& array = iStrings[ count ];    
        TInt arrayCount = array.Count();
        while( arrayCount-- ) 
            {
            DLTRACE(( _L("Unreleased string: %S, refcount: %d"), 
                &array[ arrayCount ]->String(),
                array[ arrayCount ]->RefCount() ));
            }
        DASSERT( array.Count() == 0 );
#endif        
        // Call Release() for each string
        iStrings[ count ].ResetAndDestroy();
        }
    iStrings.Close();
    delete iSearchString;
    
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CCatalogsRefString& CCatalogsStringManager::StringL( 
    TInt aType, 
    const TDesC& aString )
    {    
    RStringArray& array = ArrayL( aType );
            
    TInt index = FindString( array, aString );
    if ( index == KErrNotFound ) 
        {
        
        HBufC* copy = aString.AllocL();
        // Ownership is transferred
        return AddStringL( array, copy );
        }
    
    array[index]->AddRef();
    return *array[index];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CCatalogsRefString& CCatalogsStringManager::StringL( 
    TInt aType, 
    HBufC* aString )
    {
    DASSERT( aString );
    
    CleanupStack::PushL( aString );
    RStringArray& array = ArrayL( aType );
            
    TInt index = FindString( array, *aString );
    if ( index == KErrNotFound ) 
        {
        
        CleanupStack::Pop( aString );
        // Ownership is transferred
        return AddStringL( array, aString );
        }
    
    CleanupStack::PopAndDestroy( aString );
    array[index]->AddRef();
    return *array[index];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsStringManager::ReleaseString( 
    TInt aType,
    const CCatalogsRefString& aString )
    {
    // Only nodeidentifiermanager has a reference, release it entirely
    if ( aString.Release() == 1 ) 
        {
        // The string MUST be in the array and aType must be correct
        DASSERT( iStrings.Count() > aType );
        RStringArray& array = iStrings[ aType ];
        array.Remove( array.FindInOrder( &aString, iOrder ) );
        
        // Final delete
        aString.Release();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsStringManager::ReleaseString( 
    TInt aType,
    const CCatalogsRefString*& aString )
    {
    if ( aString ) 
        {
        ReleaseString( aType, *aString );
        aString = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsStringManager::CCatalogsStringManager() 
    : iOrder( CCatalogsRefString::Order )
    {
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCatalogsStringManager::ConstructL()
    {
    iSearchString = new( ELeave ) CCatalogsRefSearchString( KNullDesC );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCatalogsStringManager::RStringArray& 
    CCatalogsStringManager::ArrayL( TInt aIndex ) 
    {
    while ( aIndex >= iStrings.Count() ) 
        {        
        DLTRACE(("Creating new arrays"));
        RStringArray array( KStringArrayGranularity );
        CleanupClosePushL( array );
        iStrings.AppendL( array );
        CleanupStack::Pop( &array );
        }
    return iStrings[ aIndex ];
    }

    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCatalogsStringManager::FindString( 
    const RStringArray& aArray,
    const TDesC& aString ) const     
    {
    // This is class is a wrapper that is used for searching. It doesn't copy
    // the search string        
    iSearchString->SetString( aString );
    TInt index = aArray.FindInOrder( iSearchString, iOrder );    
    return index;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CCatalogsRefString& CCatalogsStringManager::AddStringL(    
    RStringArray& aArray,
    HBufC* aString )
    {
    // Ownership of aString is transferred. aString is deleted by
    // NewLC if it leaves
    CCatalogsRefString* refString = CCatalogsRefString::NewLC( aString );
    aArray.InsertInOrderL( refString, iOrder );
    CleanupStack::Pop( refString );
    refString->AddRef();
    return *refString;
    }

