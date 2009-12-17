/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IAUPDATECONTROLLERIMPL_H
#define IAUPDATECONTROLLERIMPL_H


#include <e32base.h>
#include <e32cmn.h>
#include <ncdproviderobserver.h>
#include <ncdclientlocalizer.h>
#include <ncdserverreportoperationobserver.h>
#include <catalogsengineobserver.h>


#include "iaupdatecontroller.h"
#include "iaupdateloaderobserver.h"
#include "iaupdateselfupdaterobserver.h"
#include "iaupdatetimerobserver.h"


class MCatalogsBase;
class MNcdProvider;
class MNcdNode;
class MNcdNodeContainer;
class MNcdServerReportManager;
class MNcdServerReportOperation;
class CCatalogsEngine;
class MIAUpdateNode;
class MIAUpdateAnyNode;
class CIAUpdateNodeContainer;
class CIAUpdateHistory;
class CIAUpdateLoader;
class CIAUpdateSelfUpdaterCtrl;
class CIAUpdateContentOperationManager;
class CIAUpdateCacheCleaner;
class CIAUpdateControllerFile;
class CIAUpdateTimer;


/**
 *
 */
class CIAUpdateController : public CActive, 
                            public MIAUpdateController, 
                            public MIAUpdateLoaderObserver,
                            public MIAUpdateSelfUpdaterObserver,
                            public MCatalogsEngineObserver,
                            public MNcdProviderObserver,
                            public MNcdClientLocalizer,
                            public MNcdServerReportOperationObserver,
                            public MIAUpdateTimerObserver
    {
    
public:

    static CIAUpdateController* NewLC( const TUid& aFamilyUid, 
                                       MIAUpdateControllerObserver& aObserver );
    
    static CIAUpdateController* NewL( const TUid& aFamilyUid, 
                                      MIAUpdateControllerObserver& aObserver );
    
    
    virtual ~CIAUpdateController();


    const TUid& FamilyUid() const;

    MNcdProvider& ProviderL();
    
    CIAUpdateSelfUpdaterCtrl& SelfUpdaterCtrl();
    
    CIAUpdateContentOperationManager& ContentOperationManager();
     
    
public: // MIAUpdateController   
    
    /**
     * @see MIAUpdateController::Startup
     **/
    virtual TInt Startup();
    
    /**
     * @see MIAUpdateController::StartRefreshL
     **/
    virtual void StartRefreshL( TBool aAllowNetConnection );
        
    /**
     * @see MIAUpdateController::CancelRefresh
     **/
    virtual void CancelRefresh();

    /**
     * @see MIAUpdateController::CancelReporting
     **/
    virtual void CancelReporting();
    
    /**
     * @see MIAUpdateController::HistoryL
     */
    virtual MIAUpdateHistory& HistoryL();

    /**
     * @see MIAUpdateController::SetDefaultConnectionMethodL
     */
    virtual void SetDefaultConnectionMethodL( const TIAUpdateConnectionMethod& aMethod );

    /**
     * @see MIAUpdateController::SelfUpdateDataExists
     */
    virtual TBool SelfUpdateDataExists() const;

    /**
     * @see MIAUpdateController::StartPossibleSelfUpdateL
     */ 
    virtual TBool StartPossibleSelfUpdateL( TInt aIndex, 
                                            TInt aTotalCount,
                                            const RPointerArray< MIAUpdateNode >& aPendingNodes,
                                            TBool aSilent );

    /**
     * @see MIAUpdateController::ResetSelfUpdate
     */
    virtual void ResetSelfUpdate();    


    /**
     * @see MIAUpdateController::SelfUpdateRestartInfoL
     */
    virtual CIAUpdateRestartInfo* SelfUpdateRestartInfo();


    /**
     * @see MIAUpdateController::NodeL
     */
    virtual MIAUpdateNode& NodeL( const CIAUpdaterIdentifier& aIdentifier );


    /**
     * @see MIAUpdateController::StartingUpdatesL
     */
    virtual void StartingUpdatesL();


    /**
     * @see MIAUpdateController::FinishedUpdatesL
     */
    virtual void FinishedUpdatesL( TBool aOperationsAllowed, TInt aMaxWaitTime );    


public: // MIAUpdateLoaderObserver
    
    /**
     * @see MIAUpdateLoaderObserver::LoadComplete
     **/
    virtual void LoadComplete( TInt aError );


public: // MIAupdateSelfUpdaterObserver

    /** 
     * @see MIAupdateSelfUpdaterObserver::SelfUpdaterComplete
     */
    virtual void SelfUpdaterComplete( TInt aErrorCode );
    
                                                     
public: // MCatalogsEngineObserver
    
    /**
     * @see MCatalogsEngineObserver::CatalogsEngineShutdown
     */
    virtual void CatalogsEngineShutdown();

    /**
     * @see MCatalogsEngineObserver::CatalogsUpdateNotification
     */
    virtual void CatalogsUpdateNotification( const TDesC& aTarget,
                                             const TDesC& aId,
                                             const TDesC& aVersion,
                                             const TDesC& aUri,
                                             TBool aForce );

    /**
     * @see MCatalogsEngineObserver::CatalogsConnectionEvent
     */
    virtual void CatalogsConnectionEvent( TBool aConnectionActive );
    

public: // MNcdProviderObserver
  
    /**
     * @see MNcdProvider::ForceExpirationInformationReceived
     */
    virtual void ForceExpirationInformationReceived( RCatalogsArray< MNcdNode >& aExpiredNodes );
    
    
public: // MNcdClientLocalizer
    
    /**
     * @see MNcdClientLocalizer::LocalizeString
     *
     * At the moment, this implementation always returns NULL.
     */
    virtual HBufC* LocalizeString( const TDesC& aLocalizationKey );


public: // MNcdServerReportOperationObserver

    /**
     * @see MNcdServerReportOperationObserver::ReportProgress
     */
    virtual void ReportProgress( MNcdServerReportOperation& aOperation,
                                 TNcdProgress aProgress );
        
    /**
     * @see MNcdServerReportOperationObserver::QueryReceived
     */
    virtual void QueryReceived( MNcdServerReportOperation& aOperation,
                                MNcdQuery* aQuery );

    /**
     * @see MNcdServerReportOperationObserver::OperationComplete
     */
    virtual void OperationComplete( MNcdServerReportOperation& aOperation,
                                    TInt aError );


public: // MIAUpdateTimerObserver

    /** 
     * @see MIAUpdateTimeObserver::TimerComplete
     */
    virtual void TimerComplete( TInt aError );


protected: // CActive
    
    /**
     * @see CActive::DoCancel
     */
	virtual void DoCancel();

    /**
     * @see CActive::RunL
     */
	virtual void RunL();

    /**
     * @see CActive::RunError
     */
     virtual TInt RunError( TInt aError );
		
	
private:

    // Prevent these if not implemented
    CIAUpdateController( const CIAUpdateController& aObject );
    CIAUpdateController& operator =( const CIAUpdateController& aObject );

    CIAUpdateController( const TUid& aFamilyUid, MIAUpdateControllerObserver &aObserver );
    
    void ConstructL();

    void LocalLoadL();
    
    void LocalContainerLoadL( MNcdNodeContainer& aContainer );

    void SetupConfigurationL();

    MNcdNode* NodeFromPurchaseHistoryL( const CIAUpdaterIdentifier& aIdentifier );

    void CancelPausedOperationsL();


private: // data

    enum TControllerState
        {
        ENotRunning,
        EStarting,
        EIdle,
        EInClearCache,
        EInLoadOperation,
        EInLocalLoadOperation
        };

    TUid iFamilyUid;
    
    MIAUpdateControllerObserver& iObserver;
                
    TControllerState iControllerState;

    CCatalogsEngine* iEngine;
    
    MCatalogsBase* iBaseProvider;
    MNcdProvider* iProvider;
    MNcdServerReportManager* iServerReportManager;
    
    CIAUpdateSelfUpdaterCtrl* iSelfUpdaterCtrl;
    
    CIAUpdateLoader* iLoader;
    
    CIAUpdateNodeContainer* iNodeContainer;
    
    CIAUpdateHistory* iHistory;

    CIAUpdateContentOperationManager* iContentOperationManager;

    CIAUpdateCacheCleaner* iCacheCleaner;

    CIAUpdateControllerFile* iCacheClearFile;

    // This class does not own the contents of this array.
    RPointerArray< MIAUpdateAnyNode > iNodes;
    
    MNcdServerReportOperation* iReportOperation;
    TBool iCancellingReportOperation;

    CIAUpdateTimer* iReportTimer;
    TBool iReportTimerCompleted;
    
    };


#endif  //  IAUPDATECONTROLLERIMPL_H
