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


#ifndef NCDPROTOCOLELEMENTQUERYELEMENT_H
#define NCDPROTOCOLELEMENTQUERYELEMENT_H

#include <e32base.h>

#include "ncdqueryitem.h"
//#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQueryOption;
class MNcdConfigurationProtocolDetail;
class CNcdString;


class MNcdConfigurationProtocolQueryElement
    {
public:
    
    /**
     * Type of the query element.
     */
    enum TNcdProtocolQueryElementType
        {
        EUnknown,
        EFreeText,
        ESingleSelect,
        EMultiSelect,
        EGpsLocation,
        EConfiguration,
        EFile,
        ENumeric,
        ESms,
        EPurchaseHistory
        };
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolQueryElement() {}

    /**
     * Returns the ID of this query element.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    
    /**
     * Returns the semantics of this query element.
     *
     * @return Semantics enumeration value.
     */
    virtual MNcdQueryItem::TSemantics Semantics() const = 0;
    
    /**
     * If true then the client must show this query to the end-user.
     *
     * @return True or false.
     */
    virtual TNcdProtocolQueryElementType Type() const = 0;

    /**
     * If true, this query element can be ignored.
     *
     * @return True or false.
     */
    virtual TBool Optional() const = 0;

    /**
     * Labels are used to identify the
     * what the query item is e.g. "username" or "credit card owner".
     * Typically displayed next to the actual item.
     *
     * @return The label.
     */
    virtual const CNcdString& Label() const = 0;
    
    /**
     * Returns query element message. A message related to the element,
     * can be e.g. optionally displayed in a question dialog body close to
     * the query item.
     *
     * @return The message.
     */
    virtual const CNcdString& Message() const = 0;
    
    /**
     * Returns query element description. Description is intented for
     * context-sensitive help.
     *
     * @return The description.
     */
    virtual const CNcdString& Description() const = 0;
    
    /**
     * Returns option count for this query element.
     *
     * @return Number of options.
     */
    virtual TInt OptionCount() const = 0;
    
    /**
     * Returns option by index.
     *
     * @param aIndex Index of the option. Leaves if index is out of bounds.
     * @return Option.
     * @see OptionCount()
     */
    virtual const MNcdConfigurationProtocolQueryOption& Option( TInt aIndex ) const = 0;

    /**
     * Returns custom details object.
     * Ownership is NOT transferred.
     * @return Pointer or NULL if not found.
     */
    virtual const MNcdConfigurationProtocolDetail* Detail() const = 0;

    };


#endif // NCDPROTOCOLELEMENTQUERYELEMENT_H
