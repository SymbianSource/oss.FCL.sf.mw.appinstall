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
* Description:   MNcdProtocolElementItemRef declaration
*
*/


#ifndef NCDPREMINETPROTOCOLITEMREF_H
#define NCDPREMINETPROTOCOLITEMREF_H

#include <e32base.h>

#include "ncd_pp_entityref.h"


class MNcdPreminetProtocolItemRef : public MNcdPreminetProtocolEntityRef
    {
public:
    virtual ~MNcdPreminetProtocolItemRef() {}
    // Currently this is the same as EntityRef.
    };

#endif
