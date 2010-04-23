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
	

#ifndef M_NCD_QUERY_H
#define M_NCD_QUERY_H

#include <e32cmn.h>

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "catalogsarray.h"

class MNcdQueryItem;
    
/**
 *  Describes a query.
 *
 *  The Catalogs Engine may send queries to the client as part of specific
 *  operations, or as general callbacks. 
 *
 *  Querys consist of a message and query items. A query can have just a
 *  message or just a range of query items or both.
 *
 *  The query message can be for example a disclaimer or a plain iformation
 *  message. The query message is intented to be shown separate from the query
 *  items.
 *
 *  If the query demands some input from the user it will contain query items.
 *  A query item defines one element of a whole query e.g. credit card number in a purchase 
 *  information query.
 *
 *  Query objects can be used to query a wide variety of things from the
 *  client: disclaimers, user acceptance for web-connection usage,
 *  credit card information etc.
 *
 *  Example: 
 *
 *  message: "Please enter creditcard information to complete the purchase"
 *  queryitem: "Creditcard owner:"
 *  queryitem: "Creditcard expiration date:"
 *  queryitem: "Creditcard number:"
 *  
 *
 *  Querys can be sent from a server at any time so the client needs to
 *  be prepared to handle them.
 *  
 *  Most operations use querys when user interaction is needed.
 *
 *  @note Query objects don't do any validity checks for input data.
 *
 *  @note: Querys are simple objects that have no connection to Catalogs Engine
 *         of their own. The object that sent the query may need to be notified of
 *         the completion of the query (e.g. MNcdOperation::QueryCompleted).
 *
 *  @see MNcdQueryItem
 *  @see MNcdOperation
 *  @see MNcdProviderObserver
 */
class MNcdQuery : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdQueryUid };

    /**
     * Query response states.
     *
     * 
     */
    enum TResponse
        {
        /** No response has been set. */
        ENoResponse,

        /** Query has been accepted. */
        EAccepted,

        /** Query has been rejected. */
        ERejected
        };

    /**
     * Query semantics
     */
    enum TSemantics
        {
        /** No specific semantics specified */
        ESemanticsNone,

        /** Unknown/unsupported semantics specified */
        ESemanticsUnknown,

        /** Information (just show) */
        ESemanticsInfoMessage,
        
        /** Error (just show) */
        ESemanticsErrorMessage,

        /** Disclaimer (show, request user acceptance) */
        ESemanticsDisclaimer,
        
        /** Operator selection */
        ESemanticsOperatorQuery,
        
        /** Language selection */
        ESemanticsLanguageQuery,
        
        /** User registration */        
        ESemanticsRegistrationQuery,
        
        /** User authentication */
        ESemanticsAuthenticationQuery,
        
        /** Review query (e.g. a review score for an item) */
        ESemanticsReviewQuery,

        /** Payment method selection */
        ESemanticsPaymentMethodSelectionQuery,
        
        /** Confirmation query (yes/no) */
        ESemanticsConfirmationQuery,

        /** Advertisement (just show) */
        ESemanticsAdvertisementMessage,

        /** Payment input fields */
        ESemanticsPaymentQuery

        };

    /**
     * Checks whether this query is optional.
     *
     * @return ETrue if the query is optional, EFalse if it is mandatory.
     */
    virtual TBool IsOptional() const = 0;
    
    /**
     * Semantics of the query.
     *
     * This can be used to act differently for semantically
     * different querys (disclaimer, information message etc).
     *
     * 
     * @return Semantics for the item.
     */
    virtual TSemantics Semantics() const = 0;
    
    /**
     * Getter for query message title. Intented be shown with the message body
     * if available.
     *
     * 
     * @return Message text. KNullDesC() if no message title available.
     */
    virtual const TDesC& MessageTitle() const = 0;
    
    /**
     * Getter for query message body. If available, should always be shown
     * to the user e.g. as a separate dialog.
     *
     * 
     * @return Message text. KNullDesC() if no message body available.
     */
    virtual const TDesC& MessageBody() const = 0;
    
    /**
     * Returns the query items.
     *
     * 
     * @return Array of query item pointers. Counted, Release() must be called
     *         for each after use e.g. with a call to RCatalogsArray::ResetAndDestroy().
     * @exception Leave System wide error code.
     */
    virtual RCatalogsArray< MNcdQueryItem > QueryItemsL() = 0;

    /**
     * Sets the query response.
     *
     * Query response accepts or rejects the query as a whole (including all query
     * items). Query response must be set before the query is completed.
     *
     * 
     * @note All non-optional query items need to be set before the query can be
     *       accepted (response set to EAccepted).
     * @param aResponse Either MNcdQuery::EAccepted or MNcdQuery::ERejected
     * @exception KErrNotReady Query cannot be accepted, there are
     *            non-optional query items whose value has not been set.
     */
    virtual void SetResponseL( TResponse aResponse ) = 0;

    /**
     * Gets the query response.
     *
     * @return Query response, as set with SetResponseL() (or MNcdQuery::ENoResponse
     *         if not set).
     */
    virtual TResponse Response() = 0;
    
    /**
     * Checks whether the query response will be sent over a secure connection.
     *
     * @return ETrue if connection is secure, EFalse if not.
     */
    virtual TBool IsSecureConnection() const = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdQuery() {}

    };
	
	
#endif //  M_NCD_QUERY_H
