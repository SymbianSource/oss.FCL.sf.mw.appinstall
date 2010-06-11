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
* Description:   MIAUpdateNetworkRegistrationObserver interface
*
*/



#ifndef IAUPDATE_NETWORKREGISTRATION_OBSERVER_H
#define IAUPDATE_NETWORKREGISTRATION_OBSERVER_H

/**
 * MIAUpdateNetworkRegistrationObserver interface is used to observe
 * when network registration status is monitored
 */
class MIAUpdateNetworkRegistrationObserver
    {
    
public:

    /**
     * Called when an outstanding request is completed
     *
     * @param aRoaming True value when roaming network 
     */
    virtual void StatusMonitored( TBool aRoaming ) = 0;
    
    };

#endif // IAUPDATE_NETWORKREGISTRATION_OBSERVER_H