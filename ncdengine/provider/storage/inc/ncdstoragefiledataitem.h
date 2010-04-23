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


#ifndef C_NCDSTORAGEFILEDATAITEM_H
#define C_NCDSTORAGEFILEDATAITEM_H

#include <e32base.h>

#include "ncdstoragedataitem.h"

class RFile;
class RFs;

/**
 *  Externalizes the given file to the database storage
 *  @ Consider adding an observer for errors
 */
class CNcdStorageFileDataItem : public CBase, public MNcdStorageDataItem
    {
public:

    /**
     * Creates a new file data item
     *
     * @param aFilename Path to the file that is written to the database
     * during ExternalizeL()
     * @return A new file data item
     * @throw KErrArgument If the filename is empty
     */        
    static CNcdStorageFileDataItem* NewL( const TDesC& aFilename, RFs& aFs );


    /**
     * Creates a new file data item
     *
     * @param aFilename Path to the file that is written to the database
     * during ExternalizeL()
     * @return A new file data item
     * @throw KErrArgument If the filename is empty     
     */        
    static CNcdStorageFileDataItem* NewLC( const TDesC& aFilename, RFs& aFs );



    virtual ~CNcdStorageFileDataItem();


public: // from MNcdStorageDataItem

    /**
     * Externalizes the file to the database
     *
     * @param aStream Write stream
     */
    void ExternalizeL( RWriteStream& aStream );


    /**
     * Not supported. Use MNcdStorageItem::GetDataL() instead
     * 
     * @throw KErrNotSupported
     */
    void InternalizeL( RReadStream& aStream );


private:

    CNcdStorageFileDataItem( RFs& aFs );

    void ConstructL( const TDesC& aFilename );


private: // data

    RFs& iFs;
    RBuf iFilename;

    };


#endif // C_NCDSTORAGEFILEDATAITEM_H
