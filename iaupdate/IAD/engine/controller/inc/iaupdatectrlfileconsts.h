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
* Description:   IAUpdateCtrlFileConsts
*
*/



#ifndef IA_UPDATE_CTRL_FILE_CONSTS_H
#define IA_UPDATE_CTRL_FILE_CONSTS_H

/**
 * IAUpdateCtrlFileConsts
 * Gives filename constants that are used 
 * when files are saved and read.
 *
 * @since S60 v3.2
 */
namespace IAUpdateCtrlFileConsts
    {

    /**
     * This defines the drive where the files will be saved.
     *
     * @since S60 v3.2
     */
    const TInt KDrive( EDriveC );

    /**
     * This file will contain information about the nodes that 
     * are waiting to be updated after self update is finished.
     *
     * @since S60 v3.2
     */
    _LIT( KPendingNodesFile, "iaupdatependingnodes" );

    /**
     * This file will contain information about the device language used
     * during last refresh and the time of the last cache clear.
     */
    _LIT( KCacheClearFile, "iaupdatecacheclear" );

    }
    
#endif // IA_UPDATE_CTRL_FILE_CONSTS_H
