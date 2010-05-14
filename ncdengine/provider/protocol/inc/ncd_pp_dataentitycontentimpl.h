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
* Description:   CNcdPreminetProtocolDataEntityImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H
#define NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H

#include <e32base.h>
#include "ncd_pp_dataentitycontent.h"
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolEntityDependency;

class CNcdPreminetProtocolDataEntityContentImpl 
: public CBase, public MNcdPreminetProtocolDataEntityContent
    {
    
public:

    static CNcdPreminetProtocolDataEntityContentImpl* NewL();
    static CNcdPreminetProtocolDataEntityContentImpl* NewLC();

    virtual ~CNcdPreminetProtocolDataEntityContentImpl();
    
    void ConstructL();

    
    // From base class MNcdPreminetProtocolDataEntity

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const TDesC& Timestamp() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TInt Size() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const TDesC& Mime() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const TDesC& Id() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const TDesC& Version() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TInt ValidUntilDelta() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TBool ValidUntilAutoUpdate() const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TInt ContentPurposeCount() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const TDesC& ContentPurposeL(TInt aIndex) const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TInt EntityDependencyCount() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual const MNcdPreminetProtocolEntityDependency& 
        EntityDependencyL(TInt aIndex) const;

    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TNcdSubscriptionType SubscriptionType() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TBool ChildViewable() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntityContent
     */
    virtual TBool ChildSeparatelyPurchasable() const;

private:
    CNcdPreminetProtocolDataEntityContentImpl();

public:

    HBufC* iTimestamp;
    TInt iSize;
    HBufC* iMime;
    HBufC* iId;
    HBufC* iVersion;
    TInt iValidUntilDelta;
    TBool iValidUntilAutoUpdate;
    
    RPointerArray<HBufC> iContentPurposes;
    RPointerArray<MNcdPreminetProtocolEntityDependency> iEntityDependencies;
    
    TNcdSubscriptionType iSubscriptionType;
    TBool iChildViewable;
    TBool iChildSeparatelyPurchasable;
    
    
    };

#endif // NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H
