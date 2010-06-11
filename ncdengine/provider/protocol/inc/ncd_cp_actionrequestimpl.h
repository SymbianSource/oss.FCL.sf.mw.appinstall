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


#ifndef C_NCDCONFIGURATIONPROTOCOLACTIONREQUESTIMPL_H
#define C_NCDCONFIGURATIONPROTOCOLACTIONREQUESTIMPL_H

#include "ncd_cp_actionrequest.h"
#include "ncdprotocoltypes.h"

class CNcdConfigurationProtocolUpdateDetailsImpl : public CBase,
                                                   public MNcdConfigurationProtocolUpdateDetails
    {
public:
    
    ~CNcdConfigurationProtocolUpdateDetailsImpl();

public:
    const TDesC& Id() const;
    const TDesC& Version() const;
    const TDesC& Uri() const;
    void ConstructL();

public:
    HBufC* iId;
    HBufC* iVersion;
    HBufC* iUri;
    };

class CNcdConfigurationProtocolActionRequestImpl : public CBase,
                                                   public MNcdConfigurationProtocolActionRequest
    {
public:
    ~CNcdConfigurationProtocolActionRequestImpl();

public:
    TBool Force() const;

    TNcdProtocolActionRequestType Type() const;

    const TDesC& Target() const;

    TInt MessageCount() const;

    const MNcdConfigurationProtocolQuery& MessageL( TInt aIndex ) const;

    TInt DetailCount() const;

    const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const;

    const MNcdConfigurationProtocolUpdateDetails* UpdateDetails() const;

    void ConstructL();                                                         

public:
    TBool iForce;
    TNcdProtocolActionRequestType iType;
    HBufC* iTarget;
    RPointerArray<MNcdConfigurationProtocolQuery> iMessages;
    RPointerArray<MNcdConfigurationProtocolDetail> iDetails;
    MNcdConfigurationProtocolUpdateDetails* iUpdateDetails;
    };

#endif
