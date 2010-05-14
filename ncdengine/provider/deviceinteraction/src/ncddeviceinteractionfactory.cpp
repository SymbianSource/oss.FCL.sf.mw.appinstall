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
* Description:   Definition of CNcdDeviceInteractionFactory
*
*/


#include <f32file.h>
#include <eikenv.h>

#include "ncddeviceinteractionfactory.h"
#include "ncdinstallationserviceimpl.h"
#include "ncdinstallationservice.h"
#include "ncddeviceserviceimpl.h"
#include "ncddeviceservice.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Create installation service.
// ---------------------------------------------------------------------------
//
EXPORT_C MNcdInstallationService*
    NcdDeviceInteractionFactory::CreateInstallationServiceL()
    {
    return CNcdInstallationService::NewL();
    }
    
// ---------------------------------------------------------------------------
// Create installation service.
// ---------------------------------------------------------------------------
//
EXPORT_C MNcdInstallationService*
    NcdDeviceInteractionFactory::CreateInstallationServiceLC()
    {
    return CNcdInstallationService::NewLC();
    }
    
// ---------------------------------------------------------------------------
// Create device service.
// ---------------------------------------------------------------------------
//
EXPORT_C MNcdDeviceService*
    NcdDeviceInteractionFactory::CreateDeviceServiceL()
    {
    return CNcdDeviceService::NewL();
    }
    
// ---------------------------------------------------------------------------
// Create device service.
// ---------------------------------------------------------------------------
//
EXPORT_C MNcdDeviceService*
    NcdDeviceInteractionFactory::CreateDeviceServiceLC()
    {
    MNcdDeviceService* deviceService = CNcdDeviceService::NewL();
    CleanupDeletePushL( deviceService );
    return deviceService;
    }
