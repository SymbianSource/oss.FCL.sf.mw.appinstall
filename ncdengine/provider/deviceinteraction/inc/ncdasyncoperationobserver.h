/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MNcdAsyncOperationObserver
*
*/


#ifndef NCD_ASYNC_OPERATION_OBSERVER_H
#define NCD_ASYNC_OPERATION_OBSERVER_H

/**
 * MNcdAsyncOperationObserver
 *
 * Provides callback function that may be called when an
 * asynchronous operation has been completed.
 *
 * @see CNcdActiveOperationObserver
 */
class MNcdAsyncOperationObserver
{

public:

    /**
     * This callback function may be called when an asynchronous operation
     * has been completed.
     *
     * @param aError Gives the error code of the completed operation.
     *
     * @since S60 v3.2
     */
    virtual void AsyncOperationComplete( TInt aError ) = 0; 


protected:

    /**
     * The destructor of an interface is set virtual to make sure that
     * the destructors of derived classes are called appropriately when the
     * object is destroyed.
     *
     * Destructor is defined as protected to prevent direct use of delete on 
     * observer interface pointer.
     *
     * @since S60 v3.2
     */
    virtual ~MNcdAsyncOperationObserver() { }

};

#endif // NCD_ASYNC_OPERATION_OBSERVER_H
