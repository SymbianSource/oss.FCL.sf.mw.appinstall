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
* Description:   CNcdPreminetProtocolIcon declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLICONIMPL_H
#define M_NCDPREMINETPROTOCOLICONIMPL_H

#include <e32base.h>

#include "ncd_pp_icon.h"

class CNcdPreminetProtocolIcon : public CBase,
                                 public MNcdPreminetProtocolIcon
    {
public:

    /**
     * Constructor.
     *
     * @return Protocol element icon.
     */
    static CNcdPreminetProtocolIcon* NewL();

    /**
     * Constructor.
     *
     * @return Protocol element icon.
     */
    static CNcdPreminetProtocolIcon* NewLC();

    /**
     * Constructor.
     */
    CNcdPreminetProtocolIcon();

    /**
     * Constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CNcdPreminetProtocolIcon();

// From base class MNcdPreminetProtocolIcon

    /**
     * @see MNcdPreminetProtocolIcon::Id
     */
    const TDesC& Id() const;

    /**
     * @see MNcdPreminetProtocolIcon::DataBlock
     */
    const TDesC& DataBlock() const;

    /**
     * @see MNcdPreminetProtocolIcon::Uri
     */
    const TDesC& Uri() const;

    /**
     * @see MNcdPreminetProtocolIcon::Uri
     */
    const TDesC8& Data() const;

public:

    HBufC* iId;
    HBufC* iDataBlock;
    HBufC* iUri;
    HBufC8* iData;
    
    };

#endif // M_NCDPREMINETPROTOCOLICONIMPL_H
