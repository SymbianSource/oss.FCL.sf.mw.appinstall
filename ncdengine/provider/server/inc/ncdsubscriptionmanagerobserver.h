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
	

#ifndef M_NCD_SUBSCRIPTIONMANAGER_OBSERVER_H
#define M_NCD_SUBSCRIPTIONMANAGER_OBSERVER_H


/**
 *  Server side observer interface for operations.
 *  
 * This interface offers subscription manager a way to inform
 * operations that requested tasks are complete.
 *
 *  @since S60 v3.2
 */
class MNcdSubscriptionManagerObserver
    {
    
public:

    /**
     * Called when internalization of given subscription or subscriptions
     * into subscription manager is complete.
     *
     * @param aOperation aError KErrNone if completed correctly, otherwise
     *        system-wide error code.
     */
    virtual void SubscriptionsInternalizeComplete( TInt aError ) = 0;

    };
    
#endif //  M_NCD_SUBSCRIPTIONMANAGER_OBSERVER_H
