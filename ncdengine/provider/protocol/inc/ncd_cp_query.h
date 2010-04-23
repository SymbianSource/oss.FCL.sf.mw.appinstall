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


#ifndef NCDPROTOCOLELEMENTQUERY_H
#define NCDPROTOCOLELEMENTQUERY_H

#include <e32base.h>

#include "ncdquery.h"
//#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQueryElement;
class CNcdString;


class MNcdConfigurationProtocolQuery
    {
public:
    
    enum TNcdProtocolQueryTrigger
        {
        ETriggerAutomatic, // triggered when query encountered
        ETriggerActivation // triggered when associated entity is activated
        };
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolQuery() {}

    /**
     * Returns the ID of this query.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    
    /**
     * Returns the semantics of this query.
     *
     * @return Semantics enumeration value.
     */
    virtual MNcdQuery::TSemantics Semantics() const = 0;
    
    /**
     * Explicitly specifies an event that triggers the message.
     *
     * @return TNcdProtocolQueryTrigger
     */
    virtual TNcdProtocolQueryTrigger Trigger() const = 0;
    
    /**
     * If true then the client must show this query to the end-user.
     *
     * @return True or false
     */
    virtual TBool Force() const = 0;

    /**
     * Returns the response URI of this query.
     * @return URI or KNullDesC
     */
    virtual const TDesC& ResponseUri() const = 0;
    
    /**
     * If true, the query can be omitted.
     *
     * @return True or false
     */
    virtual TBool Optional() const = 0;

    /**
     * Proposed dialog title that the client should use when displaying this
     * message.
     *
     * @return Title text.
     */
    virtual const CNcdString& Title() const = 0;
    
    /**
     * Proposed dialog body that the client should use when displaying this
     * message.
     *
     * @return Body text.
     */
    virtual const CNcdString& BodyText() const = 0;

    /**
     * Returns the number of query elements in this query.
     *
     * @return Query element count.
     */
    virtual TInt QueryElementCount() const = 0;
    
    /**
     * Returns query element by index.
     *
     * @param aIndex Query index. Leaves if index is out of bounds.
     * @return Query element 
     */
    virtual const MNcdConfigurationProtocolQueryElement& QueryElementL( 
        TInt aIndex ) const = 0;

    };


#endif // NCDPROTOCOLELEMENTQUERY_H
