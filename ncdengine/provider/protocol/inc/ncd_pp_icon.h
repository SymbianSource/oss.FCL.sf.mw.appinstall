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
* Description:   MNcdPreminetProtocolIcon declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLICON_H
#define M_NCDPREMINETPROTOCOLICON_H

class MNcdPreminetProtocolIcon
    {
public:

    /**
     * Destructor.
     */
    virtual ~MNcdPreminetProtocolIcon() {}

    /**
     * Get icon ID.
     * @return ID of the icon.
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Get icon Data Block.
     * @return Data Block of the icon.
     */
    virtual const TDesC& DataBlock() const = 0;

    /**
     * Get icon URI.
     * @return URI of the icon.
     */
    virtual const TDesC& Uri() const = 0;

    /**
     * Get icon data (if received as inline data).
     * @return Data of the icon or KNullDesC8
     */
    virtual const TDesC8& Data() const = 0;

    };

#endif // M_NCDPREMINETPROTOCOLICON_H
