/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateHistory
*
*/



// NCD headers:
#include <ncdprovider.h>
#include <ncdnode.h>
#include <ncdnodecontentinfo.h>
// Purchase history contains items
#include <ncdpurchasehistory.h>
// Contains CBase-class headers of the ncd purchase classes.
#include <ncdutils.h>


#include "iaupdatehistoryimpl.h"
#include "iaupdatehistoryitemimpl.h"
#include "iaupdatenodefactory.h"



CIAUpdateHistory* CIAUpdateHistory::NewL( const TUid& aFamilyUid,
                                          MNcdProvider& aProvider )
    {
    CIAUpdateHistory* self = 
        CIAUpdateHistory::NewLC( aFamilyUid, aProvider );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateHistory* CIAUpdateHistory::NewLC( const TUid& aFamilyUid,
                                           MNcdProvider& aProvider )
    {
    CIAUpdateHistory* self = 
        new( ELeave ) CIAUpdateHistory( aFamilyUid, aProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateHistory::CIAUpdateHistory( const TUid& aFamilyUid,
                                    MNcdProvider& aProvider )
: CBase(),
  iFamilyUid( aFamilyUid ),
  iProvider( aProvider )
    {
    
    }
    
void CIAUpdateHistory::ConstructL()
    {
    // Get the purchase history from the provider.
    // Leave if it can not be gotten.
    iHistory = Provider().PurchaseHistoryL();
    
    // The items from the NCD Engine purchase history 
    // are not fetched here.
    // Instead the user of this class has to call RefreshL to 
    // get and to update the list.
    }


CIAUpdateHistory::~CIAUpdateHistory()
    {
    iItems.ResetAndDestroy();
    History().Release();
    }


const RPointerArray< MIAUpdateHistoryItem >& CIAUpdateHistory::Items() const
    {
    return iItems;
    }


void CIAUpdateHistory::RefreshL()
    {
    iItems.ResetAndDestroy();

    // Create filter. So, we will get
    // all the purchase history items.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();

    // Add family uid to the filter
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    uids.AppendL( FamilyUid() );
    filter->SetClientUids( uids.Array() );
    CleanupStack::PopAndDestroy( &uids );
    
    // Get the ids. So, we can next get all the corresponding
    // details.
    RArray< TUint > ids =
        History().PurchaseIdsL( *filter );
    CleanupStack::PopAndDestroy( filter );
    CleanupClosePushL( ids );
    
    // Get all the details and add corresponding history items 
    // into the array.
    // Notice, that pruneSelfUpdates is not required anymore for
    // the current version of IAD but it is here just in case
    // purchase history contains old information and this kind
    // of checking is needed for them. pruneSelfUpdates flag 
    // informs if other self update items in the list should 
    // be skipped because IAD has already been inserted in the 
    // list.
    TBool pruneSelfUpdates( EFalse );
    for ( TInt i = 0; i < ids.Count(); ++i )
        {
        // We do not want to load icons. So, use EFalse.
        // Ownership is transferred here.
        CNcdPurchaseDetails* details(
            History().PurchaseDetailsL( ids[ i ], EFalse ) );

        
        // Purchase history detail contains the UID information.
        // So, use it here.
        TUid itemUid(
            TUid::Uid( 
                details->AttributeInt32( 
                  MNcdPurchaseDetails::EPurchaseAttributeContentUid ) ) );
       
        if ( IAUpdateNodeFactory::IsSelfUpdate( itemUid )
             || IAUpdateNodeFactory::IsUpdater( itemUid ) 
             || AcceptItem( *details ) )
            {
            if ( !pruneSelfUpdates )
                {
                // If pruning is not on, then all the items are accepted.
                // This takes the ownership of details.
                InsertItemL( details );                       

                if ( IAUpdateNodeFactory::IsIad( itemUid ) )
                    {
                    // IAD was found, but because pruning was not on yet,
                    // set it on.
                    pruneSelfUpdates = ETrue;
                    }
                }
            else if ( !IAUpdateNodeFactory::IsSelfUpdate( itemUid )
                      && !IAUpdateNodeFactory::IsUpdater( itemUid ) )
                {
                // If pruning is on, then accept only items that are not
                // self update related. This takes the ownership of details.
                InsertItemL( details );
                }
            else
                {
                // Item was not inserted to the array. So, delete it.
                delete details;
                details = NULL;
                }            
            }
        else
            {
            // Item was not accepted into the history.
            // So, just skip it.
            delete details;
            details = NULL;
            }
        }
    
    CleanupStack::PopAndDestroy( &ids );
    }


MNcdProvider& CIAUpdateHistory::Provider()
    {
    return iProvider;
    }

    
MNcdPurchaseHistory& CIAUpdateHistory::History()
    {
    return *iHistory;
    }


const TUid& CIAUpdateHistory::FamilyUid() const
    {
    return iFamilyUid;    
    }

  
void CIAUpdateHistory::InsertItemL( MNcdPurchaseDetails* aDetails )
    {
    if ( !aDetails )
        {
        // Nothing to do with NULL object.
        return;
        }

    CleanupStack::PushL( aDetails );
    CIAUpdateHistoryItem* item( 
        CIAUpdateHistoryItem::NewL( aDetails, Provider() ) );
    CleanupStack::Pop( aDetails );
    CleanupStack::PushL( item );
    
    if ( iItems.Count() == 0 )
        {
        // Array is empty. 
        // So, just append new item.
        iItems.AppendL( item );
        CleanupStack::Pop( item );
        return;
        }

    // Check if there exists an older version of the item
    // in the array.
    for ( TInt i = 0; i < iItems.Count(); ++i )
        {
        // Casting here is safe thing to do because this function
        // is the only one that creates the array items and the items
        // are created as CIAUpdateHistoryItem objects.
        CIAUpdateHistoryItem* arrayItem(
            static_cast< CIAUpdateHistoryItem* >( iItems[ i ] ) );
            
        // Get the purchase detail object from the arrayItem.
        // So, we can check the detail identifications.
        const MNcdPurchaseDetails& arrayItemDetails( arrayItem->Details() );

        if ( aDetails->EntityId() == arrayItemDetails.EntityId()
             && aDetails->Namespace() == arrayItemDetails.Namespace()
             && aDetails->ClientUid() == arrayItemDetails.ClientUid() )
            {
            // We have two same items. So, compare their operation times.
            if ( aDetails->LastOperationTime() 
                 > arrayItemDetails.LastOperationTime() )
                {
                // The array item was older for the same purhcase detail.
                // So, remove the old version.
                delete iItems[ i ];
                iItems[ i ] = NULL;
                iItems.Remove( i );
                
                // Continue to the phase were the current item will be
                // inserted to the correct place in the array.
                break;
                }
            else
                {
                // Let the array item be in the array because its newer.
                // Just delete the unnecessary new one.
                CleanupStack::PopAndDestroy( item );

                // Nothing to do here anymore.
                return;
                }
            }
        }

    // Insert the item into a correct place in the array.
    for ( TInt i = 0; i < iItems.Count(); ++i )
        {
        MIAUpdateHistoryItem* arrayItem( iItems[ i ] );

        TTime itemTime( item->LastOperationTime() );
        TDateTime itemDateTime( itemTime.DateTime() );
        TTime arrayItemTime( arrayItem->LastOperationTime() );
        TDateTime arrayItemDateTime( arrayItemTime.DateTime() );
        
        // The right place to insert the item is
        // 1.1. Item in the array is from previous day than this item.
        // 1.2. Items are from the same day. 
        //      Item in the array is not error item, but this item is.
        //      (Error items first)
        // 1.3. Items are from the same day.
        //      If item in array and this are same type, but the time of
        //      this item is later. (Latest ones first)
        
        TBool arrayItemPreviousDay( 
            itemDateTime.Day() > arrayItemDateTime.Day()
                && itemDateTime.Month() == arrayItemDateTime.Month()
                && itemDateTime.Year() == arrayItemDateTime.Year()
            || itemDateTime.Month() > arrayItemDateTime.Month()
                && itemDateTime.Year() == arrayItemDateTime.Year()
            || itemDateTime.Year() > arrayItemDateTime.Year() );

        TBool itemsSameDay( 
            itemDateTime.Day() == arrayItemDateTime.Day()
            && itemDateTime.Month() == arrayItemDateTime.Month()
            && itemDateTime.Year() == arrayItemDateTime.Year() );        
        
        TBool itemSuccessfullyInstalled( 
            item->LastOperationErrorCode() == KErrNone
            && item->StateL() == MIAUpdateHistoryItem::EInstalled );

        TBool arrayItemSuccessfullyInstalled( 
            arrayItem->LastOperationErrorCode() == KErrNone
            && arrayItem->StateL() == MIAUpdateHistoryItem::EInstalled );
        
        if ( arrayItemPreviousDay
             || itemsSameDay
                && ( arrayItemSuccessfullyInstalled 
                        && !itemSuccessfullyInstalled
                     || itemSuccessfullyInstalled == arrayItemSuccessfullyInstalled
                        && itemTime > arrayItemTime ) )
            {
            iItems.InsertL( item, i );
            CleanupStack::Pop( item );       
            return;            
            }
        }

    // If we came here, then the item should be appended to the end
    // of the array, because it was not inserted above.        
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


TBool CIAUpdateHistory::AcceptItem( const CNcdPurchaseDetails& aItem ) const
    {
    TBool accept( ETrue );

    const TDesC& mime( 
        aItem.AttributeString( 
            MNcdPurchaseDetails::EPurchaseAttributeContentMimeType ) );

    if ( IAUpdateNodeFactory::IsFwUpdate( mime )
         || IAUpdateNodeFactory::IsHidden( mime ) )
        {
        // Do not accept firmwares into the history.
        // Do not accept hidden items into the history.
        accept = EFalse;
        }

    return accept;
    }

