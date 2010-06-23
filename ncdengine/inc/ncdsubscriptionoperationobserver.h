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
* Description:   Definition of MNcdSubscriptionOperationObserver interface
*
*/


#ifndef M_NCDSUBSCRIPTIONOPERATIONOBSERVER_H
#define M_NCDSUBSCRIPTIONOPERATIONOBSERVER_H

class MNcdSubscriptionOperation;
class TNcdProgress;
class MNcdQuery;

/**
 *  Observer interface for subscription operations.
 *
 *  Subscription operation users should implement this interface to be able to
 *  receive callbacks.
 */
class MNcdSubscriptionOperationObserver
    {
    
public:

    /**
     * Called when a subscription operation has progressed.
     *
     * @param aOperation The operation that sent the event.
     * @param aProgress Progress of the operation.
     */
    virtual void SubscriptionProgress( MNcdSubscriptionOperation& aOperation,
        TNcdProgress aProgress ) = 0;

    /**
     * Called when a query has been received for an operation.
     *
     * @param aOperation The operation that sent the event.
     * @param aQuery Pointer to the query object. Counted, Release() must be
     *  called after use.
     */
    virtual void QueryReceived( MNcdSubscriptionOperation& aOperation,
        MNcdQuery* aQuery ) = 0;

    /**
     * Called when an operation has been completed.
     *
     * @param aOperation The operation that sent the event.
     * @param aError Error code for operation completion. KErrNone for
     * successful completion, otherwise a system wide error code.
     */
    virtual void OperationComplete( MNcdSubscriptionOperation& aOperation,
        TInt aError ) = 0;
    };
    
#endif // M_NCDSUBSCRIPTIONOPERATIONOBSERVER_H
