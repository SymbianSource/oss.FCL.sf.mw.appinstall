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


#ifndef C_NCDDOWNLOADREPORTOBSERVER_H
#define C_NCDDOWNLOADREPORTOBSERVER_H

#include "ncdreportstatusinfo.h"
#include "catalogshttptypes.h"
#include <e32base.h>

class CNcdNodeIdentifier;
class TCatalogsConnectionMethod;

/**
 * Observer interface for reporting download events
 */
class MNcdDownloadReportObserver
    {
public:

    virtual TNcdReportId RegisterDownloadL(
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
    	const TNcdReportStatusInfo& aStatus, 
    	const TDesC& aReportUri,
    	const TDesC& aReportNamespace ) = 0;
        

    virtual TNcdReportId RegisterOmaDownloadL(
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
        const TNcdReportStatusInfo& aStatus,
        const TDesC& aReportUri ) = 0;    

    virtual TInt SetDownloadReportAccessPoint( 
        const TNcdReportId& aReportId,
        const TCatalogsConnectionMethod& aAccessPoint ) = 0;

    virtual void ReportDownloadStatusL( 
        const TNcdReportId& aReportId,
        const TNcdReportStatusInfo& aStatus,
        TBool aSendable ) = 0;

protected:

    virtual ~MNcdDownloadReportObserver()
        {
        }
    };

#endif // C_NCDDOWNLOADREPORTOBSERVER_H