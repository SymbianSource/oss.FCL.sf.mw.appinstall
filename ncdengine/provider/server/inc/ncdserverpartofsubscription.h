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


#ifndef C_NCD_SERVER_PART_OF_SUBSCRIPTION
#define C_NCD_SERVER_PART_OF_SUBSCRIPTION

#include <e32base.h>



/**
 *  Class to represent partofsubscription part of purchaseoption.
 *
 *  Server-side class to represent a partofsubscription part of 
 *  a purchaseoption.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdServerPartOfSubscription : public CBase
    {

public:

    static CNcdServerPartOfSubscription* NewL();

    static CNcdServerPartOfSubscription* NewLC();

    virtual ~CNcdServerPartOfSubscription();
    
    /*
     * Setters for part of subscription data.
     */
    void SetParentEntityId( HBufC* aParentEntityId );
    void SetParentPurchaseOptionId( HBufC* aParentPurchaseOptionId );
    void SetCreditPrice( TReal32 aCreditPrice );

    /*
     * Getters for part of subscription data.
     */
    const TDesC& ParentEntityId() const;
    const TDesC& ParentPurchaseOptionId() const;
    TReal32 CreditPrice() const;


protected:

    CNcdServerPartOfSubscription();

    void ConstructL();

private:




private: // data

    /*
     * Part of subscription data.
     * Own.
     */
    HBufC* iParentEntityId;
    HBufC* iParentPurchaseOptionId;
    TReal32 iCreditPrice;

    };



#endif // C_NCD_SERVER_PART_OF_SUBSCRIPTION
