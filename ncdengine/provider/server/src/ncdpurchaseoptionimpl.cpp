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
* Description:   Implements CNcdPurchaseOptionImpl class
*
*/


#include "ncdpurchaseoptionimpl.h"
#include "ncdserversubscription.h"
#include "ncdserverpartofsubscription.h"
#include "ncdserverupgrade.h"
#include "ncdnodemetadataimpl.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "ncdnodefunctionids.h"
#include "catalogsconstants.h"
#include "ncd_pp_download.h"
#include "ncd_pp_purchaseoption.h"
#include "ncd_pp_subscriptiondetails.h"
#include "ncd_pp_descriptor.h"
#include "ncd_pp_rights.h"
#include "ncd_cp_query.h"
#include "ncdutils.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


CNcdPurchaseOptionImpl::CNcdPurchaseOptionImpl( 
    NcdNodeClassIds::TNcdNodeClassId aClassId,
    const CNcdNodeMetaData& aParentMetaData  )
        : CCatalogsCommunicable(),
          iClassId( aClassId ),
          iParentMetaData( aParentMetaData ),
          iPrice( -1 ),
          iIsFree( EFalse ),
          iRequirePurchaseProcess( ETrue )
    {
    }

void CNcdPurchaseOptionImpl::ConstructL()
    {
    } 

CNcdPurchaseOptionImpl* CNcdPurchaseOptionImpl::NewL(
    const CNcdNodeMetaData& aParentMetaData )
    {
    CNcdPurchaseOptionImpl* self =   
        CNcdPurchaseOptionImpl::NewLC( aParentMetaData );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdPurchaseOptionImpl* CNcdPurchaseOptionImpl::NewLC(
    const CNcdNodeMetaData& aParentMetaData )
    {
    CNcdPurchaseOptionImpl* self = 
        new( ELeave ) CNcdPurchaseOptionImpl( 
            NcdNodeClassIds::ENcdItemNodeMetaDataClassId,
            aParentMetaData );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }


CNcdPurchaseOptionImpl::~CNcdPurchaseOptionImpl()
    {
    DLTRACEIN((""));

    ResetMemberVariables();

    DLTRACEOUT((""));
    }
    

NcdNodeClassIds::TNcdNodeClassId CNcdPurchaseOptionImpl::ClassId() const
    {
    return iClassId;
    }


// Internalization from the protocol

void CNcdPurchaseOptionImpl::InternalizeL( 
    const MNcdPreminetProtocolPurchaseOption& aOption )
    {
    DLTRACEIN((""));

    // Just to be sure, delete old values of member-variables.
    ResetMemberVariables();

    DLTRACE(( _L("Internalizing purchaseoption from protocol.") ));

    iName = aOption.Name().AllocL();

    DLTRACE(( _L("Purchaseoption name: %S."), iName ));

    iPriceText = aOption.PriceText().AllocL();
    
    iPrice = aOption.Price();
    iPriceCurrency = aOption.PriceCurrency().AllocL();
    
    
    iIsFree = aOption.IsFree();
    iPurchaseOptionId = aOption.Id().AllocL();
    iRequirePurchaseProcess = aOption.RequirePurchaseProcess();
    

    // Let's create subscription related aggregate classes
    InternalizeSubscriptionsInfoL( aOption );


    // Let's internalize download details
    InternalizeDownloadDetailsL( aOption );

    DLTRACEOUT((""));
    }




const CNcdNodeIdentifier& 
    CNcdPurchaseOptionImpl::ParentMetaIdentifier() const
    {
    return iParentMetaData.Identifier();
    }

const CNcdServerSubscribableContent*
    CNcdPurchaseOptionImpl::ParentSubscribableContent() const
    {
    return iParentMetaData.SubscribableContent();
    }

const CNcdNodeIcon& CNcdPurchaseOptionImpl::ParentIconL() const
    {
    return iParentMetaData.IconL();
    }


const TDesC& CNcdPurchaseOptionImpl::PurchaseOptionName() const
    {
    // Not an optional element in protocol, no checkings
    // done against NULL pointer
    return *iName;
    }

void CNcdPurchaseOptionImpl::SetIdL( const TDesC& aId )
    {
    HBufC* newId = aId.AllocL();
    delete iPurchaseOptionId;
    iPurchaseOptionId = newId;
    }

const TDesC& CNcdPurchaseOptionImpl::Id() const
    {
    // Not an optional element in protocol, no checkings
    // done against NULL pointer
    return *iPurchaseOptionId;
    }
MNcdPurchaseOption::TType CNcdPurchaseOptionImpl::PurchaseOptionType() const
    {
    return iType;
    }

TBool CNcdPurchaseOptionImpl::IsFree() const
    {
    return iIsFree;
    }

TBool CNcdPurchaseOptionImpl::RequirePurchaseProcess() const
    {
    return iRequirePurchaseProcess;
    }

void CNcdPurchaseOptionImpl::SetPriceTextL( const TDesC& aPriceText )
    {
    AssignDesL( iPriceText, aPriceText);
    }

const TDesC& CNcdPurchaseOptionImpl::PriceText() const
    {
    return *iPriceText;
    }

TInt CNcdPurchaseOptionImpl::DownloadInfoCount() const
    {
    return iDownloadInfo.Count();
    }

const CNcdPurchaseDownloadInfo& CNcdPurchaseOptionImpl::DownloadInfo(
    TInt aInfoIndex ) const
    {
    DASSERT( aInfoIndex >= 0 && aInfoIndex < iDownloadInfo.Count() );
    return *iDownloadInfo[aInfoIndex];
    }

const CNcdServerSubscription*
    CNcdPurchaseOptionImpl::SubscriptionInfo() const
    {
    return iSubscription;
    }

const CNcdServerPartOfSubscription*
    CNcdPurchaseOptionImpl::PartOfSubscriptionInfo() const
    {
    return iPartOfSubscription;
    }

const CNcdServerUpgrade* CNcdPurchaseOptionImpl::UpgradeInfo() const
    {
    return iUpgrade;
    }

void CNcdPurchaseOptionImpl::SetRecentlyUpdated( TBool aNewState )
    {
    DLTRACEIN((""));
    iRecentlyUpdated = aNewState;
    DLTRACEOUT((""));
    }
   
TBool CNcdPurchaseOptionImpl::RecentlyUpdated() const
    {
    DLTRACEIN((""));
    DLTRACEOUT((""));
    return iRecentlyUpdated;
    }


// Internalization from and externalization to the database
    
void CNcdPurchaseOptionImpl::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));

    // Set all the membervariable values to the stream. So,
    // that the stream may be used later to create a new
    // object.

    ExternalizeDataForRequestL( aStream );


    // download details are not internalized to proxy-side
    // so externalization of downloadinfo to database is 
    // not done in ExternalizeDataForRequestL.
    // It has to be done here.
    TInt detailsCount( iDownloadInfo.Count() );
    
    // First we write the amount of download details
    aStream.WriteInt32L( detailsCount );
    
    // Then each details object
    TInt detailsIndexer( 0 );
    while ( detailsIndexer < detailsCount )
        {
        iDownloadInfo[detailsIndexer]->ExternalizeL( aStream );
        ++detailsIndexer;
        }


    DLTRACEOUT((""));
    }

