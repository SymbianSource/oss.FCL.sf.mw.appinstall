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

#ifndef NCD_PP_PURCHASE_H
#define NCD_PP_PURCHASE_H

#include "ncdprotocoltypes.h"
#include "ncdpaymentmethod.h"

class MNcdPreminetProtocolDownload;
class MNcdConfigurationProtocolQuery;
class CNcdString;

class MNcdPreminetProtocolSmsDetails
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolSmsDetails() {}

    /**
     * Get SMS destination address
     * @return Address string (usually the phone number)
     */
    virtual const TDesC& Address() const = 0;

    /**
     * Get SMS message
     * @return Message string
     */
    virtual const TDesC& Message() const = 0;
    
    /**
     * Clones the object.
     * @return The new object.
     */
    virtual MNcdPreminetProtocolSmsDetails* CloneL() const = 0;
    };


class MNcdPreminetProtocolPayment
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPayment() {}

    /**
     * Payment method
     * @return Method type
     * @see MNcdPaymentMethod::TNcdPaymentMethodType
     */
    virtual MNcdPaymentMethod::TNcdPaymentMethodType Method() const = 0;

    /**
     * Payment query ID
     * @return Query ID or KNullDesC
     */
    virtual const TDesC& QueryId() const = 0;

    /**
     * Payment name
     * @return Name
     */
    virtual const CNcdString& Name() const = 0;

    /**
     * SMS Details of payment
     * @param aIndex Index of SMS details element. Leaves if index is out of bounds.
     * @return SMS details object reference
     */
    virtual const MNcdPreminetProtocolSmsDetails& 
        SmsDetailsL(TInt aIndex) const = 0;

    /**
     * Amount of SMS details
     * @return SMS details count
     */
    virtual TInt SmsDetailsCount() const = 0;
    
    /**
     * Clones the object.
     * @return The new object.
     */
    virtual MNcdPreminetProtocolPayment* CloneL() const = 0;
    };


class MNcdPreminetProtocolPurchaseEntity
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPurchaseEntity() {}

    /**
     * Ticket for purchase
     * @return Ticket ID or KNullDesC
     */
    virtual const TDesC& Ticket() const = 0;
    
    /**
     * Purchase entity ID
     * @return ID or KNullDesC
     */
    virtual const TDesC& Id() const = 0;
    
    /**
     * Purchase delivery methods
     * @return Array of available methods
     */
    virtual const RArray<TNcdDeliveryMethod>& 
        DeliveryMethods() const = 0;
    
    /**
     * Purchase download details
     * @param aIndex Index of downloads. Leaves if index is out of bounds.
     * @return Download object reference.
     * @see DownloadDetailsCount()
     */
    virtual const MNcdPreminetProtocolDownload&
        DownloadDetailsL(TInt aIndex) const = 0;

    /**
     * Purchase download details count
     * @return Amount of download details
     */
    virtual TInt DownloadDetailsCount() const = 0;
    
    /**
     * Price of purchase entity
     * @return Price string or KNullDesC
     */
    virtual const TDesC& Price() const = 0;

    /**
     * Price currency string
     * @return Currency string or KNullDesC
     */
    virtual const TDesC& PriceCurrency() const = 0;

    /**
     * Price and currency as one string
     * @return Price text or KNullDesC
     */
    virtual const TDesC& PriceText() const = 0;
    };


class MNcdPreminetProtocolPurchaseInformation
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPurchaseInformation() {}

    /**
     * Purchase information URI
     * @return URI or KNullDesC
     */
    virtual const TDesC& Uri() const = 0;
    
    /**
     * Purchase information namespace
     * @return Namespace or KNullDesC
     */
    virtual const TDesC& Namespace() const = 0;
    
    /**
     * Does this purchase initiate a new session
     * @return True of false
     */
    virtual TBool InitiateSession() const = 0;
    
    /**
     * Get payment object for this purchase.
     * @param aIndex Index of purchase. Leaves if index is out of bounds.
     * @return Payment object reference
     * @see PaymentCount()
     */
    virtual const MNcdPreminetProtocolPayment&
        PaymentL(TInt aIndex) const = 0;

    /**
     * Amount of available payments for this purchase.
     * @return Amount of payments
     */
    virtual TInt PaymentCount() const = 0;
    
    /**
     * Get purchase disclaimer. Ownership is NOT transferred.
     * @return Disclaimer object pointer or NULL.
     */
    virtual const MNcdConfigurationProtocolQuery* 
        Disclaimer() const = 0;
    
    /**
     * Total price of purchase.
     * @return Price or KNullDesC
     */
    virtual const TDesC& TotalPrice() const = 0;

    /**
     * Currency of total price.
     * @return Currency string or KNullDesC
     */
    virtual const TDesC& TotalPriceCurrency() const = 0;
    

    /**
     * Get specific entity of this purchase.
     * NOTE: Currently only one entity per purchase is supported.
     * 
     * @param aIndex Index of entity. Leaves if index is out of bounds.
     * @return Purchase entity.
     * @see EntityCount()
     */
    virtual const MNcdPreminetProtocolPurchaseEntity&
        EntityL(TInt aIndex) const = 0;

    /**
     * Amount of entities in this purchase
     * @return Entity count.
     */
    virtual TInt EntityCount() const = 0;

    /**
     * Purchase transaction ID
     * @return ID or KNullDesC
     */
    virtual const TDesC& TransactionId() const = 0;

    /**
     * Purchase query ID
     * @return ID or KNullDesC
     */
    virtual const TDesC& QueryId() const = 0;
    };


class MNcdPreminetProtocolPurchaseProcessed
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPurchaseProcessed() {}

    /**
     * Purchase result code
     * @return Result code
     */
    virtual TInt ResultCode() const = 0;
    
    /**
     * Information of processed purchase.
     * Ownership is NOT transferred.
     * @return Information object pointer or NULL if not available.
     */
    virtual const MNcdConfigurationProtocolQuery* 
        Information() const = 0;

    /**
     * Get specific entity of this purchase.
     * NOTE: Currently only one entity per purchase is supported.
     * 
     * @param aIndex Index of entity. Leaves if index is out of bounds.
     * @return Purchase entity.
     * @see EntityCount()
     */
    virtual const MNcdPreminetProtocolPurchaseEntity&
        EntityL(TInt aIndex) const = 0;

    /**
     * Amount of entities in this purchase
     * @return Entity count.
     */
    virtual TInt EntityCount() const = 0;

    /**
     * Purchase transaction ID
     * @return ID or KNullDesC
     */
    virtual const TDesC& TransactionId() const = 0;

    /**
     * Purchase query ID
     * @return ID or KNullDesC
     */
    virtual const TDesC& QueryId() const = 0;
    };


class MNcdPreminetProtocolPurchase
    {
public: 

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolPurchase() {}

    };

#endif //NCD_PP_PURCHASE_H
