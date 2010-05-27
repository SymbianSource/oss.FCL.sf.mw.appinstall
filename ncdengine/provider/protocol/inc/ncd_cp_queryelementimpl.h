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


#ifndef NCDPROTOCOLELEMENTQUERYELEMENTIMPL_H
#define NCDPROTOCOLELEMENTQUERYELEMENTIMPL_H

#include <e32base.h>

#include "ncd_cp_queryelement.h"

class MNcdConfigurationProtocolQueryOption;
class CNcdConfigurationProtocolQueryOptionImpl;
class MNcdConfigurationProtocolDetail;
class CNcdConfigurationProtocolDetailImpl;
class CNcdString;


class CNcdConfigurationProtocolQueryElementImpl : public CBase,
                                                  public MNcdConfigurationProtocolQueryElement
    {
public:
    
    CNcdConfigurationProtocolQueryElementImpl();

    void ConstructL();
    
    virtual ~CNcdConfigurationProtocolQueryElementImpl();

    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const TDesC& Id() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual MNcdQueryItem::TSemantics Semantics() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual MNcdConfigurationProtocolQueryElement::TNcdProtocolQueryElementType
        Type() const;

    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual TBool Optional() const;

    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const CNcdString& Label() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const CNcdString& Message() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const CNcdString& Description() const;
        
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual TInt OptionCount() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const MNcdConfigurationProtocolQueryOption& Option( TInt aIndex ) const;

    /**
     * @see MNcdConfigurationProtocolQueryElement
     */
    virtual const MNcdConfigurationProtocolDetail* Detail() const;

public:

    HBufC* iId;
    MNcdQueryItem::TSemantics iSemantics;
    TNcdProtocolQueryElementType iType;
    TBool iOptional;
    CNcdString* iLabel;
    CNcdString* iMessage;
    CNcdString* iDescription;
    RPointerArray<CNcdConfigurationProtocolQueryOptionImpl> iOptions;
    MNcdConfigurationProtocolDetail* iDetails;
    };


#endif // NCDPROTOCOLELEMENTQUERYELEMENTIMPL_H
