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
* Description:   MIAUpdateRoamingHandlerObserver interface
*
*/



#ifndef IAUPDATE_ROAMINGHANDLER_OBSERVER_H
#define IAUPDATE_ROAMINGHANDLER_OBSERVER_H

/**
 * MIAUpdateRoamingHandlerObserver interface is used to observe
 * when CIAUpdateRoamingHandler::PrepareL() request is completed
 */
class MIAUpdateRoamingHandlerObserver
    {
    
public:

    /**
     * Called when roaming handler is prepared
     */
    virtual void RoamingHandlerPrepared() = 0;
    
    };

#endif // IAUPDATE_ROAMINGHANDLER_OBSERVER_H