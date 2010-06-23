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


#ifndef C_NCD_SUBSCRIPTIONS_SOURCE_IDENTIFIER_H
#define C_NCD_SUBSCRIPTIONS_SOURCE_IDENTIFIER_H

#include <e32base.h>


/**
 *  Class to identify subscriptions source
 *
 *  Class to identify subscriptions source which is done with
 *  URI and namespace. This knowledge is used to identify for
 *  example where subscription management query is sent.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdSubscriptionsSourceIdentifier : public CBase
    {

public:

    static CNcdSubscriptionsSourceIdentifier* NewL(
        const TDesC& aUri,
        const TDesC& aNamespace,
        TBool aRequireCapabilityCheck );

    static CNcdSubscriptionsSourceIdentifier* NewLC(
        const TDesC& aUri,
        const TDesC& aNamespace,
        TBool aRequireCapabilityCheck );

    virtual ~CNcdSubscriptionsSourceIdentifier();


    /** 
     * Comparison method. Compares the uri and namespace of two
     * identifiers to determine equivalence.
     *
     * @return  Boolean, ETrue if pairs match.
     */
    static TBool CompareIdentifiers( 
        const CNcdSubscriptionsSourceIdentifier& aFirst, 
        const CNcdSubscriptionsSourceIdentifier& aSecond );

    /**
     * Getter for Uri.
     *
     * @since S60 ?S60_version
     * @return Uri
     */
    const TDesC& Uri() const;

    /**
     * Getter for Namespace.
     *
     * @since S60 ?S60_version
     * @return Namespace
     */
    const TDesC& Namespace() const;
    
    /**
     * Getter for capability requirement check.
     *
     * @since S60 ?S60_version
     * @return Namespace
     */    
    TBool RequiresCapabilityCheck() const;

protected:


private:

    CNcdSubscriptionsSourceIdentifier( TBool aRequireCapabilityCheck );

    void ConstructL( const TDesC& aUri, const TDesC& aNamespace );

private: // data

    /**
     * Uri of the server.
     * Own.
     */
    HBufC* iUri;

    /**
     * Namespace of the server.
     * Own.
     */
    HBufC* iNamespace;
    
    /**
     * This tells whether capability check should be done before
     * sending subscriptions request to the source identified by
     * this source identifier.
     */
    TBool iRequireCapabilityCheck;

    };


#endif // C_NCD_SUBSCRIPTIONS_SOURCE_IDENTIFIER_H