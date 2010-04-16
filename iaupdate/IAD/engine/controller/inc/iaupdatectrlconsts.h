/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IA_UPDATE_CTRL_CONSTS_H
#define IA_UPDATE_CTRL_CONSTS_H

/**
 * IAUpdateCtrlConsts
 * Gives constants for the controller.
 * 
 * @note Some of these values need to be updated when 
 * new versions of the controller are published.
 *
 * @since S60 v3.2
 */
namespace IAUpdateCtrlConsts
    {
    
    /**
     * Software type is sent to the server side to inform 
     * that a software is using the iaupdate engine.
     * 
     * @note The type should always remain same 
     *       because we are always using the same engine.    
     *
     * @since S60 v3.2
     */
    _LIT( KSoftwareType, "s60-iaupdate" );

    /**
     * Version gives the version of the iaupdate engine.
     * Version informs which version of the engine is used.
     * Thus, the server can check what kind of responses
     * the client can understand.
     *
     * @note This should be updated when new releases of
     *       the controller are published. So, the server
     *       knows what kind of responses it can send.
     *
     * @since S60 v3.2
     */
    _LIT( KSoftwareVersion, "2.1.0" );
    
    /**
     * Product code value that is used when emulator is used.
     *
     * @since S60 v3.2
     */
    _LIT( KWinsProductCode, "00000000" );

    /**
     * Default storage max size in KBs. This is used if the value 
     * is not given in the configuration file. Automatic cache 
     * cleaning will be started if cache size exceeds this value. 
     * Notice, automatic cleaning can be turned off. Then, this
     * value does not have any effect.
     */
    _LIT( KDefaultStorageMaxSize, "1024" );

    /**
     * This defines the time interval after which the cache is
     * thought as expired and it can be cleared.
     */
    const TInt KCacheClearIntervalDays( 30 );
    
    }
    
#endif // IA_UPDATE_CTRL_CONSTS_H
