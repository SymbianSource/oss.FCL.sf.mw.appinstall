/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUISelectionCache
*                class member functions.
*
*/


// INCLUDE FILES
#include "SisxUISelectionCache.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUISelectionCache::CSisxUISelectionCache
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUISelectionCache::CSisxUISelectionCache()
    {
    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCache::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUISelectionCache::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCache::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUISelectionCache* CSisxUISelectionCache::NewL()
    {
    CSisxUISelectionCache* self = new( ELeave ) CSisxUISelectionCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUISelectionCache::~CSisxUISelectionCache()
    {
    iOptions.ResetAndDestroy();
    iSelections.Reset(); 
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Indicates if language selection is cached.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISelectionCache::LanguageCached() const
    {
    return iLanguageCached;    
    }
       
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Returns cached language.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLanguage CSisxUISelectionCache::Language() const
    {
    return iLanguage;    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Sets the cached language.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISelectionCache::SetLanguage( const TLanguage& aLanguage )
    {
    iLanguage = aLanguage;   
    iLanguageCached = ETrue;    
    }
     
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Indicates if drive selection is cached.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISelectionCache::DriveCached() const
    {
    TBool result( EFalse );
    
    if ( iDrive != 0 )
        {
        result = ETrue;        
        }
    return result;    
    }

        
  
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Returns cached drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDriveUnit CSisxUISelectionCache::Drive() const
    {
    return iDrive;    
    }

        

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Sets the cached drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISelectionCache::SetDrive( TDriveUnit aDrive )
    {
    iDrive = aDrive;    
    }


// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Indicates if options selection is cached. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUISelectionCache::OptionsCached( const RPointerArray<TDesC>& aOptions ) const
    {
    TBool result( EFalse );
    
    if ( iOptions.Count() > 0 && aOptions.Count() == iOptions.Count() )
        {        
        for ( TInt index = 0; index < iOptions.Count(); index++ )
            {
            if ( aOptions[index] != iOptions[index] )
                {
                result = EFalse;
                break;                
                }            
            }        
        result = ETrue;        
        }    

    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached
// Returns cached option selections.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISelectionCache::Options( RArray<TBool>& aSelections )
    {
    for ( TInt index = 0; index < aSelections.Count(); index++ )
        {
        aSelections[index] = iSelections[index];        
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::SetOptionsL
// Sets the cached options.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUISelectionCache::SetOptionsL( const RPointerArray<TDesC>& aOptions, 
                                        const RArray<TBool>& aSelections )
    {
    TInt index( 0 );
    
    iOptions.ResetAndDestroy();
    iSelections.Reset();    

    for ( index = 0; index < aOptions.Count(); index++ )
        {
        HBufC* tmp = aOptions[index]->AllocLC();
        iOptions.Append( tmp );        
        }
    
    for ( index = 0; index < aSelections.Count(); index++ )
        {
        iSelections.Append( aSelections[index] );        
        }
    }


//  End of File  
