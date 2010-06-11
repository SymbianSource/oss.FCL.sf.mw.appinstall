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
* Description:   MNcdConfigurationProtocolCookie declaration
*
*/


#ifndef M_NCDCONFIGURATIONPROTOCOLDETAIL_H
#define M_NCDCONFIGURATIONPROTOCOLDETAIL_H

#include <e32base.h>
#include "catalogsdebug.h"

class MNcdConfigurationProtocolContent
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolContent() {}

    virtual const TDesC& Key() const = 0;

    virtual const TDesC& Value() const = 0;
    
    virtual const TDesC& Content() const = 0;
    
    };

class MNcdConfigurationProtocolDetail
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdConfigurationProtocolDetail() {}

    /**
     * Returns the ID.
     * @return ID or KNullDesC
     */
    virtual const TDesC& Id() const = 0;

    /**
     * Returns the value.
     * @return Value or KNullDesC
     */
    virtual const TDesC& Value() const = 0;

    /**
     * Returns the group ID.
     * @return ID or KNullDesC
     */
    virtual const TDesC& GroupId() const = 0;

    /**
     * Returns the label.
     * @return Label or KNullDesC
     */
    virtual const TDesC& Label() const = 0;

    /**
     * Get details.
     * @return Returns an array of details objects.
     */
    virtual const RPointerArray<MNcdConfigurationProtocolDetail>& Details() const = 0;

    /**
     * Get contents.
     * @return Returns an array of content objects.
     */
    virtual const RPointerArray<MNcdConfigurationProtocolContent>& Contents() const = 0;
    
    };

#endif
