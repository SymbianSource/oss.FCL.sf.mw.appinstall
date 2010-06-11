/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MIAUpdaterCancelObserver interface
*
*/



#ifndef IAUPDATER_CANCEL_OBSERVER_H
#define IAUPDATER_CANCEL_OBSERVER_H

/**
 * MIAUpdaterCancelObserver interface is used to observe when user cancel issued
 */
class MIAUpdaterCancelObserver
    {
    
public:

    /**
     * Called when global progress note cancelled by an user
     */
    virtual void UserCancel() = 0;
    
    
    /**
     * Called when global progress note closed by end key
     */
    virtual void UserExit() = 0;
    
    
    };

#endif // IAUPDATER_CANCEL_OBSERVER_H