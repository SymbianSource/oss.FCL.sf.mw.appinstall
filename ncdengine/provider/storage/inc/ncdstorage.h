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


#ifndef M_NCDSTORAGE_H
#define M_NCDSTORAGE_H


class MNcdDatabaseStorage;
class MNcdFileStorage;
class RFs;

/**
 *  Storage interface
 *
 *  A storage can contain several databases and file storages (directories)
 *
 *
 */
class MNcdStorage
    {
public:

    /**
     * Database storage getter
     *
     * Creates the database if it doesn't already exist.
     *
     * @param aUid UID of the database
     * @return Reference to the database     
     * @throw KErrArgument if the name was empty
     */     
    virtual MNcdDatabaseStorage& DatabaseStorageL( const TDesC& aUid ) = 0;


    /**
     * File storage getter
     *
     * Creates the file storage if it doesn't already exist.
     *
     * @param aUid UID of the file storage
     * @return Reference to the file storage
     * @throw KErrArgument if the name was empty
     */     
    virtual MNcdFileStorage& FileStorageL( const TDesC& aUid ) = 0;


    /**
     * Storage namespace getter
     *
     * @return Storage namespace
     */
    virtual const TDesC& Namespace() const = 0;


    /**
     * Storage subdirectory getter
     *
     * @return Storage directory
     * @note Directory is the namespace that is encoded to ensure
     * compatibility with the filesystem
     */
    virtual const TDesC& Directory() const = 0;
    
    
    /**
     * File session getter
     *
     * @return File session
     */
    virtual RFs& FileSession() = 0;    
    
    
     /**
      * Storage size getter
      *
      * Returns the combined size of all databases and filestorages in
      * this namespace.
      *
      * @return Storage size in bytes
      */
     virtual TInt SizeL() = 0;
    
    
protected:

    virtual ~MNcdStorage()
        {
        }
    };


#endif // M_NCDSTORAGE_H
