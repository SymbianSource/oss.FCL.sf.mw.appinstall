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


#ifndef C_NCDREPORTMANAGER_H
#define C_NCDREPORTMANAGER_H

#include <e32base.h>

#include "ncdreportstatusinfo.h"
#include "ncddownloadreportobserver.h"
#include "ncdinstallreportobserver.h"
#include "catalogsaccesspointobserver.h"
#include "catalogshttpobserver.h"
#include "catalogshttptypes.h"
#include "ncdstoragedataitem.h"
#include "ncdserverreportmanager.h"


class MCatalogsContext;
class CCatalogsNetworkManager;
class CNcdNodeIdentifier;
class CNcdReport;
class MNcdStorageManager;
class MNcdProtocol;
class MCatalogsHttpSession;
class MNcdConfigurationManager;
class MNcdDatabaseStorage;
class MNcdStorageItem;
class MNcdOperationObserver;
class CNcdGeneralManager;

/**
 * Report manager
 *
 * Sends download reports and OMA notifications
 */
class CNcdReportManager : 
    public CActive,
    public MNcdDownloadReportObserver,
    public MNcdInstallReportObserver,
    public MCatalogsAccessPointObserver,
    public MCatalogsHttpObserver,
    public MNcdStorageDataItem
    {
public:

    static void ReportIdToDescriptor( 
        const TNcdReportId& aId,
        TDes& aTarget );

public:
    static CNcdReportManager* NewL(      
        const MCatalogsContext& aContext,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aSession,
        TBool aClientCrashed );
        
    virtual ~CNcdReportManager();


public: // For managing the report sending

    /**
     * Setter for the reporting method.
     *
     * @param aMethod The reporting method that is used when reports are
     * managed.
     *
     *
     */
    void SetReportingMethod( const MNcdServerReportManager::TReportingMethod& aMethod );


    /**
     * Getter for the reporting method.
     *
     * @param TReportingMethod The reporting method that is used 
     * when reports are managed.
     *
     *
     */
    const MNcdServerReportManager::TReportingMethod& ReportingMethod() const;


    /**
     * Setter for the reporting style.
     *
     * @param aMethod The style that is used for reports.
     *
     *
     */
    void SetReportingStyle( const MNcdServerReportManager::TReportingStyle& aStyle );


    /**
     * Getter for the reporting style.
     *
     * @param TReportingMethod The style that is used for reports.
     *
     *
     */
    const MNcdServerReportManager::TReportingStyle& ReportingStyle() const;


    /**
     * Starts the server report sending operation.
     *
     * @note The reference count of the operation object is increased by one. So, Release()
     * function of the operation should be called when operation is not needed anymore.
     *
     * @note The sending process is asynchronous. When sending has completed,
     * the observer callback functions are called.
     *
     * @note After sending is finished the collected reports will be automatically
     * removed.
     *
     * @note This function has an effect only if the reporting method is set to
     * EReportingManaged. If reporting is done in the background, then reports 
     * are sent automatically, and NULL is returned for the operation.
     *
     * @param aObserver Operation observer.
     * @exception Leave System wide error code
     *
     *
     */
    void StartSendReportsL( MNcdOperationObserver& aObserver );



public: // MNcdDownloadReportObserver

    virtual TNcdReportId RegisterDownloadL(
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
    	const TNcdReportStatusInfo& aStatus, 
    	const TDesC& aReportUri,
    	const TDesC& aReportNamespace );
        
    
    virtual TNcdReportId RegisterOmaDownloadL(
        const TDesC& aUri,
        const CNcdNodeIdentifier& aMetadataId,
        const TNcdReportStatusInfo& aStatus,
        const TDesC& aReportUri );    

    virtual TInt SetDownloadReportAccessPoint( 
        const TNcdReportId& aReportId,
        const TCatalogsConnectionMethod& aAccessPoint );



    virtual void ReportDownloadStatusL( 
        const TNcdReportId& aReportId,
        const TNcdReportStatusInfo& aStatus,
        TBool aSendable );


public: // MNcdInstallReportObserver

    virtual TNcdReportId RegisterInstallL(
        const TDesC& aContentIdentifier,
        const CNcdNodeIdentifier& aMetadataId,
    	const TNcdReportStatusInfo& aStatus, 
    	const TDesC& aReportUri,
    	const TDesC& aReportNamespace );
        
    virtual TInt SetInstallReportAccessPoint( 
        const TNcdReportId& aReportId,
        const TCatalogsConnectionMethod& aAccessPoint );

    virtual void ReportInstallStatusL( 
        const TNcdReportId& aReportId,
        const TNcdReportStatusInfo& aStatus );
    
public:

    /**
     * Sets all reports that are related to given metadata as used
     */
    void SetReportsAsUsedL( const CNcdNodeIdentifier& aMetadataId );
    
    /**
     * Removes unused reports after sending them with "user cancelled" code
     */
    void RemoveUnusedReportsL();


public: // For storage handling during cache cleanup

    void OpenStorageL();
    void CloseStorage();
    
    
    void LoadReportsL();

    void SaveReportL( CNcdReport& aReport );
    void RemoveReportL( const TNcdReportId& aId  );
    
    MNcdStorageItem& StorageItemForReportL( 
        const TNcdReportId& aId );
    
        
public: // MCatalogsAccessPointObserver

    virtual void HandleAccessPointEventL( 
        const TCatalogsConnectionMethod& aAp,
        const TCatalogsAccessPointEvent& aEvent );

public: // MCatalogsHttpObserver

    virtual void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );
        
    /**
     * Handles HTTP errors
     */
    virtual TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );


