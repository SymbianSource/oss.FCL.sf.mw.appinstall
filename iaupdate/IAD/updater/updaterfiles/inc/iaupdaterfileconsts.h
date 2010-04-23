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
* Description:   IAUpdaterFileConsts
*
*/



#ifndef IA_UPDATER_FILE_CONSTS_H
#define IA_UPDATER_FILE_CONSTS_H

/**
 * IAUpdaterFileConsts
 * Gives filename constants that are used 
 * when files are saved and read.
 *
 * @since S60 v3.2
 */
namespace IAUpdaterFileConsts
    {

    /**
     * This defines the drive where the files will be saved.
     *
     * @since S60 v3.2
     */
    const TInt KDrive( EDriveC );

    /**
     * This file will contain the list of the files that the
     * updater should install. Also, other install specific
     * data may be included in the file stream.
     *
     * @since S60 v3.2
     */
    _LIT( KFileListFile, "iaupdaterfilelist" );    

    /**
     * This file will contain the results of the updater install
     * operations. The data from the file is used when iaupdate engine
     * updates the purchase history after updater has started the
     * iaupdate.
     *
     * @since S60 v3.2
     */
    _LIT( KResultsFile, "iaupdaterresults" );  

    }
    
#endif // IA_UPDATER_FILE_CONSTS_H
