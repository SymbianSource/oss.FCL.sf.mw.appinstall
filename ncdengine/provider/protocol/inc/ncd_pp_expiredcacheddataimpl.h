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


#ifndef C_NCDPREMINETPROTOCOLEXPIREDCACHEDDATAIMPL_H
#define C_NCDPREMINETPROTOCOLEXPIREDCACHEDDATAIMPL_H

#include "ncd_pp_expiredcacheddata.h"

class CNcdPreminetProtocolExpiredEntityImpl : public CBase,
                                              public MNcdPreminetProtocolExpiredEntity
    {
public:
    void ConstructL();
    ~CNcdPreminetProtocolExpiredEntityImpl();
    
public: // From MNcdPreminetProtocolExpiredEntity
    const TDesC& EntityId() const;
    TBool Recursive() const;
    TBool ForceUpdate() const;

public:
    HBufC* iEntityId;
    TBool iRecursive;
    TBool iForceUpdate;
    };

class CNcdPreminetProtocolClearOldEntitiesImpl : public CBase,
                                                 public MNcdPreminetProtocolClearOldEntities
    {
public:
    void ConstructL();
    ~CNcdPreminetProtocolClearOldEntitiesImpl();
    const TDesC& StartTimestamp() const;
    const TDesC& EndTimestamp() const;

public: // From MNcdPreminetProtocolClearOldEntities
    HBufC* iStartTimestamp;
    HBufC* iEndTimestamp;
    };

class CNcdPreminetProtocolExpiredCachedDataImpl : public CBase,
                                                  public MNcdPreminetProtocolExpiredCachedData
    {
public:
    void ConstructL();
    ~CNcdPreminetProtocolExpiredCachedDataImpl();

public: // From MNcdPreminetProtocolExpiredCachedData
    TInt ExpiredEntityCount() const;

    const MNcdPreminetProtocolExpiredEntity& ExpiredEntityL( TInt aIndex ) const;
    
    const MNcdPreminetProtocolClearOldEntities& ClearOldEntities() const;

public:
    RPointerArray<MNcdPreminetProtocolExpiredEntity> iExpiredEntityCounts;
    CNcdPreminetProtocolClearOldEntitiesImpl* iClearOldEntities;
    };

#endif
