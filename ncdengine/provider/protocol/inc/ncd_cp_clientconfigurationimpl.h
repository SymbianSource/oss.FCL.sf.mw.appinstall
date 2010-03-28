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


#ifndef C_NCDCONFIGURATIONPROTOCOLCLIENTCONFIGURATIONIMPL_H
#define C_NCDCONFIGURATIONPROTOCOLCLIENTCONFIGURATIONIMPL_H

#include "ncd_cp_clientconfiguration.h"

class CNcdConfigurationProtocolClientConfigurationImpl : public CBase,
                                                         public MNcdConfigurationProtocolClientConfiguration
    {
public:
    static CNcdConfigurationProtocolClientConfigurationImpl* NewL();
    ~CNcdConfigurationProtocolClientConfigurationImpl();

public: // From MNcdConfigurationProtocolClientConfiguration
    TInt ExpirationDelta() const;

    TInt CookieCount() const;
    MNcdConfigurationProtocolCookie& CookieL( TInt aIndex ) const;
    TInt DetailCount() const;
    const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const;

private:
    CNcdConfigurationProtocolClientConfigurationImpl();
    void ConstructL();

public:
    TInt iExpirationDelta;
    CArrayPtr<MNcdConfigurationProtocolCookie>* iCookies;
    CArrayPtr<MNcdConfigurationProtocolDetail>* iDetails;
    };

#endif