void CNcdPurchaseOptionImpl::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // If there happened to be something stored already
    // in this object, destroy it
    ResetMemberVariables();
    
    aStream.ReadInt32L(); // Class id

    InternalizeDesL( iName, aStream );
    DLINFO(( _L("Internalized name: %S"), iName ));
    
    InternalizeDesL( iPriceText, aStream );

    iPrice = aStream.ReadReal32L();
    InternalizeDesL( iPriceCurrency, aStream );

    
    iIsFree = aStream.ReadInt32L();
    
    InternalizeDesL( iPurchaseOptionId, aStream );
    
    //InternalizeDesL( iPurchase, aStream );
    iType = static_cast<MNcdPurchaseOption::TType>(aStream.ReadInt32L());
    DLINFO(( "Internalized purchase option type: %d", iType ));

    iRequirePurchaseProcess = aStream.ReadInt32L();


    // Let's internalize subscription related aggregate classes

    // Internalization of these could be moved into
    // the aggregate classes

    TBool subscriptionExists( aStream.ReadInt32L() );
    if ( subscriptionExists )
        {
        iSubscription = CNcdServerSubscription::NewL();
        
        iSubscription->SetValidityDelta( aStream.ReadInt32L() );
        iSubscription->SetValidityAutoUpdate( aStream.ReadInt32L() );
        
        TReal32 tmpAmountOfCredits( aStream.ReadReal32L() );
        iSubscription->SetAmountOfCredits( tmpAmountOfCredits );
        
        HBufC* tmpAmountOfCreditsCurrency( NULL );
        InternalizeDesL( tmpAmountOfCreditsCurrency, aStream );
        iSubscription->SetAmountOfCreditsCurrency(
            tmpAmountOfCreditsCurrency );
                
        iSubscription->SetNumberOfDownloads( aStream.ReadInt32L() );
        }
    
    TBool partOfSubscriptionExists( aStream.ReadInt32L() );
    if ( partOfSubscriptionExists )
        {
        iPartOfSubscription = CNcdServerPartOfSubscription::NewL();
        
        HBufC* tmpParentEntityId( NULL );
        InternalizeDesL( tmpParentEntityId, aStream );
        iPartOfSubscription->SetParentEntityId( tmpParentEntityId );
        
        HBufC* tmpParentPurchaseOptionId( NULL );
        InternalizeDesL( tmpParentPurchaseOptionId, aStream );
        iPartOfSubscription->SetParentPurchaseOptionId(
            tmpParentPurchaseOptionId );
            
        iPartOfSubscription->SetCreditPrice( aStream.ReadReal32L() );
        }
        
    TBool upgradeExists( aStream.ReadInt32L() );
    if ( upgradeExists )
        {
        iUpgrade = CNcdServerUpgrade::NewL();

        HBufC* tmpDependencyId( NULL );
        InternalizeDesL( tmpDependencyId, aStream );
        iUpgrade->SetDependencyId( tmpDependencyId );

        iUpgrade->SetValidityDelta( aStream.ReadInt32L() );

        TReal32 tmpAmountOfCredits( aStream.ReadReal32L() );
        iUpgrade->SetAmountOfCredits( tmpAmountOfCredits );
        
        iUpgrade->SetNumberOfDownloads( aStream.ReadInt32L() );
        }



    // Next let's internalize download details

    // First read amount
    TInt detailsCount( aStream.ReadInt32L() );
    
    // Then each details object
    TInt detailsIndexer( 0 );
    while ( detailsIndexer < detailsCount )
        {
        CNcdPurchaseDownloadInfo* tempInfo = 
            CNcdPurchaseDownloadInfo::NewLC();

        tempInfo->InternalizeL( aStream );
        iDownloadInfo.AppendL( tempInfo );
        CleanupStack::Pop( tempInfo );
        
        ++detailsIndexer;
        }



    DLTRACEOUT((""));
    }


