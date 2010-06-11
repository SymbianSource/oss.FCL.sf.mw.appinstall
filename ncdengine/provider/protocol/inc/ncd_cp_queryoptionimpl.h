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


#ifndef NCD_CONFIGURATION_PROTOCOL_ELEMENT_QUERYOPTIONIMPL_H
#define NCD_CONFIGURATION_PROTOCOL_ELEMENT_QUERYOPTIONIMPL_H

#include <e32base.h>

#include "ncd_cp_queryoption.h"

/**
 * Defines a selectable option in a query element.
 */
class CNcdConfigurationProtocolQueryOptionImpl :
    public CBase,
    public MNcdConfigurationProtocolQueryOption
    {
public:
    
    void ConstructL();
    
    virtual ~CNcdConfigurationProtocolQueryOptionImpl();
            
    /**
     * @see MNcdConfigurationProtocolQueryOption
     */
    virtual const TDesC& Value() const;
    
    /**
     * @see MNcdConfigurationProtocolQueryOption
     */
    virtual const CNcdString& Name() const;

public:

    HBufC* iValue;
    CNcdString* iName;
    
    };


#endif // NCD_CONFIGURATION_PROTOCOL_ELEMENT_QUERYOPTIONIMPL_H
