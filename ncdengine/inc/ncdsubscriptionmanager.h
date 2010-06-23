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
* Description:   Definition of MNcdSubscriptionManager interface
*
*/


#ifndef M_NCDSUBSCRIPTIONMANAGER_H
#define M_NCDSUBSCRIPTIONMANAGER_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"
#include "catalogsarray.h"

class MNcdSubscription;
class MNcdSubscriptionOperation;
class MNcdSubscriptionOperationObserver;

/**
 *  Subscription manager interface.
 *
 *  This interface describes a subscription manager. Subscription manager
 *  provides methods for subscription management.
 *
 *  
 */
class MNcdSubscriptionManager : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdSubscriptionManagerUid };


    /**
     * Get all subscriptions.
     *
     * @note This function should be called to get current and valid list of
     *       subscriptions after changes to subscriptions.
     *
     * 
     * @return Array of made subscriptions. Counted, Release() must be called
     *  for each object after use, e.g. with a call to
     *  RCatalogsArray::ResetAndDestroy()
     */
    virtual RCatalogsArray< MNcdSubscription > SubscriptionsL() const = 0;

    /**
     * Refresh subscriptions from service providers. 
     *
     * 
     * @param aObserver Operation observer interface.
     * @return Subscription operation object. Counted, Release() must be called after use.
     * @exception Leave System wide error code.
     */
    virtual MNcdSubscriptionOperation* RefreshSubscriptionsL( 
        MNcdSubscriptionOperationObserver& aObserver ) = 0;

protected: // Destruction

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdSubscriptionManager() {}

    };

#endif // M_NCDSUBSCRIPTIONMANAGER_H
