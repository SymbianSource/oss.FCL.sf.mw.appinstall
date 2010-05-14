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
* Description:   MNcdConfigurationProtocolError declaration
*
*/


#ifndef M_NCDPREMINETPROTOCOLERROR_H
#define M_NCDPREMINETPROTOCOLERROR_H

#include "ncd_cp_error.h"

class MNcdPreminetProtocolError : public MNcdConfigurationProtocolError
    {
public:
    
    virtual ~MNcdPreminetProtocolError() {}

    /**
     * Retrieves the protocol element id related to this error.
     * @return Id or KNullDesC if not available.
     */
    virtual const TDesC& Id() const = 0;
    };

#endif
