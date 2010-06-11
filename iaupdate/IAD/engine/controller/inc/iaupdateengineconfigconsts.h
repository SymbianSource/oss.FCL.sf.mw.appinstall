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



#ifndef IA_UPDATE_ENGINE_CONFIG_CONSTS_H
#define IA_UPDATE_ENGINE_CONFIG_CONSTS_H

/**
 * IAUpdateEngineConfigConsts
 * Gives constants that are used when engine is configured.
 * 
 * @since S60 v3.2
 */
namespace IAUpdateEngineConfigConsts
    {
    // File where the configuration is read from.
    _LIT( KConfigFile, "iaupdateengineconfig.xml" );

    // XML config file elements and attributes
    _LIT8( KIAElementLocalName, "iaupdateEngineConfig" );
    _LIT8( KIAStorage, "storage" );
    _LIT8( KIAMaxSize, "maxSize" );
    _LIT8( KIAMasterServer, "masterServer" );
    _LIT8( KIAUri, "uri" );
    _LIT8( KIAProvisioning, "provisioning" );
    _LIT8( KIAValue, "value" );
    _LIT8( KIAClientRole, "clientRole" );        
    }

#endif // IA_UPDATE_ENGINE_CONFIG_CONSTS_H
