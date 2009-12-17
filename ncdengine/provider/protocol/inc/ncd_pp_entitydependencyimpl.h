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
* Description:   CNcdPreminetProtocolDataEntityImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_IMPL_H
#define NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_IMPL_H

#include <e32base.h>
#include "ncd_pp_entitydependency.h"
#include "ncdprotocoltypes.h"

class CNcdPreminetProtocolEntityDependencyImpl 
: public CBase, public MNcdPreminetProtocolEntityDependency
    {
    
public:

    static CNcdPreminetProtocolEntityDependencyImpl* NewL();
    static CNcdPreminetProtocolEntityDependencyImpl* NewLC();

    virtual ~CNcdPreminetProtocolEntityDependencyImpl();
    
    void ConstructL();

    
    // From base class MNcdPreminetProtocolDataEntity

    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or KNullDesC if never modified.
     */
    virtual const TDesC& Name() const;

    /**
     * Returns the size of this content.
     * @return Type
     */
    virtual TNcdDependencyType Type() const;

    /**
     * Returns the ID of this entity.
     * @return Id or KNullDesC
     */
    virtual const TDesC& ContentId() const;

    /**
     * Retuns the version of this entity.
     * @return Version or KNullDesC
     */
    virtual const TDesC& ContentVersion() const;

    /**
     * Returns the ID of this entity.
     * @return Id or KNullDesC
     */
    virtual const TDesC& EntityId() const;

    /**
     * Retuns the version of this entity.
     * @return Version or KNullDesC
     */
    virtual const TDesC& EntityTimestamp() const;

    /**
     * Returns the ID of this entity.
     * @return Id or KNullDesC
     */
    virtual const MNcdPreminetProtocolDownload* 
        DownloadDetails() const;

private:
    CNcdPreminetProtocolEntityDependencyImpl();

public:

    HBufC* iName;
    HBufC* iTimestamp;
    TNcdDependencyType iType;
    HBufC* iContentId;
    HBufC* iContentVersion;
    HBufC* iEntityId;
    HBufC* iEntityTimestamp;
    MNcdPreminetProtocolDownload* iDownloadDetails;
    
    };

#endif // NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_IMPL_H
