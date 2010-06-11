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


#ifndef C_NCDFILESTORAGEIMPL_H
#define C_NCDFILESTORAGEIMPL_H

#include <e32base.h>

#include "ncdfilestorage.h"

class RFs;
class MNcdStorageOwner;

/**
 *  File storage
 *
 *  Moves, removes and opens files from a storage directory
 *
 */
class CNcdFileStorage : public CBase, public MNcdFileStorage
    {
public:

    /**
    * Creates a new file storage
    *
    * @param aUid Storage UID
    * @param aPath Storage root path
    * @param aFileSession File session used by the storage
    */
    static CNcdFileStorage* NewL( MNcdStorageOwner& aOwner, const TDesC& aUid, 
        const TDesC& aPath );


    /**
    * Creates a new file storage and leaves it to stack
    *
    * @param aUid Storage UID
    * @param aPath Storage root path
    * @param aFileSession File session used by the storage
    */
    static CNcdFileStorage* NewLC( MNcdStorageOwner& aOwner, 
        const TDesC& aUid, const TDesC& aPath );

    virtual ~CNcdFileStorage();

public: // From MNcdFileStorage

    /**
     * @see MNcdFileStorage::AddFileL( const TDesC&, TBool )
     */
    void AddFileL( const TDesC& aFilepath, TBool aOverwrite );

     
    /**
     * @see MNcdFileStorage::RemoveFileL()
     */
    void RemoveFileL( const TDesC& aFilename );


    /**
     * @see MNcdFileStorage::OpenFileL()
     */
    RFile OpenFileL( RFs& aFs, const TDesC& aFilename, TUint aFileMode );
    
    
    /**
     * @see MNcdFileStorage::Uid()
     */
    const TDesC& Uid() const;

private:

    /**
     * Constructor
     *
     * @param aFileSession
     */
    CNcdFileStorage( MNcdStorageOwner& aOwner );

    void ConstructL( const TDesC& aUid, const TDesC& aPath );

private: // data

    MNcdStorageOwner& iOwner;
    RBuf iRoot;
    RBuf iUid;
    };


#endif // C_NCDFILESTORAGE_H
