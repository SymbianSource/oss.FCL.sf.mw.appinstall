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


#ifndef NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_H
#define NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_H

#include <e32base.h>

#include "ncdquery.h"
//#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQueryResponseValue 
    {
public:
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolQueryResponseValue() {}

    /**
     * Value ID
     * @return Id
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Semantics type.
     * @return Type
     * @see MNcdQuery::TSemantics
     */
    virtual MNcdQuery::TSemantics Semantics() const = 0;

    /**
     * Amount of value strings.
     * @return Value count
     */
    virtual TInt ValueCount() const = 0;

    /**
     * Get value by index
     * @param aIndex Value index. Leaves if index is out of bounds.
     * @return Value string.
     * @see ValueCount()
     */
    virtual TDesC& ValueL(TInt aIndex) const = 0;
    };


class MNcdConfigurationProtocolQueryResponse
    {
public:
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolQueryResponse() {}

    /**
     * Returns the ID of this query.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    
    /**
     * Returns the semantics of this query.
     *
     * @return NcdQueryItemSemantics::NcdQueryItemSemantics::NcdQuerySemantics::TNcdQuerySemantics
     */
    virtual MNcdQuery::TSemantics Semantics() const = 0;
    
    /**
     * If true then the client must show this query to the end-user.
     *
     * @return TBool 
     */
    virtual TBool Cancel() const = 0;

    /**
     * Returns the number of query elements in this query.
     *
     * @return Query element count.
     */
    virtual TInt ResponseCount() const = 0;
    
    /**
     * Returns query element by index.
     * @param aIndex Response index. Leaves if index is out of bounds.
     * @return Query element 
     * @see ResponseCount()
     */
    virtual const MNcdConfigurationProtocolQueryResponseValue& 
        ResponseL( TInt aIndex ) const = 0;

    };


#endif // NCD_CONFIGURATION_PROTOCOL_QUERYRESPONSE_H
