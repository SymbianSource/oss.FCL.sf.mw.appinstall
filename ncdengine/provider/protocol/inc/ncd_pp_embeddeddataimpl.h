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
* Description:   CNcdPreminetProtocolEmbeddedDataImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_EMBEDDEDDATA_IMPL_H
#define NCD_PREMINET_PROTOCOL_EMBEDDEDDATA_IMPL_H


#include "ncd_pp_embeddeddata.h"

class MNcdPreminetProtocolDataEntity;
class CNcdPreminetProtocolDataEntityImpl;

class CNcdPreminetProtocolEmbeddedDataImpl : 
    public CBase, public MNcdPreminetProtocolEmbeddedData
    {
public:
    static CNcdPreminetProtocolEmbeddedDataImpl* NewL();
    static CNcdPreminetProtocolEmbeddedDataImpl* NewLC();
    virtual ~CNcdPreminetProtocolEmbeddedDataImpl();
    void ConstructL();
public: // From MNcdProtocolElementEntityRef
    const TDesC& ParentId() const;
    const TDesC& Id() const;
    const TDesC& TimeStamp() const;
    const TDesC& NameSpace() const;
    const TDesC& Name() const;
    TInt DataEntityCount() const;
    const MNcdPreminetProtocolDataEntity& DataEntityL(TInt aIndex) const;
    
public:
    HBufC* iParentId;
    HBufC* iId;
    HBufC* iTimeStamp;
    HBufC* iNameSpace;
    HBufC* iName;
    RPointerArray<MNcdPreminetProtocolDataEntity> iDataEntities;
    };

#endif //NCD_PREMINET_PROTOCOL_EMBEDDEDDATA_IMPL_H
