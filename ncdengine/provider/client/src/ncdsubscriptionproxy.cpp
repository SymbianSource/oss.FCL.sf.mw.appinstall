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
* Description:   Contains CNcdSubscriptionProxy class implementation
*
*/


#include "ncdsubscriptionproxy.h"
#include "ncdoperationimpl.h"
#include "ncddownloadoperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "ncdsubscriptionoperationproxy.h"
#include "ncdsubscriptiongroupproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdnodeproxy.h"
#include "ncderrors.h"

#include "catalogsdebug.h"

// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdSubscriptionProxy::CNcdSubscriptionProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdSubscriptionGroupProxy& aParentGroup )
    : CNcdInterfaceBaseProxy( aSession, aHandle, NULL ),
      iOperationManager( aOperationManager ),
      iNodeManager( aNodeManager ),
      iObsolete( EFalse ),
      iParentGroup( aParentGroup )
    {
    }


void CNcdSubscriptionProxy::ConstructL()
    {
    // Register the interface
    MNcdSubscription* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
                                            MNcdSubscription::KInterfaceUid ) );
    
    // Is it ok if internalization fails here?
    // Earlier comment: Do not let the internalization leave here.
    //                  This object may be reinternalized later.
    InternalizeL();
    }


CNcdSubscriptionProxy* CNcdSubscriptionProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdSubscriptionGroupProxy& aParentGroup )
    {
    CNcdSubscriptionProxy* self = 
        CNcdSubscriptionProxy::NewLC( aSession,
                                      aHandle,
                                      aOperationManager,
                                      aNodeManager,
                                      aParentGroup );
    CleanupStack::Pop( self );
    return self;
    }

CNcdSubscriptionProxy* CNcdSubscriptionProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdOperationManagerProxy& aOperationManager,
    CNcdNodeManagerProxy& aNodeManager,
    CNcdSubscriptionGroupProxy& aParentGroup )
    {
    CNcdSubscriptionProxy* self = 
        new( ELeave ) CNcdSubscriptionProxy( aSession,
                                             aHandle,
                                             aOperationManager,
                                             aNodeManager,
                                             aParentGroup );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdSubscriptionProxy::~CNcdSubscriptionProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdSubscription::KInterfaceUid );
    
    ResetSubscriptionVariables();
    }


void CNcdSubscriptionProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

     CleanupStack::PushL( data );

     // Read the data from the stream and insert it to the memeber variables
     RDesReadStream stream( *data );
     CleanupClosePushL( stream );
    
     InternalizeDataL( stream );
    
     // Closes the stream
     CleanupStack::PopAndDestroy( &stream ); 
     CleanupStack::PopAndDestroy( data );
     
    DLTRACEOUT((""));    
    }


void CNcdSubscriptionProxy::SetObsolete()
    {
    iObsolete = ETrue;
    }

TDesC& CNcdSubscriptionProxy::PurchaseOptionId() const
    {
    return *iPurchaseOptionId;
    }


// MNcdSubscription functions
    

const TDesC& CNcdSubscriptionProxy::Name() const
    {
    if ( iName == NULL )
        {
        return KNullDesC;
        }
    
    return *iName;
    }

HBufC8* CNcdSubscriptionProxy::IconL() const
    {
    DLTRACEIN((""));
    if ( IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }
        
    return iParentGroup.IconL();
    }

MNcdSubscription::TStatus CNcdSubscriptionProxy::SubscriptionStatus() const
    {
    DLTRACEIN(( _L("Subscription name: %S, subscription poId: %S"),
                &(Name()),
                &(PurchaseOptionId()) ));

    if ( iExpiredOn && *iExpiredOn != KNullDesC )
        {
        DLINFO(( _L("ExpiredOn: %S"), iExpiredOn ));
        DLTRACEOUT(("Subscription expired."));
        return MNcdSubscription::ESubscriptionExpired;
        }

    if ( iSubscriptionType == MNcdSubscription::EPeriodic ||         
         ( iSubscriptionType == MNcdSubscription::EAutomaticContinous
           && iCancelled ) )
        {
        if ( iValidityTimeSet )
            {
            TTime now;
            now.HomeTime();

            if ( now > iValidUntil )
                {
                DLTRACEOUT(("Subscription expired."));
                return MNcdSubscription::ESubscriptionExpired;
                }
            }
            
        if ( iCreditLimitSet )
            {
            // This simple compare is deemed to be enough as no
            // complicated operations are done on the TReal numbers
            if ( iCreditsLeft <= 0 )
                {
                DLTRACEOUT(("Subscription expired."));
                return MNcdSubscription::ESubscriptionExpired;
                }
            }
            
        if ( iDownloadLimitSet )
            {
            if ( iDownloadsLeft <= 0 )
                {
                DLTRACEOUT(("Subscription expired."));
                return MNcdSubscription::ESubscriptionExpired;
                }
            }
        }
    

    // If we get here the subscription is not expired. Because
    // subscription like this is created when it is bought,
    // it cannot be in ENotSubscribed state. So the only alternative
    // left is ESubscriptionActive.
    DLTRACEOUT(("Subscription active."));
    return MNcdSubscription::ESubscriptionActive;
    }

