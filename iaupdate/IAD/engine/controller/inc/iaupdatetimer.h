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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_TIMER_H
#define IA_UPDATE_TIMER_H


#include "e32base.h"

class MIAUpdateTimerObserver;


/**
 * CIAUpdateTimer
 */
class CIAUpdateTimer : public CTimer
    {

public:

    /**
     * Destructor
     */
    virtual ~CIAUpdateTimer();


    /**
     * @param aObserver Observer that is informed about timer
     * completion.
     * @return CIAUpdateTimer* Created object.
     */
    static CIAUpdateTimer* NewL( 
        MIAUpdateTimerObserver& aObserver );

    /**
     * @param aObserver Observer that is informed about timer
     * completion.
     * @return CIAUpdateTimer* Created object.
     */
    static CIAUpdateTimer* NewLC( 
        MIAUpdateTimerObserver& aObserver );


protected: // CTimer

    /**
     * @see CTimer::RunL
     */    
    virtual void RunL();


private:

    // Prevent these if not implemented
    CIAUpdateTimer( const CIAUpdateTimer& aObject );
    CIAUpdateTimer& operator =( const CIAUpdateTimer& aObject );

    // Constructor functions
    CIAUpdateTimer( MIAUpdateTimerObserver& aObserver );


private: // data
    
    // Observer that is informed when timer completes.
    MIAUpdateTimerObserver& iObserver;
    
    };

#endif // IA_UPDATE_TIMER_H

