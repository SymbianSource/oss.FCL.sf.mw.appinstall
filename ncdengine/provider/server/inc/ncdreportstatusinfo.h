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


#ifndef T_NCDREPORTSTATUSINFO_H
#define T_NCDREPORTSTATUSINFO_H

#include "ncdreportdefines.h"

/**
 * Simple class that contains report status info.
 * Constructors are provided for initializations.
 *
 *
 */
class TNcdReportStatusInfo
    {    

public:

    /**
     * Constructor that can be used to set default values
     * to the variables.
     */
    TNcdReportStatusInfo():
        iStatus( ENcdReportNone ),
        iErrorCode( KErrNone )
        {
        }

    /**
     * Constructor to create the object directly with the given values.
     */
    TNcdReportStatusInfo( const TNcdReportStatus& aStatus,
                          const TInt& aErrorCode ):
        iStatus( aStatus ),
        iErrorCode( aErrorCode )
        {
        }

    /**
     * Constructor to make a copy of the given object.
     */
    TNcdReportStatusInfo( const TNcdReportStatusInfo& aObject ):
        iStatus( aObject.iStatus ),
        iErrorCode( aObject.iErrorCode )
        {
        }
    
    /**
     * Status value for the report.
     */
    TNcdReportStatus iStatus;
    
    /**
     * Exact error code for the given status.
     */
    TInt iErrorCode;
    
    };

#endif // T_NCDREPORTSTATUSINFO_H
