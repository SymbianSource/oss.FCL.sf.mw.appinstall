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
* Description:   MNcdPreminetProtocolDataEntity declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_H
#define NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolDownload;

class MNcdPreminetProtocolEntityDependency
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolEntityDependency() {}

    /**
     * Entity dependency name.
     * @return Name or KNullDesC
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Dependency type.
     * @return Type
     */
    virtual TNcdDependencyType Type() const = 0;

    /**
     * Dependency content ID
     * @return Id or KNullDesC
     */
    virtual const TDesC& ContentId() const = 0;

    /**
     * Dependency content version
     * @return Version or KNullDesC
     */
    virtual const TDesC& ContentVersion() const = 0;

    /**
     * Dependency entity ID
     * @return Id or KNullDesC
     */
    virtual const TDesC& EntityId() const = 0;

    /**
     * Dependency entity timestamp
     * @return Timestamp or KNullDesC
     */
    virtual const TDesC& EntityTimestamp() const = 0;

    /**
     * Dependency download details. Ownership is NOT transferred.
     * @return Download details pointer or NULL
     */
    virtual const MNcdPreminetProtocolDownload* 
        DownloadDetails() const = 0;

    };


#endif //NCD_PROTOCOL_ELEMENT_ENTITYDEPENDENCY_H
