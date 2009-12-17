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
* Description:   CNcdRequestPurchase implementation
*
*/


#include "ncdrequestpurchase.h"
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "catalogsdebug.h"
#include "ncdprotocolutils.h"
//#include "ncdprotocoltypes.h"
#include "ncdprotocolwords.h"
#include "ncdpaymentmethod.h"

CNcdRequestPurchase* CNcdRequestPurchase::NewL()
    {
    CNcdRequestPurchase* self = CNcdRequestPurchase::NewLC( );
    CleanupStack::Pop();
    return self;
    }

CNcdRequestPurchase* CNcdRequestPurchase::NewLC()
    {
    CNcdRequestPurchase* self = new (ELeave) CNcdRequestPurchase();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CNcdRequestPurchase::ConstructL()
    {
    CNcdRequestBase::ConstructL( KTagPreminetRequest );
    iName.SetL(KTagPurchase);
    iNamespaceUri.SetL(KDefaultNamespaceUri);
    iPrefix = TXmlEngString();
    iVersion.SetL(KAttrPreminetVersion);
    iType = TXmlEngString();
    iConfirmationSet = EFalse;
    iGetDownloadDetailsSet = EFalse;
    }
    
CNcdRequestPurchase::CNcdRequestPurchase()
: CNcdRequestBase()
    {
    }

CNcdRequestPurchase::~CNcdRequestPurchase()
    {
    DLTRACEIN((""));
    iName.Free();
    iNamespaceUri.Free();
    iPrefix.Free();
    iType.Free();
    iTransactionId.Free();
    
    for (TInt i = 0; i < iEntityDetails.Count(); ++i)
        {
        TNcdRequestPurchaseEntity e = iEntityDetails[i];
        e.id.Free();
        e.timestamp.Free();
        e.purchaseOptionId.Free();
        e.ticket.Free();
        }
    iEntityDetails.Close();


    for (TInt i = 0; i < iConfirmation.entities.Count(); ++i)
        {
        TNcdRequestDeliverablePurchaseEntity e =
            iConfirmation.entities[i];
        e.id.Free();
        e.timestamp.Free();
        e.purchaseOptionId.Free();
        e.ticket.Free();
        e.deliveryMethod.Free();
        }
    iConfirmation.entities.Close();
    iConfirmation.queryResponseId.Free();
    iConfirmation.paymentMethod.Free();


    
    iGetDownloadDetails.id.Free();
    iGetDownloadDetails.timestamp.Free();
    iGetDownloadDetails.purchaseOptionId.Free();
    iGetDownloadDetails.ticket.Free();
    DLTRACEOUT((""));
    }

void CNcdRequestPurchase::SetTransactionIdL( const TDesC& aTransactionId )
    {
    iTransactionId.SetL(aTransactionId);
    }

void CNcdRequestPurchase::SetCancelL( TBool aCancel ) 
    {
    iCancel = aCancel ? EValueTrue : EValueFalse;
    }

void CNcdRequestPurchase::UnsetCancel() 
    {
    iCancel = EValueNotSet;
    }

void CNcdRequestPurchase::AddEntityDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId )
    {
    TNcdRequestPurchaseEntity entity;
    entity.id.SetL(aId);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.ticket = TXmlEngString();
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.gift = EValueNotSet;
    iEntityDetails.AppendL(entity);
    }

void CNcdRequestPurchase::AddEntityDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, const TDesC& aTicket )
    {
    TNcdRequestPurchaseEntity entity;
    entity.id.SetL(aId);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.ticket.SetL(aTicket);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.gift = EValueNotSet;
    iEntityDetails.AppendL(entity);
    }

void CNcdRequestPurchase::AddEntityDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, const TDesC& aTicket, TBool aGift )
    {
    TNcdRequestPurchaseEntity entity;
    entity.id.SetL(aId);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.ticket.SetL(aTicket);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.gift = aGift ? EValueTrue : EValueFalse;
    iEntityDetails.AppendL(entity);
    }


void CNcdRequestPurchase::SetPurchaseConfirmationL(
    MNcdPaymentMethod::TNcdPaymentMethodType aPaymentMethod )
    {
    SetPurchaseConfirmationL(KNullDesC, aPaymentMethod);
    }

