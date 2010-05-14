/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of 
*                CIAUpdateController class member functions.
*
*/



// For the NCD Engine ECOM session closing.
#include <ecom/ecom.h>

#include <catalogsuids.h>
#include <catalogsutils.h>
#include <catalogsengine.h>
#include <ncdprovider.h>
#include <ncdutils.h>
#include <ncdconfigurationkeys.h>
#include <ncdcapabilities.h>
#include <ncdprogress.h>
#include <ncdnode.h>
#include <ncdnodecontainer.h>
#include <ncdconnectionmethod.h>
#include <ncdserverreportoperation.h>
#include <ncdserverreportmanager.h>
#include <ncdquery.h>
#include <ncdpurchasehistory.h>
#include <ncdutils.h>
#include <ncdprovideroptions.h>
#include <ncdoperation.h>
#include <ncddownloadoperation.h>
#include <ncderrors.h>

// Required for NCD debug logging.
#include <catalogsdebug.h>

#include "iaupdatecontrollerimpl.h"
#include "iaupdateloader.h"
#include "iaupdatenodecontainer.h"
#include "iaupdatenodefactory.h"
#include "iaupdatenodeimpl.h"
#include "iaupdatefwnodeimpl.h"
#include "iaupdateutils.h"
#include "iaupdatehistoryimpl.h"
#include "iaupdateenginexmlparser.h"
#include "iaupdateengineconfigdata.h"
#include "iaupdatectrlconsts.h"
#include "iaupdateprotocolconsts.h"
#include "iaupdatectrlfileconsts.h"
#include "iaupdateselfupdaterctrl.h"
#include "iaupdatecontentoperationmanager.h"
#include "iaupdatecachecleaner.h"
#include "iaupdatecontrollerfile.h"
#include "iaupdateridentifier.h"
#include "iaupdateerrorcodes.h"
#include "iaupdatetimer.h"
#include "iaupdatedebug.h"


