/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of IAUpdateClientDefines
*
*/




#ifndef IA_UPDATE_CLIENT_DEFINES_H
#define IA_UPDATE_CLIENT_DEFINES_H


#include "iaupdateuids.h"


/**
 * General values for the IAD API client purposes.
 *
 * @since S60 v3.2
 */
namespace IAUpdateClientDefines
    {
    
    /**
     * Service is provided by the UI 
     * that acts as an application server.
     */
    const TUid KIAUpdateServiceUid = { KIAUpdateUiUid };


    /**
     * These values define functions that the server
     * provides for the client.
     */
    enum TIAUpdateServerFunctions
        {
        /**
         * Function to check updates
         */
        EIAUpdateServerCheckUpdates,
        
        /**
         * Function to show updates
         */        
        EIAUpdateServerShowUpdates,

        /**
         * Function to update silently
         */
        EIAUpdateServerUpdate,
        
        /**
         * Function to show update query
         */        
        EIAUpdateServerShowUpdateQuery,
        
        /**
         * Function to cancel async request
         */
        EIAUpdateServerCancel,
        
        /**
         * Function to tell to server that the client is brought to foreground
         */
        EIAUpdateServerToForeground,
                        
        /**
         * Function to send window group id of client
         */
        EIAUpdateServerSendWgId,
        
        /**
         * Function to tell that IAD started bu launcher
         */
        EIAUpdateServerStartedByLauncher
        };
    
    }

#endif // IA_UPDATE_CLIENT_DEFINES_H
