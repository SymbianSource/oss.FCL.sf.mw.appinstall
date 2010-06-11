/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CSisxSifUiSelectionCache saves user-selections so that
*              they can be re-used without displaying the same UI
*              dialog again.
*/


#include "sisxsifuiselectioncache.h"        // CSisxSifUiSelectionCache


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CSisxSifUiSelectionCache::CSisxSifUiSelectionCache()
// -----------------------------------------------------------------------------
//
CSisxSifUiSelectionCache::CSisxSifUiSelectionCache()
    {
    }

// -----------------------------------------------------------------------------
// CSisxSifUiSelectionCache::ConstructL()
// -----------------------------------------------------------------------------
//
void CSisxSifUiSelectionCache::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSisxSifUiSelectionCache::NewL()
// -----------------------------------------------------------------------------
//
CSisxSifUiSelectionCache* CSisxSifUiSelectionCache::NewL()
    {
    CSisxSifUiSelectionCache* self = new( ELeave ) CSisxSifUiSelectionCache();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CSisxSifUiSelectionCache::~CSisxSifUiSelectionCache()
// -----------------------------------------------------------------------------
//
CSisxSifUiSelectionCache::~CSisxSifUiSelectionCache()
    {
    iOptions.ResetAndDestroy();
    iSelections.Reset(); 
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::LanguageCached()
// -----------------------------------------------------------------------------
//
TBool CSisxSifUiSelectionCache::LanguageCached() const
    {
    return iLanguageCached;    
    }
       
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::Language()
// -----------------------------------------------------------------------------
//
TLanguage CSisxSifUiSelectionCache::Language() const
    {
    return iLanguage;    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::SetLanguage()
// -----------------------------------------------------------------------------
//
void CSisxSifUiSelectionCache::SetLanguage( const TLanguage& aLanguage )
    {
    iLanguage = aLanguage;   
    iLanguageCached = ETrue;    
    }
     
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::DriveCached()
// -----------------------------------------------------------------------------
//
TBool CSisxSifUiSelectionCache::DriveCached() const
    {
    TBool result( EFalse );
    
    if( iDrive != 0 )
        {
        result = ETrue;        
        }
    return result;    
    }
  
// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::Drive()
// -----------------------------------------------------------------------------
//
TDriveUnit CSisxSifUiSelectionCache::Drive() const
    {
    return iDrive;    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::SetDrive()
// -----------------------------------------------------------------------------
//
void CSisxSifUiSelectionCache::SetDrive( TDriveUnit aDrive )
    {
    iDrive = aDrive;    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::OptionsCached()
// -----------------------------------------------------------------------------
//
TBool CSisxSifUiSelectionCache::OptionsCached( const RPointerArray<TDesC>& aOptions ) const
    {
    TBool result( EFalse );
    
    if( iOptions.Count() > 0 && aOptions.Count() == iOptions.Count() )
        {        
        for( TInt index = 0; index < iOptions.Count(); ++index )
            {
            if( aOptions[ index ] != iOptions[ index ] )
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
// CSisxUISelectionCacheAppInfo::Options()
// -----------------------------------------------------------------------------
//
void CSisxSifUiSelectionCache::Options( RArray<TBool>& aSelections )
    {
    for( TInt index = 0; index < aSelections.Count(); ++index )
        {
        aSelections[ index ] = iSelections[ index ];
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUISelectionCacheAppInfo::SetOptionsL()
// -----------------------------------------------------------------------------
//
void CSisxSifUiSelectionCache::SetOptionsL( const RPointerArray<TDesC>& aOptions,
        const RArray<TBool>& aSelections )
    {
    TInt index( 0 );
    
    iOptions.ResetAndDestroy();
    iSelections.Reset();    

    for( index = 0; index < aOptions.Count(); ++index )
        {
        HBufC* tmp = aOptions[ index ]->AllocLC();
        iOptions.Append( tmp );        
        }
    
    for( index = 0; index < aSelections.Count(); ++index )
        {
        iSelections.Append( aSelections[ index ] );
        }
    }