TBool CNcdSubscriptionProxy::IsObsolete() const
    {
    return iObsolete;
    }

TBool CNcdSubscriptionProxy::Unsubscribed() const
    {
    return iCancelled;
    }

MNcdSubscriptionOperation* CNcdSubscriptionProxy::UnsubscribeL( 
        MNcdSubscriptionOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }
        
    CNcdSubscriptionOperationProxy* operation( NULL );

    operation =
        iOperationManager.CreateSubscriptionUnsubscribeOperationL(
            PurchaseOptionId(),
            iParentGroup.EntityId(),
            iParentGroup.Namespace(),
            iParentGroup.ServerUri(),
            aObserver );

    DLTRACEOUT((""));

    return operation;
    }

MNcdSubscription::TType CNcdSubscriptionProxy::SubscriptionType() const
    {
    return iSubscriptionType;
    }

TBool CNcdSubscriptionProxy::ValidityTime( TTime& aValidUntil ) const
    {
    if ( !iValidityTimeSet )
        {
        return EFalse;
        }        
    aValidUntil = iValidUntil;

    return ETrue;
    }

TBool CNcdSubscriptionProxy::CreditLimit( TReal& aCreditsLeft,
                                          TReal& aTotalCredits ) const
    {
    if ( !iCreditLimitSet )
        {
        return EFalse;
        }
    aCreditsLeft = iCreditsLeft;
    aTotalCredits = iTotalCredits;
       
    return ETrue;
    }

TBool CNcdSubscriptionProxy::DownloadLimit( TInt& aDownloadsLeft,
                                            TInt& aTotalDownloads) const
    {
    if ( !iDownloadLimitSet )
        {
        return EFalse;
        }
    aDownloadsLeft = iDownloadsLeft;
    aTotalDownloads = iTotalDownloads;
       
    return ETrue;
    }

MNcdNode* CNcdSubscriptionProxy::OriginNodeL() const
    {
    DLTRACEIN((""));

    if ( IsObsolete() )
        {
        User::Leave( KNcdErrorObsolete );
        }

    const CNcdNodeIdentifier& groupIdentifier = iParentGroup.Identifier();

    // Notice that this function takes the metadataidentifier as a parameter.    
    MNcdNode* subscriptionNode = 
        &iNodeManager.CreateTemporaryOrSupplierNodeL( groupIdentifier );
    
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

const TDesC& CNcdSubscriptionProxy::OriginPurchaseOptionId() const
    {
    return PurchaseOptionId();
    }



// Other functions

CNcdOperationManagerProxy& CNcdSubscriptionProxy::OperationManager() const
    {
    return iOperationManager;
    }



void CNcdSubscriptionProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN(("Internalizing subscription proxy"));

    ResetSubscriptionVariables();

    InternalizeDesL( iName, aStream );
    DLINFO(( _L("Subscription proxy, name: %S"), iName ));
    InternalizeDesL( iExpiredOn, aStream );
    
    iCancelled = aStream.ReadInt32L();

    iSubscriptionType = 
        static_cast<MNcdSubscription::TType>(aStream.ReadInt32L());

    iValidityTimeSet = aStream.ReadInt32L();
    DLINFO(( "Subscription proxy, validitytimeset: %d", iValidityTimeSet ));
    TInt64 intValidUntil( -1 );
    aStream >> intValidUntil;
    iValidUntil = intValidUntil;
    TInt totalValidityDelta = aStream.ReadInt32L(); // Not needed here
    DLINFO(( "Subscription proxy, validitydelta: %d", totalValidityDelta ));
    
    iCreditLimitSet = aStream.ReadInt32L();
    iCreditsLeft = aStream.ReadReal32L();
    DLINFO(( "Subscription proxy, credits left: %f", iCreditsLeft ));
    iTotalCredits = aStream.ReadReal32L();
    DLINFO(( "Subscription proxy, total credits: %f", iTotalCredits ));

    iDownloadLimitSet = aStream.ReadInt32L();
    iDownloadsLeft = aStream.ReadInt32L();
    DLINFO(( "Subscription proxy, downloads left: %d", iDownloadsLeft ));
    iTotalDownloads = aStream.ReadInt32L();
    DLINFO(( "Subscription proxy, total downloads: %d", iTotalDownloads ));

    InternalizeDesL( iPurchaseOptionId, aStream );
    
    DLINFO(( _L("Subscription proxy, purchaseoptionid: %S"),
              iPurchaseOptionId ));
              
    DLTRACEOUT((""));
    }


void CNcdSubscriptionProxy::ResetSubscriptionVariables()
    {
    delete iName;
    iName = NULL;
    
    delete iExpiredOn;
    iExpiredOn = NULL;
    
    iCancelled = EFalse;
    
    iSubscriptionType = MNcdSubscription::EPeriodic;
    
    iValidityTimeSet = EFalse;
    iValidUntil = -1;
    
    iCreditLimitSet = EFalse;
    iCreditsLeft = -1;
    iTotalCredits = -1;
    
    iDownloadLimitSet = EFalse;
    iDownloadsLeft = -1;
    iTotalDownloads = -1;
    
    delete iPurchaseOptionId;
    iPurchaseOptionId = NULL;
    
    }
