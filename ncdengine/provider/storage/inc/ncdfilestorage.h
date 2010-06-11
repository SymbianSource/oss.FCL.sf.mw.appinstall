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
* Description:  
*
*/


#ifndef M_NCDFILESTORAGE_H
#define M_NCDFILESTORAGE_H

class RFs;

/**
 *  File storage interface
 *
 *  ?more_complete_description
 */
class MNcdFileStorage
    {
public:

    /**
     * Adds a file to the file storage
     *
     * @param aFilepath Path to the file
     * @param aOverwrite If true, an already existing file is overwritten. 
     * If false, leaves with KErrAlreadyExists
     * @throw KErrAlreadyExists if the file was already in the storage and
     * overwrite-flag was false
     * @throw KErrBadName if the given filepath was invalid
     * @note Moves the file to the storage dir
     */
    virtual void AddFileL( const TDesC& aFilepath, 
        TBool aOverwrite = EFalse ) = 0;
     
    /**
     * Removes the file from the storage
     *
     * @param aFilename Filename (without any path information)
     * @throw KErrNotFound if the file doesn't exist in the storage
     * @throw KErrBadName if the given filename was empty
     */     
    virtual void RemoveFileL( const TDesC& aFilename ) = 0;


    /**
     * Opens the file from the storage with the given file server session
     *
     * @param aFilename Filename
     * @param aFileMode File mode. See TFileMode in S60 documentation
     * @return A handle to the opened file
     * @throw KErrNotFound if the file doesn't exist in the storage
     * @throw KErrBadName if the filename was empty
     * @throw Error code returned by RFile::Open()
     * @note This should be used if the file is supposed to be shared
     * with the client.
     */     
    virtual RFile OpenFileL( RFs& aFs, const TDesC& aFilename, 
        TUint aFileMode ) = 0;


    /**
     * Storage UID getter
     *
     * @return Storage UID
     */
    virtual const TDesC& Uid() const = 0;

protected:

    virtual ~MNcdFileStorage()
        {
        }
    };


#endif // M_NCDFILESTORAGE_H