void CNcdRequestPurchase::SetPurchaseConfirmationL(
    const TDesC& aQueryResponseId, 
    MNcdPaymentMethod::TNcdPaymentMethodType aPaymentMethod )
    {
    iConfirmation.queryResponseId = TXmlEngString();
    if ( aQueryResponseId != KNullDesC )
        {
        iConfirmation.queryResponseId.SetL( aQueryResponseId );
        }

    iConfirmation.paymentMethod = TXmlEngString();
    if ( aPaymentMethod == MNcdPaymentMethod::EPaymentSms )
        {
        iConfirmation.paymentMethod.SetL( KValueSms );
        }
    else if ( aPaymentMethod == MNcdPaymentMethod::EPaymentCreditCard )
        {
        iConfirmation.paymentMethod.SetL( KValueCreditCard );
        }
    else if ( aPaymentMethod == MNcdPaymentMethod::EPaymentDirect )
        {
        iConfirmation.paymentMethod.SetL( KValueDirect );
        }
    else if ( aPaymentMethod == MNcdPaymentMethod::EPaymentCustom )
        {
        iConfirmation.paymentMethod.SetL( KValueCustom );
        }

    iConfirmationSet = ETrue;
    // entities must be set separately using AddPurchaseConfirmationEntityL()
    }

void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId )
    {
    AddPurchaseConfirmationEntityL(aId, aTimestamp, aPurchaseOptionId, EDeliveryNotSet);
    }
    
void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, 
    TNcdDeliveryMethod aDeliveryMethod )
    {
    TNcdRequestDeliverablePurchaseEntity entity;
    entity.id.SetL(aId);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.ticket = TXmlEngString();
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.gift = EValueNotSet;

    AddPurchaseConfirmationEntityL(entity, aDeliveryMethod);
    }

void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, 
    const TDesC& aTicket )
    {
    TNcdRequestDeliverablePurchaseEntity entity;
    entity.id.SetL(aId);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.ticket.SetL(aTicket);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.gift = EValueNotSet;

    AddPurchaseConfirmationEntityL(entity, EDeliveryNotSet);
    }

void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, 
    const TDesC& aTicket,
    TNcdDeliveryMethod aDeliveryMethod )
    {
    TNcdRequestDeliverablePurchaseEntity entity;
    entity.id.SetL(aId);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.ticket.SetL(aTicket);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.gift = EValueNotSet;

    AddPurchaseConfirmationEntityL(entity, aDeliveryMethod);
    }

void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, 
    const TDesC& aTicket, 
    TBool aGift,
    TNcdDeliveryMethod aDeliveryMethod )
    {
    TNcdRequestDeliverablePurchaseEntity entity;
    entity.id.SetL(aId);
    entity.purchaseOptionId.SetL(aPurchaseOptionId);
    entity.ticket.SetL(aTicket);
    if ( aTimestamp != KNullDesC )
        entity.timestamp.SetL(aTimestamp);
    entity.gift = aGift ? EValueTrue : EValueFalse;

    AddPurchaseConfirmationEntityL(entity, aDeliveryMethod);
    }
    
void CNcdRequestPurchase::AddPurchaseConfirmationEntityL( 
    TNcdRequestDeliverablePurchaseEntity aEntity,
    TNcdDeliveryMethod aDeliveryMethod )
    {
    TXmlEngString method;
    switch (aDeliveryMethod)
        {
            case EDeliverySms:
                {
                method = "sms";
                break;
                }
            case EDeliveryWapPush:
                {
                method = "wap-push";
                break;
                }
            case EDeliveryDirect:
                {
                method = "direct";
                break;
                }
            case EDeliveryNone:
                {
                method = "none";
                break;
                }
            default:
            break;
        }
    aEntity.deliveryMethod = method;
    iConfirmation.entities.AppendL(aEntity);
    }


void CNcdRequestPurchase::SetGetDownloadDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId )
    {
    SetGetDownloadDetailsL(aId, aTimestamp, aPurchaseOptionId, KNullDesC, EFalse);
    }
    
void CNcdRequestPurchase::SetGetDownloadDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, const TDesC& aTicket )
    {
    SetGetDownloadDetailsL(aId, aTimestamp, aPurchaseOptionId, aTicket, EFalse);
    }
    
void CNcdRequestPurchase::SetGetDownloadDetailsL( 
    const TDesC& aId, const TDesC& aTimestamp, 
    const TDesC& aPurchaseOptionId, const TDesC& aTicket, TBool aGift )
    {
    iGetDownloadDetails.id.SetL(aId);
    iGetDownloadDetails.timestamp.SetL(aTimestamp);
    iGetDownloadDetails.purchaseOptionId.SetL(aPurchaseOptionId);

    iGetDownloadDetails.ticket = TXmlEngString();
    if ( aTicket != KNullDesC )
        iGetDownloadDetails.ticket.SetL(aTicket);
    
    iGetDownloadDetails.gift = aGift ? EValueTrue : EValueFalse;
    iGetDownloadDetailsSet = ETrue;
    }
    

