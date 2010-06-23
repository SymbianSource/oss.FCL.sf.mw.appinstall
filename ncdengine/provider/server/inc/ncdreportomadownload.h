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


#ifndef C_NCDREPORTOMADOWNLOAD_H
#define C_NCDREPORTOMADOWNLOAD_H

#include <e32base.h>

#include "ncdreportdefines.h"
#include "ncdreport.h"

class CNcdReportManager;
class MCatalogsHttpOperation;
class CNcdNodeIdentifier;

/**
 * Class for storing download's report information
 */
class CNcdReportOmaDownload : public CNcdReport
    {
public:

    static CNcdReportOmaDownload* NewLC(
        CNcdReportManager& aReportManager,
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
	    const TNcdReportStatusInfo& aStatus, 
	    const TDesC& aReportUri );
        
    static CNcdReportOmaDownload* NewLC( 
        CNcdReportManager& aReportManager,
        RReadStream& aStream );
    
    virtual ~CNcdReportOmaDownload();

public: // From CNcdReport

    
    virtual TBool IsSendable() const;
    
    /**
     * Return true when the report can be removed
     */
    virtual TBool CanBeRemoved() const;
    
    virtual HBufC8* CreateReportL() const;
        
    virtual void UpdateTransactionConfigL( 
        MCatalogsHttpOperation& aTransaction );
    
    virtual TBool StatusIsFinal() const;

protected:

    CNcdReportOmaDownload( 
        const TNcdReportStatusInfo& aStatus, 
        CNcdReportManager& aReportManager );
    
    void ConstructL( 
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
        const TDesC& aReportUri );
    
    TInt ReportStatusToDownloadReportStatus( const TNcdReportStatusInfo& aStatus ) const;

    };
    
    

#endif // C_NCDREPORTOMADOWNLOAD_H    