void CNcdPurchaseOptionImpl::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                  TInt aFunctionNumber )
    {
    DLTRACEIN((""));    

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
    
    TInt trapError( KErrNone );
    
    // Check which function is called by the proxy side object.
    // Function number are located in ncdnodefunctinoids.h file.
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdInternalize:
            // Internalize the proxy side according to the data
            // of this object.
            TRAP( trapError, InternalizeRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdRelease:
            // The proxy does not want to use this object anymore.
            // So, release the handle from the session.
            ReleaseRequest( *aMessage );
            break;
                    
        default:
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong, the complete has not been
        // yet called for the message.
        // So, inform the client about the error if the
        // message is still available.
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;        
    
    DLTRACEOUT((""));
    }

void CNcdPurchaseOptionImpl::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }
    }


void CNcdPurchaseOptionImpl::InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );


    // Include all the necessary node data to the stream
    ExternalizeDataForRequestL( stream );     
    
    
    // Commits data to the stream when closing.
    CleanupStack::PopAndDestroy( &stream );


    // If this leaves, ReceiveMessge will complete the message.
    // NOTE: that here we expect that the buffer contains at least
    // some data. So, make sure taht ExternalizeDataForRequestL inserts
    // something to the buffer.
    aMessage.CompleteAndReleaseL( buf->Ptr( 0 ), KErrNone );        
        
    
    DLTRACE(("Deleting the buf"));
    CleanupStack::PopAndDestroy( buf );
        
    DLTRACEOUT((""));
    }
    

