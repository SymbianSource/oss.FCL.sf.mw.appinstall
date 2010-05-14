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


#include <e32base.h>

#include "ncdnodepurchaseproxy.h"

#include "ncdpurchaseoptionproxy.h"
#include "ncdpurchaseoperationproxy.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "ncdclientpartofsubscription.h"
#include "ncdclientsubscribablecontent.h"
#include "ncdnodemanagerproxy.h"

#include "ncdnodeproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationimpl.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "catalogsconstants.h"
#include "ncderrors.h"


// ======== MEMBER FUNCTIONS ========


CNcdNodePurchaseProxy::CNcdNodePurchaseProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    : CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
      iMetadata( aMetadata )
    {
    // This would be faster in the construct list but because
    // resetting is needed also elsewhere it is better to keep
    // the reset values only in one place.
    ResetHistoryData();
    }

void CNcdNodePurchaseProxy::ConstructL()
    {
    DLTRACEIN((""));
    
    // Register the interface
    MNcdNodePurchase* purchase( this );
    AddInterfaceL( CCatalogsInterfaceIdentifier::NewL(
                        purchase,
                        this,
                        MNcdNodePurchase::KInterfaceUid ) );
    // Try to internalize already here. And if it fails,
    // then let it leave.
    InternalizeL();

    DLTRACEOUT(("Success. Options: %d", iPurchaseOptions.Count()));
    }


CNcdNodePurchaseProxy* CNcdNodePurchaseProxy::NewL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodePurchaseProxy* self = 
        CNcdNodePurchaseProxy::NewLC( aSession,
                                      aHandle,
                                      aMetadata );
    CleanupStack::Pop( self );
    return self;
    }


CNcdNodePurchaseProxy* CNcdNodePurchaseProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodePurchaseProxy* self =
        new( ELeave ) CNcdNodePurchaseProxy( aSession,
                                             aHandle,
                                             aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodePurchaseProxy::~CNcdNodePurchaseProxy()
    {
    DLTRACEIN((""));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodePurchase::KInterfaceUid );

    delete iSubscribableContent;
    iSubscribableContent = NULL;

    // When purchase is destroyed, also purchaseoptions go
    // with it if no references are left to them.
    DeletePurchaseOptions();
    
    ResetHistoryData();

    DLTRACEOUT((""));
    }

void CNcdNodePurchaseProxy::InternalizeL()
    {
    DLTRACEIN((""));    
    
    TRAPD( error,
           {
           InternalizeMeansL();
           InternalizeHistoryL();
           } );
    if ( error != KErrNone )
        {
        DLERROR(("Purchase internalize error"));
        // Because we can't fully construct the purchase info,
        // we don't give even parts of info to the user because it could
        // be misleading.
        DeletePurchaseOptions();
        ResetHistoryData();
        User::Leave( error );
        }
    DLTRACEOUT((""));    
    }


void CNcdNodePurchaseProxy::InternalizeMeansL()
    {
    DLTRACEIN((""));

    // Request the purchase option ids from server side
    CDesCArray* poIds = RequestPurchaseOptionIdsL();
    CleanupStack::PushL( poIds );
    
    // Delete purchase options that don't exist in server side.
    ReleaseMissingPurchaseOptions( *poIds );

    // Reinternalize purchase options that existed already.
    InternalizeExistingPurchaseOptionsL();
    
    // Create purchase options that exists in the server side but not
    // in proxy side.
    CDesCArray* newPurchaseOptions = new( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( newPurchaseOptions );
    
    // Find the purchase options that don't exist in the proxy side.
    for ( TInt i = 0; i < poIds->Count(); i++ ) 
        {
        TPtrC id = (*poIds)[i];
        if ( !HasPurchaseOption( id ) ) 
            {
            newPurchaseOptions->AppendL( id );
            }
        }

    InternalizeMeansL( *newPurchaseOptions );
    CleanupStack::PopAndDestroy( newPurchaseOptions );
    newPurchaseOptions = NULL;
    CleanupStack::PopAndDestroy( poIds );
    poIds = NULL;

    DLTRACEOUT((""));    
    }


void CNcdNodePurchaseProxy::InternalizeHistoryL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalizePurchaseHistory,
                               KNullDesC8,
                               data,
                               Metadata().Handle(),
                               0 ) );

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );


    // Read the data from the stream
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    TRAPD( error, InternalizeHistoryDataL( stream ) );
    if ( error != KErrNone )
        {
        User::Leave( error );
        }
        
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    
    DLTRACEOUT((""));    
    }

