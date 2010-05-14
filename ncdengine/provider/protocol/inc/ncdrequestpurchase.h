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
* Description:   CNcdRequestPurchase declaration
*
*/


#ifndef C_NCD_REQUEST_PURCHASE_HH
#define C_NCD_REQUEST_PURCHASE_HH

#include <e32std.h>
#include <e32base.h>
#include "ncdrequestbase.h"
#include "ncdrequestconstants.h"
//#include "ncdprotocoltypes.h"
#include "ncdpaymentmethod.h"

class CNcdRequestPurchase : public CNcdRequestBase
    {

public:

struct TNcdRequestPurchaseEntity 
    {
    TXmlEngString id;
    TXmlEngString timestamp;
    TXmlEngString purchaseOptionId;
    TXmlEngString ticket;
    TNcdBool gift;
    };

struct TNcdRequestDeliverablePurchaseEntity 
    {
    TXmlEngString id;
    TXmlEngString timestamp;
    TXmlEngString purchaseOptionId;
    TXmlEngString ticket;
    TNcdBool gift;
    TXmlEngString deliveryMethod;
    };

struct TNcdRequestPurchaseConfirmation
    {
    TXmlEngString queryResponseId;
    RArray<TNcdRequestDeliverablePurchaseEntity> entities;
    TXmlEngString paymentMethod;
    };


    HBufC8* CreateRequestL();

    void SetTransactionIdL( const TDesC& aTransactionId );
    void SetCancelL( TBool aCancel );
    void UnsetCancel();


    void AddEntityDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId);

    void AddEntityDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket);

    void AddEntityDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket, 
        TBool aGift );
    

    void SetPurchaseConfirmationL( 
        MNcdPaymentMethod::TNcdPaymentMethodType aPaymentMethod );
        
    void SetPurchaseConfirmationL( 
        const TDesC& aQueryResponseId, 
        MNcdPaymentMethod::TNcdPaymentMethodType aPaymentMethod );
        
    void AddPurchaseConfirmationEntityL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId );

    void AddPurchaseConfirmationEntityL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId,
        TNcdDeliveryMethod aDeliveryMethod );

    void AddPurchaseConfirmationEntityL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket );

    void AddPurchaseConfirmationEntityL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket,
        TNcdDeliveryMethod aDeliveryMethod );

    void AddPurchaseConfirmationEntityL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket, 
        TBool aGift, TNcdDeliveryMethod aDeliveryMethod );


    void SetGetDownloadDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId );
    
    void SetGetDownloadDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket );
    
    void SetGetDownloadDetailsL( 
        const TDesC& aId, const TDesC& aTimestamp, 
        const TDesC& aPurchaseOptionId, const TDesC& aTicket,  
        TBool aGift );
    
    
public:
    static CNcdRequestPurchase* NewL();
    static CNcdRequestPurchase* NewLC();

    void ConstructL();
    
    ~CNcdRequestPurchase();

private:
    CNcdRequestPurchase();

    void AddPurchaseConfirmationEntityL( 
        TNcdRequestDeliverablePurchaseEntity aEntity, 
        TNcdDeliveryMethod aDeliveryMethod );

private:
    TXmlEngString iName;
    TXmlEngString iNamespaceUri;
    TXmlEngString iPrefix;
    TXmlEngString iType;

    TXmlEngString iTransactionId;
    TNcdBool iCancel;
    
    RArray<TNcdRequestPurchaseEntity> iEntityDetails;
    
    TNcdRequestPurchaseConfirmation iConfirmation;
    TBool iConfirmationSet;
    
    TNcdRequestPurchaseEntity iGetDownloadDetails;
    TBool iGetDownloadDetailsSet;
    
    };

#endif //C_NCD_REQUEST_PURCHASE_HH
    