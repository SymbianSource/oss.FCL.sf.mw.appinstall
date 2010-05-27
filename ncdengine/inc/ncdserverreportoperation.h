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
	

#ifndef M_NCD_SERVER_REPORT_OPERATION_H
#define M_NCD_SERVER_REPORT_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"
#include "ncdoperationdatatypes.h"

/**
 *  Server report operation interface.
 *
 *  Server reports may be sent to the server to inform about the success of
 *  ownload and install operations.
 *
 *  @see MNcdServerReportManager
 *  @see MNcdServerReportOperationObserver
 *  
 *  
 */
class MNcdServerReportOperation : public MNcdOperation
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdServerReportOperationUid };    
    

protected:

    /**
    * Destructor.
    *
    * @see MCatalogsBase::~MCatalogsBase
    */
    virtual ~MNcdServerReportOperation() {}

    };
	
	
#endif //  M_NCD_SERVER_REPORT_OPERATION_H