CNcdNodeMetadataProxy& CNcdNodePurchaseProxy::Metadata() const
    {
    return iMetadata;
    }

const CNcdClientSubscribableContent*
    CNcdNodePurchaseProxy::SubscribableContent() const
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    return iSubscribableContent;    
    }



// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// In this function we return purchaseoptions to ui that are available for
// the client at the moment.
// ---------------------------------------------------------------------------
//
RCatalogsArray<MNcdPurchaseOption>
    CNcdNodePurchaseProxy::PurchaseOptionsL() const
    {
    DLTRACEIN(( "" ));
    
    RCatalogsArray<MNcdPurchaseOption> options;
    
    // in case of leave, destroy its contents (release)
    CleanupResetAndDestroyPushL( options );

    
    TInt optionsCount( iPurchaseOptions.Count() );
    TInt optionsIndexer( 0 );
    while ( optionsIndexer < optionsCount )
        {
        DASSERT( iPurchaseOptions[optionsIndexer] );

        // Purchaseoption that we handle at the moment        
        CNcdPurchaseOptionProxy* purchaseOption =
            iPurchaseOptions[optionsIndexer];
            
        // Type of the purchaseoption
        MNcdPurchaseOption::TType purchaseoptionType =
            purchaseOption->PurchaseOptionType();
            
        // dataentity to whome the purchaseoption belongs to
        CNcdNodeMetadataProxy& meta =
            purchaseOption->ParentNodePurchase()->Metadata();

        

        // NOTICE: At the moment this function does not take into count
        //         situations where purchaseoption could be of several
        //         types like ESubscription and ESubscriptionPurchase
        

        // Can purchase of the item be done if it is already 
        // purchased?


        if ( purchaseoptionType == MNcdPurchaseOption::EPurchase )
            {

            // Should check if parentfolder has separately purchaseable
            // false and if it has then do not show normal purchase
            // purchaseoptions to ui

            options.AppendL( purchaseOption );        
            TInt ref = purchaseOption->AddRef();            
            DLTRACE(("Purchase option ref count after addref: %d", ref));
            }
        else if ( purchaseoptionType == 
                      MNcdPurchaseOption::ESubscriptionPurchase )
            {
            // Part of subscription -type purchaseoptions are
            // always shown to ui
            options.AppendL( purchaseOption );        
            purchaseOption->AddRef(); 
            }
        else if ( purchaseoptionType ==
                      MNcdPurchaseOption::ESubscription )
            {
            // If this subscription is already bought, we can't show
            // it to ui.
            
            TBool isBought = 
                Metadata().Node().NodeManager().
                SubscriptionManager().
                ActiveSubscriptionExists( meta.Id(),
                                          meta.Namespace(),
                                          purchaseOption->Id() );

            if ( !isBought )
                {
                options.AppendL( purchaseOption );        
                purchaseOption->AddRef();
                }
            }
        else if ( purchaseoptionType ==
                      MNcdPurchaseOption::ESubscriptionUpgrade )
            {
            // If target of the purchaseoption is not bought, upgrade
            // cannot be used
            if ( purchaseOption->IsUsableL() )
                {
                options.AppendL( purchaseOption );        
                purchaseOption->AddRef();
                }
            }
        else
            {
            // Not supported
            DASSERT( false );
            }
            
        ++optionsIndexer;
        }
        
    
    CleanupStack::Pop( &options );
    
    DLTRACEOUT(( "" ));
    return options;
    }

// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// ?implementation_description
// ---------------------------------------------------------------------------
//
MNcdPurchaseOption* CNcdNodePurchaseProxy::PurchaseOptionL(
    const TDesC& aPurchaseOptionId ) const
    {
    DLTRACEIN((""));
    TInt optionsCount( iPurchaseOptions.Count() );
    TInt optionsIndexer( 0 );
    while ( optionsIndexer < optionsCount )
        {
        DASSERT( iPurchaseOptions[optionsIndexer] );
        
        if ( iPurchaseOptions[optionsIndexer]->Id() == aPurchaseOptionId )
            {
            iPurchaseOptions[optionsIndexer]->AddRef();
            
            DLTRACEOUT(( "" ));
            return iPurchaseOptions[optionsIndexer];
            }
        
        ++optionsIndexer;
        }
    
    DLTRACEOUT(( "" ));
    return NULL;
    }


// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// Notice that this function accepts purchase options also from other nodes
// than into which this interface belongs to. Could be changed to accept
// only purchase options from this interface, but at the moment UI
// does not permit this in case of subscription.
// ---------------------------------------------------------------------------
//
MNcdPurchaseOperation* CNcdNodePurchaseProxy::PurchaseL(
        MNcdPurchaseOption& aPurchaseOption,
        MNcdPurchaseOperationObserver& aObserver )
    {
    DLTRACEIN((""));

        
    CNcdPurchaseOperationProxy* operation( NULL );
    
    CNcdPurchaseOptionProxy& option =
        *static_cast<CNcdPurchaseOptionProxy*>( &aPurchaseOption );


    // Given purchaseoption is old and should not be used anymore
    if ( option.IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }


    // Now if validuntil for subscription has passed, we should not allow it
    // to be bought anymore
    if ( option.PurchaseOptionType() == MNcdPurchaseOption::ESubscription )
        {
        TTime now;
        now.HomeTime();
        
        // The time until the node is subscribable is stored in the
        // subscribable content of the NodePurchase interface where to
        // the GIVEN PURCHASEOPTION belongs to.
        const CNcdClientSubscribableContent* subscribableContent =
            option.ParentNodePurchase()->SubscribableContent();

        DASSERT( subscribableContent );
        if ( subscribableContent &&
             subscribableContent->ValidUntilSet() &&
             now > subscribableContent->ValidUntil() )
            {
            User::Leave( KNcdErrorSubscriptionNotSubscribableAnymore );
            }

        }


    DLTRACE(( _L("Purchase started with option whose name is: %S"),
              &option.Name() ));
  
  
    // NOTICE: In the case of subscription or upgrade the purchaseoption
    //         can be from another node. That is why we give
    //         node-reference from the purchaseoption.
    operation = 
        Metadata().Node().OperationManager().
        CreatePurchaseOperationL( option.ParentNodePurchase()->Metadata().Node(), 
                                  option,
                                  &aObserver );

    // It is enough that the observer is informed
    // when the file operation is ready. So, no need to add
    // node or this class object to be observers.

    // No need to increase the operation ref count here because
    // it should be initially one.

    DLTRACEOUT((""));
        
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// ?implementation_description
// ---------------------------------------------------------------------------
//
TBool CNcdNodePurchaseProxy::IsPurchased() const
    {
    return iIsPurchased;
    }

    
// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// ?implementation_description
// ---------------------------------------------------------------------------
//
MNcdPurchaseOption* CNcdNodePurchaseProxy::PurchasedOptionL() const
    {
    if ( !IsPurchased() )
        {
        return NULL;
        }

    return PurchaseOptionL( *iPurchasedOptionId );
    }
    
// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// ?implementation_description
// ---------------------------------------------------------------------------
//
TTime CNcdNodePurchaseProxy::TimeOfPurchaseL() const
    {
    if ( !IsPurchased() )
        {
        User::Leave( KErrNotFound );
        }
    
    return iTimeOfPurchase;    
    }

// ---------------------------------------------------------------------------
// From class MNcdNodePurchase.
// ?implementation_description
// ---------------------------------------------------------------------------
//
const TDesC& CNcdNodePurchaseProxy::PurchasedPriceL() const
    {
    if ( !IsPurchased() )
        {
        User::Leave( KErrNotFound );
        }
    
    return *iPurchasedPrice;
    }    
    
    
void CNcdNodePurchaseProxy::InternalizeMeansDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
        
    // Subscribable content is read into its own class
    TBool subscribableContentFound( aStream.ReadInt32L() );
    if ( subscribableContentFound )
        {
        if ( iSubscribableContent == NULL )
            {
            iSubscribableContent = CNcdClientSubscribableContent::NewL();
            }        
        iSubscribableContent->InternalizeL( aStream );
        }

    
    TInt handleCount = aStream.ReadInt32L();
    DLINFO((( "handle count: %d" ), handleCount ));
    
    for ( TInt i = 0; i < handleCount; i++ ) 
        {
        TInt tmpProxyHandle = aStream.ReadInt32L();
        CNcdPurchaseOptionProxy* tmpPurchaseOption = 
            CNcdPurchaseOptionProxy::NewLC( 
                ClientServerSession(),
                tmpProxyHandle,
                *this );
        
        TRAPD( addError, iPurchaseOptions.AppendL( tmpPurchaseOption ) );
        if ( addError != KErrNone )
            {
            CleanupStack::PopAndDestroy( tmpPurchaseOption );
            User::Leave( addError );
            }
            
        CleanupStack::Pop( tmpPurchaseOption );
        tmpPurchaseOption->InternalizeL();
        }

    DLTRACEOUT((""));
    }


