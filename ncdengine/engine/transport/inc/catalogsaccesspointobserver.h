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


#ifndef M_CATALOGSACCESSPOINTOBSERVER_H
#define M_CATALOGSACCESSPOINTOBSERVER_H

#include "catalogshttptypes.h"

class TCatalogsConnectionMethod;

enum TCatalogsAccessPointEvent
    {
    ECatalogsAccessPointOpened,
    ECatalogsAccessPointClosed,
    ECatalogsAccessPointReallyClosed
    };

/**
 * Observer interface for observing access point events
 */
class MCatalogsAccessPointObserver
    {
public:

    virtual void HandleAccessPointEventL( 
        const TCatalogsConnectionMethod& aAp,
        const TCatalogsAccessPointEvent& aEvent ) = 0;

protected:
    virtual ~MCatalogsAccessPointObserver()
        {
        }
    };
    
#endif // M_CATALOGSACCESSPOINTOBSERVER_H
    