void CNcdPurchaseOptionImpl::ExternalizeDataForRequestL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));

    // First insert data that the creator of this class object will use to
    // create this class object. Class id informs what class object
    // will be created.
    aStream.WriteInt32L( ClassId() );
    
    DLINFO(( _L("Externalizing name: %S"), iName ));

    ExternalizeDesL( *iName, aStream );
    ExternalizeDesL( *iPriceText, aStream );

    aStream.WriteReal32L( iPrice );
    ExternalizeDesL( *iPriceCurrency, aStream );
    
    aStream.WriteInt32L( iIsFree );
    
    ExternalizeDesL( *iPurchaseOptionId, aStream );

    //ExternalizeDesL( *iPurchase, aStream );
    aStream.WriteInt32L( iType );
    
    DLINFO(( "Externalizing purchase option type: %d", iType ));
    
    aStream.WriteInt32L( iRequirePurchaseProcess );

    DLINFO(( "Externalizing require purchase process: %d", iRequirePurchaseProcess ));


    // Let's externalize subscription related aggregate classes

    // Externalization of these could be moved into
    // the aggregate classes
    
    if ( iSubscription != NULL )
        {
        // indicate that subscription exists
        aStream.WriteInt32L( ETrue );

        aStream.WriteInt32L( iSubscription->ValidityDelta() );
        DLINFO(( "Externalized validitydelta: %d",
                 iSubscription->ValidityDelta() ));
        
        aStream.WriteInt32L( iSubscription->ValidityAutoUpdate() );
        DLINFO(( "Externalized validity auto update: %d",
                 iSubscription->ValidityAutoUpdate() ));
        
        aStream.WriteReal32L( iSubscription->AmountOfCredits() );
        DLINFO(( "Externalized amount of credits: %f.",
                 iSubscription->AmountOfCredits() ));
        
        ExternalizeDesL(
            iSubscription->AmountOfCreditsCurrency(),
            aStream );
        DLINFO(( _L("Externalized credits currency: %S"),
                 &iSubscription->AmountOfCreditsCurrency() ));
        
        aStream.WriteInt32L( iSubscription->NumberOfDownloads() );
        DLINFO(( _L("Externalized number of downloads: %d"),
                 iSubscription->NumberOfDownloads() ));
        
        }
    else
        {
        // Does not exist
        aStream.WriteInt32L( EFalse );
        DLINFO(( _L("Externalizing po impl: Subscription component did not exist.") ));
        }


    if ( iPartOfSubscription != NULL )
        {
        // indicate that part of subscription exists
        aStream.WriteInt32L( ETrue );
        
        DLINFO(( _L("Parent entity id: %S"),
                 &iPartOfSubscription->ParentEntityId() ));
        DLINFO(( _L("Parent entity po id: %S"),
                 &iPartOfSubscription->ParentPurchaseOptionId() ));
        
        ExternalizeDesL(
            iPartOfSubscription->ParentEntityId(),
            aStream );
        ExternalizeDesL(
            iPartOfSubscription->ParentPurchaseOptionId(),
            aStream );
            
        aStream.WriteReal32L( iPartOfSubscription->CreditPrice() );
        
        DLINFO(( _L("Externalized part of subscription info") ));
        }
    else
        {
        // Does not exist
        aStream.WriteInt32L( EFalse );
        DLINFO(( _L("Externalizing po impl: Part of subscription component did not exist.") ));
        }

    if ( iUpgrade != NULL )
        {
        // indicate that upgrade exists
        aStream.WriteInt32L( ETrue );

        ExternalizeDesL( iUpgrade->DependencyId(), aStream );
        DLINFO(( _L("Upgrade dependency id: %S"),
                 &iUpgrade->DependencyId() ));

        aStream.WriteInt32L( iUpgrade->ValidityDelta() );
        aStream.WriteReal32L( iUpgrade->AmountOfCredits() );
        aStream.WriteInt32L( iUpgrade->NumberOfDownloads() );
        }
    else
        {
        // Does not exist
        aStream.WriteInt32L( EFalse );
        DLINFO(( _L("Externalizing po impl: Upgrade component did not exist.") ));
        }

    DLTRACEOUT((""));
    }

void CNcdPurchaseOptionImpl::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );

    // Send complete information back to proxy.
    aMessage.CompleteAndRelease( KErrNone );
        
    // Remove this object from the session.
    requestSession.RemoveObject( handle );
        
    DLTRACEOUT((""));
    }


