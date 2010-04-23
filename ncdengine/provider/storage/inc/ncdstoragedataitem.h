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


#ifndef M_NCDSTORAGEITEMDATA_H
#define M_NCDSTORAGEITEMDATA_H


#include <s32strm.h> // RWriteStream, RReadStream


/**
 * Interface for database items.
 *
 * All items that are added to the database storage must implement this
 *
 * @note Do not try to use the database from ExternalizeL or InternalizeL since
 * it leads to a DBMS-Table 11 panic.
 *  
 */
class MNcdStorageDataItem
{
public:


    /**
     * Writes the implementing object's data to the stream
     *
     * @param aStream Write stream
     */
    virtual void ExternalizeL( RWriteStream& aStream ) = 0;


    /**
     * Reads the implementing object's data from the stream
     * 
     * @param aStream Read stream
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;


protected:

    virtual ~MNcdStorageDataItem()
        {
        }
};


#endif // M_NCDSTORAGEITEMDATA_H