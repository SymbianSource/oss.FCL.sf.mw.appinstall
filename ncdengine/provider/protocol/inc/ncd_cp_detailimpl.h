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
* Description:  
*
*/


#ifndef C_NCDCONFIGURATIONPROTOCOLDETAILIMPL_H
#define C_NCDCONFIGURATIONPROTOCOLDETAILIMPL_H

#include "ncd_cp_detail.h"

class CNcdConfigurationProtocolContentImpl : public CBase,
                                             public MNcdConfigurationProtocolContent
    {
public:
    static CNcdConfigurationProtocolContentImpl* NewL();
    static CNcdConfigurationProtocolContentImpl* NewLC();

    ~CNcdConfigurationProtocolContentImpl();

public: // From MNcdConfigurationProtocolContent

    const TDesC& Key() const;
    const TDesC& Value() const;
    const TDesC& Content() const;

private:    
    void ConstructL();

public:
    HBufC* iKey;
    HBufC* iValue;
    HBufC* iContent;
    };

class CNcdConfigurationProtocolDetailImpl : public CBase,
                                            public MNcdConfigurationProtocolDetail
    {
public:
    static CNcdConfigurationProtocolDetailImpl* NewL();
    static CNcdConfigurationProtocolDetailImpl* NewLC();

    ~CNcdConfigurationProtocolDetailImpl();

public: // From MNcdConfigurationProtocolDetail

    const TDesC& Id() const;
    const TDesC& Value() const;
    const TDesC& GroupId() const;
    const TDesC& Label() const;
    const RPointerArray<MNcdConfigurationProtocolDetail>& Details() const;
    const RPointerArray<MNcdConfigurationProtocolContent>& Contents() const;

private:    
    void ConstructL();

public:
    HBufC* iId;
    HBufC* iValue;
    HBufC* iGroupId;
    HBufC* iLabel;
    RPointerArray<MNcdConfigurationProtocolDetail> iDetails;
    RPointerArray<MNcdConfigurationProtocolContent> iContents;
    };

#endif