void CNcdPurchaseOptionImpl::InternalizeSubscriptionsInfoL( 
    const MNcdPreminetProtocolPurchaseOption& aOption )
    {
    
    DLTRACEIN((""));

    TNcdPurchaseType purchase = aOption.Purchase();

    // Conversion to type declared in the purchaseoption interface
    
    switch ( purchase )
        {
        case ETypeNotSet: // Flow through, default is EContent
        case EContent:
            iType = MNcdPurchaseOption::EPurchase;
            break;
        case ESubscription:
            iType = MNcdPurchaseOption::ESubscription;
            break;
        case EPartOfSubscription:
            iType = MNcdPurchaseOption::ESubscriptionPurchase;
            break;
        case EUpgrade:
            iType = MNcdPurchaseOption::ESubscriptionUpgrade;
            break;
        default:
            DASSERT( false ); // unsupported type
            break;
        }

    DLTRACE(( _L("Purchaseoption impl, purchase option type: %d."),
              iType ));


    // Po cannot be a subscription and an upgrade at the same time
    // because they use same variables with different meaning
    if( purchase == EUpgrade )
        {
        iUpgrade = CNcdServerUpgrade::NewL();
        
        // If subscription upgrade the po that is upgraded.
        // Possibly also some other meaning?
        iUpgrade->SetDependencyId( aOption.DependencyId().AllocL() );

        // Subscription upgrade stuff...
        iUpgrade->SetValidityDelta( aOption.SubscriptionDetails()->ValidityDelta() );
        iUpgrade->SetAmountOfCredits(
            aOption.SubscriptionDetails()->AmountOfCredits() );
        iUpgrade->SetNumberOfDownloads(
            aOption.SubscriptionDetails()->NumberOfDownloads() );

        DLINFO(( "Purchaseoption impl, upgrade, amount of credits: %f.",
                 iUpgrade->AmountOfCredits() ));
        DLINFO(( "Purchaseoption impl, upgrade, validitydelta: %d.",
                 iUpgrade->ValidityDelta() ));
        DLINFO(( "Purchaseoption impl, upgrade, downloads: %d.",
                 iUpgrade->NumberOfDownloads() ));

        }
    else if ( purchase == ESubscription )
        {        
        // check if we have a subscription
        
        iSubscription = CNcdServerSubscription::NewL();
        
        iSubscription->SetValidityDelta(
            aOption.SubscriptionDetails()->ValidityDelta() );
        iSubscription->SetValidityAutoUpdate(
            aOption.SubscriptionDetails()->ValidityAutoUpdate() );
        iSubscription->SetAmountOfCredits(
            aOption.SubscriptionDetails()->AmountOfCredits() );
        iSubscription->SetAmountOfCreditsCurrency(
            aOption.SubscriptionDetails()->AmountOfCreditsCurrency().AllocL() );
        iSubscription->SetNumberOfDownloads(
            aOption.SubscriptionDetails()->NumberOfDownloads() );

        DLINFO(( "Purchaseoption impl, subscription, amount of credits in protocol: %f.",
                 aOption.SubscriptionDetails()->AmountOfCredits() ));
        DLINFO(( "Purchaseoption impl, subscription, amount of credits: %f.",
                 iSubscription->AmountOfCredits() ));
        DLINFO(( "Purchaseoption impl, subscription, validitydelta: %d.",
                 iSubscription->ValidityDelta() ));
        DLINFO(( "Purchaseoption impl, subscription, downloads: %d.",
                 iSubscription->NumberOfDownloads() ));
        }
        
    // Still have to check if we have a part of subscription

    DLTRACE(( _L("Purchaseoption impl, Checking part of subscription info.") ));
    
    const TDesC& tmpParentSubscriptionEntityId =
        aOption.ParentSubscriptionEntityId();

    DLTRACE(( _L("Purchaseoption impl, parent entity id: \"%S\"."),
              &tmpParentSubscriptionEntityId ));

    // According to protocol if we have a part of subscription then
    // parent subscription entity id is a mandatory field.
    if ( tmpParentSubscriptionEntityId != KNullDesC )
        {
        iPartOfSubscription = CNcdServerPartOfSubscription::NewL();
        
        iPartOfSubscription->SetParentEntityId(
            aOption.ParentSubscriptionEntityId().AllocL() );
        iPartOfSubscription->SetParentPurchaseOptionId(
            aOption.ParentSubscriptionPurchaseOptionId().AllocL() );
        iPartOfSubscription->SetCreditPrice( aOption.CreditPrice() );


        DLINFO(( "Purchaseoption impl, part of subscription, credit price: %f.",
                 iPartOfSubscription->CreditPrice() ));
        DLINFO(( _L("Purchaseoption impl, part of subscription, parent entity id: %S."),
                 &iPartOfSubscription->ParentEntityId() ));
        DLINFO(( _L("Purchaseoption impl, part of subscription, parent po id: %S."),
                 &iPartOfSubscription->ParentPurchaseOptionId() ));
        }

    DLTRACEOUT((""));
    }



