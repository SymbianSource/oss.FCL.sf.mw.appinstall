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
* Description:   MNcdPreminetProtocolEmbeddedData declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_EMBEDDEDDATA_H
#define NCD_PREMINET_PROTOCOL_EMBEDDEDDATA_H

#include <e32base.h>

class MNcdPreminetProtocolDataEntity;

class MNcdPreminetProtocolEmbeddedData
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolEmbeddedData() {}

    /**
     * Returns the ID of the parent entity.
     * This entity should be added to the parent's child list,
     * @return Parent id or KNullDesC if parentless.
     */
    virtual const TDesC& ParentId() const = 0;
    
    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or 0 if never modified.
     */
    virtual const TDesC& TimeStamp() const = 0;

    /**
     * Returns the namespace for this entity
     * @return namespace or KNullDesC 
     */
    virtual const TDesC& NameSpace() const = 0;

    /**
     * Returns the query ids related to this entity.
     * The array may be empty.
     * @return Query id array
     */
    virtual const TDesC& Name() const = 0;
    
    /**
     * The amount of data entities.
     * @return Entity count
     */
    virtual TInt DataEntityCount() const = 0;
    
    /**
     * Data entity.
     * @param aIndex Index of data entity.
     * @return Data entity reference. Leaves if index is out of bounds.
     * @see DataEntityCount().
     */
    virtual const MNcdPreminetProtocolDataEntity& 
        DataEntityL(TInt aIndex) const = 0;
    };

#endif
