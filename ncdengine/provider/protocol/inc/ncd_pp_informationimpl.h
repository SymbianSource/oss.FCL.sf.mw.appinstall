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


#ifndef C_NCDPREMINETPROTOCOLINFORMATIONIMPL_H
#define C_NCDPREMINETPROTOCOLINFORMATIONIMPL_H

#include "ncd_pp_information.h"

class CNcdPreminetProtocolInformationImpl : public CBase,
                                            public MNcdPreminetProtocolInformation
    {
public:
    void ConstructL();
    ~CNcdPreminetProtocolInformationImpl();

public: // From MNcdPreminetProtocolInformation

    TInt CookieCount() const;

    const MNcdConfigurationProtocolCookie& CookieL( TInt aIndex ) const;
    
    TInt MessageCount() const;

    const MNcdConfigurationProtocolQuery& MessageL( TInt aIndex ) const;

    TInt DetailCount() const;

    const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const;

    TInt ResendAfter() const;

    const MNcdPreminetProtocolExpiredCachedData* ExpiredCachedData() const;

    const MNcdConfigurationProtocolServerDetails* ServerDetails() const;

    const TDesC& Namespace() const;
    
public:
    CArrayPtr<MNcdConfigurationProtocolCookie>* iCookies;
    CArrayPtr<MNcdConfigurationProtocolQuery>* iMessages;
    CArrayPtr<MNcdConfigurationProtocolDetail>* iDetails;
    TInt iResendAfter;
    MNcdPreminetProtocolExpiredCachedData* iExpiredCachedData;
    MNcdConfigurationProtocolServerDetails* iServerDetails;
    HBufC* iNamespace;
    };

#endif
