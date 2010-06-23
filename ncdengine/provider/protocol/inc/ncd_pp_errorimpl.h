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
* Description:   ?description
*
*/


#ifndef C_NCDPREMINETPROTOCOLERRORIMPL_H
#define C_NCDPREMINETPROTOCOLERRORIMPL_H

#include "ncd_pp_error.h"

class CNcdPreminetProtocolErrorImpl : public CBase,
                                      public MNcdPreminetProtocolError
    {
public:
    
    ~CNcdPreminetProtocolErrorImpl();
    
public:
    TInt Code() const;
    const MNcdConfigurationProtocolQuery* Message() const;
    const TDesC& Id() const;

public:
    TInt iCode;
    MNcdConfigurationProtocolQuery* iMessage;
    HBufC* iId;
    };

#endif
