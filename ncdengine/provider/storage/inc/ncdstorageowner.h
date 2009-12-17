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


#ifndef M_NCDSTORAGEOWNER_H
#define M_NCDSTORAGEOWNER_H

class RFs;
class CFileMan;

/**
 * Interface for storage owners
 * INTERNAL
 */
class MNcdStorageOwner
    {
public:
    /**
     * Generates the root path
     *
     * @param aParse Target
     */
    virtual void AppendRoot( TDes& aPath ) const = 0;
    
    /**
     * File session getter
     *
     * @return File session
     */
    virtual RFs& FileSession() = 0;
    
    
    /**
     * File manager getter
     *
     * @return File manager
     */
    virtual CFileMan& FileManager() = 0;
    
protected:

    virtual ~MNcdStorageOwner()
        {
        }
    };


#endif // M_NCDSTORAGEOWNER_H