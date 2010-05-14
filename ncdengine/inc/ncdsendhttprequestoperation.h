/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef M_NCDSENDHTTPREQUESTOPERATION_H
#define M_NCDSENDHTTPREQUESTOPERATION_H

#include <e32base.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"

/**
 * Operation for sending a standard HTTP request
 */
class MNcdSendHttpRequestOperation : public MNcdOperation
    {
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdSendHttpRequestOperationUid };    
    
    /**
     * Request response
     *
     * @return HTTP response for the sent request. Ownership is transferred
     * @leave KErrNotReady if the operation has not finished
     * @leave Other Symbian error code
     */
    virtual HBufC8* ResponseL() const = 0;

protected:

    virtual ~MNcdSendHttpRequestOperation()
        {
        }
    };

#endif // M_NCDSENDHTTPREQUESTOPERATION_H
