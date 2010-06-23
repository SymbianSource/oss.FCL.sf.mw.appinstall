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
* Description:   Contains MStorable interface
*
*/


#ifndef M_NCD_STORABLE_H
#define M_NCD_STORABLE_H


// RWriteStream, RReadStream
#include <s32strm.h> 


/**
 * Interface for items that may be inserted to the database.
 *
 * @note Objects that are inserted to the database should
 * implement MNcdStorageDataItem-interface. This interface
 * is meant for the objects that MNcdStorageDataItem-object
 * may want to insert to the database. In other words,
 * MNcdStorageDataItem-forwards InternalizeL and ExternalizeL
 * calls to the objects that implement this interface.
 *
 */
class MNcdStorable
{

public:

    /** 
     * Destructor
     *
     * By safety reasons it is always good idea to implement
     * virtual destructor for interfaces. If the object
     * is deleted using this interface, then also destructors
     * of child classes will be called.
     */
    virtual ~MNcdStorable() { }

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
};


#endif // M_NCD_STORABLE_H