void CNcdNodePurchaseProxy::InternalizeHistoryDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // Reset values so that if we receive message that for some reason
    // does not contain purchase history info, we don't keep the old
    // purchase history info in memory.
    ResetHistoryData();
    
    // Node class id. Not actually needed.
    aStream.ReadInt32L();
    
    
    iIsPurchased = aStream.ReadInt32L();
    if ( iIsPurchased == EFalse )
        {
        return;
        }

    InternalizeDesL( iPurchasedOptionId, aStream );

    TInt64 integerTimeOfPurchase( 0 );
    // Store framework provides the necessary implementation for 
    // the operator>> to internalise the 64-bit integer
    aStream >> integerTimeOfPurchase;
    iTimeOfPurchase = TTime( integerTimeOfPurchase );

    InternalizeDesL( iPurchasedPrice, aStream );


    DLTRACEOUT((""));
    }


void CNcdNodePurchaseProxy::DeletePurchaseOptions()
    {
    DLTRACEIN(("Handling %d purchase options", iPurchaseOptions.Count() ));
    for ( TInt i = iPurchaseOptions.Count() - 1; i >= 0; --i )
        {
        CNcdPurchaseOptionProxy* tmpOption = iPurchaseOptions[i];
        
        if ( tmpOption->MyTotalRefCount() > 0 )
            {
            // to prevent loops remove purchaseoption first
            // so it won't become deleted            
            iPurchaseOptions.Remove( i );
            
            tmpOption->SetObsolete(); 
            // Remove the purchase option from the interface parent.
            // This will update the parent reference count.
            // Because purchase option does not provide any interfaces for
            // the parent, it is safe to use this function.
            tmpOption->RemoveFromParent();
            if ( tmpOption->TotalRefCount() == 0 )
                {
                DLINFO(("Purchase option can be deleted here."));
                delete tmpOption;
                }
            }
        }
    DLTRACE(("Deleting %d purchase options", iPurchaseOptions.Count() ));
    iPurchaseOptions.ResetAndDestroy();    
    }

