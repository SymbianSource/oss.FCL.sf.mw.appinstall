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


#ifndef C_NCDSTORAGECLIENT_H
#define C_NCDSTORAGECLIENT_H

#include <e32base.h>

#include "ncdstorageclient.h"
#include "ncdstorageowner.h"

class CNcdStorage;

/**
 *  Storage client implementation
 *
 *  Each of the provider's clients can have their own storages. 
 *  CNcdStorageClient handles all storages of a client.
 *
 */
class CNcdStorageClient : public CBase, public MNcdStorageClient,
    public MNcdStorageOwner
    {
public:

    /**
     * Creator
     *
     * @param aOwner Owner of this client
     * @param aUid UID of this client
     * @return A new storage client
     */
    static CNcdStorageClient* NewL( MNcdStorageOwner& aOwner, 
        const TDesC& aUid );


    /**
     * Creator
     *
     * @param aOwner Owner of this client
     * @param aUid UID of this client
     * @return A new storage client
     */
    static CNcdStorageClient* NewLC( MNcdStorageOwner& aOwner, 
        const TDesC& aUid );


    /**
     * Destructor
     */
    virtual ~CNcdStorageClient();


public: // From MNcdStorageClient

    /**
     * @see MNcdStorageClient::CreateStorageL()
     */
    MNcdStorage& CreateStorageL( const TDesC& aNamespace );
    
    
    /**
     * @see MNcdStorageClient::StorageL()
     */
    MNcdStorage& StorageL( const TDesC& aNamespace );
    
    
    /**
     * @see MNcdStorageClient::RemoveStorageL()
     */
    void RemoveStorageL( const TDesC& aNamespace );
    
    /**
     * @see MNcdStorageClient::ClientUid()
     */
    const TDesC& ClientUid() const;


    /**
     * @see MNcdStorageClient::NamespacesL()
     */
    MDesCArray* NamespacesLC() const;

public: // From MNcdStorageOwner

    /**
     * @see MNcdStorageOwner::AppendRoot()
     */
    void AppendRoot( TDes& aPath ) const;


    /**
     * @see MNcdStorageOwner::FileSession()
     */
    RFs& FileSession();
    
    
    /**
     * @see MNcdStorageOwner::FileManager()
     */
    CFileMan& FileManager();

private:

    /**
     * Constructor
     * 
     * @param aOwner Owner of this client
     */
    CNcdStorageClient( MNcdStorageOwner& aOwner );


    void ConstructL( const TDesC& aUid );


    /**
     * Searches for a storage that matches the given namespace
     * 
     * @param aNamespace
     * @return Index in iStorages     
     */
    TInt FindStorageByNamespace( const TDesC& aNamespace ) const;
    
    
    /**
     * Creates a storage for each found subdirectory
     *
     * @param aPath 
     */
    //void CreateStoragesFromFileSystemL( const TDesC& aPath );


    /**
     * Reads namespaces and their corresponding directories from 
     * the namespace file and creates storage clients
     */      
    void ReadNamespacesL();
    
    /**
     * Saves namespaces to disk
     */
    void SaveNamespacesL();
    
    /**
     * Checks if the given directory is a subdirectory of the root
     *
     * @param aDir Directory
     * @return ETrue if the directory is found
     */     
    TBool DirectoryExistsL( const TDesC& aDir );

private: // data

    MNcdStorageOwner& iOwner;
    RBuf iUid;
    RPointerArray<CNcdStorage> iStorages;
    };


#endif // C_NCDSTORAGECLIENT
