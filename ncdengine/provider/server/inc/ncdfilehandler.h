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
* Description:   File handler interface
*
*/


#ifndef M_NCDFILEHANDLER_H
#define M_NCDFILEHANDLER_H

/**
 * Helper interface for simplifying file handling in
 * operations
 */
class MNcdFileHandler
    {
public:

    /**
     * Moves the source file to the destination.
     * The interpretation of the destination parameters depends on
     * the implementing class. Classes that use storages (both database
     * and file), should interpret them as the destination namespace and id. 
     *
     * Classes that use file system normally, should interpret the parameters
     * as the destination directory and name.
     * 
     * @param aSourceFile Path and name of the source file
     * @param aNamespaceOrDir Destination namespace or directory
     * @param aIdOrName Destination ID or name
     * @param aOverwrite If true, existing target file is overwritten, otherwise
     * should leave with KErrAlreadyExists
     */
    virtual void MoveFileL( const TDesC& aSourceFile,
        const TDesC& aNamespaceOrDir, const TDesC& aIdOrName,
        TBool aOverwrite = ETrue ) = 0;

    /**
     * The implementations of this class CAN be deleted through this
     * interface
     */
    virtual ~MNcdFileHandler()
        {
        }
    };

#endif // M_NCDFILEHANDLER_H