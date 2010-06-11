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
* Description:   Declaration of MNcdConfigurationData interface.
*
*/


#ifndef C_NCDCONFIGURATIONDATA_H
#define C_NCDCONFIGURATIONDATA_H

#include "ncdinterfaceids.h"
#include "catalogsbase.h"

/**
 * This interface can be used to get the configuration data obtained
 * from CDB. The interface can be queried from load root node operation.
 */
class MNcdConfigurationData : public virtual MCatalogsBase
    {
public:
    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdConfigurationDataUid };

    /**
     * @exception Leave System wide error code
     * @return The configuration response if obtained by the operation.
     */
    virtual HBufC8* ProtocolResponseDataL() = 0;
    };
    
#endif