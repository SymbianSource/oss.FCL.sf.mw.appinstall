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
* Description:   MNcdPreminetProtocolRights declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_RIGHTS_H
#define NCD_PREMINET_PROTOCOL_RIGHTS_H

#include <e32base.h>
#include "ncdprotocoltypes.h"


class MNcdPreminetProtocolRights
    {
public:
    virtual ~MNcdPreminetProtocolRights() {}
    virtual const TDesC& ActivationKey() const = 0;
    virtual const TDesC& IssuerUri() const = 0;
    virtual const TDesC& Uri() const = 0;
    virtual const TDesC& Type() const = 0;
    virtual const TDesC& Name() const = 0;
    
    /**
     * Rights object, data block id
     * @return Datablock id or KNullDesC
     */
    virtual const TDesC& RightsObjectDataBlock() const = 0;

    /**
     * Rights object, mime type
     * @return Mime type or KNullDesC
     */
    virtual const TDesC& RightsObjectMime() const = 0;

    /**
     * Rights object, inline object data
     * @return Data or KNullDesC
     */
    virtual const TDesC8& RightsObjectData() const = 0;
    };


#endif //NCD_PREMINET_PROTOCOL_RIGHTS_H
