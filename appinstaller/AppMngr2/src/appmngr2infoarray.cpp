/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class for AppInfo and PackageInfo arrays
*
*/


#include "appmngr2infoarray.h"          // CAppMngr2InfoArray
#include <appmngr2debugutils.h>         // FLOG macros

const TInt KGranularity = 32;


// ======== LOCAL FUNCTIONS =========

// ---------------------------------------------------------------------------
// CompareInfoNamesC
// ---------------------------------------------------------------------------
// Compares two InfoBase objects and defines the order how they are displayed
// in the UI (in installed applications list or in installation files list).
// Returns positive integer if aInfo1 > aInfo2 (in defined/alphabetical order),
// zero if aInfo == aInfo2, and negative integer if aInfo1 < aInfo2.
TInt CompareInfoNamesC( const CAppMngr2InfoBase& aInfo1, const CAppMngr2InfoBase& aInfo2 )
    {
    // IsShowOnTop() must denote "less than" to get the item be displayed before others.
    // If IsShowOnTop()==ETrue then item is less than item having IsShowOnTop()==EFalse.
    TInt result = ( aInfo2.IsShowOnTop() - aInfo1.IsShowOnTop() ); 
    if( !result )
        {
        result = aInfo1.Name().CompareC( aInfo2.Name() );
        if( !result )
            {
            // Compare also locations if the names are the same, as otherwise the order of
            // two items having the same name changes in MoveCachedItemsToArrayInOrderL().
            // Memory card location is "greater than" location in phone internal memory.
            result = ( aInfo1.Location() - aInfo2.Location() );
            }
        }
    return result;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::CAppMngr2InfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoArray::CAppMngr2InfoArray( MAppMngr2InfoArrayObserver& aObserver ) :
        iObserver( aObserver), iArray( KGranularity ), iCache( KGranularity ),
        iAlphabeticalOrder( CompareInfoNamesC ), iQuickRefreshes( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::~CAppMngr2InfoArray()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoArray::~CAppMngr2InfoArray()
    {
    iArray.ResetAndDestroy();
    iCache.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::At()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase* CAppMngr2InfoArray::At( TInt aIndex ) const
    {
    if( IsCacheUsed() )
        {
        return iCache[ aIndex ];
        }
    return iArray[ aIndex ];
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::Count()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2InfoArray::Count() const
    {
    if( IsCacheUsed() )
        {
        return iCache.Count();
        }
    return iArray.Count();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::IncrementCacheUseL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::IncrementCacheUseL()
    {
    if( !iUseCache )
        {
        // First time - take the cache in use. This starts adding new
        // items to the CAppMngr2InfoArray. New items are always added
        // to iArray, and iCache is displayed during adding.
        if( iForceCacheUse )
            {
            // Cache already used - initialize iArray for new items.
            iArray.ResetAndDestroy();
            
            // Make sure that observer is notified even if no items
            // are added to iArray (e.g. "last item deleted" -case).
            iArrayChangedObserverNeedsNotification = ETrue;
            }
        else
            {
            // Cache not used - move items to cache and start using it.
            MoveItemsToCacheMaintainingOrderL();
            
            // Reset flag - need to know if cache is forced in use
            // while it is still in use because of this increment.
            iForceCacheUseWhenAddingComplete = EFalse;
            }
        }
    iUseCache++;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::IncrementCacheUseStartingNewRoundL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::IncrementCacheUseStartingNewRoundL()
    {
    if( iUseCache || iForceCacheUse )
        {
        // Cache already used - initialize iArray for new items.
        iArray.ResetAndDestroy();
        iArrayChangedObserverNeedsNotification = ETrue;

        if( iQuickRefreshes )
            {
            // Touch luck - cache is not really used (in order to
            // show the first items quickly on startup) and hence
            // it is now necessary to show empty list to the user.
            // If iObserver would not be notified, there would be
            // USER 130 panics when UI would try to access iArray
            // items that are deleted.
            iObserver.ArrayContentChanged( this, iUseCache + 1 );
            iArrayChangedObserverNeedsNotification = EFalse;
            }
        }
    IncrementCacheUseL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::DecrementCacheUse()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::DecrementCacheUse()
    {
    if( iUseCache > 0 )
        {
        iUseCache--;
    
        if( !iUseCache )
            {
            // Quick refreshes are enabled on startup, but they need to be
            // turned off when the first item adding round is complete.
            // After the first round the cache is used to handle array
            // content changes.
            if( iQuickRefreshes )
                {
                iQuickRefreshes = EFalse;
                }
        
            // If adding is now complete, check if there is a request to
            // force cache on. Note that iForceInUse cannot be set when
            // cache is already in use (because items are added to iArray),
            // otherwise partial list would be in iCache and the rest in
            // iArray. This is why force cache in use is enabled only after
            // the addition is complete.
            if( iForceCacheUseWhenAddingComplete )
                {
                iForceCacheUse = ETrue;
                iForceCacheUseWhenAddingComplete = EFalse;
                }
            }
        
        // Notify UI if cache is not used any more (or quickrefreshing is on)
        if( !IsCacheUsed() )
            {
            NotifyObserver();
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::DisableRefreshNotificationsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::DisableRefreshNotificationsL()
    {
    // Refresh notifications are disabled by forcing cache on. All changes go
    // to the iArray and current iCache content is displayed unitl notifications
    // are enabled again. Observer is not called while refreshes are disabled.
    if( !iForceCacheUse )
        {
        if( iUseCache )
            {
            // Cannot force cache in use now because items are still being added
            // to iArray. This would result partial list to be in iCache and the
            // rest of items in iArray. Later when deletions are enabled, only the
            // items in iArray would be displayed. So, set a flag that forces cache
            // on when item additions are complete, so that observer is notified
            // only after deletions are enabled again.
            iForceCacheUseWhenAddingComplete = ETrue;
            }
        else
            {
            // Set cache in use
            MoveItemsToCacheMaintainingOrderL();
            iForceCacheUse = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::EnableRefreshNotificationsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::EnableRefreshNotificationsL()
    {
    // It is safe to call EnableRefreshNotificationsL() many times, even if
    // notifications have not been disabled first.
    iForceCacheUseWhenAddingComplete = EFalse;
    if( iForceCacheUse )
        {
        iForceCacheUse = EFalse;
        if( !iUseCache )
            {
            // Refresh notifications were disabled by moving items to iCache. If no
            // new items were added to iArray while notifications were disabled,
            // then those moved items must be moved back to iArray again. Otherwise
            // disabling cache would show just an empty list. If new items were added
            // while refresh notifications were disabled, then observer needs
            // notification and cache content can be ignored, as new content is
            // already available in iArray.
            if( iArrayChangedObserverNeedsNotification )
                {
                NotifyObserver();
                }
            else
                {
                MoveCachedItemsToArrayInOrderL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::AddItemInOrderL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::AddItemInOrderL( CAppMngr2InfoBase* aInfo )
    {
    if( aInfo )
        {
        FLOG( "CAppMngr2InfoArray::AddItemInOrderL( %S )", &( aInfo->Name() ) );
        
        // All items are added to iArray and iCache is used while the changes
        // are not wanted to be seen via Count() and At() functions. Observer
        // is notified when the iArray is taken in use. 
        iArray.InsertInOrderAllowRepeatsL( aInfo, iAlphabeticalOrder );
        if( iForceCacheUse || iForceCacheUseWhenAddingComplete )
            {
            iArrayChangedObserverNeedsNotification = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::ImmediateDelete()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::ImmediateDelete( CAppMngr2InfoBase* aInfo )
    {
    FLOG( "CAppMngr2InfoArray::ImmediateDelete( 0x%08x )", aInfo );
    
    RPointerArray<CAppMngr2InfoBase>* array;
    if( IsCacheUsed() )
        {
        array = &iCache;
        }
    else
        {
        array = &iArray;
        }
    
    TBool found = EFalse;
    TInt count = array->Count();
    for( TInt index = 0; index < count && !found; index++ )
        {
        if( ( *array )[ index ] == aInfo )
            {
            array->Remove( index );
            delete aInfo;
            found = ETrue;
            }
        }

    if( found )
        {
        iObserver.ArrayContentChanged( this, iUseCache );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::MoveItemsToCacheMaintainingOrderL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::MoveItemsToCacheMaintainingOrderL()
    {
    // Move items from iArray to iCache maintaining order.
    iCache.ResetAndDestroy();
    TInt count = iArray.Count();
    for( TInt index = count - 1; index >= 0; index-- )
        {
        iCache.InsertL( iArray[ index ], 0 );
        iArray.Remove( index );
        }
    iArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::MoveCachedItemsToArrayInOrderL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::MoveCachedItemsToArrayInOrderL()
    {
    // Move items from iCache to iArray using proper order. There may be
    // new items in iArray hence it is not possible to simply move items
    // as in MoveItemsToCacheMaintainingOrderL().
    TInt count = iCache.Count();
    for( TInt index = count - 1; index >= 0; index-- )
        {
        iArray.InsertInOrderAllowRepeatsL( iCache[ index ], iAlphabeticalOrder );
        iCache.Remove( index );
        }
    iCache.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::NotifyObserver()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoArray::NotifyObserver()
    {
    iObserver.ArrayContentChanged( this, iUseCache );
    iArrayChangedObserverNeedsNotification = EFalse;
    iCache.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoArray::IsCacheUsed()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2InfoArray::IsCacheUsed() const
    {
    // New items are added to iArray and static old content may be displayed
    // from iCache while iArray is being updated. Priorities: 1) cache forced
    // in use, 2) quick refreshes, and 3) normal inc/dec cache usage. If cache
    // is forced in use, it prevents also changes that quick refreshes might
    // otherwise display. If quick refreshing is enabled, then item additions
    // to iArray are visible to observer, although normally iCache would be used.
    if( iForceCacheUse )
        {
        return ETrue;
        }
    if( iQuickRefreshes )
        {
        return EFalse;
        }
    return ( iUseCache > 0 );
    }

