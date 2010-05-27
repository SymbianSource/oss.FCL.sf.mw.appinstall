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
* Description:   CNcdPreminetProtocolSkin declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLSKINIMPL_H
#define M_NCDPREMINETPROTOCOLSKINIMPL_H

#include <e32base.h>

#include "ncd_pp_skin.h"

class CNcdPreminetProtocolSkin : public CBase,
                                 public MNcdPreminetProtocolSkin
    {
public:

    /**
     * Constructor.
     *
     * @return Protocol element icon.
     */
    static CNcdPreminetProtocolSkin* NewL();

    /**
     * Constructor.
     *
     * @return Protocol element icon.
     */
    static CNcdPreminetProtocolSkin* NewLC();

    /**
     * Constructor.
     */
    CNcdPreminetProtocolSkin();

    /**
     * Constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CNcdPreminetProtocolSkin();

// From base class MNcdPreminetProtocolSkin

    /**
     * @see MNcdPreminetProtocolSkin::Id
     */
    const TDesC& Id() const;

    /**
     * @see MNcdPreminetProtocolSkin::Timestamp
     */
    const TDesC& Timestamp() const;

    /**
     * @see MNcdPreminetProtocolSkin::Uri
     */
    const TDesC& Uri() const;

public:

    HBufC* iId;
    HBufC* iTimestamp;
    HBufC* iUri;

    };

#endif // M_NCDPREMINETPROTOCOLSKINIMPL_H