public:

    const MCatalogsContext& Context() const;
    
    void CancelReportSending();


    TCatalogsConnectionMethod DefaultConnectionMethod();

    CNcdGeneralManager& GeneralManager() const;
    
public: // MNcdStorageDataItem

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected: // From CActive

    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError( TInt aError );

private:

        
    /**
     * Checks if the server supports reports or not
     */
    TBool ServerSupportsReports( 
        TNcdReportType aType,
        const TDesC& aServerUri,
        const TDesC& aServerNamespace ) const;

private:

    // Array for reports
    typedef RPointerArray<CNcdReport> RNcdReportArray;

    enum TNcdReportManagerState
        {
        ENcdReportManagerIdle,
        ENcdReportManagerPreparing,
        ENcdReportManagerSending,
        ENcdReportManagerShuttingDown
        };
            
    
private:

    
    CNcdReportManager( 
        const MCatalogsContext& aContext,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aSession,
        TBool aClientCrashed );
        
    void ConstructL();

    CNcdReportManager( const CNcdReportManager& );
    CNcdReportManager& operator=( const CNcdReportManager& );
    
    CNcdReport* FindReport( 
        const TNcdReportId& aReportId,
        RNcdReportArray& aArray ) const;
    

    CNcdReport* FindReport( 
        const TDesC& aId,
        const CNcdNodeIdentifier& aNodeId,
        const TDesC& aReportUri,
        const TNcdReportType& aReportType,
        RNcdReportArray& aArray ) const;


    void SetManagerStateL( TNcdReportManagerState aState );
    
    void SendReportsL();
    
    TBool IsAccessPointOpen( 
        const TCatalogsConnectionMethod& aAp ) const;
    
    void BundleReportsL( 
        TInt aIndex, 
        MCatalogsHttpOperation& aTransaction );

    void UpdateReportAttributesForSendingL( CNcdReport& aReport );

    TNcdReportBundleMatch CanBundleWithL(     
        const CNcdReport& aReport, 
        const CNcdReport& aReport2 ) const;

    void PrepareReportL( 
        CNcdReport& aReport,
        MCatalogsHttpOperation& aTransaction );


    void FinishReportL( MCatalogsHttpOperation& aOperation, TInt aErrorCode );
    

    void ExecuteRunL( TInt aError );
    
    
    TNcdReportId GenerateReportId();
    
    void SetReportStatusL( 
        CNcdReport& aReport,
        const TNcdReportStatusInfo& aStatus,
        TBool aSendable );

    /** 
     * Compares two connection methods and returns ETrue if the connection 
     * methods are compatible for report sending.
     */
    TBool CompareConnectionMethods( 
        const CNcdReport& aFirstReport,
        const CNcdReport& aSecondReport ) const;

    
private:
    
    const MCatalogsContext& iContext;
    CNcdGeneralManager& iGeneralManager;
    MNcdConfigurationManager& iConfigurationManager;
    MNcdStorageManager& iStorageManager;

    MCatalogsHttpSession& iHttpSession;
    
    MNcdServerReportManager::TReportingMethod iReportingMethod;
    MNcdServerReportManager::TReportingStyle iReportingStyle;
        
    // Network manager, not owned
    CCatalogsNetworkManager* iNetworkManager;
    
    // Not owned
    MNcdDatabaseStorage* iDb;
        
    // Currently active reports
    // This array can contain multiple reports for the same URI & metadata
    // combination if a new report is registered when a 
    // failure/cancellation/success has not yet been sent
    RNcdReportArray iReports;
    
    RCatalogsHttpOperationArray iTransactions;
    TNcdReportManagerState iManagerState; 
    TNcdReportId iNewReportId;    
    TBool iClientCrashed;
    
    // Not owned.
    MNcdOperationObserver* iObserver;
    };


#endif // C_NCDREPORTMANAGER_H
