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
* Description:   Declaration of CNcdDelay class.
*
*/


#ifndef C_NCDDELAY_H
#define C_NCDDELAY_H

#include <e32base.h>

/**
 * Utility for creating delays and timer callbacks.
 * When a delay or timer callback is active, the thread is not stopped
 * and active scheduler events occur normally.
 */
class CNcdDelay : public CTimer
    {
public:

    /**
     * Constructor.
     * 
     * @param aCallBack Function that gets called after the timer goes off.
     * @return COsmDelay* Pointer to the created COsmDelay object.
     */
    static CNcdDelay* CNcdDelay::NewL( TCallBack aCallBack );

private:

    /**
     * Constructor.
     * 
     * @param aCallBack Function that gets called after timer goes off.
     */
    CNcdDelay( TCallBack aCallBack );

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

#endif
