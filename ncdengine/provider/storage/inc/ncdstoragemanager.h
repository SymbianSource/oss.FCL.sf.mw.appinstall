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


#ifndef M_NCDSTORAGEMANAGER_H
#define M_NCDSTORAGEMANAGER_H

#include <e32base.h>

class MNcdStorage;
class CNcdStorage;
class MNcdStorageClient;
class RFs;
class MDesCArray;

/**
 *  Interface for the storage manager
 *
 *  @note UIDs and namespace can, in fact, contain any text that can
 *  be used in a directory/filename. 
 *
 */
class MNcdStorageManager 
    {
public: 
    virtual ~MNcdStorageManager() {}


    /**
     * Creates storage if necessary, otherwise returns an existing one
     *
     * @param aClientUid  
     * @param aNamespace
     * @return Storage    
     */
    virtual MNcdStorage& CreateOrGetStorageL( const TDesC& aClientUid,
        const TDesC& aNamespace ) = 0;
        

    /**
     * Client storage creator
     *
     * @param aClientUid Client's UID
     * @param aNamespace Namespace for the storage
     * @return A reference to the created storage
     */
    virtual MNcdStorage& CreateStorageL( const TDesC& aClientUid, 
        const TDesC& aNamespace ) = 0;


    /**
     * Provider storage getter
     *
     * @return Provider's storage
     */
    virtual MNcdStorage& ProviderStorageL( const TDesC& aClientUid ) = 0;
    
    
    /**
     * Client storage getter
     * 
     * @param aClientUid Client's UID
     * @param aNamespace Storage namespace
     * @return A Reference to the found storage
     * @throw KErrNotFound if the storage was not found
     * @throw KErrArgument if either client UID or storage 
     * namespace were empty
     */
    virtual MNcdStorage& StorageL( const TDesC& aClientUid, 
        const TDesC& aNamespace ) = 0;
    
    
    /**
     * Lists storage namespaces
     *
     * @param aClientUid Client's UID
     * @return An array of client's namespaces. Ownership is transferred.
     */
    virtual MDesCArray* StorageNamespacesLC( 
        const TDesC& aClientUid ) const = 0;

    /**
     * Storage client getter
     * 
     * @param aClientUid Client UID
     * @return Storage client
     */
     virtual MNcdStorageClient& StorageClientL( const TDesC& aClientUid ) = 0;
    
    
    /**
     * Client remover.
     *
     * Removes the client and all its storages.
     * @note Storages are actually deleted from disk.
     * @note None of the files under the storage can be in use or this will
     * leave with KErrInUse and leave some of the file intact.
     *
     * @param aClientUid Client UID
     */
    //virtual void RemoveClientL( const TDesC& aClientUid ) = 0;
    
    
    /**
     * Client storage remover
     * 
     * @param aClientUid Client's UID
     * @param aNamespace Storage namespace
     * @throw KErrNotFound if the storage was not found
     * @throw KErrArgument if either of the parameters was invalid
     */
    virtual void RemoveStorageL( const TDesC& aClientUid, 
        const TDesC& aNamespace ) = 0;

    
    /**
     * Closes client's resources
     */
    virtual void CloseClient( const TDesC& aClientUid ) = 0;
   
    /**
     * File session getter
     *
     * @return File session used by storages
     */
    virtual RFs& FileSession() = 0;
};



#endif // M_NCDSTORAGEMANAGER_H
