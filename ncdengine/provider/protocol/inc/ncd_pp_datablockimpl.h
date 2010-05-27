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


#ifndef C_NCDPREMINETPROTOCOLDATABLOCKIMPL_H
#define C_NCDPREMINETPROTOCOLDATABLOCKIMPL_H

#include "ncd_pp_datablock.h"

class CNcdPreminetProtocolDataBlockImpl : public CBase,
                                          public MNcdPreminetProtocolDataBlock
    {
public:
    static CNcdPreminetProtocolDataBlockImpl* NewL();
    static CNcdPreminetProtocolDataBlockImpl* NewLC();
    void ConstructL();
    ~CNcdPreminetProtocolDataBlockImpl();

public: // From MNcdPreminetProtocolDataBlock

    const TDesC& Id() const;
    const TDesC& NameSpace() const;
    const TDesC8& Content() const;

public:
    HBufC* iId;
    HBufC* iNameSpace;
    HBufC8* iContent;
    };

#endif