// -----------------------------------------------------------------------------
// CIAUpdateController::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CIAUpdateController* CIAUpdateController::NewLC( 
    const TUid& aFamilyUid, 
    MIAUpdateControllerObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NewLC() begin");
    CIAUpdateController* self = 
        new( ELeave ) CIAUpdateController( aFamilyUid, aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NewLC end");
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//    
CIAUpdateController* CIAUpdateController::NewL( 
    const TUid& aFamilyUid, 
    MIAUpdateControllerObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NewL() begin");
    CIAUpdateController* self = 
        CIAUpdateController::NewLC( aFamilyUid, aObserver );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NewL end");
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::CIAUpdateController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateController::CIAUpdateController( 
    const TUid& aFamilyUid,
    MIAUpdateControllerObserver& aObserver ) 
: CActive( CActive::EPriorityStandard ),
  iFamilyUid( aFamilyUid ),
  iObserver( aObserver ),
  iControllerState( ENotRunning )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CIAUpdateController() begin");

    // Required for NCD debug logging.
    DLINIT;

    CActiveScheduler::Add( this );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CIAUpdateController() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateController::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ConstructL() begin");
    
    iEngine = CCatalogsEngine::NewL( *this );

    if ( !iEngine )
        {
        IAUPDATE_TRACE("[IAUPDATE] !iEngine");
        User::Leave( KErrNotFound );
        }
    
    iNodeContainer = CIAUpdateNodeContainer::NewL( *this );
    
    iSelfUpdaterCtrl = CIAUpdateSelfUpdaterCtrl::NewL( *this );
    iContentOperationManager = CIAUpdateContentOperationManager::NewL();

    // Notice, that this will read the data from the file 
    // and adjust variables from the file if the file exists.
    iCacheClearFile =
        CIAUpdateControllerFile::NewL( 
            IAUpdateCtrlFileConsts::KCacheClearFile );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ConstructL() end");
    }    
   
    
// -----------------------------------------------------------------------------
// CIAUpdateController::~CIAUpdateController
// Destructor
// -----------------------------------------------------------------------------
//    
CIAUpdateController::~CIAUpdateController()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::~CIAUpdateController() begin");

    // This is always a good thing to do with active objects.
    Cancel();
    
    // Also cancel possible report operation.
    CancelReporting();

    delete iReportTimer;
    delete iContentOperationManager;
    delete iLoader;    
    delete iNodeContainer;
    delete iCacheCleaner;
    delete iCacheClearFile;
        
    // Before releasing engine, be sure to release or
    // delete all other objects that may contain connetions
    // to the engine.
    
    // History uses the services that are provided throught the provider.
    // So, delete history here before releasing the provider and closing
    // the engine.
    delete iHistory;
    
    // Do not delete the contents here.
    // This array does not own the content.
    iNodes.Reset();

    delete iSelfUpdaterCtrl;

    if ( iServerReportManager )
        {
        iServerReportManager->Release();
        }

    if ( iProvider )
        {
        iProvider->Release();
        }

    if ( iBaseProvider )
        {
        iBaseProvider->Release();
        }
         
    if ( iEngine )
        {
        iEngine->Close();
        delete iEngine;
        }    

    // Make sure that the NCD Engine ECOM session is closed. 
    // SKD help describes this:
    // Direct users of ECOM plugins must call this method when all 
    // implementations they have created have been destroyed and they 
    // are finished using ECOM e.g. library shutdown. It will garbage 
    // collect the last previously destroyed implementation and close 
    // the REComSession if no longer in use.
    REComSession::FinalClose();

    // Required for NCD debug logging.
    DLUNINIT;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::~CIAUpdateController() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::FamilyUid
//
// -----------------------------------------------------------------------------
//    
const TUid& CIAUpdateController::FamilyUid() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::FamilyUid()");
    IAUPDATE_TRACE_1("[IAUPDATE] family uid: %x", iFamilyUid.iUid );
    return iFamilyUid;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::ProviderL
// 
// -----------------------------------------------------------------------------
//
MNcdProvider& CIAUpdateController::ProviderL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ProviderL() begin");

    if ( !iProvider )
        {
        User::Leave( KErrNotFound );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ProviderL() end");

    return *iProvider;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SelfUpdaterCtrl
// 
// -----------------------------------------------------------------------------
//
CIAUpdateSelfUpdaterCtrl& CIAUpdateController::SelfUpdaterCtrl()
    {
    return *iSelfUpdaterCtrl;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::ContentOperationManager
// 
// -----------------------------------------------------------------------------
//
CIAUpdateContentOperationManager& CIAUpdateController::ContentOperationManager()
    {
    return *iContentOperationManager;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::Startup
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateController::Startup()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::Startup() begin");

    if ( iControllerState == EStarting )
        {
        IAUPDATE_TRACE("[IAUPDATE] Error code: KErrInUse");        
        return KErrInUse;
        }
    else if ( iControllerState != ENotRunning )
        {
        IAUPDATE_TRACE("[IAUPDATE] Error code: KErrAlreadyExists");
        return KErrAlreadyExists;
        }
    
    // Turn off the cache cleaner because we do not want it to remove any items
    // even if allowed db size is exceeded.
    // Make sure that IMEI is send in server requests.
    // Disable HEAD requests for optimization. With SISX content this is 
    // safe to do.
    TUint32 providerOptions( ENcdProviderDisableNodeCacheCleaner
                             | ENcdProviderSendImei
                             | ENcdProviderDisableHttpHeadRequest );
    
    TInt connectErr = iEngine->Connect( FamilyUid() );
    if ( connectErr != KErrNone )
        {
        return connectErr;
        }
    
    TRAPD ( err, 
            iEngine->CreateProviderL( KNcdProviderUid, 
                                      iBaseProvider, 
                                      iStatus, 
                                      providerOptions ) );
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", err );        
    if ( err != KErrNone )
        {
        return err;
        }
    
    SetActive();

    iControllerState = EStarting;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::Startup() end");

    return KErrNone;
    }
 
    
// -----------------------------------------------------------------------------
// CIAUpdateController::StartRefreshL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::StartRefreshL( TBool aAllowNetConnection )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartRefreshL() begin");
    IAUPDATE_TRACE_2("[IAUPDATE] iControllerState %d allow net connection %d", iControllerState, aAllowNetConnection );

    if ( iControllerState == ENotRunning )
        {
        User::Leave( KErrNotReady );
        }
    else if ( iControllerState != EIdle )
        {
        User::Leave( KErrInUse );
        }

    if ( !iLoader )
        {
        IAUPDATE_TRACE("[IAUPDATE] Create loader");
        iLoader = CIAUpdateLoader::NewL( *iProvider, *this );
        // For optimization reasons, skip child count refreshes.
        iLoader->SetSkipChildCountRefresh( ETrue );       
        }

    // Reset the node list. New one will be created when this completes. 
    // Notice, that the array content is not owned by this array.
    iNodeContainer->Clear();
    iNodes.Reset();
        
    if ( aAllowNetConnection )
        {
        IAUPDATE_TRACE("[IAUPDATE] Net connection allowed");

        if ( iLoader->RootExpiredL() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Root has expired");

            // Notice, that we will clean the cache only if the
            // root load is required. Otherwise, if cache was cleaned,
            // the root would be reloaded also. Then, unwanted
            // CDB connections would occur.
            
            // Update the data from the file just in case.
            // Actually the data was already read when the object
            // was created if the file exists. But, this way we
            // can be sure that the data matches the file even if
            // in some situation the write operation has failed
            // and the object data could not be synchronized into
            // the file.
            iCacheClearFile->ReadControllerDataL();

            TLanguage currentLanguage = User::Language();
            TLanguage lastTimeLanguage = iCacheClearFile->Language();
            TTime lastClearTime( iCacheClearFile->RefreshTime() );
            const TTimeIntervalDays KCacheClearInterval( 
                IAUpdateCtrlConsts::KCacheClearIntervalDays );
            TTime expireTime( lastClearTime + KCacheClearInterval );
            TTime universalTime;
            universalTime.UniversalTime();

            if ( currentLanguage != lastTimeLanguage
                 || expireTime < universalTime
                 || lastClearTime > universalTime  )
                {
                IAUPDATE_TRACE("[IAUPDATE] Clear cache");
                // Database is expired because languages do not match
                // or current time has passed the expiration time. 
                // Also, sanity check is made. If last refresh time is larger 
                // than current time, then the last refresh value has been set wrong, 
                // and the database can be thought as expired. This might be the case 
                // if the user has changed the time in the phone.

                // Before starting to load data from the net,
                // clear an old cache. This way files related to unfinished
                // updates will be deleted and there is no danger of them becoming
                // hanging data that can not be handled when old nodes are removed.
                if ( !iCacheCleaner )
                    {
                    iCacheCleaner = CIAUpdateCacheCleaner::NewL( *iProvider );
                    }
                iCacheCleaner->ClearL( iStatus );
                SetActive();
                iControllerState = EInClearCache;
                }
            }

        if ( iControllerState == EIdle )
            {
            IAUPDATE_TRACE("[IAUPDATE] Refresh directly from net");
            // Because controller state is still idle, we did not start
            // cache cleaning above.
            // There is no need to clean the cache here. 
            // So, skip that step and start loading directly.     
            // This starts an asynchronous operation which will end
            // when callback is called.
            iLoader->LoadNodesL();
            iControllerState = EInLoadOperation;
            }
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Start local refresh");
        // Load should be done locally.
        // So, just delegate the thing to the RunL which will handle
        // the local getting of the data in a next asynchronous step.
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* ptrStatus = &iStatus;
        User::RequestComplete( ptrStatus, KErrNone );
        iControllerState = EInLocalLoadOperation;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartRefreshL() end");    
    }

        
// -----------------------------------------------------------------------------
// CIAUpdateController::CancelRefresh
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CancelRefresh()
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateController::CancelRefresh() begin: %d",
                     iControllerState);

    switch ( iControllerState )
        {
        case EInClearCache:
            // Cache cleaner is responsible of this operation.
            iCacheCleaner->Cancel();
            break;

        case EInLoadOperation:
            // Loader is responsible of this operation.
            iLoader->Cancel();
            break;

        case EInLocalLoadOperation:
            // Use the normal cancellation of this active class because 
            // these operations are handled by this active object itself.
            Cancel();
            break;

        default:
            break;
        }

    // New state should be idle.
    iControllerState = EIdle;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CancelRefresh() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::CancelReporting
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CancelReporting()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CancelReport() begin");

    // Because report operation is cancelled, no need for the timer
    // anymore. So, delete timer if it exists. Deletion will automatically
    // cancel the timer operation. The timer may be started for some special
    // cases even if report operation was not created. So, try to delete it
    // here just in case.
    delete iReportTimer;
    iReportTimer = NULL;

    if ( iReportOperation )
        {
        // Set this for the callback because operation call operation complete
        // when the operation is cancelled. We do not want to call ui callback there.
        iCancellingReportOperation = ETrue;

        iReportOperation->CancelOperation();

        // Notice, that the operation complete will release the operation and set the
        // operation pointer to NULL. Do not do it here.

        // Set the cancelling flag to EFalse because cancell operation is finished.
        iCancellingReportOperation = EFalse;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CancelReport() end");
    }
    
    
// -----------------------------------------------------------------------------
// CIAUpdateController::HistoryL
//
// -----------------------------------------------------------------------------
//    
MIAUpdateHistory& CIAUpdateController::HistoryL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::HistoryL() begin");

    if ( !iHistory )
        {
        iHistory = 
            CIAUpdateHistory::NewL( FamilyUid(), ProviderL() );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::HistoryL() end");

    return *iHistory;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SetDefaultConnectionMethodL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::SetDefaultConnectionMethodL( 
    const TIAUpdateConnectionMethod& aMethod )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::SetDefaultAccessPointL begin");
    IAUPDATE_TRACE_1("[IAUPDATE] access point: %d", aMethod.iId );

    if ( !iProvider )
        {
        User::Leave( KErrNotFound );
        }

    TNcdConnectionMethodType type( ENcdConnectionMethodTypeAlwaysAsk );
    switch ( aMethod.iType )
        {
        case TIAUpdateConnectionMethod::EConnectionMethodTypeAlwaysAsk:
            type = ENcdConnectionMethodTypeAlwaysAsk;
            break;

        case TIAUpdateConnectionMethod::EConnectionMethodTypeDestination:
            type = ENcdConnectionMethodTypeDestination;
            break;

        case TIAUpdateConnectionMethod::EConnectionMethodTypeAccessPoint:
            type = ENcdConnectionMethodTypeAccessPoint;
            break;

        case TIAUpdateConnectionMethod::EConnectionMethodTypeDefault:
            type = ENcdConnectionMethodTypeDefault;
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }
        
    TNcdConnectionMethod method( aMethod.iId, type );
    iProvider->SetDefaultConnectionMethodL( method );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::SetDefaultAccessPointL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SelfUpdateDataExists
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateController::SelfUpdateDataExists() const
    {
    return iSelfUpdaterCtrl->DataExists();
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::StartPossibleSelfUpdateL
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateController::StartPossibleSelfUpdateL( 
    TInt aIndex, 
    TInt aTotalCount,
    const RPointerArray< MIAUpdateNode >& aPendingNodes,
    TBool aSilent )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartPossibleSelfUpdateL");
    return SelfUpdaterCtrl().StartL( aIndex, aTotalCount, aPendingNodes, aSilent );
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::ResetSelfUpdate
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::ResetSelfUpdate()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ResetSelfUpdate() begin");

    SelfUpdaterCtrl().Reset();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ResetSelfUpdate() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SelfUpdateRestartInfo
//
// -----------------------------------------------------------------------------
//
CIAUpdateRestartInfo* CIAUpdateController::SelfUpdateRestartInfo()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::SelfUpdateRestartInfo");
    return SelfUpdaterCtrl().SelfUpdateRestartInfo();    
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::NodeL
//
// -----------------------------------------------------------------------------
//
MIAUpdateNode& CIAUpdateController::NodeL( 
    const CIAUpdaterIdentifier& aIdentifier )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeL begin");

    CIAUpdateNode* node( NULL );

    for ( TInt i = 0; i < iNodeContainer->AllNodes().Count(); ++i )
        {
        CIAUpdateNode* tmpNode( iNodeContainer->AllNodes()[ i ] );
        if ( tmpNode->MetaNamespace() == aIdentifier.Namespace()
             && tmpNode->MetaId() == aIdentifier.Id() ) 
            {
            node = tmpNode;
            break;
            }
        }

    if ( !node )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeL node was not in the list");
        for ( TInt i = 0; i < iNodeContainer->ExcessNodes().Count(); ++i )
            {
            CIAUpdateNode* tmpNode( iNodeContainer->ExcessNodes()[ i ] );
            if ( tmpNode->MetaNamespace() == aIdentifier.Namespace()
                 && tmpNode->MetaId() == aIdentifier.Id() ) 
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeL excess node found");
                node = tmpNode;
                break;
                }
            }        
        }

    if ( node == NULL )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeL create excess node");
        MNcdNode* ncdNode( NodeFromPurchaseHistoryL( aIdentifier ) );
        node = IAUpdateNodeFactory::CreateNodeLC( ncdNode, *this );
        iNodeContainer->AddExcessNodeL( node );
        CleanupStack::Pop( node );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeL end");

    return *node;        
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::StartingUpdatesL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::StartingUpdatesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartingUpdatesL begin");

    if ( !iServerReportManager )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartingUpdatesL not found");
        User::Leave( KErrNotFound );
        }    

    // Start collecting server reports because updates are going to be downloaded
    // and installed.
    // Set the reporting method to the manager method instead of letting NCD Engine
    // do sending automatically. 
    // After we have manually sent the reports, we may change the method back to 
    // automatic and let the NCD Engine to do its thing as it seems best.
    // The reason why to set the reporting method here when the updates start is that
    // if for some reason the application was not able to send all the reports last time,
    // in background mode they will be sent automatically when there is process time
    // for that, for example before starting to do new updates.
    iServerReportManager->SetReportingMethodL( MNcdServerReportManager::EReportingManaged );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::StartingUpdatesL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::FinishedUpdatesL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::FinishedUpdatesL( 
    TBool aOperationsAllowed, TInt aMaxWaitTime )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::FinishedUpdatesL begin");

    if ( !iProvider )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: Provider not found");
        User::Leave( KErrNotFound );
        }

    // Reset the timer related flag. 
    iReportTimerCompleted = EFalse;
        
    if ( aOperationsAllowed ) 
        {
        IAUPDATE_TRACE("[IAUPDATE] Operations allowed");
        if ( !iReportOperation )
            {
            IAUPDATE_TRACE("[IAUPDATE] Create operation");
            // Send the reports that have been collected.
            // Notice, that the created operation needs to be released
            // at some point.
            MNcdServerReportOperation* tmpOperation(
                iServerReportManager->SendL( *this ) );
            if ( tmpOperation )
                {
                IAUPDATE_TRACE("[IAUPDATE] Start operation");

                // Make sure that if the starting of the operation
                // leaves, then the operation will be released correctly.
                CleanupReleasePushL( *tmpOperation );

                // Start the operation
                tmpOperation->StartOperationL();

                CleanupStack::Pop( tmpOperation );

                // Because operation was successfully started,
                // it can now be inserted to memeber variable.
                iReportOperation = tmpOperation;

                // Notice, that we do not Release the operation here. 
                // We release it when the operation has finished and
                // the callback is called.
                }
            }
        }

    if ( !iReportOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Operation was not created");
        // Set aMaxWaitTime as one. Then, we will get the timer
        // to do only one quick asynchronous loop and callback
        // is called after that. This way we make sure that
        // callback is always called even if operation is not
        // created here.
        aMaxWaitTime = 0;
        }

    // Create timer if needed.
    if ( aMaxWaitTime >= 0 )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Max wait time: %d", aMaxWaitTime);
        if ( !iReportTimer )
            {
            IAUPDATE_TRACE("[IAUPDATE] Create timer");
            // Timer does not exist yet. So, create new one.
            iReportTimer = CIAUpdateTimer::NewL( *this );
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Cancel timer");
            // Timer already exists.
            // Cancel possible already ongoing operation
            // before starting the new one.
            iReportTimer->Cancel();
            }
        IAUPDATE_TRACE("[IAUPDATE] Start timer");
        iReportTimer->After( aMaxWaitTime );
        }

    // Because operation has now been started there is nothing to manage anymore.
    // So, set the NCD Engine to automatic mode. Then, if there is something to do later
    // in the background, the engine can do it when it suits it best.
    iServerReportManager->SetReportingMethodL( MNcdServerReportManager::EReportingBackground );
       
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::FinishedUpdatesL end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::LoadComplete
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::LoadComplete( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LoadComplete begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );

    // Because node hierarchy is now concidered up-to-date,
    // get all the required nodes from the local cache to the
    // head node list that will be given to the observer.

    TRAPD ( trapError, LocalLoadL() );
    if ( trapError != KErrNone )
        {
        aError = trapError;        
        }

    // Update the state, now that everything has been done.
    iControllerState = EIdle;

    // Inform, the observer.            
    iObserver.RefreshComplete( iNodes, aError );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LoadComplete end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SelfUpdaterComplete
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::SelfUpdaterComplete( TInt aErrorCode )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateController::SelfUpdaterComplete() error code: %d", aErrorCode );
    iObserver.SelfUpdaterComplete( aErrorCode );
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CatalogsEngineShutdown()
    {
    // IAD handles self updates in its own updater.
    // Therefore, self updates are not started by NCD Engine itself.
    // So, this callback function should be never called.  
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::CatalogsUpdateNotification
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CatalogsUpdateNotification( 
    const TDesC& /*aTarget*/, 
    const TDesC& /*aId*/,
    const TDesC& /*aVersion*/,
    const TDesC& /*aUri*/,
    TBool /*aForce*/ )
    {
    // Called when a Catalogs OTA update is available.
    // IAD handles self updates in its own updater.    
    // This callback function should be never called.
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::CatalogsConnectionEvent
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CatalogsConnectionEvent( 
    TBool /*aConnectionActive*/ )
    {
    // This callback function is called when data is transferred in the
    // network connections. IAD does not use this information for now.
    }    


// -----------------------------------------------------------------------------
// CIAUpdateController::ForceExpirationInformationReceived
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::ForceExpirationInformationReceived( 
    RCatalogsArray< MNcdNode >& /*aExpiredNodes*/ )
    {
    // IAD UI is not forced to be updated by the server side.
    // It is up to the UI to decide when to update its content.
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::LocalizeString
//
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateController::LocalizeString( 
    const TDesC& /*aLocalizationKey*/ )
    {
    return NULL;
    }


// ---------------------------------------------------------------------------
// CIAUpdateController::ReportProgress
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateController::ReportProgress( 
    MNcdServerReportOperation& /*aOperation*/, 
    TNcdProgress /*aProgress*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::ReportProgress()");
    }


// ---------------------------------------------------------------------------
// CIAUpdateController::QueryReceived
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateController::QueryReceived( 
    MNcdServerReportOperation& aOperation, 
    MNcdQuery* aQuery )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::QueryReceived() begin");

    // Install query received.
    // Always accept queries.
    // Queries should not be requested from this client.
    TInt trapError( KErrNone );
    if ( aQuery )
        {
        TRAP ( trapError, 
               aQuery->SetResponseL( MNcdQuery::EAccepted );
               aOperation.CompleteQueryL( *aQuery ); );
        // Release needs to be called to the query after it is not used.
        aQuery->Release();        
        }

    if ( ( trapError != KErrNone ) || ( !aQuery ) )
        {
        // Error occurred when query was handled.
        // So, operation can not continue.
        // Cancel operation. Notice, that OperationComplete will be called
        // by the operation when cancel is called.
        aOperation.CancelOperation();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::QueryReceived() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateController::OperationComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateController::OperationComplete( 
    MNcdServerReportOperation& aOperation, 
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::OperationComplete() begin");

    // Report operation has completed.
    
    if ( &aOperation == iReportOperation )
        {
        IAUPDATE_TRACE("[IAUPDATE] Acceptable server report operation");

        // We should always come here
        // because only one operation at a time is going on.

        // Release the operation because we do not need it anymore
        // and because operation reference count was increased when
        // it was created for this class object.        
        iReportOperation->Release();
        iReportOperation = NULL;

        // By checking if the timer has completed its job, we know if
        // the observer should be informed about the completion of the
        // operation. This way we will avoid duplicate callbacks, for example,
        // after timer completion has called callback and the report operation
        // completes after that.
        if ( !iReportTimerCompleted )
            {
            IAUPDATE_TRACE("[IAUPDATE] Timer has not informed observer yet");

            // Because operation is completed, no need for the timer anymore.
            // So, delete timer if it exists. Deletion will automatically
            // cancel the timer operation.
            delete iReportTimer;
            iReportTimer = NULL;

            // Do not call callback function if cancel was started by user. 
            if ( !iCancellingReportOperation )
                {
                IAUPDATE_TRACE("[IAUPDATE] Inform observer.");
                iObserver.ServerReportSent( aError );        
                }
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::OperationComplete() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateController::TimerComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateController::TimerComplete( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateController::TimerComplete() begin: %d",
                     aError);

    // If we come here it means that timer has completed before
    // the reports were actually sent and before the operation is released. 

    // Timer has done its job. So delete it.
    delete iReportTimer;
    iReportTimer = NULL;

    // Set the flag. Then, observer will not be informed twice when the
    // report operation is actually completed.
    iReportTimerCompleted = ETrue;

    // Inform the observer that it should continue even if reports are still
    // being sent in the background. Notice, that the report operation most likely
    // will complete later and then OperationComplete will be called.
    iObserver.ServerReportSent( aError );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::TimerComplete() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::DoCancel
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::DoCancel() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] ControllerState: %d", iControllerState );

    switch ( iControllerState )
        {
        case EStarting:
            {
            // This controller is trying to create the 
            // provider. Cancel that.
            iEngine->CancelProviderCreation();
            iControllerState = ENotRunning;            
            }
            break;

        case EInLocalLoadOperation:
            {
            // Local loading was cancelled. 
            // Nothing to cancel, because complete of the request already issued
            iControllerState = EIdle;
            }
            break;

        default:
            // We should never come here.
            break;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::DoCancel() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::RunL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::RunL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] ControllerState: %d", iControllerState);
    IAUPDATE_TRACE_1("[IAUPDATE] Error code: %d", iStatus.Int());        

    // If we were trying to start the provider we may get different
    // error codes that are still acceptable. For example, the DB may have
    // been cleaned because of some error situation. But, then we just have
    // to continue after that normally. 
    // In other cases, KErrNone will inform about success.
    // Possible feedback from the provider contains the following error
    // codes that are interpreted as success: 
    // KErrNone, KNcdDatabasesClearedAfterCrash, KNcdPurchaseHistoryVersionMismatch,
    // KNcdGeneralDatabaseVersionMismatch. 
    // Notice, that the actual error code can be a combination of these numbers.

    // Get the error code.
    TInt errorCode( iStatus.Int() );
 
    // This will leave if error code is negative. 
    // KErrNone and positive error codes are interpreted as success.
    // If leave occurs, let RunError handle it.
    User::LeaveIfError( errorCode );

    switch ( iControllerState )
        {
        case EStarting:
            {
            if ( !iBaseProvider )
                {
                User::Leave( KErrGeneral );
                }

            // Change the error code to iaupdate specific if necessary.
            if ( errorCode > 0 )
                {
                IAUPDATE_TRACE("[IAUPDATE] NCD cache cleared. Change error code to iaupdate specific.");
                errorCode = IAUpdateErrorCodes::KErrCacheCleared;        
                }
                
            iProvider = iBaseProvider->QueryInterfaceL<MNcdProvider>();
            iProvider->SetObserver( this );
            iProvider->SetStringLocalizer( *this );
            
            SetupConfigurationL();

            iServerReportManager = 
                iProvider->QueryInterfaceL<MNcdServerReportManager>();
            // Because we want to send S60 error codes instead of general
            // codes into the server, set the style here.
            iServerReportManager->
                SetReportingStyleL( 
                    MNcdServerReportManager::EReportingStyleS60 );

            // Cancel possible paused operations.
            // So, they do not prevent others to start their operations.
            CancelPausedOperationsL();
            
            // Everything was handled correctly. So, set the state and
            // inform observer.
            iControllerState = EIdle;            
            iObserver.StartupComplete( errorCode );
            }
            break;

        case EInClearCache:
            {
            IAUPDATE_TRACE("[IAUPDATE] Load nodes from net.");
            // Cache has been cleaned.

            // Save the current information to clear file.
            iCacheClearFile->SetCurrentData();
            iCacheClearFile->WriteControllerDataL();

            // Next, load necessary nodes from the net.
            // This starts an asynchronous operation which will end
            // when callback is called.
            iLoader->LoadNodesL();

            iControllerState = EInLoadOperation;
            }
            break;

        case EInLocalLoadOperation:
            {
            // Nodes should be gotten from the local database instead of 
            // from the internet.
            LocalLoadL();
                
            // New state should be idle because after this function
            // everything is done and observer is informed.
            // Notice, that the state is inserted here after all the other
            // functionality has been handeled because if the code above leaves,
            // then RunError will be able to check the state and handle things 
            // correctly.
            iControllerState = EIdle;

            // Inform, the observer.            
            iObserver.RefreshComplete( iNodes, KErrNone );            
            }
            break;
        
        default:
            break;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::RunL() end");
    }
  
    
// -----------------------------------------------------------------------------
// CIAUpdateController::RunError
//
// -----------------------------------------------------------------------------
//    
TInt CIAUpdateController::RunError( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::RunError() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] ControllerState: %d", iControllerState );

    switch ( iControllerState )
        {
        case EStarting:
            iControllerState = ENotRunning;
            iObserver.StartupComplete( aError );
            break;

        case EInLocalLoadOperation:
        case EInClearCache:
            // Local load left in RunL.
            iControllerState = EIdle;
            iObserver.RefreshComplete( iNodes, aError );
            break;

        default:
            // We should not come here.
            break;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::RunError() end");    

    return KErrNone;
    }
 

// -----------------------------------------------------------------------------
// CIAUpdateController::LocalLoadL()
//
// -----------------------------------------------------------------------------
// 
void CIAUpdateController::LocalLoadL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LocalLoadL() begin");

    MNcdNode* rootNode( iProvider->RootNodeL() );
    if ( !rootNode )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: NULL root was given.");
        User::Leave( KErrNotFound );
        }

    if ( rootNode->State() == MNcdNode::EStateNotInitialized )
        {
        IAUPDATE_TRACE("[IAUPDATE] Root node uninitialized. Do not continue local load.");
        rootNode->Release();
        return;
        }

    CleanupReleasePushL( *rootNode );

    MNcdNodeContainer* rootContainer = 
        rootNode->QueryInterfaceLC< MNcdNodeContainer >();
    if ( !rootContainer )
        {
        IAUPDATE_TRACE("[IAUPDATE] ERROR: NULL container for root.");
        // Root should always have container interface.
        User::Leave( KErrNotFound );
        }

    // The given node was root node.

    LocalContainerLoadL( *rootContainer );

    CleanupStack::PopAndDestroy( rootContainer );
    rootContainer = NULL;
            
    CleanupStack::PopAndDestroy( rootNode );
    rootNode = NULL;

    // The iNodeContainer now contains all the necessary nodes.
    // Get the nodes from the iNodeContainer.
    // Make sure that the array is clean before adding new items.
    iNodes.Reset();

    // Get references to the node arrays.
    const RPointerArray< CIAUpdateFwNode >& fwNodes = 
        iNodeContainer->FwNodes();
    const RPointerArray< CIAUpdateNode >& headNodes = 
        iNodeContainer->HeadNodesL();

    // Make sure we have enough memory for the array.
    TInt fwNodeCount( fwNodes.Count() );
    TInt headNodeCount( headNodes.Count() );
    iNodes.ReserveL( fwNodeCount + headNodeCount );

    // Insert the firmware nodes into the beginning of the array.
    for ( TInt i = 0; i < fwNodeCount; ++i )
        {
        // Notice, that the ownership is not transferred here.
        MIAUpdateAnyNode* anyNode( fwNodes[ i ] );
        iNodes.AppendL( anyNode );
        }

    // Insert the head nodes into the array.
    for ( TInt i = 0; i < headNodeCount; ++i )
        {
        // Notice, that the ownership is not transferred here.
        MIAUpdateAnyNode* anyNode( headNodes[ i ] );
        iNodes.AppendL( anyNode );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LocalLoadL() end");
    }

    
// -----------------------------------------------------------------------------
// CIAUpdateController::LocalContainerLoadL
//
// -----------------------------------------------------------------------------
//   
void CIAUpdateController::LocalContainerLoadL( 
    MNcdNodeContainer& aContainer )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LocalContainerLoadL() begin");

    // Insert all the item children (that are not uninitilized) 
    // from the container node into the iNodeContainer. 
    // So, iNodeContainer will have all the item children in its list.
    
    TInt childCount( aContainer.ChildCount() );	        
    IAUPDATE_TRACE_1("[IAUPDATE] childcount: %d", childCount);	        

    for ( TInt i = 0; i < childCount; ++i )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] child index: %d", i );
        
        // This call increases the reference count of the child.
        MNcdNode* child( aContainer.ChildL( i ) );

        if ( child )
            {
            // Notice, that we need to check if the child really exists.
            // If a container has been loaded from the net, it has 
            // its child count. But, children may have not been loaded.
            IAUPDATE_TRACE("[IAUPDATE] Child was loaded.");
            if ( child->State() == MNcdNode::EStateNotInitialized )
                {
                IAUPDATE_TRACE("[IAUPDATE] Skip uninitialized child.");
                // Skip uninitialized child.
                // Remember to release it here.
                child->Release();
                child = NULL;
                }
            else
                {
                // Insert child into the cleanup stack. 
                // So, if functions leave, it will be released.
                CleanupReleasePushL( *child );

                // Notice, that this has to be released.
                MNcdNodeContainer* childContainer( 
                    child->QueryInterfaceL< MNcdNodeContainer >() );

                if ( !childContainer )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Item node");            

                    TBool isFwNode( 
                        IAUpdateNodeFactory::IsFwUpdateL( *child ) ); 

                    // Because the factory takes care of the deletion of the child object,
                    // just pop it from the cleanup stack here. So, no release is called here.
                    CleanupStack::Pop( child );

                    if ( isFwNode  && !IAUpdateUtils::IsFirmwareChangedL() ) 
                        {
                        // if phone's firmware changed after previous successfull network refresh,
                        // firmware nodes are skipped. That's a workaround to hide them from UI just after 
                        // firmware update. 
                                      
                        // Notice, that the IAUpdateNodeFactory and CIAUpdateNodeContainer 
                        // take the ownership and release the node if the creation leaves.
                        CIAUpdateFwNode* node( NULL );
                        // Trap error here. 
                        // If one node fails, others may still be created.
                        TRAP_IGNORE( 
                            node = IAUpdateNodeFactory::CreateFwNodeL( child, *this ) );
                        if ( node )
                            {
                            IAUPDATE_TRACE("[IAUPDATE] Fw node created successfully");
                            // If leave occurs, then AddNodeL will itself delete the created node.
                            // So, do not insert the node into the cleanup stack here.
                            iNodeContainer->AddFwNodeL( node );
                            }
                        }
                    else
                        {
                        // Notice, that the IAUpdateNodeFactory and CIAUpdateNodeContainer 
                        // take the ownership and release the node if the creation leaves.
                        CIAUpdateNode* node( NULL );
                        // Trap error here. 
                        // If one node fails, others may still be created.
                        TRAP_IGNORE( 
                            node = IAUpdateNodeFactory::CreateNodeL( child, *this ) );
                        if ( node )
                            {
                            IAUPDATE_TRACE("[IAUPDATE] Node created successfully");
                            // If leave occurs, then AddNodeL will itself delete the created node.
                            // So, do not insert the node into the cleanup stack here.
                            iNodeContainer->AddNodeL( node );
                            }
                        }
                    }
                else
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Container node");

                    // The ownership of the child was not transferred to
                    // anybody, the node has to be released here.
                    CleanupStack::PopAndDestroy( child );

                    CleanupReleasePushL( *childContainer );

                    // Because this is a container, use recursion to insert its
                    // child items into the node container.
                    LocalContainerLoadL( *childContainer );

                    CleanupStack::PopAndDestroy( childContainer );
                    }
                }
            }
        }
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::LocalContainerLoadL() begin");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::SetupConfigurationL
//
// -----------------------------------------------------------------------------
// 
void CIAUpdateController::SetupConfigurationL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::SetupConfigurationL() begin");

    CIAUpdateEngineXmlParser* parser( CIAUpdateEngineXmlParser::NewLC() );
    parser->ParseL();
    const CIAUpdateEngineConfigData& data( parser->ConfigData() );

    CNcdKeyValuePair* pair( NULL );
    

    // Master server (CDB) uri
    IAUPDATE_TRACE_1("[IAUPDATE] master server: %S", &data.MasterServerUri() );
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KMasterServer(), 
                                    data.MasterServerUri() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );
        
    // Max storage size
    // Notice, this value does not have any effect if the cache cleaner is
    // turned off when provider is created.
    const TDesC& storageMaxSize( data.StorageMaxSize() );
    if ( storageMaxSize == KNullDesC )
        {
        IAUPDATE_TRACE("[IAUPDATE] Use default storage max size");
        pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KMaxStorageSize(), 
                                        IAUpdateCtrlConsts::KDefaultStorageMaxSize() );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] Use config file storage max size");
        pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KMaxStorageSize(), 
                                        storageMaxSize );        
        }
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );

    // Software version
    // Notice, that here we use the hardcoded value instead of parsing the data
    // from the configuration XML file. This should be hardcoded value because the
    // version number is strictly related to the version of the engine. So, the version
    // should not be altered by different configuration files.
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KSoftwareVersion(), 
                                    IAUpdateCtrlConsts::KSoftwareVersion() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );
    
    // Software type
    // Notice, that here we use the hardcoded value instead of parsing the data
    // from the configuration XML file. This should be hardcoded value because the
    // software type is always same for the engine. It should not be allowed to
    // change by defining new value in config file.
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KSoftwareType(), 
                                    IAUpdateCtrlConsts::KSoftwareType() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );

    // Provisioning
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KProvisioning(), 
                                    data.Provisioning() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );

    // Client role
    // This value is used in the server requests.
    pair = CNcdKeyValuePair::NewLC( IAUpdateProtocolConsts::KIAClientRole(), 
                                    data.ClientRole() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );
    iObserver.ClientRole( data.ClientRole() );


    // No need for the parser anymore.
    CleanupStack::PopAndDestroy( parser );
    parser = NULL;

    
    // Language
    TLanguage language( User::Language() );
    HBufC* languageDes( HBufC::NewLC( 32 ) );
    TPtr languagePtr( languageDes->Des() );
    languagePtr.AppendNum( language );
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KSoftwareLanguage(), 
                                    languagePtr );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );
    CleanupStack::PopAndDestroy( languageDes );


    // Capabilities accepted by the client.
    // These capabilities define actions between the client and the server.
    
    // Download report
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KCapability(),
                                    NcdCapabilities::KDownloadReport() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );
    
    // Install report
    pair = CNcdKeyValuePair::NewLC( NcdConfigurationKeys::KCapability(),
                                    NcdCapabilities::KInstallationReport() );
    iProvider->AddConfigurationL( *pair );
    CleanupStack::PopAndDestroy( pair );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::SetupConfigurationL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::NodeFromPurchaseHistoryL
//
// -----------------------------------------------------------------------------
//
MNcdNode* CIAUpdateController::NodeFromPurchaseHistoryL( 
    const CIAUpdaterIdentifier& aIdentifier )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeFromPurchaseHistoryL() begin");

    // This will contain the correct details object.
    CNcdPurchaseDetails* details( NULL );

    // Create filter. So, we will get
    // all the purchase history items.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();
    filter->SetNamespaceL( aIdentifier.Namespace() );
    filter->SetEntityIdL( aIdentifier.Id() );
    
    // Add family uid to the filter
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    uids.AppendL( FamilyUid() );
    filter->SetClientUids( uids.Array() );
    CleanupStack::PopAndDestroy( &uids );

    MNcdPurchaseHistory* history( ProviderL().PurchaseHistoryL() );
    CleanupReleasePushL( *history );
    
    // Get the ids. So, we can next get all the corresponding
    // details.
    RArray< TUint > ids = history->PurchaseIdsL( *filter );
    // Temporarily remove history from cleanup stack
    CleanupStack::Pop( history );
    CleanupStack::PopAndDestroy( filter );
    CleanupReleasePushL( *history );
    CleanupClosePushL( ids );
    
    if ( ids.Count() > 0 )
        {
        // If purchase details exist, then use the most up-to-date one.
        details = 
            history->PurchaseDetailsL( ids[ 0 ], EFalse );
        }
    
    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::PopAndDestroy( history );
    
    if ( details == NULL )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        // The details was created but not inserted into
        // the cleanup stack there. Insert it into the cleanupstack now.
        CleanupStack::PushL( details );
        }

    MNcdNode* node( ProviderL().NodeL( *details ) );
    if ( node == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( details );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::NodeFromPurchaseHistoryL() end");

    return node;
    }


// -----------------------------------------------------------------------------
// CIAUpdateController::NodeFromPurchaseHistoryL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateController::CancelPausedOperationsL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CancelPausedOperationsL() begin");
    
    // Cancel possible paused operations here.
    // IAD Engine does not itself pause operations. 
    // But, in some error cases download operation may have become paused.
    // By cancelling a paused operation, we can avoid locking if multiple IAD UIs 
    // are opened. Else, the paused operations that belong to first opened UI 
    // will block the starting of the operations from another IAD UIs.
    // Because these operations are created for the one IAD, 
    // they will be blocking other UI operations because multiple 
    // simultaneous operations are not allowed for same metadata content.
    // When the IAD is started, the operations that are paused will be
    // created in the NCD Provider and they are part of that IAD UI via NCD
    // Provider Proxy.
    
    // Notice, Release needs to be called for array elements.
    RCatalogsArray< MNcdOperation > operations = 
        iProvider->OperationsL();

    // Push the array into the cleanupstack. So, PopAndDestroy will
    // call Release to the array items and finally reset the array.
    CleanupResetAndDestroyPushL( operations );
        
    TInt count( operations.Count() );
    IAUPDATE_TRACE_1("[IAUPDATE] Pending operation count: %d", count);
    for ( TInt i = 0; i < count; ++i )
        {
        MNcdOperation* operation( operations[ i ] );
        MNcdDownloadOperation* download( 
            operation->QueryInterfaceLC< MNcdDownloadOperation >() );
        if ( download && download->IsPaused() )
            {
            // A download operation has been left hanging as paused.
            // Just, cancel it. So, it will not prevent possible other
            // download attempts from other IAD UIs. We cancel this operations
            // already now, because we may not actually start it later in this
            // UI.
            IAUPDATE_TRACE_1("[IAUPDATE] Cancel paused operation: %d", i);
            operation->CancelOperation();
            CleanupStack::PopAndDestroy( download );
            }
        }
    
    // This will Release array elements and reset the array.
    CleanupStack::PopAndDestroy( &operations );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateController::CancelPausedOperationsL() begin");
    }
