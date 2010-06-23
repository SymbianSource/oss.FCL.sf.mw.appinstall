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
* Description:   Download operation interface definition
*
*/
	

#ifndef M_NCD_RIGHTS_OBJECT_OPERATION_H
#define M_NCD_RIGHTS_OBJECT_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"
#include "ncdoperationdatatypes.h"

/**
 *  DRM rights object download and install operation interface.
 *
 *  Rights object operation handles downloading a DRM rights object from a
 *  given URI and installing the rights object (adding the rights to the terminal
 *  so that contents protected with the rights can be used).
 *  
 *  
 */
class MNcdRightsObjectOperation : public MNcdOperation
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdRightsObjectOperationUid };    
    

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdRightsObjectOperation() {}

    };
	
	
#endif //  M_NCD_DOWNLOAD_OPERATION_H
