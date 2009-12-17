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
* Description:   ?Description
*
*/


#include <e32base.h>

#include "ncdpurchaseoptionproxy.h"
#include "ncdnodemetadataproxy.h"
#include "catalogsinterfaceidentifier.h"
#include "ncdclientsubscription.h"
#include "ncdclientpartofsubscription.h"
#include "ncdclientupgrade.h"
#include "catalogsclientserver.h"
#include "ncdnodefunctionids.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "ncdnodeidentifier.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodepurchaseproxy.h"
#include "ncdnodeproxy.h"
#include "ncdsubscriptionproxy.h"
#include "ncderrors.h"



// ======== MEMBER FUNCTIONS ========


CNcdPurchaseOptionProxy::CNcdPurchaseOptionProxy(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CNcdNodePurchaseProxy& aParentNodePurchase ) :
    CNcdInterfaceBaseProxy( aSession, aHandle, &aParentNodePurchase ),
    iParentNodePurchase( &aParentNodePurchase ),
    iIsFree( EFalse ),
    iObsolete( EFalse )
    {
    }

void CNcdPurchaseOptionProxy::ConstructL()
    {
    DLTRACEIN(("this-ptr: %x", this));
    // Purchaseoption interfaces are not registered
    // to the parent as there can be several purchaseoptions
    }


CNcdPurchaseOptionProxy* CNcdPurchaseOptionProxy::NewL( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodePurchaseProxy& aParentNodePurchase )
    {
    CNcdPurchaseOptionProxy* self = 
        CNcdPurchaseOptionProxy::NewLC( aSession,
                                        aHandle,
                                        aParentNodePurchase );
    CleanupStack::Pop( self );
    return self;
    }


