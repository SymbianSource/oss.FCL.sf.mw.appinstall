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
* Description:   CNcdPreminetProtocolFolderRefImpl declaration
*
*/


#ifndef NCDPREMINETPROTOCOLFOLDERREFIMPL_H
#define NCDPREMINETPROTOCOLFOLDERREFIMPL_H

#include "ncd_pp_folderref.h"

class CNcdPreminetProtocolFolderRefImpl : public CBase,
                                          public MNcdPreminetProtocolFolderRef
    {
public:
    static CNcdPreminetProtocolFolderRefImpl* NewL();
    static CNcdPreminetProtocolFolderRefImpl* NewLC();
    virtual ~CNcdPreminetProtocolFolderRefImpl();
    void ConstructL();
public:
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
    // From MNcdProtocolElementFolderRef
    TInt Count() const;
    TInt AuthorizedCount() const;
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
    TInt iCount;
    TInt iAuthorizedCount;
    };

#endif
