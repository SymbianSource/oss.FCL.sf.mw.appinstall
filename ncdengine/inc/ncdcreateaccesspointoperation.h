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
* Description:   Create access point operation interface definition
*
*/
	

#ifndef M_NCD_CREATEACCESSPOINT_OPERATION_H
#define M_NCD_CREATEACCESSPOINT_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"

class MNcdCreateAccessPointOperationObserver;

/**
 *  Create accesspoint operation interface.
 *
 *  @note A new accesspoint is not created if a matching accesspoint
 *  already exists
 *
 *  
 *  @see MNcdCreateAccessPointOperationObserver
 */
class MNcdCreateAccessPointOperation : public MNcdOperation
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdCreateAccessPointOperationUid };    
    

    /**
     * Returns the id of the created accesspoint
     *
     * @return Accesspoint id or 0 if the operation has not finished yet
     */
    virtual TUint32 AccessPoint() const = 0;

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdCreateAccessPointOperation() {}

    };
	
	
#endif //  M_NCD_CREATEACCESSPOINT_OPERATION_H
