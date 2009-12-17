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
* Description:   MNcdProtocolElementEntity declaration
*
*/


#ifndef NCDPROTOCOLELEMENTQUERYIMPL_H
#define NCDPROTOCOLELEMENTQUERYIMPL_H

#include <e32base.h>

#include "ncd_cp_serverdetails.h"

class CNcdConfigurationProtocolServerDetailsImpl : public CBase,
                                                   public MNcdConfigurationProtocolServerDetails
    {
public:
    
    void ConstructL();
    
    virtual ~CNcdConfigurationProtocolServerDetailsImpl();

public: // From MNcdConfigurationProtocolServerDetails

    const TDesC& Version() const;

    TInt DetailCount() const;

    const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const;

    TInt CapabilityCount() const;

    const TDesC& CapabilityL( TInt aIndex ) const;

public:
    
    HBufC* iVersion;
    CArrayPtr<HBufC>* iCapabilities;
    RPointerArray<MNcdConfigurationProtocolDetail> iDetails;
    
    };


#endif // NCDPROTOCOLELEMENTQUERYIMPL_H
