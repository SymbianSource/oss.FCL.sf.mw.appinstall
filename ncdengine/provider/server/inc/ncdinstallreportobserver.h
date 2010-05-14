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


#ifndef M_NCDINSTALLREPORTOBSERVER_H
#define M_NCDINSTALLREPORTOBSERVER_H

#include <e32base.h>

#include "ncdreportstatusinfo.h"
#include "catalogshttptypes.h"

class CNcdNodeIdentifier;
class TCatalogsConnectionMethod;

/**
 * Observer interface for reporting install events
 */
class MNcdInstallReportObserver
    {
public:

    virtual TNcdReportId RegisterInstallL(
        const TDesC& aContentIdentifier,
        const CNcdNodeIdentifier& aMetadataId,
    	const TNcdReportStatusInfo& aStatus, 
    	const TDesC& aReportUri,
    	const TDesC& aReportNamespace ) = 0;
        
    virtual TInt SetInstallReportAccessPoint( 
        const TNcdReportId& aReportId,
        const TCatalogsConnectionMethod& aAccessPoint ) = 0;

    virtual void ReportInstallStatusL( 
        const TNcdReportId& aReportId,
        const TNcdReportStatusInfo& aStatus ) = 0;

protected:

    virtual ~MNcdInstallReportObserver()
        {
        }
    };

#endif // M_NCDINSTALLREPORTOBSERVER_H