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
* Description:   Declaration of CNcdDeviceInteractionFactory
*
*/


#ifndef NCD_DEVICE_INTERACTION_FACTORY_H
#define NCD_DEVICE_INTERACTION_FACTORY_H

class MNcdInstallationService;
class MNcdDeviceService;

/**
 * Device Interaction factory class.
 */
class NcdDeviceInteractionFactory
    {
public: // New functions

    /**
     * Create installation service.
     * @return MNcdInstallationService* Installation service.
     */
    IMPORT_C static MNcdInstallationService* CreateInstallationServiceL();
    
    /**
     * Create installation service.
     * @return MNcdInstallationService* Installation service.
     */
    IMPORT_C static MNcdInstallationService* CreateInstallationServiceLC();
    
    /**
     * Create device service.
     * @return MNcdDeviceService* Device service.
     */
    IMPORT_C static MNcdDeviceService* CreateDeviceServiceL();
    
    /**
     * Create device service.
     * @return MNcdDeviceService* Device service.
     */
    IMPORT_C static MNcdDeviceService* CreateDeviceServiceLC();
    
    };

#endif // NCD_DEVICE_INTERACTION_FACTORY_H