void CNcdPurchaseOptionImpl::InternalizeDownloadDetailsL( 
    const MNcdPreminetProtocolPurchaseOption& aOption )
    {
    DLTRACEIN((""));
    
    const TInt KDetailsCount( aOption.DownloadDetailsCount() );
    TInt detailsIndexer( 0 );

    DLTRACE(( _L("Purchaseoption download details amount: %d."),
              KDetailsCount ));
    
    while ( detailsIndexer < KDetailsCount )
        {        
        const MNcdPreminetProtocolDownload& tmpDownloadDetails =
            aOption.DownloadDetailsL( detailsIndexer );
        
        CNcdPurchaseDownloadInfo* tempInfo = 
            CNcdPurchaseDownloadInfo::NewLC();
            
        TNcdDownloadTargetType tmpTarget = tmpDownloadDetails.Target();
        // Conversion from TNcdDownloadTargetType to TContentUsage
        MNcdPurchaseDownloadInfo::TContentUsage tmpUsage = 
            MNcdPurchaseDownloadInfo::EDownloadable;
        
        switch ( tmpTarget )
            {
            case EDownloadTargetDownloadable:
                tmpUsage = MNcdPurchaseDownloadInfo::EDownloadable;
                break;
            case EDownloadTargetConsumable:
                tmpUsage = MNcdPurchaseDownloadInfo::EConsumable;
                break;
            default:
                DASSERT( false ); // unsupported type
                break;
            }
        tempInfo->SetContentUsage( tmpUsage );
            
        tempInfo->SetContentUriL( tmpDownloadDetails.Uri() );
        tempInfo->SetContentMimeTypeL( tmpDownloadDetails.Mime() );
        tempInfo->SetContentSize( tmpDownloadDetails.Size() );
        tempInfo->SetLaunchable( tmpDownloadDetails.Launchable() );


        const MNcdPreminetProtocolDescriptor* descriptor = 
            tmpDownloadDetails.Descriptor();
        if ( descriptor != NULL )
            {
            tempInfo->SetDescriptorTypeL( descriptor->Type() );
            tempInfo->SetDescriptorNameL( descriptor->Name() );
            tempInfo->SetDescriptorUriL( descriptor->Uri() );
            tempInfo->SetDescriptorDataL( descriptor->Data() );
            }


        const MNcdPreminetProtocolRights* rights =
            tmpDownloadDetails.Rights();
        if ( rights != NULL )
            {        
            tempInfo->SetRightsUriL( rights->Uri() );        
            tempInfo->SetRightsTypeL( rights->Type() );
            }

        tempInfo->SetContentValidityDelta(
            tmpDownloadDetails.ValidityDelta() );

        iDownloadInfo.AppendL( tempInfo );
        CleanupStack::Pop( tempInfo );
        
        ++detailsIndexer;
        }

    DLTRACEOUT((""));
    }


void CNcdPurchaseOptionImpl::ResetMemberVariables()
    {
    // In all cases this state variable is in the beginning false
    // This has to be set especially by the user.
    iRecentlyUpdated = EFalse;

    delete iName;
    iName = NULL;         
    delete iPriceText;
    iPriceText = NULL;
    
    iPrice = -1;
    delete iPriceCurrency;
    iPriceCurrency = NULL;
    
    iIsFree = EFalse;
    delete iPurchaseOptionId; 
    iPurchaseOptionId = NULL;    

    iType = MNcdPurchaseOption::EPurchase;
    iRequirePurchaseProcess = EFalse;

    iDownloadInfo.ResetAndDestroy();

    delete iSubscription;
    iSubscription = NULL;
    delete iPartOfSubscription;
    iPartOfSubscription = NULL;
    delete iUpgrade;
    iUpgrade = NULL;
    }
