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


#ifndef M_NCDSTORAGECLIENT_H
#define M_NCDSTORAGECLIENT_H


class MNcdStorage;
class RFs;
class MDesCArray;

/**
 *  Interface for storage clients
 *
 *
 */
class MNcdStorageClient
    {
public:

    /**
     * Creates a new storage for the client
     *
     * @param aNamespace Storage namespace. Must be unique in the client 
     * and also usable in a directory path.
     * @return A new storage
     * @throw KErrArgument if the namespace couldn't be used in a path or 
     * it was empty
     */
    virtual MNcdStorage& CreateStorageL( const TDesC& aNamespace ) = 0;
    
    
    /**
     * Client storage getter
     *
     * @param aNamespace Storage namespace
     * @return A reference to the storage
     * @throw KErrNotFound if the storage was not found
     * @throw KErrArgument if the namespace was empty
     */
    virtual MNcdStorage& StorageL( const TDesC& aNamespace ) = 0;
    
    
    /**
     * Client storage remover
     *
     * The storage is entirely deleted from disk.
     *
     * @param aNamespace Storage namespace
     * @throw KErrArgument if the namespace was empty    
     */    
    virtual void RemoveStorageL( const TDesC& aNamespace ) = 0;
    
    
    /**
     * Client UID getter
     *
     * @return Client's UID
     */
    virtual const TDesC& ClientUid() const = 0;
    

    /**
     * Lists client's namespaces
     *
     * @return An array of client's namespaces. Ownership is transferred
     * @note Namespaces are invalidated when a storage is removed
     */
    virtual MDesCArray* NamespacesLC() const = 0;
    

    /**
     * File session getter
     *
     * @return Client's filesession
     */
    virtual RFs& FileSession() = 0;
    
    
   
protected:

    virtual ~MNcdStorageClient()
        {
        }
    };


#endif // M_NCDSTORAGECLIENT_H
