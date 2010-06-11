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


#ifndef C_NCDSTORAGEMANAGER_H
#define C_NCDSTORAGEMANAGER_H

#include <e32base.h>
#include <f32file.h>

#include "ncdstoragemanager.h"
#include "ncdstorageowner.h"

class MNcdStorage;
class CNcdStorage;
class CNcdStorageClient;


/**
 *  Manager for all provider's storages
 *
 *  @note UIDs and namespace can, in fact, contain any text that can
 *  be used in a directory/filename. 
 *
 */
class CNcdStorageManager : public CBase, public MNcdStorageManager,
    public MNcdStorageOwner
    {
public: 

    /**
     * Storage manager creator. 
     *
     * Also creates the provider storage
     *
     * @param 
     * @param aRootDirectory Root directory where all of the provider's 
     * storages will be created to. The directory name must end with a \    
     * @return A new storage manager
     * @throw KErrArgument if either of the parameters were empty or
     * the path was invalid
     */
    static CNcdStorageManager* NewL( 
        RFs& aFs, 
        const TDesC& aRootDirectory );


    virtual ~CNcdStorageManager();


public: // From MNcdStorageManager

    /**
     * @see MNcdStorageManager::CreateOrGetStorageL()
     */
    MNcdStorage& CreateOrGetStorageL( const TDesC& aClientUid, 
        const TDesC& aNamespace );


    /**
     * @see MNcdStorageManager::CreateStorageL()
     */
    MNcdStorage& CreateStorageL( const TDesC& aClientUid, const TDesC& aNamespace );


    /**
     * @see MNcdStorageManager::ProviderStorage
     */
    MNcdStorage& ProviderStorageL( const TDesC& aClientUid );
    
    
    /**
     * @see MNcdStorageManager::StorageL()
     */
    MNcdStorage& StorageL( const TDesC& aClientUid, const TDesC& aNamespace );
    
    /**
     * @see MNcdStorageManager::StorageNamespacesLC()
     */
    MDesCArray* StorageNamespacesLC( const TDesC& aClientUid ) const;
    
    /**
     * @see MNcdStorageManager::StorageClientL()
     */
    MNcdStorageClient& StorageClientL( const TDesC& aClientUid );
    
    /**
     * @see MNcdStorageManager::RemoveClientL()
     */
    //void RemoveClientL( const TDesC& aClientUid );    
    
    
    /**
     * @see MNcdStorageManager::RemoveStorageL()
     */
    void RemoveStorageL( const TDesC& aClientUid, const TDesC& aNamespace );
    
    
    void CloseClient( const TDesC& aClientUid );
    
    
public: // From MNcdStorageOwner

    /**
     * @see MNcdStorageOwner::FileSession()
     */
    RFs& FileSession();
           
    /**
     * @see MNcdStorageOwner::AppendRoot()
     */
    void AppendRoot( TDes& aPath ) const;
    
    /**
     * @see MNcdStorageOwner::FileManager()
     */
    CFileMan& FileManager();
    
public: // New methods

    /**
     * Utility function for removing a directory
     */
    static void RemoveDirectoryL( RFs& aFileSession, const TDesC& aDirectory );
        
    
private: // Private methods
 

    CNcdStorageManager( RFs& aFs );

    void ConstructL( const TDesC& aRootDirectory );

    
    /**
    * Searches for the client by UID
    * 
    * @param aUid Client's UID
    * @return Index in iClients
    * @throw KErrNotFound if the client was not found
    */
    TInt FindClientByUidL( const TDesC& aUid ) const;

    TInt FindClientByUid( const TDesC& aUid ) const;
    
private: // data

    RBuf iRootDir;
    
    RPointerArray<CNcdStorageClient> iClients;    
    RFs& iFs;
    
    CFileMan* iFileMan;
    };


#endif // C_NCDSTORAGEMANAGER_H
