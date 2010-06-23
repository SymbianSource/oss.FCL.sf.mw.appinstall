/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdSubscriptionDataCompleterDataCompleter class
*
*/


#include "ncdsubscriptiondatacompleter.h"

#include "ncdsubscriptionmanagerimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdsubscriptionmanagerobserver.h"
#include "catalogsdebug.h"
#include "ncdnodemanager.h"
#include "ncdsubscriptionimpl.h"
#include "ncdsubscriptiongroup.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeiconimpl.h"
#include "ncdpurchaseoptionimpl.h"
#include "ncdnodeimpl.h"


CNcdSubscriptionDataCompleter::CNcdSubscriptionDataCompleter(
    CNcdSubscriptionManager& aSubscriptionManager,
    CNcdNodeManager& aNodeManager,
    CNcdOperationManager& aOperationManager ) :
    iSubscriptionManager( aSubscriptionManager ),
    iNodeManager( aNodeManager ),
    iOperationManager( aOperationManager )
    {
    }

void CNcdSubscriptionDataCompleter::ConstructL()
    {
    }


CNcdSubscriptionDataCompleter* CNcdSubscriptionDataCompleter::NewL(
    CNcdSubscriptionManager& aSubscriptionManager,
    CNcdNodeManager& aNodeManager,
    CNcdOperationManager& aOperationManager )
    {
    CNcdSubscriptionDataCompleter* self =   
        CNcdSubscriptionDataCompleter::NewLC( aSubscriptionManager,
                                              aNodeManager,
                                              aOperationManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdSubscriptionDataCompleter* CNcdSubscriptionDataCompleter::NewLC(
    CNcdSubscriptionManager& aSubscriptionManager,
    CNcdNodeManager& aNodeManager,
    CNcdOperationManager& aOperationManager )
    {
    CNcdSubscriptionDataCompleter* self = 
        new( ELeave ) CNcdSubscriptionDataCompleter( aSubscriptionManager,
                                                     aNodeManager,
                                                     aOperationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }

CNcdSubscriptionDataCompleter::~CNcdSubscriptionDataCompleter()
    {
    DLTRACEIN((""));

    iNodeIds.ResetAndDestroy();
    delete iPurchaseOptionIds;
    iPurchaseOptionIds = NULL;
    
    iContext = NULL;
    iInternalizationObserver = NULL;

    DLTRACEOUT((""));
    }

TInt CNcdSubscriptionDataCompleter::CompleteSubscriptions(
    TUid aClientUid,
    RPointerArray<CNcdNodeIdentifier> aNodeIds,
    MDesCArray* aPurchaseOptionIds,
    MCatalogsContext* aContext,
    MNcdSubscriptionManagerObserver* aObserver )
    {
    DLTRACEIN((""));
    iClientUid = aClientUid;    
    iNodeIds = aNodeIds;
    iPurchaseOptionIds = aPurchaseOptionIds;
    iContext = aContext;
    iInternalizationObserver = aObserver;

    if ( aNodeIds.Count() != iPurchaseOptionIds->MdcaCount() )
        {
        return KErrArgument;
        }

    iIndexer = 0;


    ProceedCompletion();
    
        
    DLTRACEOUT((""));
    return KErrNone;
    }

void CNcdSubscriptionDataCompleter::ProceedCompletion()
    {
    DLTRACEIN((""));
    const TInt KSubscriptionAmount( iNodeIds.Count() );
    while ( iIndexer < KSubscriptionAmount )
        {
        
        CNcdNodeIdentifier& currentNodeIdentifier = *iNodeIds[iIndexer];
        const TDesC& currentPurchaseOptionId =
            iPurchaseOptionIds->MdcaPoint( iIndexer );
        
        TRAPD( error, 
            {
            // pointer to current subscription should not be
            // stored to prevent errors if the subscription is for
            // some reason deleted
            CNcdSubscription& currentSubscription =
                SubscriptionL( currentNodeIdentifier,
                               currentPurchaseOptionId );
        
            if ( SubscriptionIncomplete( currentSubscription ) )
                {                
                CNcdNodeMetaData& meta =
                    iNodeManager.NodeMetaDataL( currentNodeIdentifier );

                CompleteSubscriptionL( currentSubscription,
                                       meta,
                                       currentPurchaseOptionId,
                                       iClientUid );
                }
            } );
        if ( error != KErrNone )
            {
            // Nothing to be done, meta data was not found
            // or some other error situation and could not
            // complete the subscription info
            }
        
        ++iIndexer;
        }
    
    // All subscriptions completed. Notify and delete this.
    iSubscriptionManager.NotifyListeners( iClientUid );
    if ( iInternalizationObserver )
        {
        iInternalizationObserver->SubscriptionsInternalizeComplete(
            KErrNone );
        }        
    iSubscriptionManager.SubscriptionCompleted( this );
    }

void CNcdSubscriptionDataCompleter::CompleteSubscriptionL(
    CNcdSubscription& aCurrentSubscription,
    const CNcdNodeMetaData& aCurrentNodeMetaData,
    const TDesC& aPurchaseOptionId,
    TUid aClientUid ) const
    {
    DLTRACEIN((""));
    
    CompleteSubscriptionNameL( aCurrentSubscription, 
                               aCurrentNodeMetaData,
                               aPurchaseOptionId );

    TRAPD( error, CompleteSubscriptionIconL(
                      aCurrentSubscription,
                      aCurrentNodeMetaData ) );
    // Leave on error situations. If the meta data does not
    // have an icon then this is not an error situation.
    if ( error != KErrNone && error != KErrNotFound )
        {
        User::Leave( error );
        }

    TRAP_IGNORE( iSubscriptionManager.SaveGroupIntoDatabaseL( 
                     aClientUid,
                     aCurrentSubscription.ParentGroup() ) );    

    DLTRACEOUT((""));
    }


CNcdSubscription& CNcdSubscriptionDataCompleter::SubscriptionL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const TDesC& aPurchaseOptionId ) const
    {
    DLTRACEIN((""));
    return iSubscriptionManager.SubscriptionL(
            aNodeIdentifier.NodeId(),
            aNodeIdentifier.NodeNameSpace(),
            aPurchaseOptionId,
            aNodeIdentifier.ClientUid() );
    }

TBool CNcdSubscriptionDataCompleter::SubscriptionIncomplete(
    const CNcdSubscription& aSubscription ) const
    {
    DLTRACEIN((""));
    if ( aSubscription.SubscriptionName() == KNullDesC )
        {
        return ETrue;
        }
    if ( aSubscription.ParentGroup().Icon() == KNullDesC8 )
        {
        return ETrue;
        }
    return EFalse;
    }

void CNcdSubscriptionDataCompleter::CompleteSubscriptionNameL(
    CNcdSubscription& aSubscription,
    const CNcdNodeMetaData& aMetaData,
    const TDesC& aPurchaseOptionId ) const
    {
    DLTRACEIN((""));
    CNcdPurchaseOptionImpl& subscriptionPo =
        aMetaData.PurchaseOptionByIdL( aPurchaseOptionId );
    const TDesC& subscriptionName = subscriptionPo.PurchaseOptionName();        
    aSubscription.SetSubscriptionNameL( subscriptionName );
    DLTRACEOUT((""));
    }

void CNcdSubscriptionDataCompleter::CompleteSubscriptionIconL(
    CNcdSubscription& aSubscription,
    const CNcdNodeMetaData& aMetaData ) const
    {
    DLTRACEIN((""));
    CNcdNodeIcon& nodeIcon = aMetaData.IconL();
    HBufC8* icon = nodeIcon.IconDataL();
    CleanupStack::PushL( icon );
    aSubscription.ParentGroup().SetIconL( *icon );
    CleanupStack::PopAndDestroy( icon );
    DLTRACEOUT((""));
    }