void CNcdNodePurchaseProxy::ResetHistoryData()
    {
    iIsPurchased = EFalse;
    delete iPurchasedOptionId;
    iPurchasedOptionId = NULL;
    iTimeOfPurchase = TTime( 0 );    
    delete iPurchasedPrice;
    iPurchasedPrice = NULL;
    }

CDesCArray* CNcdNodePurchaseProxy::RequestPurchaseOptionIdsL() 
    {
    DLTRACEIN((""));
    
    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdPurchaseOptionIds,
                               KNullDesC8,
                               data,
                               Handle(),
                               0 ) );

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    // Read the data from the stream
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    TInt purchaseOptionCount = stream.ReadInt32L();
    CDesCArray* array = new( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( array );
    for ( TInt i = 0; i < purchaseOptionCount; i++ ) 
        {
        HBufC* des( NULL );
        InternalizeDesL( des, stream );
        CleanupStack::PushL( des );
        array->AppendL( *des );
        CleanupStack::PopAndDestroy( des );
        }
        
    CleanupStack::Pop( array );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( data );
    
    return array;
    }
    
void CNcdNodePurchaseProxy::ReleaseMissingPurchaseOptions(
    const CDesCArray& aPurchaseOptionIds ) 
    {
    DLTRACEIN((""));
    
    for ( TInt i = iPurchaseOptions.Count() - 1; i >= 0; i-- ) 
        {
        TInt index;
        if ( aPurchaseOptionIds.Find( iPurchaseOptions[i]->Id(), index ) != 0 ) 
            {
            iPurchaseOptions[i]->SetObsolete();
            // Remove the purchase option from the interface parent.
            // This will update the parent reference count.
            // Because purchase option does not provide any interfaces for
            // the parent, it is safe to use this function.
            iPurchaseOptions[i]->RemoveFromParent();
            if ( iPurchaseOptions[ i ]->TotalRefCount() == 0 )
                {
                DLINFO(("Purchase option can be deleted."));
                delete iPurchaseOptions[ i ];
                }
            iPurchaseOptions.Remove( i );
            }
        }
    }
    
void CNcdNodePurchaseProxy::InternalizeExistingPurchaseOptionsL() 
    {
    DLTRACEIN((""));
    
    for ( TInt i = 0; i < iPurchaseOptions.Count(); i++ ) 
        {
        iPurchaseOptions[i]->InternalizeL();
        }
    }
    
void CNcdNodePurchaseProxy::InternalizeMeansL(
    const CDesCArray& aPurchaseOptionIds ) 
    {
    DLTRACEIN((""));

    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
                
    RBufWriteStream writeStream( *buf );
    CleanupClosePushL( writeStream );
    writeStream.WriteInt32L( aPurchaseOptionIds.Count() );
    for ( TInt i = 0; i < aPurchaseOptionIds.Count(); i++ ) 
        {
        ExternalizeDesL( aPurchaseOptionIds[i], writeStream );     
        }
    
    CleanupStack::PopAndDestroy( &writeStream );
    HBufC8* messageToSend = HBufC8::NewL( buf->Size() );
    messageToSend->Des().Copy( buf->Ptr( 0 ) );
    
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PushL( messageToSend );
    
    // Request handles to the new purchase options.
    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalizePurchaseMeans,
                               *messageToSend,
                               data,
                               Handle(),
                               0 ) );
                               
    CleanupStack::PopAndDestroy( messageToSend );
    messageToSend = NULL;

    if ( data == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( data );

    // Read the data from the stream
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );

    InternalizeMeansDataL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( data );
    }
    
TBool CNcdNodePurchaseProxy::HasPurchaseOption( const TDesC& aId ) const 
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iPurchaseOptions.Count(); i++ ) 
        {
        if ( iPurchaseOptions[i]->Id() == aId ) 
            {
            return ETrue;
            }
        }
        
    return EFalse;
    }
