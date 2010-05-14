/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateUITimer class.
*
*/

#ifndef IA_UPDATEUI_TIMER_H
#define IA_UPDATEUI_TIMER_H


#include "e32base.h"

class MIAUpdateUITimerObserver;

/**
 * CIAUpdateUITimer
 */
class CIAUpdateUITimer : public CTimer
    {

public:
     
    enum TTimerType
        {
        EBackgroundDelay,       
        EForegroundDelay ,
        EProcessStartDelay
        };
    
public:   
    /**
     * Destructor
     */
    virtual ~CIAUpdateUITimer();


    /**
     * @param aObserver Observer that is informed about timer
     * completion.
     * @return CIAUpdateUITimer* Created object.
     */
    static CIAUpdateUITimer* NewL( 
        MIAUpdateUITimerObserver& aObserver,
        TTimerType aTimerType );

    /**
     * @param aObserver Observer that is informed about timer
     * completion.
     * @return CIAUpdateUITimer* Created object.
     */
    static CIAUpdateUITimer* NewLC( 
        MIAUpdateUITimerObserver& aObserver,
        TTimerType aTimerType );


protected: // CTimer

    /**
     * @see CTimer::RunL
     */    
    virtual void RunL();


private:

    // Prevent these if not implemented
    CIAUpdateUITimer( const CIAUpdateUITimer& aObject );
    CIAUpdateUITimer& operator =( const CIAUpdateUITimer& aObject );

    // Constructor functions
    CIAUpdateUITimer( MIAUpdateUITimerObserver& aObserver,
                      TTimerType aTimerType );


private: // data
    
    // Observer that is informed when timer completes.
    MIAUpdateUITimerObserver& iObserver;
    TTimerType iTimerType;
    };

#endif // IA_UPDATE_TIMER_H

