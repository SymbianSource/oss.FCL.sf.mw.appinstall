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
* Description:   CNcdPreminetProtocolDescriptorImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_DESCRIPTOR_IMPL_H
#define NCD_PREMINET_PROTOCOL_DESCRIPTOR_IMPL_H

#include <e32base.h>
#include "ncd_pp_descriptor.h"
#include "ncdprotocoltypes.h"


class CNcdPreminetProtocolDescriptorImpl
: public CBase, public MNcdPreminetProtocolDescriptor
    {
public:
    static CNcdPreminetProtocolDescriptorImpl* NewL();
    static CNcdPreminetProtocolDescriptorImpl* NewLC();
    void ConstructL();
    
    virtual ~CNcdPreminetProtocolDescriptorImpl();
    virtual const TDesC& Type() const;
    virtual const TDesC& Name() const;
    virtual const TDesC& Uri() const;
    virtual const TDesC8& Data() const;

private:
    CNcdPreminetProtocolDescriptorImpl();
    
public:
    HBufC* iType;
    HBufC* iName;
    HBufC* iUri;
    HBufC8* iData;    
    };
    

#endif //NCD_PREMINET_PROTOCOL_DESCRIPTOR_IMPL_H
