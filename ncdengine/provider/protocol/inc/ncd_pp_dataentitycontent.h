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
* Description:   MNcdPreminetProtocolDataEntity declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DATAENTITYCONTENT_H
#define NCD_PROTOCOL_ELEMENT_DATAENTITYCONTENT_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolEntityDependency;

class MNcdPreminetProtocolDataEntityContent
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolDataEntityContent() {}

    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or KNullDesC if never modified.
     */
    virtual const TDesC& Timestamp() const = 0;

    /**
     * Returns the size of this content.
     * @return Size, 0 if not set
     */
    virtual TInt Size() const = 0;

    /**
     * Entity mime type.
     * @return Mime type or KNullDesC 
     */
    virtual const TDesC& Mime() const = 0;

    /**
     * Returns the ID of this entity.
     * @return Id or KNullDesC
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Retuns the version of this entity.
     * @return Version or KNullDesC
     */
    virtual const TDesC& Version() const = 0;

    /**
     * Retuns the validity time in minutes.
     * @return Validity time.
     */
    virtual TInt ValidUntilDelta() const = 0;

    /**
     * Retuns whether the client may update the content automagically.
     * @return true or false
     */
    virtual TBool ValidUntilAutoUpdate() const = 0;

    /**
     * Returns the amount of content purposes.
     * @return Content purpose count.
     */
    virtual TInt ContentPurposeCount() const = 0;
    
    /**
     * Returns the content purpose object
     * @param aIndex Index ( 0 .. PurchaseOptionCount()-1 )
     * @return Content purpose pointer. Ownership is NOT transferred.
     */
    virtual const TDesC& ContentPurposeL(TInt aIndex) const = 0;

    /**
     * Returns the amount of entity dependencies.
     * @return Entity dependency count.
     */
    virtual TInt EntityDependencyCount() const = 0;
    
    /**
     * Returns the purchase option.
     * @param aIndex Index ( 0 .. PurchaseOptionCount()-1 )
     * @return Purchase option pointer. Ownership is NOT transferred.
     */
    virtual const MNcdPreminetProtocolEntityDependency& 
        EntityDependencyL(TInt aIndex) const = 0;

    /**
     * Returns the subscription type of content.
     * @return Subscription type, ENotSubscribable if not subscribable.
     */
    virtual TNcdSubscriptionType SubscriptionType() const = 0;
    
    /**
     * Child accessibility. (Subscriptions only)
     * Returns true if child is viewable.
     * @return true or false
     */
    virtual TBool ChildViewable() const = 0;
    
    /**
     * Child accessibility. (Subscriptions only)
     * Returns true if child is separately purchasable.
     * @return true or false
     */
    virtual TBool ChildSeparatelyPurchasable() const = 0;

    };


#endif //NCD_PROTOCOL_ELEMENT_DATAENTITYCONTENT_H
