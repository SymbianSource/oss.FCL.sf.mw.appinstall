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
	

#ifndef M_NCD_QUERY_ITEM_H
#define M_NCD_QUERY_ITEM_H

#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
    
/**
 *  Describes a query item.
 *
 *  A common interface for all query items. A query item defines
 *  one element of a whole query e.g. credit card number in a purchase 
 *  information query.
 *
 *  All query items that are not optional must be answered/filled before 
 *  completing a query.
 *
 *  @see MNcdQuery
 */
class MNcdQueryItem : public virtual MCatalogsBase
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdQueryItemUid };

    /**
     * Query item semantics
     */
    enum TSemantics
        {
        /** No specific semantics specified */
        ESemanticsNone,

        /** Unknown/unsupported semantics specified */
        ESemanticsUnknown,

        /** MSISDN (phone number) */
        ESemanticsMsisdn,

        /** Email address */
        ESemanticsEmailAddress,

        /** Credit card number */
        ESemanticsCreditCardNumber,

        /** Credit card expiration year */
        ESemanticsCreditCardExpirationYear,

        /** Credit card expiration month */
        ESemanticsCreditCardExpirationMonth,
        
        /** Credit card owner */
        ESemanticsCreditCardOwner,

        /** Credit card verification code */
        ESemanticsCreditCardVerificationCode,

        /** Credit card type */
        ESemanticsCreditCardType,

        /** Street address */
        ESemanticsAddressStreet,

        /** ZIP code */
        ESemanticsAddressZipCode,

        /** City */
        ESemanticsAddressCity,

        /** Country */
        ESemanticsAddressCountry,

        /** User name */
        ESemanticsUserName,

        /** Password */
        ESemanticsPassword,

        /** PIN code */
        ESemanticsPinCode,
        
        /** IMEI */
        ESemanticsImei
        };

    /**
     * Type of the query.
     *
     * 
     * @return Interface id of the operation.
     */
    virtual TNcdInterfaceId Type() const = 0;
    
    /**
     * Semantics of the query item.
     *
     * This can be used to act differently for semantically
     * different query items that have the same basic type
     * (as indicated by Type()).
     *
     * 
     * @return Semantics for the item.
     */
    virtual TSemantics Semantics() const = 0;

    /**
     * Getter for query item label. Labels are used to identify the
     * what the query item is e.g. "username" or "credit card owner".
     * Typically displayed next to the actual item.
     *
     * 
     * @return Label text. KNullDesC() if no label available.
     */
    virtual const TDesC& Label() const = 0;
    
    /**
     * Getter for query item message. Optionally displayed in a question
     * dialog body close to the query item. Can offer information/advice on what
     * the user should input e.g. "Please enter the cardholder's name".
     *
     * 
     * @return Message text. KNullDesC() if no message available.
     */
    virtual const TDesC& Message() const = 0;
    
    /**
     * Getter for query item description. Additional longer description text
     * about the query element, intended for context help
     *
     * 
     * @return Message text. KNullDesC() if no description available.
     */
    virtual const TDesC& Description() const = 0;
    
    /**
     * Checks whether this query item is optional i.e. does not need
     * to be answered/filled before the query is accepted.
     *
     * @return ETrue if the item is optional, EFalse if it is mandatory.
     */
    virtual TBool IsOptional() const = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdQueryItem() {}

    };
	
#endif //  M_NCD_QUERY_ITEM_H
