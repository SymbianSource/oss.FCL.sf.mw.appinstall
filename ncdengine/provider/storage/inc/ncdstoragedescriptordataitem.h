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
* Description:   Declaration of CNcdStorageDescriptorDataItem class.
*
*/


#ifndef C_NCDSTORAGEDESCRIPTORDATAITEM_H
#define C_NCDSTORAGEDESCROPTORDATAITEM_H

#include <e32base.h>
#include "ncdstoragedataitem.h"

/**
 * General purpose storage data item for descriptor data.
 */
class CNcdStorageDescriptorDataItem : public CBase, public MNcdStorageDataItem 
{
public:
    /**
     * Two phase constructor. Constructs the object with the given data.
     *
     * @param aData The descriptor data.
     */
    static CNcdStorageDescriptorDataItem* NewL( const TDesC8& aData );

    /**
     * Two phase constructor. Constructs the object with the given data.
     *
     * @param aData The descriptor data.
     */
    static CNcdStorageDescriptorDataItem* NewLC( const TDesC8& aData );

    ~CNcdStorageDescriptorDataItem();
    
public: // From MNcdStorageDataItem
    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );

protected:
    CNcdStorageDescriptorDataItem();
    void ConstructL( const TDesC8& aData );
    
private:
    HBufC8* iData;
};

#endif