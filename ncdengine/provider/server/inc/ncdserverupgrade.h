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


#ifndef C_NCD_SERVER_UPGRADE
#define C_NCD_SERVER_UPGRADE

#include <e32base.h>



/**
 *  Class to represent upgrade part of purchaseoption.
 *
 *  Server-side class to represent a upgrade part of 
 *  a purchaseoption.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdServerUpgrade : public CBase
    {

public:

    static CNcdServerUpgrade* NewL();

    static CNcdServerUpgrade* NewLC();

    virtual ~CNcdServerUpgrade();


    /*
     * Setters for upgrade data.
     */
    void SetDependencyId( HBufC* aDependencyId );
    void SetValidityDelta( TInt aValidityDelta );
    void SetAmountOfCredits( TReal32 aAmountOfCredits );
    void SetNumberOfDownloads( TInt aNumberOfDownloads );

    /*
     * Getters for upgrade data.
     */
    const TDesC& DependencyId() const;
    TInt ValidityDelta() const;
    TReal32 AmountOfCredits() const;
    TInt NumberOfDownloads() const;

protected:

    CNcdServerUpgrade();

    void ConstructL();

private:




private: // data


    /*
     * Upgrade data.
     * Own.
     */
    HBufC* iDependencyId;
    TInt iValidityDelta;
    TReal32 iAmountOfCredits;
    TInt iNumberOfDownloads;

    };



#endif // C_NCD_SERVER_UPGRADE
