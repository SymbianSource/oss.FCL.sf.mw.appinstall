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


#ifndef NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_IMPL_H
#define NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_IMPL_H

#include <e32base.h>

#include "ncd_cp_queryresponse.h"

class CNcdString;


class CNcdConfigurationProtocolQueryResponseValueImpl
: public CBase, public MNcdConfigurationProtocolQueryResponseValue 
    {
public:
    static CNcdConfigurationProtocolQueryResponseValueImpl* NewL();
    static CNcdConfigurationProtocolQueryResponseValueImpl* NewLC();
    void ConstructL();
    virtual ~CNcdConfigurationProtocolQueryResponseValueImpl();
private:
    CNcdConfigurationProtocolQueryResponseValueImpl();
public:
    virtual const TDesC& Id() const;
    virtual MNcdQuery::TSemantics Semantics() const;
    virtual TInt ValueCount() const;
    virtual TDesC& ValueL(TInt aIndex) const;
public:
    RPointerArray<HBufC> iValues;
    HBufC* iId;
    MNcdQuery::TSemantics iSemantics;
    };


class CNcdConfigurationProtocolQueryResponseImpl 
: public CBase, public MNcdConfigurationProtocolQueryResponse
    {
public:
    
    static CNcdConfigurationProtocolQueryResponseImpl* NewL();
    static CNcdConfigurationProtocolQueryResponseImpl* NewLC();
    
    CNcdConfigurationProtocolQueryResponseImpl();

    void ConstructL();
    
    virtual ~CNcdConfigurationProtocolQueryResponseImpl();

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const TDesC& Id() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual MNcdQuery::TSemantics Semantics() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TBool Cancel() const;

    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual TInt ResponseCount() const;
    
    /**
     * @see MNcdPerminetProtocolQuery
     */
    virtual const MNcdConfigurationProtocolQueryResponseValue& 
        ResponseL( TInt aIndex ) const;
        
public:
    
    HBufC* iId;
    MNcdQuery::TSemantics iSemantics;
    TBool iCancel;

    RPointerArray<CNcdConfigurationProtocolQueryResponseValueImpl> iResponses;
    

    };


#endif // NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_IMPL_H
