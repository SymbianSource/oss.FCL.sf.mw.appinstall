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



#ifndef IA_UPDATE_UI_CONFIG_CONSTS_H
#define IA_UPDATE_UI_CONFIG_CONSTS_H

/**
 * IAUpdateUiConfigConsts
 * Gives constants that are used when engine is configured.
 * 
 * @since S60 v3.2
 */
namespace IAUpdateUiConfigConsts
    {
    // File where the configuration is read from.
    _LIT( KConfigFile, "iaupdateuiconfig.xml");

    // XML config file elements and attributes
    _LIT8( KIAElementLocalName, "iaupdateUiConfig" );
    _LIT8( KIAGridRefresh, "gridRefresh" );
    _LIT8( KIADeltaHours, "deltaHours" );
    _LIT8( KIAQueryHistory, "queryHistory" );
    _LIT8( KIADelayHours, "delayHours" );    
    }

#endif // IA_UPDATE_UI_CONFIG_CONSTS_H
