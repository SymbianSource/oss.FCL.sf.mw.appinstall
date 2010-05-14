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
* Description:   Contains CNcdClientSubscribableContent class
*
*/


#ifndef NCD_CLIENT_SUBSCRIBABLE_CONTENT_H
#define NCD_CLIENT_SUBSCRIBABLE_CONTENT_H


#include <e32base.h>
// For streams
#include <s32mem.h>

#include "ncdsubscription.h"

/**
 *  This server side class contains the subscribableContent info
 *  of a node.
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdClientSubscribableContent : public CBase
    {

public:
    /**
     * NewL
     *
     * @return CNcdClientSubscribableContent* Pointer to the created object 
     * of this class.
     */
    static CNcdClientSubscribableContent* NewL();

    /**
     * NewLC
     *
     * @return CNcdClientSubscribableContent* Pointer to the created object 
     * of this class.
     */
    static CNcdClientSubscribableContent* NewLC();

    /**
     * Destructor
     *
     */
    virtual ~CNcdClientSubscribableContent();


   
public:


    /**
     * Internalizer
     */
    virtual void InternalizeL( RReadStream& aStream );

    TTime ValidUntil() const;
    TBool ValidUntilSet() const;
    TBool ChildSeparatelyPurchasable() const;

protected:

    /**
     * Constructor
     * Is set in the NewLC function 
     */
    CNcdClientSubscribableContent();

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

private:

    // Prevent these two if they are not implemented
    CNcdClientSubscribableContent(
        const CNcdClientSubscribableContent& aObject );
    CNcdClientSubscribableContent& operator=(
        const CNcdClientSubscribableContent& aObject );

    /**
     * Function to reset member variables.
     */
    void ResetMemberVariables();

private: // data

    TTime iValidUntil;
    TBool iValidUntilSet;
    MNcdSubscription::TType iSubscriptionType;
    TBool iChildSeparatelyPurchasable;

    };
    
#endif // NCD_CLIENT_SUBSCRIBABLE_CONTENT_H