// generates the dom nodes
HBufC8* CNcdRequestPurchase::CreateRequestL()
    {
    DLTRACEIN((_L("entry")));
    // generate browse part of the request
    CNcdRequestBase::SetProtocolVersionL(KPreminetReqVersion);
    iRequestElement = NcdProtocolUtils::NewElementL(iDocument, KTagPurchase);
    if (iConfiguration)
        iConfiguration->SetNamespacePrefixL( KAttrCdpNamespacePrefix );
    
    if ( iCancel != EValueNotSet )
        NcdProtocolUtils::NewAttributeL( iRequestElement, KAttrCancel, iCancel );
    
    if ( iTransactionId.NotNull() ) 
        NcdProtocolUtils::NewAttributeL( iRequestElement, KAttrTransactionId, iTransactionId );
        
    
    // only one of the following parts is allowed at a time
    if (!iConfirmationSet && iEntityDetails.Count() > 0) 
        {
        for (TInt i = 0; i < iEntityDetails.Count(); ++i)
            {
            // create remote entity elements
            TNcdRequestPurchaseEntity entity = iEntityDetails[i];
            TXmlEngElement entityElement = 
                NcdProtocolUtils::NewElementL( 
                    iDocument, iRequestElement, KTagEntityDetails );
            NcdProtocolUtils::NewAttributeL( entityElement, KAttrId, entity.id);

            if (entity.timestamp.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrTimestamp, entity.timestamp);

            if (entity.purchaseOptionId.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrPurchaseOptionId, entity.purchaseOptionId);

            if (entity.ticket.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrTicket, entity.ticket);

            if (entity.gift != EValueNotSet) 
                NcdProtocolUtils::NewBoolAttributeL( 
                    entityElement, KAttrGift, entity.gift);
            }
        }
    else if (iConfirmationSet && iConfirmation.entities.Count() > 0) 
        {
        TXmlEngElement confirmation = 
            NcdProtocolUtils::NewElementL( iDocument, iRequestElement, KTagConfirmation );
        
        NcdProtocolUtils::NewAttributeL( confirmation, KAttrQueryResponseId,
            iConfirmation.queryResponseId );
        TXmlEngElement entities = 
            NcdProtocolUtils::NewElementL( iDocument, confirmation, KTagEntities );
        for (TInt i = 0; i < iConfirmation.entities.Count(); ++i)
            {
            TNcdRequestDeliverablePurchaseEntity entity = 
                iConfirmation.entities[i];
            TXmlEngElement entityElement = 
                NcdProtocolUtils::NewElementL( iDocument, entities, KTagEntity );
            
            NcdProtocolUtils::NewAttributeL( 
                entityElement, KAttrId, entity.id);
            
            if (entity.timestamp.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrTimestamp, entity.timestamp);
            
            if (entity.purchaseOptionId.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrPurchaseOptionId, 
                    entity.purchaseOptionId);

            if (entity.ticket.NotNull())
                NcdProtocolUtils::NewAttributeL( 
                    entityElement, KAttrTicket, entity.ticket);

            if (entity.gift != EValueNotSet)
                NcdProtocolUtils::NewBoolAttributeL( 
                    entityElement, KAttrGift, entity.gift);

            if (entity.deliveryMethod.NotNull())
                {
                TXmlEngElement deliveryElement = 
                    NcdProtocolUtils::NewElementL( 
                        iDocument, iRequestElement, KTagDelivery );
                NcdProtocolUtils::NewAttributeL( 
                    deliveryElement, KAttrMethod, entity.deliveryMethod);
                }
            }
        TXmlEngElement payment = 
            NcdProtocolUtils::NewElementL( iDocument, confirmation, KTagPayment );
        NcdProtocolUtils::NewAttributeL( payment, KAttrMethod, iConfirmation.paymentMethod);
        
        }
    else if (iGetDownloadDetailsSet) 
        {
        TXmlEngElement getDownloadDetails = 
            NcdProtocolUtils::NewElementL( iDocument, iRequestElement, KTagGetDownloadDetails );
        NcdProtocolUtils::NewAttributeL( 
            getDownloadDetails, KAttrId, iGetDownloadDetails.id);
        NcdProtocolUtils::NewAttributeL( 
            getDownloadDetails, KAttrTimestamp, iGetDownloadDetails.timestamp);
        NcdProtocolUtils::NewAttributeL( 
            getDownloadDetails, KAttrPurchaseOptionId, iGetDownloadDetails.purchaseOptionId);
        NcdProtocolUtils::NewAttributeL( 
            getDownloadDetails, KAttrTicket, iGetDownloadDetails.ticket);
        NcdProtocolUtils::NewBoolAttributeL( 
            getDownloadDetails, KAttrGift, iGetDownloadDetails.gift);
        }
        
    DLTRACEOUT((_L("exit")));
    
    // base class generates the complete request
    return CNcdRequestBase::CreateRequestL();
    }
    
    
