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


#ifndef M_NCDPREMINETPROTOCOLDATABLOCK_H
#define M_NCDPREMINETPROTOCOLDATABLOCK_H

#include <e32base.h>

class MNcdPreminetProtocolDataBlock
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolDataBlock() {}

    /**
     * Datablock id.
     * @return Id or KNullDesC
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Datablock namespace.
     * @return Namespace or KNullDesC
     */
    virtual const TDesC& NameSpace() const = 0;

    /**
     * Datablock Content.
     * @return Content data or KNullDesC8
     */
    virtual const TDesC8& Content() const = 0;

    };

#endif
