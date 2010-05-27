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
* Description:  
*
*/


#ifndef C_NCDCONTENTDOWNLOADOPERATION_H
#define C_NCDCONTENTDOWNLOADOPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationdatatypes.h"
#include "ncddownloadoperationstates.h"
#include "ncdoperationobserver.h"
#include "ncdstoragedataitem.h"
#include "ncddownloadreportobserver.h"
#include "catalogshttpincludes.h"

class MCatalogsHttpSession;
class CNcdNodeManager;
class MNcdStorageClient;
class CNcdNodeIdentifier;
class MNcdSessionHandler;
class MNcdConfigurationManager;
class MCatalogsContext;
class MNcdOperationRemoveHandler;
class MCatalogsHttpHeaders;
class MNcdDatabaseStorage;
class MCatalogsAccessPointManager;
class CNcdNode;
class CNcdNodeDownload;
class CNcdDownloadSubOperation;
class CNcdDescriptorDownloadSubOperation;
class CNcdContentDescriptor;
class CNcdSendNotificationSubOperation;
class CNcdPurchaseDetails;
class CNcdGeneralManager;

/** 
 * Content download operation
 * 
 */
class CNcdContentDownloadOperation : public CNcdBaseOperation,
    public MNcdStorageDataItem
    {
public:
    
    /**
     * NewL
     *
     * @param aHttpSession HTTP session used for creating download operations
     *
     * @note Preferably the HTTP session has suitable defaults but if not then
     * Config() can be used to retrieve and change the settings for the 
     * download.
     * @note At least the target directory and access point must be set to the 
     * configuration.
     */
    static CNcdContentDownloadOperation* NewL( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        const CNcdNodeIdentifier& aNodeId,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdDownloadReportObserver& aReportObserver,
        MNcdSessionHandler* aSessionHandler,
        MNcdDatabaseStorage& aDownloadStorage,
        MCatalogsSession& aSession,
        TInt aDownloadIndex );

    /**
     * NewL
     *
     * Internalizes the download from the stream
     */     
    static CNcdContentDownloadOperation* NewLC( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdDownloadReportObserver& aReportObserver,
        MNcdDatabaseStorage& aDownloadStorage,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    ~CNcdContentDownloadOperation();
        
    
public: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();    
    
    
    /**
     * @see CNcdBaseOperation::ReceiveMessage()
     */
    void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );    


    /**
     * @see CNcdBaseOperation::CounterPartLost()
     */
    void CounterPartLost( const MCatalogsSession& aSession );

public: // From MNcdStorageDataItem

    /**
     * @see MNcdStorageDataItem::ExternalizeL()
     */
    void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL()
     */
    void InternalizeL( RReadStream& aStream );


public: // New methods

    TBool MatchDownload( const CNcdNodeIdentifier& aId, 
        TNcdDownloadDataType& aType, TInt aIndex ) const;


    /**
     * Node id getter
     *
     * @return Node id of the node used by the operation
     */
    const CNcdNodeIdentifier& NodeId() const;
    
    /**
     * Metadata id getter
     *
     * @return Metadata id of the node used by the operation
     */     
    const CNcdNodeIdentifier& MetadataId() const;
    
    /**     
     */
    TBool IsOk() const;
    
    TInt CurrentDownload() const;
    

protected: // From MNcdOperationObserver

    /**
     * @see MNcdOperationObserver::Progress()
     */
    void Progress( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationObserver::QueryReceived()
     */
    void QueryReceived( CNcdBaseOperation& aOperation,
                                CNcdQuery* aQuery );

    /**
     * @see MNcdOperationObserver::OperationComplete()
     */
    void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError );


protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    

    /**
     * @see CNcdBaseOperation::Initialize()
     */    
    TInt Initialize();
    

protected:
   

    // Constructor
    CNcdContentDownloadOperation( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        CNcdGeneralManager& aGeneralManager, 
        MCatalogsHttpSession& aHttpSession, 
        MNcdDownloadReportObserver& aReportObserver,
        MNcdSessionHandler* aSessionHandler,
        MCatalogsSession& aSession,
        MNcdDatabaseStorage& aDownloadStorage );
        
    void ConstructL( const CNcdNodeIdentifier& aNodeId, 
        TInt aDownloadIndex );

    void ConstructL();

private: // enumerations

    enum TContentDownloadState
        {
        ENoDownload,
        ERightsDownload,
        EEmbeddedDescriptor,        
        EDescriptorDownload,
        EContentDownload,          
        EDownloadDone    
        };
        
    enum TDescriptorType
        {
        EDescriptorUnknown,
        EDescriptorDd,
        EDescriptorJad
        };
    
private:

    // Does the actual operation initialization
    void DoInitializationL();

    void FinishDescriptorDownloadL();

    /** Initializes the download in the given index (or in the next
     *  index that needs to be downloaded)
     *  @throw KNoDownloads if there are no downloads that need to be 
     *  downloaded
     */
    void InitializeDownloadL( TInt aIndex );
    
    void StartDownloadL();

    TBool FinishDownloadL();

    void UpdatePurchaseHistoryL( 
        const TDesC& aDownloadedFile );

    // Updates skipped downloads to purchase history
    void UpdateSkippedDownloadToPurchaseHistoryL( TInt aIndex );


    // Update the download request headers
    void UpdateHeadersL( MCatalogsHttpHeaders& aHeaders );
    
    
    void ReleaseDownload( CNcdBaseOperation* aOperation );


    void HandleDescriptorL( 
        const TDesC& aDescriptorType, const TDesC8& aDescriptor );

    // Checks if the download is a dependency/upgrade and whether
    // it is currently installed or not
    TBool IsFileInstalledL( TInt aIndex, TBool aCheckOnly );
    
    TBool SkipInstalledFilesL();
    
    TInt CalculateMissingFilesL();
    
    void SaveStateL();
    
    void GenerateStorageUidL();
    
    void RemoveTempInfoL();
    
    TDescriptorType MatchDescriptor( const TDesC& aMimeType ) const;
    
    CNcdPurchaseDetails* GetPurchaseDetailsLC();
    
    void UpdateAccessPointsL( 
        const CNcdNodeIdentifier& aNodeId );

    void ReportStatusL( 
        const TNcdReportStatusInfo& aStatus,
        TBool aSendable = ETrue );
    
    void SendOmaNotificationL( const TNcdReportStatusInfo& aStatus );
    
    // Updates dependencies from CNcdNodeDependency to purchase history
    // so that they are up-to-date
    void UpdateDependenciesL();
        
    void GetPausableStateL( MCatalogsBaseMessage& aMessage );

    // Registers a download to report manager for download reporting
    void RegisterDownloadL(
        const TDesC& aUri,
        const CNcdNodeIdentifier& aIdentifier );
    
private:

    MCatalogsHttpSession& iHttpSession;
    MNcdDownloadReportObserver& iReportObserver;
    MNcdSessionHandler* iSessionHandler;
    MNcdConfigurationManager& iConfigurationManager;
    MCatalogsAccessPointManager& iAccessPointManager;
    MNcdDatabaseStorage& iStorage;
    CNcdNode* iNode;
    
    const MCatalogsContext& iContext;
    
    TNcdDownloadState iDownloadState;    
    HBufC* iSessionId;
    

    CNcdDownloadSubOperation* iDownload;
    CNcdDescriptorDownloadSubOperation* iDescriptorDownload;
    
    CNcdNodeDownload* iNodeDownload;
    
    // Access point used for content downloads
    TCatalogsConnectionMethod iApId;
    TCatalogsConnectionMethod iReportAp;
    
    TInt iDownloadIndex;   
    
    HBufC* iMimeType;
    TBool iMimeUpdated;
    
    TBool iStartNextFile;
    TContentDownloadState iContentDownloadState;
    HBufC8* iDescriptor;
    TBool iDeleting;
    CNcdContentDescriptor* iContentDescriptor;
    
    TPtrC iContentUri;    
    TPtrC iContentMime;
    HBufC* iContentFilename;
    TPtrC iNotificationUri;
   
    TDescriptorType iDownloadType;
    
    // ETrue if the operation was created successfully
    // This is used to circumvent the cumbersome 
    // storage error handling
    TBool iIsOk; 
    TBool iUnhandledEvent;
    HBufC* iStorageUid;
    TNcdReportId iReportId;
    TBool iDependenciesUpdated;
    TInt iTotalFileCount;
    TInt iCurrentFile;
    };

#endif // C_NCDCONTENTDOWNLOADOPERATION_H
