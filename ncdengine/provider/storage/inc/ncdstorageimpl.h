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


#ifndef C_NCDSTORAGE_H
#define C_NCDSTORAGE_H

#include <e32base.h>

#include "ncdstorage.h"
#include "ncdstorageowner.h"

class CNcdDbStorageManager;
class CNcdFileStorage;

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdStorage : public CBase,
                    public MNcdStorage
    {
public:

    static CNcdStorage* NewL( MNcdStorageOwner& aOwner, 
        const TDesC& aNamespace );

    static CNcdStorage* NewLC( MNcdStorageOwner& aOwner, 
        const TDesC& aNamespace );

    static CNcdStorage* NewL( MNcdStorageOwner& aOwner, 
        HBufC* aNamespace, HBufC* aDirectory );
    

    virtual ~CNcdStorage();

public: // MNcdStorage

    /**
     * @see MNcdStorage::DatabaseStorageL()
     */     
    MNcdDatabaseStorage& DatabaseStorageL( const TDesC& aUid );


    /**
     * @see MNcdStorage::FileStorageL()
     */     
    MNcdFileStorage& FileStorageL( const TDesC& aUid );


    /**
     * @see MNcdStorage::Namespace()
     */
    const TDesC& Namespace() const;


    /**
     * @see MNcdStorage::Directory()
     */
    const TDesC& Directory() const;


    /**
     * @see MNcdStorage::FileSession()
     */
     RFs& FileSession();
     
     
     /**
      * @see MNcdStorage::SizeL()
      */
     TInt SizeL();

private:

    CNcdStorage( MNcdStorageOwner& aOwner );

    void ConstructL( const TDesC& aNamespace );
    
    void ConstructL( HBufC* aNamespace, HBufC* aDirectory );
    
    
    TInt FindDbStorageByUid( const TDesC& aUid );
    
    TInt FindFileStorageByUid( const TDesC& aUid );

    // Put current path + aUid to aPath and ensures that
    // the path exists on drive
    void CreateCurrentPathL( TDes& aPath, const TDesC& aUid ) const;
    
    
    HBufC* CurrentPathLC() const;

private: // data

    MNcdStorageOwner& iOwner;    
    RBuf iNamespace;
    RBuf iDirectory;
    RPointerArray<CNcdDbStorageManager> iDbStorages;
    RPointerArray<CNcdFileStorage> iFileStorages;
    };



#endif // C_NCDSTORAGE_H
