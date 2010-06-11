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
* Description:   ?Description
*
*/
	

#ifndef M_NCDOPERATIONQUEUE_H
#define M_NCDOPERATIONQUEUE_H

class CNcdBaseOperation;

/**
 *  An interface for handling operations that must not run simultaneously.
 *
 *  This interface is used add an operation to queue and to notify the
 *  queue when a queued operation is complete.
 */
class MNcdOperationQueue                                    
    {
    
public:


    /**
     * Queues the operation.
     *
     * @param aOperation The operation to queue.
     */
    virtual void QueueOperationL( CNcdBaseOperation& aOperation ) = 0;
    
    /**
     * Notifies the queue about completion of the operation, so the next
     * operation in the queue can be started.
     */
    virtual void QueuedOperationComplete( CNcdBaseOperation& aOperation ) = 0;

protected:

    /**
     *
     */
    virtual ~MNcdOperationQueue() {}

	};
	
	
#endif //  M_NCDOPERATIONQUEUE_H
