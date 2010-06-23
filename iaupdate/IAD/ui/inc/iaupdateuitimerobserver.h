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
* Description:   This file contains the header file of the MIAUpdateUITimerObserver class.
*
*/

#ifndef IA_UPDATE_UI_TIMER_OBSERVER_H
#define IA_UPDATE_UI_TIMER_OBSERVER_H


/**
 * MIAUpdateUITimerObserver
 */
class MIAUpdateUITimerObserver
    {

public:

    /**
     * Called when the timer has completed its operation.
     * @param aError Error code of the operation.
     */
    virtual void ForegroundDelayComplete( TInt aError ) = 0;
    
    /**
    * Called when the timer has completed its operation.
    * @param aError Error code of the operation.
    */
    virtual void BackgroundDelayComplete( TInt aError ) = 0;
    
    /**
    * Called when the timer has completed its operation.
    * @param aError Error code of the operation.
    */
    virtual void ProcessStartDelayComplete( TInt aError ) = 0;
   

protected:

    /**
     * Protected virtual destructor to prevent unwanted
     * deletions of this interface object. 
     */
    virtual ~MIAUpdateUITimerObserver() { }
    
    };

#endif // IA_UPDATE_UI_TIMER_OBSERVER_H
