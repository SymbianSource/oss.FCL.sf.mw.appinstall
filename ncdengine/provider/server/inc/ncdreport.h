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


#ifndef C_NCDREPORT_H
#define C_NCDREPORT_H

#include <e32base.h>

#include "catalogshttptypes.h"
#include "ncdreportstatusinfo.h"
#include "ncdattributes.h"
#include "ncdstoragedataitem.h"
#include "catalogsconnectionmethod.h"

class MCatalogsHttpOperation;
class CNcdNodeIdentifier;
class CNcdReportManager;
class CNcdRequestInstallation;
class CCatalogsAccessPoint;

class CNcdReport : public CBase, 
                   public MNcdStorageDataItem
    {
public:

    virtual ~CNcdReport();

public: // From MNcdStorageDataItem

    virtual void ExternalizeL( RWriteStream& aStream );
    virtual void InternalizeL( RReadStream& aStream );

public: // Common methods

    /**
     * @return TNcdReportType Report type getter.
     */
    const TNcdReportType& ReportType() const;

    /**
     * Access point setter
     */
    virtual void SetAccessPoint( const TCatalogsConnectionMethod& aAp );
    
    /**
     * Access point getter
     *
     */
    virtual const TCatalogsConnectionMethod& ConnectionMethod() const;


    /**
     * Sets the id for the report
     *
     */
    virtual void SetReportId( const TNcdReportId& aReportId );
    
    
    /**
     * Report id getter
     */
    virtual const TNcdReportId& ReportId() const;
    

    /** 
     * Attribute getter
     */
    CNcdAttributes& Attributes();
    const CNcdAttributes& Attributes() const;
               

    /**
     * Status getter
     */        
    virtual const TNcdReportStatusInfo& Status() const;
    
    /**
     * @param aStatus New status
     * @return Symbian error code. Default implementation always returns 
     * KErrNone
     */
    virtual TInt SetStatus( const TNcdReportStatusInfo& aStatus );


    /** 
     * Sets a timestimp
     */
    virtual void SetTimeStamp( 
        const TNcdReportTimeStamps& aTimeStamp,
        const TTime& aTime );
    
    /**    
     * Timestamp getter
     */
    virtual const TInt64& TimeStamp( 
        const TNcdReportTimeStamps& aTimeStamp ) const;

    /**
     * Reset time stamps
     */
    virtual void ResetTimeStampsL();
    
    
    /**
     * Current report transaction getter
     *
     * @return Current transaction used for sending the report or NULL
     */
    virtual const MCatalogsHttpOperation* ReportTransaction() const;
    
    
    /**
     * Report transaction setter
     */
    virtual void SetReportTransaction(         
        MCatalogsHttpOperation* aTransaction );
    
    
    /**
     * Metadata id 
     */    
    virtual const CNcdNodeIdentifier& MetadataId() const; 


    /**
     * Compares this report to given parameters and returns true
     * if they match
     */
    virtual TBool Match( 
        const CNcdNodeIdentifier& aMetadataId,
        const TDesC& aGenericId,
        const TDesC& aReportUri,
        const TNcdReportType& aReportType ) const;
    
    /**
     * Compares two reports
     */
    static TInt Compare( 
        const CNcdReport& aFirst,
        const CNcdReport& aSecond );
    

    /**
     * Sets current time to the timestamp pointed by aTime
     */
    void SetCurrentTime( const TNcdReportTimeStamps& aTime );
    
    
    /**
     * Calculates the time between start and end timestamps. The result
     * is in seconds
     */
    TInt DurationInSecondsL( 
        const TNcdReportTimeStamps& aStart,
        const TNcdReportTimeStamps& aEnd ) const;
    

    /**
     * Reports should return ETrue if they can be bundled with other 
     * reports. Default implementation returns EFalse
     */
    virtual TBool CanBundle() const;    
    
    /** 
     * Used to add report data to a bundle
     * Default implementation leaves with KErrNotSupported
     */
    virtual void AddReportDataL( CNcdRequestInstallation& aRequest ) const;

public: // Must be implemented by child classes

    virtual TBool IsSendable() const = 0;
    
    /**
     * Return true when the report can be removed
     */
    virtual TBool CanBeRemoved() const = 0;
    
    virtual HBufC8* CreateReportL() const = 0;
    
    virtual void UpdateTransactionConfigL( 
        MCatalogsHttpOperation& aTransaction ) = 0;
    
    /**
     * @return ETrue if the status cannot change anymore
     */
    virtual TBool StatusIsFinal() const = 0;

protected:    

    CNcdReport( 
        TNcdReportType aType, 
        CNcdReportManager& aReportManager,
        const TNcdReportStatusInfo& aStatus );
        
    void BaseConstructL( const CNcdNodeIdentifier& aMetadataId );

    const CNcdReportManager& ReportManager() const;
    
    CNcdReportManager& ReportManager();


private:

    CNcdReport( const CNcdReport& );
    CNcdReport& operator=( const CNcdReport& );    
    

private:

    TNcdReportType iType;
    CNcdReportManager& iReportManager;    
    TNcdReportStatusInfo iStatus;	
    
    TCatalogsConnectionMethod iAccessPoint;
    RArray<TInt64> iTimeStamps;
    MCatalogsHttpOperation* iReportTransaction;
    CNcdAttributes* iAttributes; 
    CNcdNodeIdentifier* iMetadataId;   
    TNcdReportId iReportId;
    };

#endif // C_NCDREPORT_H
