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
* Description:   MNcdPreminetProtocolInformation declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLEXPIREDCACHEDDATA_H
#define M_NCDPREMINETPROTOCOLEXPIREDCACHEDDATA_H

#include <e32base.h>

class MNcdPreminetProtocolExpiredEntity
    {
public:
    virtual ~MNcdPreminetProtocolExpiredEntity() {}
    virtual const TDesC& EntityId() const = 0;
    virtual TBool Recursive() const = 0;
    virtual TBool ForceUpdate() const = 0;
    };

class MNcdPreminetProtocolClearOldEntities
    {
public:
    virtual ~MNcdPreminetProtocolClearOldEntities() {}
    virtual const TDesC& StartTimestamp() const = 0;
    virtual const TDesC& EndTimestamp() const = 0;
    };

class MNcdPreminetProtocolExpiredCachedData
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolExpiredCachedData() {}

    /**
     * Amount of expired entities
     * @return Entity count
     */
    virtual TInt ExpiredEntityCount() const = 0;

    /**
     * Gets the specified expired entity
     * @param aIndex Entity index
     * @return Entity reference. Leaves if index is out of bounds.
     * @see ExpiredEntityCount()
     */
    virtual const MNcdPreminetProtocolExpiredEntity& ExpiredEntityL( TInt aIndex ) const = 0;
    
    /**
     * Clears old entities.
     * @return 
     */
    virtual const MNcdPreminetProtocolClearOldEntities& ClearOldEntities() const = 0;
    
    };

#endif
