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
* Description:   IAUpdateFileConsts
*
*/



#ifndef IA_UPDATE_FILE_CONSTS_H
#define IA_UPDATE_FILE_CONSTS_H

/**
 * IAUpdateFileConsts
 * Gives filename constants that are used 
 * when files are saved and read.
 */
namespace IAUpdateFileConsts
    {

    /**
     * This defines the drive where the files will be saved.
     */
    const TInt KDrive( EDriveC );

    /**
     * This file contains parameter data that is given
     * through the API. This file is used when self updater
     * is started and the parameter values need to be saved
     * for possible IAD restart.
     */
    _LIT( KParamsFile, "iaupdateparamsfile" );    

    /**
     * This file contains data that is used to check if
     * network access is allowed for refresh operation.
     */
    _LIT( KControllerFile, "iaupdatecontrollerfile" );

    }
    
#endif // IA_UPDATE_FILE_CONSTS_H
