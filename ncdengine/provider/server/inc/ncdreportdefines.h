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


#ifndef C_NCDREPORTDEFINES_H
#define C_NCDREPORTDEFINES_H

#include <e32err.h>

typedef TInt TNcdReportId;
const TInt KNcdReportIdLength16Bit = 11;

const TNcdReportId KNcdReportNotSupported = KErrNotSupported;

enum TNcdReportType 
    {
    ENcdReportDownload,
    ENcdReportOmaDownload,
    ENcdReportInstall
    };


/**
 * Report status enumerations
 */
enum TNcdReportStatus 
    {
    ENcdReportNone,
    ENcdReportCreate,
    ENcdReportStart,
    ENcdReportPause,
    ENcdReportResume,
    ENcdReportCancel,
    ENcdReportFail,
    ENcdReportSuccess
    };

enum TNcdReportTimeStamps 
    {    
    /**
     * Timestamp of the latest start
     */
    ENcdReportTimeStart,
    
    /**
     * Timestamp of the latest stop
     */
    ENcdReportTimeStop,
    /**
     * Total elapsed time in seconds from the
     * first start to the latest stop
     */
    ENcdReportTimeTotal,    
    
    /**
     * Internal, keep this last
     */
    ENcdReportTimeInternal
    };


enum TNcdReportAttribute
    {
    ENcdReportAttributeReportUri = 0,
    ENcdReportAttributeReportNamespace,
    ENcdReportAttributeGenericId,
    ENcdReportAttributeReportBeingSent,
    ENcdReportAttributeLatestSentReport,
    ENcdReportAttributeIsUsed,
    ENcdReportAttributeSendable,
    // Keep this last
    ENcdReportAttributeInternal
    };

enum TNcdReportBundleMatch
    {
    ENcdReportBundleUriMatch = -1,
    ENcdReportBundleNoMatch,
    ENcdReportBundleMatch
    };

/**
 * Download report status codes
 */
const TInt KNcdDownloadReportSuccess = 1000;
const TInt KNcdDownloadReportPause   = 1001;
const TInt KNcdDownloadReportCancel  = 1002;
const TInt KNcdDownloadReportFail    = 1003;
const TInt KNcdDownloadReportCrash   = 1004;

const TInt KNcdReportDataType = 1;

/**
 * Not supported
 */
const TInt KNcdDownloadReportNotSupported = KErrNotSupported;


/**
 * OMA report bodies
 */
_LIT8( KNcdReportOmaSuccess, "900 Success" );
_LIT8( KNcdReportOmaCancel, "902 User Cancelled" );



#endif // C_NCDREPORTDEFINES_H
