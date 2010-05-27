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


#ifndef M_NCDSTORAGEITEM_H
#define M_NCDSTORAGEITEM_H


class MNcdStorageDataItem;

/**
 * Database storage item interface
 *
 * Used to operate on items in the database
 */
class MNcdStorageItem
{
public:

    /**
     * Opens the item for reading/writing. 
     * Must be saved or rollbacked
     */
    virtual void OpenL() = 0;

    /**
     * Marks the item as saved but doesn't commit it to storage until all 
     * opened items are saved
     */
    virtual void SaveL() = 0;

    /**
     * Rollbacks modifications to this item and all other open or saved but
     * not committed items in the database
     */
    virtual void Rollback() = 0;

    /**
     * Internalizes data from the database to the data item set for this item.
     */
    virtual void ReadDataL() = 0;


    /**
     * Externalizes data from the data item to the database
     */
    virtual void WriteDataL() = 0;

    /**
     * Removes the item from the database
     */
    virtual void RemoveFromStorageL() = 0;

    /**
     * Reads the data from the database and returns it in a descriptor.     
     *
     * @return Item's data in a 8-bit descriptor. The descriptor is added
     * to the cleanupstack
     */
    virtual HBufC8* GetDataLC() = 0;


    /**
     * Open-state getter
     *
     * @return ETrue if the item is open, otherwise EFalse
     */
    virtual TBool IsOpen() const = 0;

    
    /**
     * Item UID getter
     *
     * @return Item's UID
     */
    virtual const TDesC& Uid() const = 0;

    
    /**
     * Item type getter
     * 
     * @return Item's type
     */
    virtual TInt Type() const = 0;

    
    /**
     * Sets data item for this item.
     *
     * ReadDataL() and WriteDataL() internalize and externalize data from
     * this data item to/from the database
     */
    virtual void SetDataItem( MNcdStorageDataItem* aDataItem ) = 0;

protected:

    virtual ~MNcdStorageItem()
        {
        }
};



#endif