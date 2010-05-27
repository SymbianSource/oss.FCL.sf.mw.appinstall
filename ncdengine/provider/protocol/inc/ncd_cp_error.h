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
* Description:   MNcdConfigurationProtocolError declaration
*
*/


#ifndef M_NCDCONFIGURATIONPROTOCOLERROR_H
#define M_NCDCONFIGURATIONPROTOCOLERROR_H

class MNcdConfigurationProtocolQuery;

class MNcdConfigurationProtocolError
    {
public:
    
    virtual ~MNcdConfigurationProtocolError() {}

    /**
     * Retrieves the error code describing the situation.
     */
    virtual TInt Code() const = 0;
    /**
     * Retrieves the message related to this error.
     * @return Query or NULL if not available.
     */
    virtual const MNcdConfigurationProtocolQuery* Message() const = 0;
    };

#endif
