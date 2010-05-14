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
* Description:   Declaration of the COsmDelay class.
*
*/


#ifndef Hb0ac1e7a851b4571bffbc4428f4401f4
#define Hb0ac1e7a851b4571bffbc4428f4401f4

#include <e32base.h>

/**
 * Utility for creating delays and timer callbacks.
 * When a delay or timer callback is active, the thread is not stopped
 * and active scheduler events occur normally.
 */
class COsmDelay : public CTimer
    {
public:

    /**
     * Constructor.
     * 
     * @param aCallBack Function that gets called after the timer goes off.
     * @return COsmDelay* Pointer to the created COsmDelay object.
     */
    static COsmDelay* COsmDelay::NewL( TCallBack aCallBack );

    /**
     * Creates a delay of a specified time using CActiveScheduler
     * Start()-Stop() sequence.
     * @param aTime Time in microseconds to wait.
     */
    static void SynchronousWaitL( TInt aTime );

    /**
     * Small wait of 1/100 sec.
     */
    static void SmallWaitL();

    /**
     * Small wait of 1/100 sec. Non-leavable version.
     * @return Error code
     */
    static TInt SmallWait();

private:

    /**
     * Constructor.
     * 
     * @param aCallBack Function that gets called after timer goes off.
     */
    COsmDelay( TCallBack aCallBack );

    /**
     * @see CActive::RunL()
     */
    void RunL();

private:
    /**
     * Callback function
     */
    TCallBack iCallBack;
    };

#endif // Hb0ac1e7a851b4571bffbc4428f4401f4
