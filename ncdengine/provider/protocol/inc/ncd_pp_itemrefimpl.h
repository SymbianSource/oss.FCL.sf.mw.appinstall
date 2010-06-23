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
* Description:   CNcdPreminetProtocolItemRefImpl declaration
*
*/


#ifndef NCDPPITEMREFIMPL_H
#define NCDPPITEMREFIMPL_H

#include "ncd_pp_itemref.h"

class CNcdPreminetProtocolItemRefImpl : public CBase,
                                        public MNcdPreminetProtocolItemRef
    {
public:
    static CNcdPreminetProtocolItemRefImpl* NewL();
    static CNcdPreminetProtocolItemRefImpl* NewLC();
    virtual ~CNcdPreminetProtocolItemRefImpl();
    void ConstructL();
public: // From MNcdProtocolElementEntityRef
    TType Type() const;
    TDescription Description() const;
    const TDesC& ParentId() const;
    const TDesC& Id() const;
    const TDesC& Timestamp() const;
    const TDesC& RemoteUri() const;
    const TDesC& Namespace() const;
    const RArray<TInt>& Queries() const;
    const TDesC& ServerUri() const;
    const TDesC& ParentNamespace() const;
    TInt ValidUntilDelta() const;
    TBool ValidUntilAutoUpdate() const;
    
public:
    TDescription iDescription;
    HBufC* iParentId;
    HBufC* iId;
    HBufC* iTimestamp;
    HBufC* iRemoteUri;
    HBufC* iNamespace;
    RArray<TInt> iQueries;
    HBufC* iServerUri;
    HBufC* iParentNamespace;
    TInt iValidUntilDelta;
    TBool iValidUntilAutoUpdate;
    };

#endif
