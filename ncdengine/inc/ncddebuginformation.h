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
* Description:   Contains MNcdProvider interface
*
*/


#ifndef M_NCDDEBUGINFORMATION_H
#define M_NCDDEBUGINFORMATION_H

#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdDebugInformation : public virtual MCatalogsBase 
    {
public:
    
    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdDebugInformationUid };
    
    /**
     * Returns the client ID.
     */
    virtual HBufC* ClientIdL() = 0;
    
    
    /**
     * Returns engine's type
     */
    virtual HBufC* EngineTypeL() = 0;
    
    /**
     * Returns engine's version
     */
    virtual HBufC* EngineVersionL() = 0;        
    
    
    /**
     * Returns engine's provisioning
     */
    virtual HBufC* EngineProvisioningL() = 0;
    
    
    /**
     * Returns engine's UID
     */
    virtual HBufC* EngineUidL() = 0;
    
    };

#endif