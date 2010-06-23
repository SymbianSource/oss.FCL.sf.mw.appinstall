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


#ifndef C_NCDREPORTDOWNLOAD_H
#define C_NCDREPORTDOWNLOAD_H

#include <e32base.h>

#include "ncdreportstatusinfo.h"
#include "ncdreport.h"

class CNcdReportManager;
class MCatalogsHttpOperation;
class CNcdNodeIdentifier;

/**
 * Class for storing download's report information
 */
class CNcdReportDownload : public CNcdReport
    {
public:

    static CNcdReportDownload* NewLC(
        CNcdReportManager& aReportManager,
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
	    const TNcdReportStatusInfo& aStatus, 
	    const TDesC& aReportUri,
	    const TDesC& aReportNamespace );
        
    static CNcdReportDownload* NewLC( 
        CNcdReportManager& aReportManager,
        RReadStream& aStream );
    
    virtual ~CNcdReportDownload();

public: // From CNcdReport

    virtual TInt SetStatus( const TNcdReportStatusInfo& aStatus );

    virtual TBool CanBundle() const;    
    
    /**
     * Used to add report data to a bundle
     */
    virtual void AddReportDataL( CNcdRequestInstallation& aRequest ) const;

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

    CNcdReportDownload( 
        const TNcdReportStatusInfo& aStatus, 
        CNcdReportManager& aReportManager );
    
    void ConstructL( 
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
        const TDesC& aReportUri,
        const TDesC& aReportNamespace );
    
    TInt ReportStatusToDownloadReportStatus( const TNcdReportStatusInfo& aStatus ) const;

    void UpdateReportTimeStampsL( const TNcdReportStatusInfo& aStatus );
    
    void UpdateTotalTimeL();

    };
    
    

#endif // C_NCDREPORTDOWNLOAD_H    
