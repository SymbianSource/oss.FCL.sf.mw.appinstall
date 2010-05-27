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
* Description:   CNcdActiveOperationObserver
*
*/


#ifndef NCD_ACTIVE_OPERATION_OBSERVER_H
#define NCD_ACTIVE_OPERATION_OBSERVER_H


#include <e32base.h>

class MNcdAsyncOperationObserver;


/**
 * CNcdActiveOperationObserver
 *
 * This is a simple CActive class that observes when another
 * operation has finished. When this observer is informed
 * about the completion of another operation 
 * this class object informs the MNcdAsyncOperationObserver
 * about the completion.
 *
 * @see MNcdAsyncOperationObserver
 */
class CNcdActiveOperationObserver : public CActive
{

public:

    /**
     * @param MNcdAsyncOperationObserver& The information about the completion of
     * an operation is forwarded to this observer by calling its callback
     * function.
     * @return CNcdActiveOperationObserver* Newly created object.
     */
    static CNcdActiveOperationObserver* NewL( MNcdAsyncOperationObserver& aObserver );

    /**
     * @see NewL
     */
    static CNcdActiveOperationObserver* NewLC( MNcdAsyncOperationObserver& aObserver );


    /**
     * Destructor
     */
    virtual ~CNcdActiveOperationObserver();


    /**
     * Sets this object active for observing.
     *
     * @note iStatus has to be set to value KRequestPending
     * by some other object before calling this function.
     */
    void StartToObserve();
    
    
protected: // CActive

    /**
     * @see CActive::DoCancel
     */
    virtual void DoCancel();

    /**
     * @see CActive::RunL
     */
    virtual void RunL();


protected:

    /**
     * @see NewL
     */ 
    CNcdActiveOperationObserver( MNcdAsyncOperationObserver& aObserver );

    /**
     * @see NewL
     */ 
    virtual void ConstructL();
    
    
    /**
     * @return MNcdAsyncOperationObserver& The information 
     * about the completion of an operation is forwarded to 
     * this observer by calling its callback function.
     * Ownership is not transferred.
     */
    MNcdAsyncOperationObserver& AsyncObserver() const;


private:

    // Prevent these if not implemented
    CNcdActiveOperationObserver( const CNcdActiveOperationObserver& aObject );
    CNcdActiveOperationObserver& operator =( const CNcdActiveOperationObserver& aObject );


private: // data

    // The information about the completion of
    // an operation is forwarded to this observer 
    // by calling its callback function.
    MNcdAsyncOperationObserver& iObserver;

};

#endif // NCD_ACTIVE_OPERATION_OBSERVER_H