CNcdPurchaseOptionProxy* CNcdPurchaseOptionProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodePurchaseProxy& aParentNodePurchase )
    {
    CNcdPurchaseOptionProxy* self =
        new( ELeave ) CNcdPurchaseOptionProxy( aSession,
                                               aHandle,
                                               aParentNodePurchase );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdPurchaseOptionProxy::~CNcdPurchaseOptionProxy()
    {
    DLTRACEIN(("this-ptr: %x", this));
    delete iSubscriptionComponent;
    delete iPartOfSubscriptionComponent;
    delete iUpgradeComponent;
    
    delete iName;
    delete iPriceText;
    
    delete iPriceCurrency;
    
    delete iPurchaseOptionId;
    DLTRACEOUT(("this-ptr: %x", this));
    }
    
void CNcdPurchaseOptionProxy::InternalizeL()
    {
    DLTRACEIN(("this-ptr: %x", this));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data id, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    User::LeaveIfError(
            ClientServerSession().
                SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
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
    
    InternalizeDataL( stream );

    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );
    
    DLTRACEOUT((""));    
    }

const TDesC& CNcdPurchaseOptionProxy::Id() const
    {
    return *iPurchaseOptionId;
    }

CNcdNodePurchaseProxy* CNcdPurchaseOptionProxy::ParentNodePurchase() const
    {
    return iParentNodePurchase;
    }

const CNcdClientPartOfSubscription*
    CNcdPurchaseOptionProxy::PartOfSubscription() const
    {
    return iPartOfSubscriptionComponent;
    }

void CNcdPurchaseOptionProxy::SetObsolete()
    {
    DLTRACEIN((""));
    iObsolete = ETrue;
    iParentNodePurchase = NULL;
    DLTRACEOUT(( _L("PO name: %S now set as obsolete"),iName ));   
    }

const TDesC& CNcdPurchaseOptionProxy::Name() const
    {
    return *iName;
    }

const TDesC& CNcdPurchaseOptionProxy::Price() const
    {
    return *iPriceText;
    }

TBool CNcdPurchaseOptionProxy::IsFree() const
    {
    return iIsFree;
    }

MNcdPurchaseOption::TType CNcdPurchaseOptionProxy::PurchaseOptionType() const
    {
    return iType;
    }

TBool CNcdPurchaseOptionProxy::IsObsolete() const
    {
    DLTRACEIN((""));
    DLTRACEOUT(( _L("PO name: %S IsObsolete: %d"),iName, iObsolete ));
    return iObsolete;
    }

TBool CNcdPurchaseOptionProxy::IsUsableL() const
    {
    DLTRACEIN((""));
    
    // If purchase option is not valid anymore it should not be used.
    // This is because we cannot be sure that the parent node for example
    // still exists.
    if ( IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }
    
    if ( iType == MNcdPurchaseOption::ESubscriptionPurchase )
        {
        // If parent subscription is not bought, return false.
        // (Parent subscription cannot be used.)

        DASSERT( iPartOfSubscriptionComponent != NULL );
                
        const TDesC& parentSubscriptionEntityId =
            iPartOfSubscriptionComponent->ParentEntityId();
        const TDesC& parentSubscriptionPoId =
            iPartOfSubscriptionComponent->ParentPurchaseOptionId();
        
        // NOTICE: Namespace is assumed to be the same as of
        //         the metadata that owns this purchase option.
            
        TBool isBought = 
            ParentNodePurchase()->Metadata().Node().NodeManager().
            SubscriptionManager().
            ActiveSubscriptionExists( 
                parentSubscriptionEntityId,
                ParentNodePurchase()->Metadata().Namespace(),
                parentSubscriptionPoId );
         
        if ( !isBought )
            {
            DLTRACEOUT(("False"));
            return EFalse;
            }
        }
    else if ( iType == MNcdPurchaseOption::ESubscriptionUpgrade )
        {
        // If target subscription is not bought, return false
        // If the subscription is not bought, it cannot be upgraded.)
        
        DASSERT( iUpgradeComponent != NULL );
        
        const TDesC& targetPurchaseOptionId =
            iUpgradeComponent->DependencyId();

        // Subscription that DependencyId references is from the
        // same dataentity as this purchaseoption so use other
        // info than purchaseoption id from the metadata that
        // owns this purchaseoption
        TBool isBought = 
        ParentNodePurchase()->Metadata().Node().NodeManager().
        SubscriptionManager().ActiveSubscriptionExists(
                 ParentNodePurchase()->Metadata().Id(),
                 ParentNodePurchase()->Metadata().Namespace(),
                 targetPurchaseOptionId );        
        
        if ( !isBought )
            {
            DLTRACEOUT(("False"));
            return EFalse;
            }
        }
    DLTRACEOUT(("True"));
    return ETrue;
    }

MNcdSubscription* CNcdPurchaseOptionProxy::ParentSubscriptionL() const
    {
    DLTRACEIN((""));

    // If purchase option is not valid anymore it should not be used.
    // This is because we cannot be sure that the parent node for example
    // still exists.
    if ( IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }
    // Also if we are not handling a purchase option of type
    // MNcdPurchaseOption::ESubscriptionPurchase this function should
    // not be used.
    if ( iType != MNcdPurchaseOption::ESubscriptionPurchase ||
         iPartOfSubscriptionComponent == NULL )
        {
        // Should change to leave with own error code
        DLERROR(("PO not part of subscription or part of subscription component not found."));
        User::Leave( KNcdErrorPurchaseOptionNotValidSubscriptionPurchase );
        }


    DASSERT( iPartOfSubscriptionComponent != NULL );
            
    const TDesC& parentSubscriptionEntityId =
        iPartOfSubscriptionComponent->ParentEntityId();
    const TDesC& parentSubscriptionPoId =
        iPartOfSubscriptionComponent->ParentPurchaseOptionId();
    
    // NOTICE: Namespace is assumed to be the same as of
    //         the metadata that owns this purchase option.

    CNcdSubscriptionProxy* searchedSubscription( NULL );        
    searchedSubscription =
        ParentNodePurchase()->Metadata().Node().NodeManager().
        SubscriptionManager().
        Subscription( parentSubscriptionEntityId,
                      ParentNodePurchase()->Metadata().Namespace(),
                      parentSubscriptionPoId );

    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    if ( searchedSubscription != NULL )
        {
        searchedSubscription->AddRef();
        }
        
    DLTRACEOUT((""));
    return searchedSubscription;
    }

MNcdNode* CNcdPurchaseOptionProxy::ParentSubscriptionNodeL() const
    {
    DLTRACEIN((""));

    // If purchase option is not valid anymore it should not be used.
    // This is because we cannot be sure that the parent node for example
    // still exists.
    if ( IsObsolete() )
        {
        DLERROR(("Using obsolete purchaseoption"));
        User::Leave( KNcdErrorObsolete );
        }
    
    if ( iPartOfSubscriptionComponent == NULL )
        {
        DLERROR(("Asking parent subscription node from non part of subscription"));
        DLTRACEOUT((""));
        User::Leave( KNcdErrorPurchaseOptionNotValidSubscriptionPurchase );
        }


    const TDesC& parentSubscriptionEntityId =
        iPartOfSubscriptionComponent->ParentEntityId();

    CNcdNodeIdentifier& subscriptionOriginIdentifier =
        ParentNodePurchase()->Metadata().Node().NodeIdentifier();


    // Dataentity of the parent subscription is assumed to
    // be in the same namespace as the node that owns this
    // purchaseoption
 
    CNcdNodeIdentifier* subscriptionNodeIdentifier =
        CNcdNodeIdentifier::NewLC(
            subscriptionOriginIdentifier.NodeNameSpace(),
            parentSubscriptionEntityId,
            subscriptionOriginIdentifier.ServerUri(),
            subscriptionOriginIdentifier.ClientUid() );


    // Node manager will handle the creation of the node.
    CNcdNodeManagerProxy& nodeManager(
        ParentNodePurchase()->Metadata().Node().NodeManager() );
    
    // Notice that this function takes the metadataidentifier as a parameter.    
    MNcdNode* subscriptionNode = 
        &nodeManager.CreateTemporaryOrSupplierNodeL( *subscriptionNodeIdentifier );

    CleanupStack::PopAndDestroy( subscriptionNodeIdentifier );
    
    // Increase also the reference counter by one here.
    // So, the root ref count is at least one when the user
    // gets it.
    if ( subscriptionNode != NULL )
        {
        subscriptionNode->AddRef();
        }
    
    DLTRACEOUT((""));
    
    return subscriptionNode;
    }

     
const TDesC&
    CNcdPurchaseOptionProxy::ParentSubscriptionPurchaseOptionIdL() const
    {
    DLTRACEIN((""));
    
    if ( iPartOfSubscriptionComponent == NULL )
        {
        User::Leave( KNcdErrorPurchaseOptionNotValidSubscriptionPurchase );
        }

    DLTRACEOUT((""));
    return iPartOfSubscriptionComponent->ParentPurchaseOptionId();
    }

 
void CNcdPurchaseOptionProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // First read the class id. Because, it is the first thing in the stream.
    // Actually we do not need that information here.
    aStream.ReadInt32L();

    InternalizeDesL( iName, aStream );
    
    DLTRACE(( _L("Purchaseoption-proxy, name: %S"), iName ));
    
    InternalizeDesL( iPriceText, aStream );
    
    DLTRACE(( _L("Purchaseoption-proxy, price: %S"), iPriceText ));

    iPrice = aStream.ReadReal32L();
    InternalizeDesL( iPriceCurrency, aStream );
    
    iIsFree = aStream.ReadInt32L();
    
    DLTRACE(( "Purchaseoption-proxy, isFree: %d", iIsFree ));
    
    InternalizeDesL( iPurchaseOptionId, aStream );
    
    DLTRACE(( _L("Purchaseoption-proxy, po-id: %S"), iPurchaseOptionId ));

    iType = static_cast<MNcdPurchaseOption::TType>( aStream.ReadInt32L() );
    
    DLTRACE(( _L("Purchaseoption-proxy, type: %d"), iType ));
    
    iRequirePurchaseProcess = aStream.ReadInt32L();
    
    DLTRACE(( "Purchaseoption-proxy, requirePurchaseProcess: %d",
              iRequirePurchaseProcess ));
    
 
    DLTRACE(( "Other data not listed yet." ));
 

    // Let's internalize aggregate classes

    TBool subscriptionExists( aStream.ReadInt32L() );
    if ( subscriptionExists )
        {
        delete iSubscriptionComponent;
        iSubscriptionComponent = NULL;
        iSubscriptionComponent = CNcdClientSubscription::NewL();
        
        iSubscriptionComponent->SetValidityDelta( aStream.ReadInt32L() );
        iSubscriptionComponent->SetValidityAutoUpdate( aStream.ReadInt32L() );
        
        iSubscriptionComponent->SetAmountOfCredits( aStream.ReadReal32L() );
        
        HBufC* tmpAmountOfCreditsCurrency( NULL );
        InternalizeDesL( tmpAmountOfCreditsCurrency, aStream );
        iSubscriptionComponent->SetAmountOfCreditsCurrency(
            tmpAmountOfCreditsCurrency );
                
        iSubscriptionComponent->SetNumberOfDownloads( aStream.ReadInt32L() );

        }
    
    TBool partOfSubscriptionExists( aStream.ReadInt32L() );
    if ( partOfSubscriptionExists )
        {

        delete iPartOfSubscriptionComponent;
        iPartOfSubscriptionComponent = 0;
        iPartOfSubscriptionComponent = CNcdClientPartOfSubscription::NewL();
        
        HBufC* tmpParentEntityId( NULL );
        InternalizeDesL( tmpParentEntityId, aStream );
        iPartOfSubscriptionComponent->SetParentEntityId( tmpParentEntityId );
        
        HBufC* tmpParentPurchaseOptionId( NULL );
        InternalizeDesL( tmpParentPurchaseOptionId, aStream );
        iPartOfSubscriptionComponent->SetParentPurchaseOptionId(
            tmpParentPurchaseOptionId );
            
        iPartOfSubscriptionComponent->SetCreditPrice(
            aStream.ReadReal32L() );

        }
        
    TBool upgradeExists( aStream.ReadInt32L() );
    if ( upgradeExists )
        {

        delete iUpgradeComponent;
        iUpgradeComponent = NULL;
        iUpgradeComponent = CNcdClientUpgrade::NewL();

        HBufC* tmpDependencyId( NULL );
        InternalizeDesL( tmpDependencyId, aStream );
        iUpgradeComponent->SetDependencyId( tmpDependencyId );

        iUpgradeComponent->SetValidityDelta( aStream.ReadInt32L() );
        iUpgradeComponent->SetAmountOfCredits( aStream.ReadReal32L() );
        iUpgradeComponent->SetNumberOfDownloads( aStream.ReadInt32L() );

        }

    DLTRACEOUT((""));    
    }
