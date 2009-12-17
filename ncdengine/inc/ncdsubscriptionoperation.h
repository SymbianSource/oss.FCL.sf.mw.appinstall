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
* Description:   Definition of MNcdSubscriptionOperation interface
*
*/
	

#ifndef M_NCDSUBSCRIPTIONOPERATION_H
#define M_NCDSUBSCRIPTIONOPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"

class MNcdSubscriptionOperationObserver;

/**
 *  Subscription operation interface
 */
class MNcdSubscriptionOperation : public MNcdOperation
    {

public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdSubscriptionOperationUid };

    /**
     * Subscription operation type.
     *
     * 
     */
    enum TType
        {
                
        /** Stop an active subscription */
        EUnsubscribe,
        
        /** Refresh subscriptions from service providers */
        ERefreshSubscriptions

        };


    /**
     * Return the operation type.
     *
     * 
     * @return Type of the operation.
     */
    virtual TType SubscriptionOperationType() const = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdSubscriptionOperation() {}

    };
	
	
#endif // M_NCDSUBSCRIPTIONOPERATION_H
