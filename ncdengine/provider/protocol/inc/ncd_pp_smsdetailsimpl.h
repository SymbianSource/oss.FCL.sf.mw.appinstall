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
* Description:   CNcdPreminetProtocolPaymentImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_SMSDETAILS_IMPL_H
#define NCD_PREMINET_PROTOCOL_SMSDETAILS_IMPL_H

#include "ncd_pp_purchase.h"
#include "ncdstoragedataitem.h"

class CNcdPreminetProtocolSmsDetailsImpl :
    public CBase,
    public MNcdPreminetProtocolSmsDetails,
    public MNcdStorageDataItem
    {
    
public:

    static CNcdPreminetProtocolSmsDetailsImpl* NewL();
    static CNcdPreminetProtocolSmsDetailsImpl* NewLC();

    virtual ~CNcdPreminetProtocolSmsDetailsImpl();
    
    void ConstructL();


    virtual const TDesC& Address() const;
    virtual const TDesC& Message() const;
    virtual CNcdPreminetProtocolSmsDetailsImpl* CloneL() const;

public: // From MNcdStorageDataItem

    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );

public:

    HBufC* iAddress;
    HBufC* iMessage;
    
    };

#endif // NCD_PREMINET_PROTOCOL_SMSDETAIL_IMPL_H