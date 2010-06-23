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
* Description:   MNcdPreminetProtocolSkin declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLSKIN_H
#define M_NCDPREMINETPROTOCOLSKIN_H

class MNcdPreminetProtocolSkin
    {
public:

    /**
     * Destructor.
     */
    virtual ~MNcdPreminetProtocolSkin() {}

    /**
     * Get skin ID.
     *
     * @return ID of the skin.
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Get skin timestamp.
     *
     * @return Timestamp of the skin.
     */
    virtual const TDesC& Timestamp() const = 0;

    /**
     * Get skin URI.
     *
     * @return URI of the skin.
     */
    virtual const TDesC& Uri() const = 0;

    };

#endif // M_NCDPREMINETPROTOCOLSKIN_H
