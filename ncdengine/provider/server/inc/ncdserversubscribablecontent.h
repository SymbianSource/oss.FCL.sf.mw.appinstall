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
* Description:   Contains CNcdServerSubscribableContent class
*
*/


#ifndef NCD_SERVER_SUBSCRIBABLE_CONTENT_H
#define NCD_SERVER_SUBSCRIBABLE_CONTENT_H


#include <e32base.h>
// For streams
#include <s32mem.h>

#include "ncdsubscription.h"

class MNcdPreminetProtocolDataEntityContent;

/**
 *  This server side class contains the subscribableContent info
 *  of a node.
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdServerSubscribableContent : public CBase
    {

public:
    /**
     * NewL
     *
     * @return CNcdServerSubscribableContent* Pointer to the created object 
     * of this class.
     */
    static CNcdServerSubscribableContent* NewL();

    /**
     * NewLC
     *
     * @return CNcdServerSubscribableContent* Pointer to the created object 
     * of this class.
     */
    static CNcdServerSubscribableContent* NewLC();

    /**
     * Destructor
     *
     */
    virtual ~CNcdServerSubscribableContent();



    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the
     * given protocol element.
     *
     * 
     * @param aData The data is set in the protocol parser and can
     *              be used to initialize this class object.
     */
    void InternalizeL( const MNcdPreminetProtocolDataEntityContent& aData );


    
public:


    /**
     * Externalizer
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * Internalizer
     */
    virtual void InternalizeL( RReadStream& aStream );


    MNcdSubscription::TType SubscriptionType() const;


protected:

    /**
     * Constructor
     * Is set in the NewLC function 
     */
    CNcdServerSubscribableContent();

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

private:

    // Prevent these two if they are not implemented
    CNcdServerSubscribableContent(
        const CNcdServerSubscribableContent& aObject );
    CNcdServerSubscribableContent& operator=(
        const CNcdServerSubscribableContent& aObject );

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
    
#endif // NCD_SERVER_SUBSCRIBABLE_CONTENT_H
