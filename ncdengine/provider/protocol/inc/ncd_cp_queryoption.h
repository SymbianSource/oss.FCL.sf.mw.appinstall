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


#ifndef NCDPROTOCOLELEMENTQUERYOPTION_H
#define NCDPROTOCOLELEMENTQUERYOPTION_H

#include <e32base.h>

class CNcdString;

/**
 * Defines a selectable option in a query element.
 */
class MNcdConfigurationProtocolQueryOption
    {
public:
    
    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolQueryOption() {}

    
    /**
     * Unique identifier for the option. This needs to be sent
     * back in a query response if this option is selected.
     *
     * @return TNcdProtocolQueryTrigger
     */
    virtual const TDesC& Value() const = 0;
    
    /**
     * Name of the option. Should be shown to the end user.
     *
     * @return Option name.
     */
    virtual const CNcdString& Name() const = 0;    

    };


#endif // NCDPROTOCOLELEMENTQUERYOPTION_H
