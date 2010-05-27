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
* Description:   CNcdPreminetProtocolRightsImpl declaration
*
*/


#ifndef NCD_PREMINET_PROTOCOL_RIGHTS_IMPL_H
#define NCD_PREMINET_PROTOCOL_RIGHTS_IMPL_H

#include <e32base.h>
#include "ncd_pp_rights.h"
#include "ncdprotocoltypes.h"


class CNcdPreminetProtocolRightsImpl
: public CBase, public MNcdPreminetProtocolRights
    {
public:
    static CNcdPreminetProtocolRightsImpl* NewL();
    static CNcdPreminetProtocolRightsImpl* NewLC();
    void ConstructL();
    
    virtual ~CNcdPreminetProtocolRightsImpl();
    virtual const TDesC& ActivationKey() const;
    virtual const TDesC& IssuerUri() const;
    virtual const TDesC& Uri() const;
    virtual const TDesC& Type() const;
    virtual const TDesC& Name() const;
    
    virtual const TDesC& RightsObjectDataBlock() const;
    virtual const TDesC& RightsObjectMime() const;
    virtual const TDesC8& RightsObjectData() const;
    

private:
    CNcdPreminetProtocolRightsImpl();
    
public:
    HBufC* iActivationKey;
    HBufC* iIssuerUri;
    HBufC* iUri;
    HBufC* iType;
    HBufC* iName;    

    HBufC* iRightsObjectDataBlock;    
    HBufC* iRightsObjectMime;    
    HBufC8* iRightsObjectData;    
    };
    

#endif //NCD_PREMINET_PROTOCOL_RIGHTS_IMPL_H
