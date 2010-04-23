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


#ifndef C_NCD_CLIENT_SUBSCRIPTION
#define C_NCD_CLIENT_SUBSCRIPTION


#include <e32base.h>



/**
 *  Class to represent subscription part of purchaseoption.
 *
 *  Client-side class to represent a subscription part of 
 *  a purchaseoption.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdClientSubscription : public CBase
    {

public:

    static CNcdClientSubscription* NewL();

    static CNcdClientSubscription* NewLC();

    virtual ~CNcdClientSubscription();


    /*
     * Setters for subscription data.
     */
    void SetValidityDelta( TInt aValidityDelta );
    void SetValidityAutoUpdate( TBool aValidityAutoUpdate );
    void SetAmountOfCredits( TReal32 aAmountOfCredits );
    void SetAmountOfCreditsCurrency( HBufC* aAmountOfCreditsCurrency );
    void SetNumberOfDownloads( TInt aNumberOfDownloads );

    




protected:

    CNcdClientSubscription();

    void ConstructL();

private:




private: // data


    /*
     * Subscription data.
     * Own.
     */
    TInt iValidityDelta;
    TBool iValidityAutoUpdate;
    TReal32 iAmountOfCredits;
    HBufC* iAmountOfCreditsCurrency;
    TInt iNumberOfDownloads;


    // Also subscribableiteminfo here?

    };



#endif // C_NCD_CLIENT_SUBSCRIPTION
