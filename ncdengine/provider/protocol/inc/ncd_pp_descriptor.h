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
* Description:   MNcdPreminetProtocolDownload declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_DESCRIPTOR_H
#define NCD_PREMINET_PROTOCOL_DESCRIPTOR_H

#include <e32base.h>
#include "ncdprotocoltypes.h"


class MNcdPreminetProtocolDescriptor
    {
public:
    virtual ~MNcdPreminetProtocolDescriptor() {}

    /**
     * Descriptor type.
     * @return Type or KNullDesC
     */
    virtual const TDesC& Type() const = 0;

    /**
     * Descriptor name.
     * @return Name or KNullDesC
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Descriptor uri.
     * @return Uri or KNullDesC
     */
    virtual const TDesC& Uri() const = 0;

    /**
     * Descriptor data.
     * @return Data or KNullDesC
     */
    virtual const TDesC8& Data() const = 0;
    };


#endif //NCD_PREMINET_PROTOCOL_DESCRIPTOR_H
