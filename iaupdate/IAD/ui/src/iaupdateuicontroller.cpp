/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateUiController
*                class member functions.
*
*/



// INCLUDES
#include <centralrepository.h>
#include <featurecontrol.h>
#include <cmmanager.h>
#include <qapplication.h>
#include <hbaction.h>
#include <hbprogressdialog.h>
#include <iaupdateparameters.h>

#include "iaupdateuicontroller.h"
#include "iaupdateuicontrollerobserver.h"
#include "iaupdate.hrh"
#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"
#include "iaupdatenodefilter.h"
#include "iaupdatefactory.h"
#include "iaupdatestarter.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatecontrollerfile.h"
#include "iaupdateuiconfigdata.h"
#include "iaupdateutils.h"
#include "iaupdateagreement.h"
#include "iaupdateautomaticcheck.h"
#include "iaupdateroaminghandler.h"
#include "iaupdatependingnodesfile.h"
#include "iaupdaterestartinfo.h"
#include "iaupdaterresultsfile.h"
#include "iaupdaterresult.h"
#include "iaupdateridentifier.h"
#include "iaupdateruids.h"
#include "iaupdaterdefs.h"
#include "iaupdateparametersfilemanager.h"
#include "iaupdateerrorcodes.h"
#include "iaupdatefileconsts.h"
#include "iaupdatefirsttimeinfo.h"
#include "iaupdaterefreshhandler.h"
#include "iaupdatenodeid.h"
#include "iaupdatewaitdialog.h"
#include "iaupdatedialogutil.h"
#include "iaupdatedebug.h"


// cenrep in emulator:
// copy 2000F85A.txt to '\epoc32\release\winscw\udeb\Z\private\10202be9\'
// delete 2000F85A.txt from 'epoc32\winscw\c\private\10202be9\persists'
//
// cenrep in hardware:
// copy 2000F85A.txt to '\epoc32\data\Z\private\10202be9'
//


// This is a static function that is used with RPointerArray::Sort to sort
// the nodes according to their node depths and other special values.
TInt SortSelectedNodes( const MIAUpdateNode& aNode1, const MIAUpdateNode& aNode2 )
    {
    // Temporarily use the const_cast here to get the base node.
    // After that, base node is set back to const. So, the casting here
    // is safe to do.
    const MIAUpdateBaseNode& baseNode1( 
        const_cast< MIAUpdateNode& >( aNode1 ).Base() );
    const MIAUpdateBaseNode& baseNode2( 
        const_cast< MIAUpdateNode& >( aNode2 ).Base() );

    if ( baseNode1.Uid().iUid == KIAUpdaterUid
         && baseNode2.Uid().iUid != KIAUpdaterUid )
        {
        // Self updater should always be the first one
        // in the node array.
        return -1;
        }
    else if ( baseNode2.Uid().iUid == KIAUpdaterUid
              && baseNode1.Uid().iUid != KIAUpdaterUid )
        {
        // Self updater should always be the first one
        // in the node array.
        return 1;
        }
    else if ( aNode1.IsSelfUpdate() && !aNode2.IsSelfUpdate() )
        {
        // Self updates always before normal nodes.
        return -1;
        }
    else if ( !aNode1.IsSelfUpdate() && aNode2.IsSelfUpdate() )
        {
        // Self updates always before normal nodes.
        return 1;
        }
    else
        {
        // The depth can be used in normal cases to make sure that dependency
        // nodes are handled before their dependants. Notice, that this is
        // a safe way even if dependency chain contains hidden nodes.
        // The dependency nodes should be before its dependants in the array.
        // The depth value informs how deep in the dependency hierarchy the node is.
        // A depth value zero means that the node is a root. If multiple branches lead to
        // a same node, then the greatest depth value is used for the node.
        // The node can not ever depend on the node that has the same or greater depth.

        TInt depthDiff( aNode2.Depth() - aNode1.Depth() );

        if ( depthDiff != 0 )
            {
            // Depths were different.
            // In some cases, the user may have chosen a dependency node only
            // and left the dependant out. Then this node may be moved unnecessarily
            // in the array. But because dependecies may change the selection order
            // usually quite much, the possible unnecessary movement here should not
            // matter. Notice, that using the depth values is really safe way to make
            // sure the dependency nodes are handled before their dependants.
            return depthDiff;
            }
        else
            {
            // Because depthDiffs equal we need to do some additional checking.
            // This kind of comparison is also used in the filter.
            // See: CIAUpdateNodeFilter::CompareAndMarkFilterNodes
            TInt importanceDiff( 
                baseNode2.Importance() - baseNode1.Importance() );
            if ( importanceDiff != 0 )
                {
                // Sort according to the importance because importances
                // differ.
                return importanceDiff;
                }
            else
                {
                // Because evertyhing else matched, we finally check
                // the name. Items should be handled in alphabetical order
                // now.
                return baseNode1.Name().CompareC( baseNode2.Name() );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateUiController* CIAUpdateUiController::NewLC( 
    MIAUpdateUiControllerObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::NewLC() begin");
    CIAUpdateUiController* self = 
        new( ELeave ) CIAUpdateUiController( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::NewLC() end");
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateUiController* CIAUpdateUiController::NewL( 
    MIAUpdateUiControllerObserver& aObserver )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::NewL() begin");
    CIAUpdateUiController* self = 
        CIAUpdateUiController::NewLC( aObserver );
    CleanupStack::Pop( self );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::NewL() end");
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::CIAUpdateUiController
// C++ constructor
// -----------------------------------------------------------------------------
//
CIAUpdateUiController::CIAUpdateUiController( 
    MIAUpdateUiControllerObserver& aObserver )
: iObserver( aObserver ),
  iState( EIdle )
    {    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CIAUpdateUiController()");
    iEikEnv = CEikonEnv::Static();
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::ConstructL()
    {    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConstructL() begin");
        
    iClosingAllowedByClient = ETrue;
    iController = 
        IAUpdateFactory::CreateControllerL( 
            TUid::Uid( KIAUpdateFamilyUid ), *this );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConstructL() 1");
    iFilter = CIAUpdateNodeFilter::NewL();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConstructL() 2");

    // Notice, this will read the init values from the file
    // if the file exists. Otherwise, default values are used.
    iControllerFile = 
        CIAUpdateControllerFile::NewL( IAUpdateFileConsts::KControllerFile );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConstructL() 3");
    
    iConfigData = CIAUpdateUiConfigData::NewL();
    
    iOffConfigurated = !IAUpdateEnabledL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConstructL() end");   
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::~CIAUpdateUiController
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateUiController::~CIAUpdateUiController()
    {    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::~CIAUpdateUiController() begin");

    CancelOperation();
    delete mWaitDialog;
    delete iController;
    iNodes.Reset();
    iFwNodes.Reset();
    iSelectedNodesArray.Reset();
    iServicePackNodes.Reset();
    iPreviousSelections.Reset();
    delete iFilter;
    delete iControllerFile;
    delete iIdle;
    delete iStarter;
    delete iConfigData;
    delete iRoamingHandler;
    delete iParams;
    delete iRefreshHandler;
    delete mDialogUtil;

    // If dialogs have not been released yet, release them now.
    // ProcessFinishedL() should normally be used for dialogs but
    // here just use non-leaving delete. In normal cases, dialogs should
    // already be released in the end of the update flow before coming here.
    //delete iWaitDialog;
    //delete iProgressDialog;
        
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::~CIAUpdateUiController() end");
    }        


// -----------------------------------------------------------------------------
// CIAUpdateUiController::ConfigData
// Returns the configuration data from the config file.
// -----------------------------------------------------------------------------
//
const CIAUpdateUiConfigData& CIAUpdateUiController::ConfigData() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ConfigData()");
    return *iConfigData;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::SetDefaultConnectionMethodL
// Sets the connection method for the update network connection.
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::SetDefaultConnectionMethodL( const TIAUpdateConnectionMethod& aMethod )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SetDefaultConnectionMethodL() begin");

    iController->SetDefaultConnectionMethodL( aMethod );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SetDefaultConnectionMethodL() end"); 
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::CheckUpdatesL
// Updates the update item list.
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::CheckUpdatesL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesL() begin");
    
    iCountOfAvailableUpdates = 0;
    
    if ( iParams )
        {
    	iFilter->SetFilterParams( iParams ); //iParams ownership is changed
    	iParams = NULL;
        }
     
    TBool agreementAccepted( EFalse );
    if ( !ForcedRefresh() )
        {
        CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
        agreementAccepted = agreement->AgreementAcceptedL();
        if ( ( !agreementAccepted )&& ( iRequestType != IAUpdateUiDefines::ECheckUpdates ) )
            {
            // agreement (disclaimer) dialog is not prompted when CheckUpdates is called
            //
            // Refresh from network is allowed when first time case 
            iRefreshFromNetworkDenied = EFalse;
            agreementAccepted = agreement->AcceptAgreementL();  
            if (!agreementAccepted)
                {
                if ( !mDialogUtil )
                    {
                    mDialogUtil = new IAUpdateDialogUtil(NULL, this);
                    }
                if ( mDialogUtil )
                    {
                    mPrimaryAction = NULL;
                    //mPrimaryAction = new HbAction(hbTrId("txt_software_button_accept"));
					mPrimaryAction = new HbAction("Accept");
                    HbAction *secondaryAction = NULL;
                    //secondaryAction = new HbAction(hbTrId("txt_software_button_decline"));
					secondaryAction = new HbAction("Decline");
                    mDialogUtil->showAgreement(mPrimaryAction,secondaryAction);
                    iDialogState = EAgreement;
                    }
                }
            }
        CleanupStack::PopAndDestroy( agreement );
        }
    if ( iDialogState != EAgreement )
        {
        AgreementHandledL();
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesL() end");
    return;
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController:: AgreementHandledL
// Updates the update item list.
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::AgreementHandledL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::AgreementHandledL() begin");
    TBool agreementAccepted( EFalse );
    CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
    agreementAccepted = agreement->AgreementAcceptedL();
    CleanupStack::PopAndDestroy( agreement );        
    if ( !agreementAccepted && !ForcedRefresh() )
        {
        if ( iRequestType == IAUpdateUiDefines::ECheckUpdates )
            {
            CIAUpdateAutomaticCheck* automaticCheck = CIAUpdateAutomaticCheck::NewLC();
            TBool autoUpdateCheckEnabled = automaticCheck->AutoUpdateCheckEnabledL();
            CleanupStack::PopAndDestroy( automaticCheck );
            if ( !autoUpdateCheckEnabled )
                {
                iObserver.RefreshCompleteL( ETrue, KErrNone );
                return;
                }
            }
        else if ( iRequestType == IAUpdateUiDefines::EShowUpdates )
            {
       	    iObserver.UpdateCompleteL( KErrNone ); 
       	    return;
       	    }
       	else
       	    {
       	    qApp->quit();
       	    return;	
       	    }
        }
    if ( iRequestType == IAUpdateUiDefines::ECheckUpdates && 
         AllowNetworkRefreshL() && 
         iRoamingHandler->IsRoaming() && 
         !AutomaticConnectionWhenRoamingL() )
        {
        // In Silent check updates case, global setting when roaming to be automatic 
        // Promt dialog is not allowed in the silent check
        // error is returned that a client (backround checker) will try again later 
        iObserver.RefreshCompleteL( EFalse, KErrAbort );
        }  
    else if ( iRequestType == IAUpdateUiDefines::EShowUpdates && iRefreshed )
        {
    	iFilter->FilterAndSortNodesL( iNodes, iFwNodes );
        iObserver.RefreshCompleteL( ETrue, KErrNone );
        }
    else
        {
        TInt ret( iController->Startup() );
        if ( ret == KErrAlreadyExists )   
            {
            IAUPDATE_TRACE("[IAUPDATE] Controller startup already done.");
            // Controller startup has already been done. 
            // So, no operation was started from the controller. 
            // So, call StartupComplete directly from here. 
    	    StartupComplete( KErrNone );
            }
        else
            {
            IAUPDATE_TRACE_1("[IAUPDATE] Controller startup error code: %d", ret);
            // Something went wrong with controller startup if error code is not
            // KErrNone here. Above, KErrAlreadyExists was handled separately.
            // If ret is KErrInUse, then startup was still going on
            // and new startup should not be called. Also, leave in that case.
            User::LeaveIfError( ret );	
            }	
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::AgreementHandledL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartUpdateL
// Starts software updating
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::StartUpdateL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartUpdateL() begin");

    // Just to be sure that self update related information
    // is always in resetted state when new update flows are
    // started.
    iController->ResetSelfUpdate();

    CreateSelectedNodesArrayL();
                
    if ( !IAUpdateUtils::SpaceAvailableInInternalDrivesL( iSelectedNodesArray ) )
        {
        if ( !mDialogUtil )
            {
            mDialogUtil = new IAUpdateDialogUtil(NULL, this);
            }
        if ( mDialogUtil )
            {
            mPrimaryAction = NULL;
            mPrimaryAction = new HbAction(hbTrId("txt_common_button_ok"));
            //mDialogUtil->showInformation(hbTrId("txt_software_info_insufficient_memory_free_some_m"), mPrimaryAction);
			mDialogUtil->showInformation(QString("Insufficient memory. Free some memory and try again."), mPrimaryAction);
            iDialogState = EInsufficientMemory;
            }
        }
    else
        {
        if ( !IsStartedByLauncher() )
            {
            if ( !iStarter )
                {
                // Notice, that the ownership of the filter parameters will
                // remain in the filter.
                CIAUpdateParameters* params = iFilter->FilterParams();
      	        iStarter = CIAUpdateStarter::NewL( params->CommandLineExecutable(), 
        	                                           params->CommandLineArguments() );
                }
            }
        // Inform the controller that we are now starting updates. This way the
        // controller can handle situations as a whole and not as one item at the
        // time.
        iController->StartingUpdatesL();
        
        //Store current node list before update
        iFilter->StoreNodeListL( iNodes );
        
        iFilter->SortSelectedNodesFirstL( iSelectedNodesArray, iNodes );
                    
        iFileInUseError = EFalse;
        // Set the node index to -1 because ContinueUpdateL increases it by one
        // in the beginning of the function. So, we can use the ContinueUpdateL
        // also in here.
        iNodeIndex = -1;
        ContinueUpdateL( EFalse );
        }

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartUpdateL() end");
    }

  
// ---------------------------------------------------------------------------
// CIAUpdateUiController::ContinueUpdateL
// Continues software updating from the next node
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::ContinueUpdateL( TBool aSelfUpdateFinished )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateUiController::ContinueUpdateL() begin: %d",
                   aSelfUpdateFinished );
    
    // Get the node index of the next node.
    // The node that is indexed by iNodeIndex has already been handled in
    // previous loop. So, increase the temp index by one here as it will be done
    // in the while loop below for real index.
    TInt tmpIndex( iNodeIndex + 1 );
    if ( tmpIndex > 0 
        && tmpIndex < iSelectedNodesArray.Count()
        && iSelectedNodesArray[ tmpIndex - 1 ]->IsSelfUpdate()
        && !iSelectedNodesArray[ tmpIndex ]->IsSelfUpdate()
        && !aSelfUpdateFinished
        && iController->SelfUpdateDataExists() )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Self updates have been handled now: %d",
                         iNodeIndex);
        // Self update should be started now because
        // current node is not anymore self update and previous nodes were.
        // UpdateCompleteL will start the self updater and inform observers.
        // If no self update data can be found, then there has been some error
        // when self update data has been set. So, then there is no reason to start
        // self updater here. In that case, continue normally in the while loop below if
        // other items are waiting for update.
        iNodeIndex++;
        UpdateCompleteL( KErrNone );

        // Do not continue after this.
        return;
        }

    TBool nextUpdate( ETrue );
    while ( nextUpdate )
       {
       IAUPDATE_TRACE_1("[IAUPDATE] Next update while loop: %d",
                        iNodeIndex);
       iNodeIndex++;
       if ( iNodeIndex < iSelectedNodesArray.Count() )
            {
            MIAUpdateNode* selectedNode = iSelectedNodesArray[ iNodeIndex ];

            // Only update items that have not been installed yet.
            if ( !selectedNode->IsInstalled() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Item not installed yet"); 
                iFilter->SortThisNodeFirstL( selectedNode, iNodes );
                if ( !selectedNode->IsDownloaded() )
                    {
                    // Because content has not been downloaded or installed yet,
                    // it needs to be downloaded before it can be installed.
                    IAUPDATE_TRACE("[IAUPDATE] Download");
                    selectedNode->DownloadL( *this );
                    iState = EDownloading;
                    iClosingAllowedByClient = ETrue;
                    selectedNode->SetUiState( MIAUpdateNode::EDownloading );
                    iObserver.RefreshUI();
                    nextUpdate = EFalse;
                    }
                else
                    {
                    // Because content has been downloaded but not installed yet,
                    // it needs to be installed now.
                    IAUPDATE_TRACE("[IAUPDATE] Install");
                    selectedNode->InstallL( *this );
                    iState = EInstalling;
                    iClosingAllowedByClient = EFalse;
                    selectedNode->SetUiState( MIAUpdateNode::EInstalling );
                    iObserver.RefreshUI();
                    nextUpdate = EFalse;                     
                    }      
                }
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Update flow complete");
            nextUpdate = EFalse;
            UpdateCompleteL( KErrNone );	
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ContinueUpdateL() end");
    }
   

// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartInstallL
// Starts software installing
// ---------------------------------------------------------------------------
//    
void CIAUpdateUiController::StartInstallL( MIAUpdateNode& aNode )   
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartInstallL() begin");
   
    if ( aNode.Type() == MIAUpdateNode::EPackageTypeServicePack )
        {
        iServicePackNodes.Reset();
        aNode.GetDependenciesL( iServicePackNodes, ETrue );
        }
            
    aNode.InstallL( *this );
    iState = EInstalling;
    aNode.SetUiState( MIAUpdateNode::EInstalling );
    iObserver.RefreshUI();
        
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartInstallL() end");
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartRefreshL()
// Starts meta data refresh
// ---------------------------------------------------------------------------
// 
void CIAUpdateUiController::StartRefreshL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartRefreshL() begin");
    
    iCountOfAvailableUpdates = 0;
    iClosingAllowedByClient = ETrue;
    if ( iOffConfigurated  )
        {
        iObserver.RefreshCompleteL( ETrue, KErrNone );
        }
    else
        {
        iState = ERefreshing;    
        TBool allowNetworkRefresh = AllowNetworkRefreshL();
        iSelectedNodesArray.Reset();
        iController->StartRefreshL( allowNetworkRefresh );
            
        if ( iRequestType == IAUpdateUiDefines::ECheckUpdates && iFilter->FilterParams() )
            {
            if ( iFilter->FilterParams()->ShowProgress() )
                {
                // to avoid flickering IAD is brought to foreground only when wait dialog is really shown
                if ( allowNetworkRefresh )
                    {
                    iEikEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );
                    ShowWaitDialogL( "Refreshing updates list", ETrue );
                    }
                }
            }
        else
            {
            ShowWaitDialogL( "Refreshing updates list", ETrue );
            }
        }
    
       
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartRefreshL() end");
    }
 
   
// ---------------------------------------------------------------------------
// CIAUpdateUiController::Nodes
// 
// ---------------------------------------------------------------------------
//     
const RPointerArray< MIAUpdateNode >& CIAUpdateUiController::Nodes() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::Nodes()");
    return iNodes;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::FwNodes
// 
// ---------------------------------------------------------------------------
//     
const RPointerArray<MIAUpdateFwNode>& CIAUpdateUiController::FwNodes() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::FwNodes()");
    return iFwNodes;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::HistoryL
// 
// ---------------------------------------------------------------------------
//  
MIAUpdateHistory& CIAUpdateUiController::HistoryL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HistoryL()");
    return iController->HistoryL();
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::SetRequestType
// 
// ---------------------------------------------------------------------------
void CIAUpdateUiController::SetRequestType( 
    IAUpdateUiDefines::TIAUpdateUiRequestType aRequestType )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SetRequestType() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] request type: %d", aRequestType );

    iRequestType = aRequestType;    
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SetRequestType() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::CancelOperation
// 
// ---------------------------------------------------------------------------    
void CIAUpdateUiController::CancelOperation() 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CancelOperation() begin");

    // Set the flag to inform possible callback functions
    // that operation cancell is going on.
    iCancelling = ETrue;

    if ( iIdle )
        {
    	iIdle->Cancel();
        }
    if ( iStarter )
        {
    	iStarter->Cancel();
        }
    if ( iRoamingHandler )
        {
    	iRoamingHandler->CancelPreparing();
        }
    
	if ( iState == EDownloading || iState == EInstalling )
	    {
	    // Current node.
        MIAUpdateNode* node( iSelectedNodesArray[ iNodeIndex ] );

        // Cancel the operation of that node.
    	node->CancelOperation();

        // Also set the purchase history information to other nodes
        // even if their operations were not started yet.
        for ( TInt i = iNodeIndex + 1; i < iSelectedNodesArray.Count(); ++i )
            {
            node = iSelectedNodesArray[ i ];
            if ( MIAUpdateNode::EPackageTypeServicePack == node->Type()
                  && node->IsInstalled() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Service pack was completed");
                // Even if cancel occurred, some of the nodes inside the
                // service pack were installed from some other dependency
                // chain way. And, now there is nothing to be installed
                // inside the service pack.
                TRAP_IGNORE ( node->Base().
                    SetInstallStatusToPurchaseHistoryL( KErrNone, ETrue ) );
                }
            else
                {
                IAUPDATE_TRACE("[IAUPDATE] Node cancelled");
                // Notice, that this can also still be service pack node.
                // Because we are handling nodes that are visible in UI,
                // we can force the node as visible in history.
                TRAP_IGNORE ( 
                    node->Base().SetIdleCancelToPurchaseHistoryL( ETrue ) ); 
                }
            }
	    }
    else if ( iState == ERefreshing )
        {
        iController->CancelRefresh();
        }

    // Just to be sure that self update related information
    // is always in resetted state when new update flows are
    // started after operation cancel.
	if ( iController )
	    {
	    iController->ResetSelfUpdate();
	    }
    // After cancellation, the new state is idle.
    iState = EIdle;
    iClosingAllowedByClient = ETrue;

    // Cancelling is over. So, set the flag accrodingly.
    iCancelling = EFalse;

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CancelOperation() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::ResultsInfo
// 
// ---------------------------------------------------------------------------    
TIAUpdateResultsInfo CIAUpdateUiController::ResultsInfo() const
	{
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ResultsInfo() begin");

	TIAUpdateResultsInfo resultsInfo( 0, 0, 0, iFileInUseError, EFalse );
	for ( TInt i = 0; i < iSelectedNodesArray.Count(); ++i )
	    {
	    MIAUpdateNode* node( iSelectedNodesArray[ i ] );
	    TInt lastErrorCode( KErrNone );
	    TRAPD ( trapError, 
	            lastErrorCode = node->Base().LastUpdateErrorCodeL() );
	    if ( node->IsInstalled() )
	        {
	        // Because node is installed, update must have been a success.
	        ++resultsInfo.iCountSuccessfull;
	        if ( node->Base().RebootAfterInstall() )
	            {
	            resultsInfo.iRebootAfterInstall = ETrue;
	            }
	        }
        else if ( trapError == KErrNone 
                  && lastErrorCode == KErrCancel )
            {
            ++resultsInfo.iCountCancelled;
            }
        else
            {
            ++resultsInfo.iCountFailed;
            }
	    }
	    
	IAUPDATE_TRACE_1("[IAUPDATE] Successfull count: %d", resultsInfo.iCountSuccessfull );
	IAUPDATE_TRACE_1("[IAUPDATE] Cancelled count: %d", resultsInfo.iCountCancelled );
	IAUPDATE_TRACE_1("[IAUPDATE] Failed count: %d", resultsInfo.iCountFailed );
	
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ResultsInfo() end");

	return resultsInfo;
	}


// ---------------------------------------------------------------------------
// CIAUpdateUiController::CountOfAvailableUpdates
// 
// ---------------------------------------------------------------------------    
TInt CIAUpdateUiController::CountOfAvailableUpdates() const
	{
	IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateUiController::CountOfAvailableUpdates() count: %d",
                     iCountOfAvailableUpdates );
	return iCountOfAvailableUpdates;
	}


// ---------------------------------------------------------------------------
// CIAUpdateUiController::Starter()
// 
// ---------------------------------------------------------------------------    
const CIAUpdateStarter* CIAUpdateUiController::Starter() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::Starter()");
    return iStarter;	
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::Filter()
// 
// --------------------------------------------------------------------------- 
const CIAUpdateNodeFilter* CIAUpdateUiController::Filter() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::Filter()");
    return iFilter;	
    }

// -----------------------------------------------------------------------------
// CIAUpdateUiController::DoCancelIfAllowed
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateUiController::DoCancelIfAllowed()
   {
   IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DoCancelIfAllowed() begin");

   if ( iClosingAllowedByClient )
       {
   	   CancelOperation();
       }

   IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DoCancelIfAllowed() end");

   return iClosingAllowedByClient;	
   }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::ClosingAllowedByClient
// 
// -----------------------------------------------------------------------------
//  
 TBool CIAUpdateUiController::ClosingAllowedByClient()
     {
     IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ClosingAllowedByClient()");
     IAUPDATE_TRACE_1("[IAUPDATE] closing allowed: %d", iClosingAllowedByClient );
     TBool closingAllowed = iClosingAllowedByClient;
     if ( iRequestType != IAUpdateUiDefines::EShowUpdates )
         {
         // iaupdate can be orphaned from its client only when ShowUpdates() was called  
         closingAllowed = ETrue;
         }
 	 return closingAllowed;
     }
 	

// -----------------------------------------------------------------------------
// CIAUpdateUiController::CheckUpdatesDeferredL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::CheckUpdatesDeferredL( CIAUpdateParameters* aParams, 
                                                   TBool aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesDeferredL() begin");
    
    iClosingAllowedByClient = ETrue;
    delete iParams;
    iParams = aParams; //ownership of parameters is taken
    iRefreshFromNetworkDenied = aRefreshFromNetworkDenied; 
    delete iIdle;
    iIdle = NULL;
	iIdle = CIdle::NewL( CActive::EPriorityIdle ); 
    iIdle->Start( TCallBack( CheckUpdatesDeferredCallbackL, this ) );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesDeferredL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::PrepareRoamingHandlerL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::PrepareRoamingHandlerL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::PrepareRoamingHandlerL() begin");
	    
    if ( !iRoamingHandler )
        {
    	iRoamingHandler = CIAUpdateRoamingHandler::NewL();
        }
    if ( !iRoamingHandler->Prepared() )
        {
    	iRoamingHandler->CancelPreparing();
    	//async call that will return in RoamingHandlerPrepared()
    	iRoamingHandler->PrepareL( *this );
        }
    else
        {
        CheckUpdatesL();	
        }
    
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::PrepareRoamingHandlerL() end");
    }



// -----------------------------------------------------------------------------
// CIAUpdateUiController::HandlePossibleSelfUpdateRestartL
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateUiController::HandlePossibleSelfUpdateRestartL( TBool aShutdownRequest )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandlePossibleSelfUpdateRestartL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] ShutdownRequest: %d", aShutdownRequest );

    TBool restarted( EFalse );

    CIAUpdateRestartInfo* info( iController->SelfUpdateRestartInfo() );

    if ( info )
        {
        IAUPDATE_TRACE("[IAUPDATE] Self updater related info available.");

        CleanupStack::PushL( info );

        // Delete the unnecessary files because we will still have 
        // required objects available.
        // Notice, that if for some reason a leave occurs after this and the
        // application does not continue the update, then next time the application
        // is started, update restart is not rehandled any more.
        info->DeleteFiles();

        // Get the data for the updater results.            
        CIAUpdaterResultsFile& results( info->ResultsFile() );

        // This flag is set if self updater operation was cancelled.
        TBool selfUpdateCancelled( EFalse );

        // Because we have restarted the iaupdate after self updating,
        // also start to gather possible new update flow reports into 
        // a report bundle. Inform the controller, so the controller 
        // can handle situations as a whole and not as one item at the
        // time.
        iController->StartingUpdatesL();

        RPointerArray< CIAUpdaterResult >& resultArray( results.Results() );
        // Because self updater will insert the possible hidden dependencies
        // before the actual item, start from the beginning of the array to
        // set the error codes. This way, the main item will be set last and
        // it will get the latest time for the purchase history.
        for ( TInt i = 0; i < resultArray.Count(); ++i )
            {
            CIAUpdaterResult* result( resultArray[ i ] );
            
            if ( result->ErrorCode() == KErrCancel )
                {
                IAUPDATE_TRACE("[IAUPDATE] Self Updater cancelled"); 
            	selfUpdateCancelled = ETrue;
                }

            // Because, the self update installation was finished.
            // Update the purchase history with the correct information.
            // Notice, that we can just set the purchase history here for
            // the self updater items. If some of the items were installed,
            // before self updater items, then their information was already
            // correctly into the history. Notice, that result nodes may be hidden
            // nodes that were not initially visible in the UI. So, do not
            // force the results visible in purchase history.
            MIAUpdateNode& node( iController->NodeL( result->Identifier() ) );
            node.Base().
                SetInstallStatusToPurchaseHistoryL( 
                    result->ErrorCode(), EFalse );
            }

        IAUPDATE_TRACE("[IAUPDATE] Self update info inserted to purchase history."); 

           
        // Get the data for the pending node info.
        CIAUpdatePendingNodesFile& pendings(
            info->PendingNodesFile() );


        // Because ContinueUpdateL will use the counters and
        // node index and iSelectedNodesArray, we set the correponding information
        // here. Then, the update operation will continue correctly for the pending
        // nodes.
            

        // Set the node index to -1 because ContinueUpdateL increases it by one
        // in the beginning of the function. So, we can use the ContinueUpdateL
        // also in the end of this function to start the actual update process.
        iNodeIndex = -1;
        iSelectedNodesArray.Reset();
        
        // Also, reset nodearray nodes as not selected because original parameter
        // settings may have been overruled by the user before user started 
        // the self update from UI. Correct nodes will be chosen in the for-loops below.
        for ( TInt i = 0; i < iNodes.Count(); ++i )
            {
            MIAUpdateNode& node( *iNodes[ i ] );
            node.Base().SetSelected( EFalse );
            }
        for ( TInt i = 0; i < iFwNodes.Count(); ++i )
            {
            MIAUpdateFwNode& fwNode( *iFwNodes[ i ] );  
            fwNode.Base().SetSelected( EFalse );
            }
        

        if ( !selfUpdateCancelled )
            {
            IAUPDATE_TRACE("[IAUPDATE] Self update not cancelled.");

            // Notice, that the ownership of the filter parameters will
            // remain in the filter.
            CIAUpdateParameters* params( iFilter->FilterParams() );
            if ( !IsStartedByLauncher() || params )
                {
                IAUPDATE_TRACE("[IAUPDATE] Self update handling will set parameters.");

                // Check if IAD was started from the grid or through API.
                // If IAD was started like from the grid but it still has parameters set,
                // then act as it was started through API. Most likely, then the IAD was
                // originally started through API but because of self update IAD was restarted
                // and now it has original parameters set.

                if ( !iStarter )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] iStarter not set");
            	    iStarter = CIAUpdateStarter::NewL( params->CommandLineExecutable(), 
            	                                       params->CommandLineArguments() );
                    }
                }
            }

        // Get list of nodes that were initially chosen for the flow.
        RPointerArray< CIAUpdaterIdentifier >& pendingIdentifiers = 
            pendings.PendingNodes();
        TInt pendingIdentifiersCount( pendingIdentifiers.Count() );
        // Get the index to the node that has not yet been handled.
        TInt pendingsIndex( pendings.Index() );
        iSelectedNodesArray.ReserveL( pendingIdentifiersCount );
        for ( TInt i = 0; i < pendingIdentifiersCount; ++i )
            {
            CIAUpdaterIdentifier* identifier( pendingIdentifiers[ i ] );
            MIAUpdateNode& node( 
                iController->NodeL( *identifier ) );
            iSelectedNodesArray.AppendL( &node );

            // Also, set the node as selected because then UI can easily check
            // which nodes should be marked when the UI is restarted after selfupdate.
            node.Base().SetSelected( ETrue );

            if ( i < pendingsIndex )
                {
                // We come here if index suggests that node has already been handled

                // Increase the node index. So, it will already point
                // to the correct place. Because, we want to omit these files
                // when list is gone through.
                ++iNodeIndex;

            	if ( iStarter && node.IsInstalled() )
            	    {
                    IAUPDATE_TRACE("[IAUPDATE] Self update handling will set parameters, node installed");
                    // Because application starting parameters may have been given through the
                    // API, we need to pass UID of installed package to starter if it exists.
                    // Then, the starter will start the given exe in the end of the install flow
                    // if at least one file was succesfully installed.
            		iStarter->CheckInstalledPackageL( node.Base().Uid() );
            	    }
                }
            else if ( selfUpdateCancelled )
                {
                IAUPDATE_TRACE("[IAUPDATE] Set idle cancel info to purchase history");
                // Check if self updater actions were cancelled.
                // Set the purchase history information to nodes not handled yet.
                // If visible service pack was changed to hidden during the operation
                // then force the current history info visible. Then the history
                // corresponds the situation that was when operation flow was started.
                // This may occure if some of the items were installed after all.
                // Notice, that here we can think that node was initially visible
                // because all nodes in UI selection list are visible and possible
                // hidden self update nodes have been handled already in the beginning
                // of this for loop.
                if ( MIAUpdateNode::EPackageTypeServicePack == node.Type()
                      && node.IsInstalled() )
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Service pack was completed");
                    // Even if cancel occurred, some of the nodes inside the
                    // service pack were installed. And, now there is nothing
                    // to be installed inside the service pack.
                    node.Base().
                        SetInstallStatusToPurchaseHistoryL( KErrNone, ETrue );
                    }
                else
                    {
                    IAUPDATE_TRACE("[IAUPDATE] Node cancelled");
                    // Notice, that this can also still be service pack node.
                    // Because we are handling nodes that are visible in UI,
                    // we can force the node as visible in history.
                    node.Base().SetIdleCancelToPurchaseHistoryL( ETrue ); 
                    }
                }
            else if ( MIAUpdateNode::EPackageTypeServicePack == node.Type()
                      && node.IsInstalled() )
                {
                IAUPDATE_TRACE("[IAUPDATE] Service pack has become installed");
                // Because service pack has become installed during self update
                // it means that the service pack was originally missing only
                // the selfupdate. Set, the service pack as installed also into
                // purchase history because no other operation will be done to
                // it after this. Notice, that because service pack was originally
                // in the list, it has been visible for UI. So, make sure that it
                // is also visible in history. IAD Engine will set service packs
                // whose all content is installed as hidden. Therefore, force the
                // visibility here.
                // Notice, that here we can think that node was initially visible
                // because all nodes in UI selection list are visible and possible
                // hidden self update nodes have been handled already in the beginning
                // of this for loop.
                node.Base().
                    SetInstallStatusToPurchaseHistoryL( KErrNone, ETrue );
                }
            }

        // Info is not needed any more. 
        // So, delete it before update flow will be continued below 
        // if necessary.
        CleanupStack::PopAndDestroy( info );

        // Check if self updater actions was closed by using red key.
        // If so, then do not continue update flow here. 
        // Instead, this application should be closed.
        if ( !aShutdownRequest )
            {
            IAUPDATE_TRACE("[IAUPDATE] Self update not closed with red key.");            
            if ( selfUpdateCancelled )
                {
                IAUPDATE_TRACE("[IAUPDATE] Self update cancelled.");
                // Because self update is cancelled, do not continue here either.
                // Instead end the update flow directly.
                UpdateCompleteL( KErrCancel );
                }
            else
                {
                IAUPDATE_TRACE("[IAUPDATE] Self update finished, continue udpate.");
                // Continue udpate normally.
                ContinueUpdateL( ETrue );                
                }
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] Self update closed with red key. Handle dialogs.");
            // Because shutdown was requested, we do not continue the update flow.
            // A updating dialog may still show. So, release it here.
            RemoveUpdatingDialogsL();
            }

        // Self updater provided some data that was handeld above.
        restarted = ETrue;
        }

    IAUPDATE_TRACE_1("[IAUPDATE] restarted: %d", restarted );   
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandlePossibleSelfUpdateRestartL() end");

    return restarted;
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::TestRole
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateUiController::TestRole() const 
    {
    return iTestRole;	
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::RefreshComplete
// 
// -----------------------------------------------------------------------------
//                            
void CIAUpdateUiController::RefreshComplete( 
    const RPointerArray< MIAUpdateAnyNode >& aNodes,
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshComplete() begin");
    TInt completionError = KErrNone;
    if ( iState == EUiRefreshing )
        {
    	iState = EIdle;
    	TRAP( completionError, RefreshUiCompleteL( aNodes, aError ) );
        }
    else
        {
        iState = EIdle;
        TRAP ( completionError, RemoveWaitDialogL() );
        if ( completionError == KErrNone )
            {
    	    TRAP( completionError, RefreshCompleteL( aNodes, aError ) );
            }	
        }
    if ( completionError == KErrNone )
        {
        iRefreshed = ETrue;
        }
    else
        {
        iObserver.HandleLeaveErrorWithoutLeave( completionError );
        }
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshComplete() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::SelfUpdaterComplete
// 
// -----------------------------------------------------------------------------
//                            
void CIAUpdateUiController::SelfUpdaterComplete( TInt aErrorCode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SelfUpdaterComplete() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aErrorCode );

    // Self updater completed its job.
    // Normally we should not come here because self update should shut down this
    // application before installation. But, now because we came here, complete
    // the update operation.

    // Remove params file if it exists. This is a temporary file that would be
    // otherwise read during next grid start if not removed here. There is no need
    // to use that file now, because all the necessary parameters are already set
    // because restart of IAD did not occur.
    
    iState = EIdle;
    TRAP_IGNORE ( ParamsRemoveFileL() );

    TInt err( KErrNone );
    TBool restartDataWasAvailable( EFalse );
    TRAP ( err, 
           restartDataWasAvailable = 
            HandlePossibleSelfUpdateRestartL( 
                aErrorCode == IAUpdaterDefs::KIAUpdaterShutdownRequest ) );
    if ( err != KErrNone )
        {
    	iObserver.HandleLeaveErrorWithoutLeave( err );
        }
    else if ( !restartDataWasAvailable )
        {
        IAUPDATE_TRACE("[IAUPDATE] Restart data not available");
        // For some reason the self updater was not able to give the restart data.
        // So, try to continue the update in a normal way.
        TRAP ( err, UpdateFailedSelfUpdaterInfoToPurchaseHistoryL( aErrorCode ) );
        if ( err == KErrNone )
            {
            IAUPDATE_TRACE("[IAUPDATE] Continue update flow");
        	TRAP ( err, ContinueUpdateL( EFalse ) );
            }
        if ( err != KErrNone )
            {
            IAUPDATE_TRACE_1("[IAUPDATE] Purchase history update error: %d", err);
        	iObserver.HandleLeaveErrorWithoutLeave( err );
            }
        }

    if ( aErrorCode == IAUpdaterDefs::KIAUpdaterShutdownRequest )
        {
        //CIAUpdateAppUi* appUi = static_cast<CIAUpdateAppUi*>( iEikEnv->EikAppUi() );
    	//appUi->Exit();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::SelfUpdaterComplete() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::ServerReportSent
// 
// -----------------------------------------------------------------------------
//                            
void CIAUpdateUiController::ServerReportSent( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateUiController::ServerReportSent() begin: %d",
                     aError);

    // This call back is called when MIAUpdateController::UpdateFinishedL
    // function is called and that asynchronous operation completes.
    // Report sending is done in the background.
    if ( iState == ESendingReport )
        {
        IAUPDATE_TRACE("[IAUPDATE] End the update flow now that report has been sent");
        // Reports are sent only when update flow has finished. Also, in normal 
        // flow, we wait that report sending finishes before continuing to the end. 
        TRAP_IGNORE( EndUpdateFlowL( aError ) ); 
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ServerReportSent() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::ClientRole
// 
// -----------------------------------------------------------------------------
//     
void CIAUpdateUiController::ClientRole( const TDesC& aClientRole )
    {
	if ( aClientRole == IAUpdateUiDefines::KTestRole() )
	    {
	    iTestRole = ETrue;
	    }
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::DownloadProgress
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::DownloadProgress( MIAUpdateNode& /*aNode*/, 
                                              TUint /*aProgress*/,
                                              TUint /*aMaxProgress*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadProgress() begin");  

    if ( iProgressDialog )
        {
    	//TRAP_IGNORE ( iProgressDialog->SetProgressDialogFinalValueL( aMaxProgress ) );
        //TRAP_IGNORE ( iProgressDialog->UpdateProgressDialogValueL( aProgress ) );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadProgress() end");  
    }                            


// -----------------------------------------------------------------------------
// CIAUpdateUiController::DownloadComplete
// 
// -----------------------------------------------------------------------------
//                            
void CIAUpdateUiController::DownloadComplete( MIAUpdateNode& aNode,
                                              TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadComplete() begin");

    iState = EIdle;
	TRAPD ( completionError, DownloadCompleteL( aNode, aError ) );
    if ( completionError != KErrNone )
        {
        // If we came here, DownloadCompleteL function may have left
        // before dialog was removed. So, try one more time to remove it here
        // before informing the observer.
        TRAP_IGNORE( RemoveUpdatingDialogsL() );
        iObserver.HandleLeaveErrorWithoutLeave( completionError );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadComplete() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::InstallProgress
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::InstallProgress( MIAUpdateNode& /*aNode*/, 
                                             TUint /*aProgress*/,
                                             TUint /*aMaxProgress*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallProgress()");    
    }                            


// -----------------------------------------------------------------------------
// CIAUpdateUiController::InstallComplete
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::InstallComplete( MIAUpdateNode& aNode,
                                             TInt aError )  
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallComplete() begin");

    iState = EIdle;
    //CIAUpdateAppUi* appUi = static_cast<CIAUpdateAppUi*>( iEikEnv->EikAppUi() );
    //appUi->StopWGListChangeMonitoring();
    if ( aError == KErrInUse )
        {
    	iFileInUseError = ETrue;
        }
    
	TRAPD ( completionError, InstallCompleteL( aNode, aError ) );
    if ( completionError != KErrNone )
        {
        // If we came here, InstallCompleteL function may have left
        // before dialog was removed. So, try one more time to remove it here
        // before informing the observer.
        TRAP_IGNORE( RemoveUpdatingDialogsL() );
        iObserver.HandleLeaveErrorWithoutLeave( completionError );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallComplete() end");
    }

    
// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartupComplete
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateUiController::StartupComplete( TInt aError )
    {  
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartupComplete() begin");  

    if ( aError == IAUpdateErrorCodes::KErrCacheCleared )
        {
        IAUPDATE_TRACE("[IAUPDATE] Cache cleared during startup.");  
        // Because cache was cleared, change the refresh information data
        // so that the network refresh will be done during refresh operation.
        
        // The values have been read from the file when the controller file 
        // object was created or the defaul values are used if the file did
        // not exist. So, no need to re-read them here.
        // Replace the time with zero. So, the zero time will
        // suggest that network refresh is required later on. 
        iControllerFile->SetRefreshTime( 0 );

        // Save data into the file.
        // Change aError value to correct one. 
        // aError will be KErrNone if everything goes ok inside trap,
        // or else it will contain the correct error code.
        TRAP ( aError, iControllerFile->WriteControllerDataL(); );
        
        CIAUpdateRestartInfo* info( iController->SelfUpdateRestartInfo() ); 
        if ( info ) 
            { 
            IAUPDATE_TRACE("[IAUPDATE] Delete restart info because of cache clean.");
            // Restart info was available. 
            // Delete self upate files because otherwise a net connection
            // may be prevented during refresh and the cleared cache can not
            // be updated. If this is not done here, HandlePossibleSelfUpdateRestartL
            // may leave because required nodes are not found. Notice, that also
            // HandlePossibleSelfUpdateRestartL deletes these files but by handling
            // this special case here, an application can continue without leave later.
            info->DeleteFiles();
            delete info; 
            info = NULL; 
            }
        }
        
    iObserver.StartupComplete( aError );
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartupComplete() end");  
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartExecutableCompletedL
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateUiController::StartExecutableCompletedL( TInt /*aError*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartExecutableCompletedL() begin"); 

    // Before informing the observer, close the possible existing dialog.
    RemoveUpdatingDialogsL();
    
    // Possible error is not passed to a client. Typically client is already closed. 
    // Even in the case when client's request is still ongoing, it's disinformation to return
    // error from failed executable start. 
    TRAPD ( err, iObserver.UpdateCompleteL( KErrNone ) );
    if ( err != KErrNone )
        {
    	iObserver.HandleLeaveErrorL( err );
        }

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartExecutableCompletedL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RoamingHandlerPrepared
// 
// ---------------------------------------------------------------------------
// 
void CIAUpdateUiController::RoamingHandlerPrepared()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RoamingHandlerPrepared() begin");
    TRAPD ( err, CheckUpdatesL() );
	if ( err != KErrNone )
        {
    	iObserver.HandleLeaveErrorWithoutLeave( err );
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RoamingHandlerPrepared() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateUiController::HandleWaitDialogCancel
// Called when wait dialog is cancelled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::HandleWaitDialogCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleWaitDialogCancel() begin");
    
    TRAPD ( err, HandleUserCancelL() );
    if ( err != KErrNone )
        {
      	iObserver.HandleLeaveErrorL( err );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleWaitDialogCancel() end");

    }

// -----------------------------------------------------------------------------
// CIAUpdateUiController::dialogFinished
// Called when dialog is finished.
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::dialogFinished(HbAction *action)
    {
    TDialogState dialogState = iDialogState;
    iDialogState = ENoDialog;
    
    switch ( dialogState )
        {
        case EAgreement:
            if ( action == mPrimaryAction )
                 {
                CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
                agreement->SetAgreementAcceptedL();

                CleanupStack::PopAndDestroy(agreement);
                }
            AgreementHandledL();        
            break;
        case EInsufficientMemory:    
            break;
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateUiController::HandleUiRefreshL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateUiController::HandleUiRefreshL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleUiRefreshL() begin"); 

	if ( iRequestType == IAUpdateUiDefines::ECheckUpdates )
	    {
	    iRefreshed = EFalse;
	    }
	else if ( iState == EIdle )
	    {
	    //CIAUpdateAppUi* appUi = static_cast< CIAUpdateAppUi* >( iEikEnv->EikAppUi() );
        //if ( appUi->UiRefreshAllowed() )
		    {
	    	iState = EUiRefreshing;
	        // store node identification (Id and namespace) of currently selected nodes
	        // to restore selections after refresh
	        iPreviousSelections.Reset();
	    	for ( TInt i = 0; i < iNodes.Count(); ++i )
                {
                MIAUpdateNode* node( iNodes[ i ] );
                CIAUpdateNodeId* nodeId = CIAUpdateNodeId::NewLC();
                nodeId->SetIdL( node->Base().MetaId() );
                nodeId->SetNamespaceL( node->Base().MetaNamespace() );
                nodeId->SetNodeType( MIAUpdateAnyNode::ENodeTypeNormal );
                nodeId->SetSelected( node->Base().IsSelected() );
                iPreviousSelections.AppendL( nodeId );
                CleanupStack::Pop( nodeId );
                
                }
            
	    	for ( TInt j = 0; j < iFwNodes.Count(); ++j )
                {
                MIAUpdateFwNode* fwNode( iFwNodes[ j ] );
                CIAUpdateNodeId* nodeId = CIAUpdateNodeId::NewLC();
                nodeId->SetIdL( fwNode->Base().MetaId() );
                nodeId->SetNamespaceL( fwNode->Base().MetaNamespace() );
                nodeId->SetNodeType( MIAUpdateAnyNode::ENodeTypeFw );
                nodeId->SetSelected( fwNode->Base().IsSelected() );
                iPreviousSelections.AppendL( nodeId );
                CleanupStack::Pop( nodeId );
                }
        
   	        iController->StartRefreshL( EFalse );
	        }
	    }
	
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleUiRefreshL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RefreshCompleteL
// 
// ---------------------------------------------------------------------------
//        
void CIAUpdateUiController::RefreshCompleteL( 
    const RPointerArray< MIAUpdateAnyNode >& aNodes,
    TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshCompleteL() begin");

    iState = EIdle;
    if ( !iRefreshHandler  && !iOffConfigurated )
        {
        iRefreshHandler = CIAUpdateRefreshHandler::NewL();
        }
    if ( aError == KErrNone ) 
        {
        // Inform refresh to other IAD instances 
        if ( !iOffConfigurated  && AllowNetworkRefreshL() && iRefreshHandler )
            {
            iRefreshHandler->InformRefreshL();   
            }
                  
        // Because refresh was successfully completed,
        // update the controller information to the controller file
        UpdateControllerFileL();
        }

    // Even in a case when refresh failed, there may be old cache available.
    // It's also possible that only firmware node or normal node refresh failed, so there is 
    // partial list available
    // ==> all nodes returned by engine are shown in UI
    iNodes.Reset();
    iFwNodes.Reset();
    iNodes.ReserveL( aNodes.Count() );
    for ( TInt i = 0; i < aNodes.Count(); ++i )
        {
        MIAUpdateAnyNode* anyNode( aNodes[ i ] );
        if ( anyNode->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
            {
            MIAUpdateNode* node( 
                static_cast< MIAUpdateNode* >( anyNode ) );
            iNodes.AppendL( node );                
            }
        else if ( anyNode->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
            {
            MIAUpdateFwNode* fwNode( 
                static_cast< MIAUpdateFwNode* >( anyNode ) );
            iFwNodes.AppendL( fwNode );
            }
        }

    switch ( iRequestType )
        {
        case IAUpdateUiDefines::ECheckUpdates:
        	
            if ( iFilter->FilterParams() )
                {
           		iFilter->CountOfAvailableUpdatesL( 
               		                         iNodes, 
               		                         iFwNodes,
               		                         iCountOfAvailableUpdates ); 
                }
            break;
        
        default: 
            // Sort the nodes according to the filter values
            // This function performs default marking also
            iFilter->FilterAndSortNodesL( iNodes, iFwNodes );
                                          
            break;
        }

    if ( aError == KErrNone )
        {
        // Now, that refresh has been done, 
        // check if there is some self update related settings .
        HandlePossibleSelfUpdateRestartL( EFalse );
        }
    
    if ( !iOffConfigurated  && iRefreshHandler )
        {
        iRefreshHandler->StartListeningL( this );
        }
    iObserver.RefreshCompleteL( ETrue, aError );        
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshCompleteL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RefreshUiCompleteL
// 
// ---------------------------------------------------------------------------
//        
void CIAUpdateUiController::RefreshUiCompleteL( 
    const RPointerArray< MIAUpdateAnyNode >& aNodes,
    TInt /*aError*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshUiCompleteL() begin");
    // : Is error handling needed?
    iNodes.Reset();
    iFwNodes.Reset();
    iNodes.ReserveL( aNodes.Count() );
    // Find out if there was umarked firmware update in old selections,
    // that case needs special handling
    TBool unmarkedFw = EFalse;
    for ( TInt i = 0; i < iPreviousSelections.Count(); ++i )
        {
        CIAUpdateNodeId* prevNodeId( iPreviousSelections[ i ] );
        if ( prevNodeId->NodeType() == MIAUpdateAnyNode::ENodeTypeFw  && !prevNodeId->Selected() )
             {
             unmarkedFw = ETrue;
             }
        }
     
    for ( TInt i = 0; i < aNodes.Count(); ++i )
        {
        MIAUpdateAnyNode* anyNode( aNodes[ i ] );
        if ( anyNode->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
            {
            MIAUpdateNode* node( static_cast< MIAUpdateNode* >( anyNode ) );
            iNodes.AppendL( node );                
            }
        else if ( anyNode->NodeType() == MIAUpdateAnyNode::ENodeTypeFw && !unmarkedFw )
            {
            // if umarked firmware update in old selections, firmawares are not appended 
            // before iFilter->FilterAndSortNodesL() is called
            MIAUpdateFwNode* fwNode( 
                    static_cast< MIAUpdateFwNode* >( anyNode ) );
            iFwNodes.AppendL( fwNode );
            }
        }
    iFilter->FilterAndSortNodesL( iNodes, iFwNodes );
    
    if ( unmarkedFw )
        {
        for ( TInt i = 0; i < aNodes.Count(); ++i )
            {
            MIAUpdateAnyNode* anyNode( aNodes[ i ] );
            if ( anyNode->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                MIAUpdateFwNode* fwNode( 
                        static_cast< MIAUpdateFwNode* >( anyNode ) );
                iFwNodes.AppendL( fwNode );
                }
            }
        }
    
    //restore previous selections/deselections  
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        MIAUpdateNode* node( iNodes[ i ] );
        for ( TInt j = 0; j < iPreviousSelections.Count(); ++j )
            {
            CIAUpdateNodeId* prevNodeId( iPreviousSelections[ j ] );
        	if ( node->Base().MetaId() == prevNodeId->Id() && 
        	     node->Base().MetaNamespace() == prevNodeId->Namespace() )
        	    {
        	    node->Base().SetSelected( prevNodeId->Selected() );
        	    }
            }
        }
    
    // When dependant is marked, all dependencies to be marked also
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        MIAUpdateNode* node( iNodes[ i ] );
        if ( node->Base().IsSelected() )
            {
            iFilter->SetDependenciesSelectedL( *node, iNodes );
            }
        }
    
    TBool firmwareSelected = EFalse;
    for ( TInt i = 0; i < iFwNodes.Count(); ++i )
        {
        MIAUpdateFwNode* fwNode( iFwNodes[ i ] );
    	for ( TInt j = 0; j < iPreviousSelections.Count(); ++j )
    	    {
    	    CIAUpdateNodeId* prevNodeId( iPreviousSelections[ j ] );
    		if ( prevNodeId->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
    		    {
    			fwNode->Base().SetSelected( prevNodeId->Selected() );
    		    }
    	    }
    	if ( fwNode->Base().IsSelected() )
    	    {
    	    firmwareSelected = ETrue;
    	    }
        }
    
    // if firmware is marked, normal nodes to be unmarked
    if ( firmwareSelected )
        {
        for ( TInt i = 0; i < iNodes.Count(); ++i )
            {
            MIAUpdateNode* node( iNodes[ i ] );
            node->Base().SetSelected( EFalse );
            }
        }
    
    CreateSelectedNodesArrayL();
    iObserver.RefreshCompleteL( EFalse, KErrNone );
     
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RefreshUiCompleteL() end"); 
    }
    
    
    
// ---------------------------------------------------------------------------
// CIAUpdateUiController::DownloadCompleteL
// 
// ---------------------------------------------------------------------------
//            
void CIAUpdateUiController::DownloadCompleteL( MIAUpdateNode& aNode, TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadCompleteL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );

    if ( aError == KErrAbort )
        {
        // Abort is interpreted so that a connection was not allowed by the user.
        // So, do not continue to a new operation. 
        // Instead, complete the update flow now.
        // Set the purchase history information to nodes.
        // Because the current node has KErrAbort as the last operation error code,
        // change it to KErrCancel because we want to think it as cancellation instead
        // of as abort error. This way, error counters and history info will show cancel
        // instead of failed. Also, update cancel info to other pending nodes.
        for ( TInt i = iNodeIndex; i < iSelectedNodesArray.Count(); ++i )
            {
            MIAUpdateNode* node( iSelectedNodesArray[ i ] );
            // Because we are handling nodes that are visible in UI,
            // we can force the node as visible in history.
            TRAP_IGNORE ( 
                node->Base().SetIdleCancelToPurchaseHistoryL( ETrue ) );
            }
        UpdateCompleteL( KErrNone );        
        }
    else if ( aError != KErrNone )
        {
    	InstallCompleteL( aNode, aError );
        }
    else   
        {
	    StartInstallL( aNode );
        }    
 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::DownloadCompleteL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::InstallCompleteL
// 
// ---------------------------------------------------------------------------
//      
void CIAUpdateUiController::InstallCompleteL( MIAUpdateNode& aNode, 
                                              TInt aError )    
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallCompleteL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );

    if ( aError == KErrNone )
        {
        aNode.SetUiState( MIAUpdateNode::EUpdated );   
        iObserver.RefreshUI();
        }
    else
        {
        aNode.SetUiState( MIAUpdateNode::EFailed );
        iObserver.RefreshUI();
        }
    
    // pass UID of installed package to starter
    if ( iStarter )
        {
    	if ( aNode.Type() == MIAUpdateNode::EPackageTypeServicePack )
    	    { 
    	    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallCompleteL() node type is service pack");
     	    for ( TInt i = 0; i < iServicePackNodes.Count(); ++i )
    	        {
    	        if ( iServicePackNodes[i]->IsInstalled() )   
    	            {
    	            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallCompleteL() node is installed");
    	            iStarter->CheckInstalledPackageL( iServicePackNodes[i]->Base().Uid() );
    	            }
    	        }
    	    iServicePackNodes.Reset();
    	    }
    	else if ( aNode.IsInstalled() )
    	    {
    	    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallCompleteL() node is installed");
    	    iStarter->CheckInstalledPackageL( aNode.Base().Uid() );
    	    }
        }
    
    // If installation was marked as KErrCancel here,
    // we think that it was just the disclaimer and
    // let this continue.        
    ContinueUpdateL( EFalse );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::InstallCompleteL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::UpdateCompleteL
// 
// ---------------------------------------------------------------------------
//      
void CIAUpdateUiController::UpdateCompleteL( TInt aError ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateCompleteL() begin");

    iState = EIdle;
    iClosingAllowedByClient = EFalse;
       
    TInt error( aError );
    TBool selfUpdaterStarted( EFalse );

    if ( error == KErrNone )
        {
        IAUPDATE_TRACE("[IAUPDATE] Check self update start");
        // Set the error to correspond to the trap error. So, 
        // the observer will get correct information about the end result
        // of the update.
        TRAP ( error, 
               selfUpdaterStarted = StartPossibleSelfUpdateL() );
        if ( error != KErrNone )
            {
            IAUPDATE_TRACE_1("[IAUPDATE] trap error: %d", error);
            // Because self update could not be started the counters may not be
            // correct. Also, update purchase history with the error value 
            // because install operation failed here.
            UpdateFailedSelfUpdaterInfoToPurchaseHistoryL( error ); 
            }
        }                   

    // Check if self updater was started.
    // If it was not, then there most likely was not any self updates set,
    // or something went wrong when self updater was tried to be started.
    if ( selfUpdaterStarted )
        {
        iState = ESelfUpdating;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] SelfUpdater was not started");

        // Notice, that if the self updater was started, then
        // do not inform the UI here that the operation was completed.
        // Also, do not start possible executable yet.
        // Instead, let the self updater do its thing in the background.
        // Most likely, the updater will close this application during installation.
        // But, if it does not, it will result to the SelfUpdaterComplete function
        // call, and we can continue the installation from there.

        // Inform the controller that we finished the updates. This way the
        // controller can handle situations as a whole and not as one item at the
        // time. Notice, that this will start the asynchronous operation that sends
        // possible reports to the server. Callback is called when operation is finished.
        // Also notice, that this reporting operation is not part of the normal update
        // flow but a separate action meant  after update flow finishes. 
        // The destructor of the engine controller can handle the cancellation of 
        // the reporting operation. Also, no callbacks are called back to UI when 
        // cancellation occurs. Notice, that if close request for application is issued
        // before reports are sent, then objects are delete immediately and we will not 
        // wait report sending to finish but reports are sent next time the application 
        // is started.
        // Maximum wait time for server reports is given in micro seconds.
        // Notice, that in case of cancellations there would not be any need
        // for timer. But just use it as in normal cases. Then, server report
        // completion or timer will call the callback later. But, that callback
        // will be ignored in CIAUpdateUiController::ServerReportSent because
        // iState is already then changed from ESendingReport to something else. 
        
        //const TInt KServerReportMaxWaitTime( 10000000 );         //Commented TEMP 
        //iController->FinishedUpdatesL( ETrue, KServerReportMaxWaitTime ); //Commented TEMP
        //iState = ESendingReport;  //Commented temporarily
        EndUpdateFlowL( KErrNone );  //TEMP
        return;                      //TEMP  
        }

    // When reports are sent, EndUpdateFlowL is called via the callback functions
    // and the update flow will be finished there. But, if we come here when cancel
    // is pressed, then call EndUpdateFlowL directly because then we do not wait
    // report sending to finish. This will also set the state to EIdle to inform
    // that when cancelling, we do not wait for the report sending to complete.
    if ( aError == KErrCancel )
        {
        IAUPDATE_TRACE("[IAUPDATE] Cancelling. Call EndUpdateFlowL directly");
        EndUpdateFlowL( error );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateCompleteL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::EndUpdateFlowL
// 
// ---------------------------------------------------------------------------
//  
void CIAUpdateUiController::EndUpdateFlowL( TInt aError )    
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::EndUpdateFlowL() begin");

    // Because we are ending the flow, set the state to idle.
    iState = EIdle;
    
    // Notice, that the ownership of the filter parameters will
    // remain in the filter.
    CIAUpdateParameters* params( iFilter->FilterParams() );
    if ( !IsStartedByLauncher()
         || params )
        {
        IAUPDATE_TRACE_1("[IAUPDATE] Request type check matched: %d", iRequestType);

        // Check if IAD was started from the grid or through API.
        // If IAD was started like from the grid but it still has parameters set,
        // then act as it was started through API. Most likely, then the IAD was
        // originally started through API but because of self update IAD was restarted
        // and now it has original parameters set.

        if ( !iStarter )
            {
            IAUPDATE_TRACE("[IAUPDATE] iStarter not set");
        	iStarter = CIAUpdateStarter::NewL( params->CommandLineExecutable(), 
        	                                   params->CommandLineArguments() );
            }

        // Notice, that when starter calls StartExecutableCompletedL callback function,
        // the observer is informed from there about the completion of the update flow.
        // So, do not inform observer here.
        iStarter->StartExecutableL( *this );
        }
    else
        {
        // Do not try to start any application. 
        // But, use the start application callback function
        // to end the update flow correctly (same way as after 
        // application start with other options).
        IAUPDATE_TRACE_1("[IAUPDATE] Update complete error: %d", aError );
        StartExecutableCompletedL( aError );
        }    

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::EndUpdateFlowL() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::CheckUpdatesDeferredCallbackL
// CheckUpdates as deferred to get empty main view visible before CheckUpdates
//
// ---------------------------------------------------------------------------
//
TInt CIAUpdateUiController::CheckUpdatesDeferredCallbackL( TAny* aPtr )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesDeferredCallbackL() begin");

    CIAUpdateUiController* uiCtrl = static_cast<CIAUpdateUiController*>( aPtr );
    
    TRAPD ( err, uiCtrl->PrepareRoamingHandlerL() );
    if ( err != KErrNone )
        {
        uiCtrl->iObserver.HandleLeaveErrorL( err );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CheckUpdatesDeferredCallbackL() end");

	return KErrNone;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::ShowUpdatingDialogL
// 
// ---------------------------------------------------------------------------
//
/*
void CIAUpdateUiController::ShowUpdatingDialogL( TInt aTextResourceId,
                                                 const TDesC& aName,
                                                 TInt aNumber,
                                                 TInt aTotalCount )   
                                                    
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowUpdatingDialogL() begin");

    // Before trying ot create a new dialog,
    // remove possible existing dialog.
    RemoveUpdatingDialogsL();

    CDesCArray* stringArray = new( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( stringArray );
    stringArray->AppendL( aName );
    CArrayFix<TInt>* numberArray = new( ELeave ) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( numberArray );
    numberArray->AppendL( aNumber ); 
    numberArray->AppendL( aTotalCount );
    HBufC* noteText = NULL;
    //noteText = StringLoader::LoadLC( aTextResourceId, 
    //                                 *stringArray, 
    //                                 *numberArray );
    TPtr ptr = noteText->Des();
    //AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
    if ( iState == EDownloading )
        {
    	ShowProgressDialogL( *noteText, ETrue );
        }
    else
        {
        //ShowWaitDialogL( *noteText, ETrue );	
        }
    
    CleanupStack::PopAndDestroy( noteText ); 
    CleanupStack::PopAndDestroy( numberArray );
    CleanupStack::PopAndDestroy( stringArray );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowUpdatingDialogL() end");
    }*/
 

// ---------------------------------------------------------------------------
// CIAUpdateUiController::ShowWaitDialogL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::ShowWaitDialogL( const QString& aDisplayString, 
                                             TBool /*aVisibilityDelayOff*/ ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowWaitDialogL() begin");
    if ( !mWaitDialog )
        {
        mWaitDialog = new IAUpdateWaitDialog();
        mWaitDialog->SetCallback( this );
        mWaitDialog->showDialog( aDisplayString );
        }
    
    
    
    
    /*if ( iWaitDialog )
        {
        IAUPDATE_TRACE("[IAUPDATE] Wait dialog already existed. Remove it first");
        // in some rare cases previous dialog still exists
        // it's now just deleted (not recommended way) to avoid forwarding problem(s). 
        //delete iWaitDialog;
        iWaitDialog = NULL;
        }*/
 /*   iWaitDialog = 
        new( ELeave ) CIAUpdateWaitDialog( 
            reinterpret_cast< CEikDialog** >( &iWaitDialog ),
            aVisibilityDelayOff );
                
    iWaitDialog->SetTextL( aDisplayString );
    iWaitDialog->SetCallback( this );        
    iWaitDialog->ExecuteLD( R_IAUPDATE_WAIT_DIALOG ); */

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowWaitDialogL() end");
    }


// --------------------------------------------------------------------------
// CIAUpdateUiController::ShowProgressDialogL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::ShowProgressDialogL( const TDesC& /*aDisplayString*/, 
                                                 TBool /*aVisibilityDelayOff*/ ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowProgressDialogL() begin");

    if ( iProgressDialog )
        {
        IAUPDATE_TRACE("[IAUPDATE] Progress dialog already existed. Remove it first");
        // in some rare cases previous dialog still exists
        // it's now just deleted (not recommended way) to avoid forwarding problem(s). 
        //delete iProgressDialog;
        iProgressDialog = NULL;
        }
/*    iProgressDialog = 
        new( ELeave ) CIAUpdateProgressDialog( 
            reinterpret_cast< CEikDialog** >( &iProgressDialog ),
            aVisibilityDelayOff );
                
    iProgressDialog->SetTextL( aDisplayString );
    iProgressDialog->SetCallback( this );        
    iProgressDialog->ExecuteLD( R_IAUPDATE_PROGRESS_DIALOG ); */

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ShowProgressDialogL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RemoveUpdatingDialogsL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::RemoveUpdatingDialogsL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveUpdatingDialogsL() begin");
    
    RemoveWaitDialogL();
    RemoveProgressDialogL();

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveUpdatingDialogsL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RemoveWaitDialogL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::RemoveWaitDialogL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveWaitDialogL() begin");
    if ( mWaitDialog )
        {
        mWaitDialog->close();  
        }
    /*if ( !iCancelling )  //that's needed because AVKON  in 3.2.3 wk12, returning
                         //EFalse in TryToExitL() causes a crash   
        {
    	if ( iWaitDialog )
            {
    	    iWaitDialog->ProcessFinishedL();
            }
        }*/
    
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveWaitDialogL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RemoveProgressDialogL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::RemoveProgressDialogL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveProgressDialogL() begin");

    if ( !iCancelling )  //that's needed because AVKON  in 3.2.3 wk12, returning
                         //EFalse in TryToExitL() causes a crash   
        {
    	if ( iProgressDialog )
            {
    	    //iProgressDialog->ProcessFinishedL();
            }
        }
    
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RemoveProgressDialogL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::AllowNetworkRefreshL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::AllowNetworkRefreshL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::AllowNetworkRefreshL() begin");
    
    TBool allowRefresh( EFalse );
    if ( IsStartedByLauncher() )
        {
    	if ( !iRefreshFromNetworkDenied && 
    	     !RestartedFromSelfUpdate() )
    	    {
    		if ( LocalNodesExpiredL() )
    		    {
    			allowRefresh = ETrue;
    		    }
    	    }
        }
    else   
        {
        // started by an application
        if ( iFilter )
    	    {
    	    CIAUpdateParameters* params( iFilter->FilterParams() );
    	    if ( params )
    	        {
    	    	if ( params->Refresh() )
    	    	    {
    	    	    if ( ForcedRefresh() )
    	    	        {
    	    	        allowRefresh = ETrue;
    	    	        }
    	    	    else
    	    	        {
      	      		    // Check from the central repocitory what are the automatic checking and 
                        // roaming settings.     
                        TInt autoUpdateCheckValue( 0 );
                        CRepository* cenrep( 
                             CRepository::NewLC( KCRUidIAUpdateSettings ) );
                        // Notice, that KIAUpdateSettingAutoUpdateCheck can give following values
                        // 0 = No automatic update check
                        // 1 = Automatic update check enabled when not roaming
                        // 2 = Automatic update enabled

                        User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, 
                                                         autoUpdateCheckValue ) );
                        CleanupStack::PopAndDestroy( cenrep );
                        if ( ( autoUpdateCheckValue == EIAUpdateSettingValueEnable ) || 
    	                    ( autoUpdateCheckValue == EIAUpdateSettingValueDisableWhenRoaming &&
    	                      !iRoamingHandler->IsRoaming() ) )
                            {
                    	    allowRefresh = ETrue;
                            }
    	    	        }
     	    	    }
    	        }
    	    }
        }
    
    IAUPDATE_TRACE_1("[IAUPDATE] allowRefresh: %d", allowRefresh );    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::AllowNetworkRefreshL() end");  
    return allowRefresh;
    }



// ---------------------------------------------------------------------------
// CIAUpdateUiController::LocalNodesExpiredL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::LocalNodesExpiredL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::LocalNodesExpiredL() begin");

    // Notice, that the values for the iControllerFile, that are used here for
    // expiration checkings, are set after successfull completion of refresh
    // operation. See, UpdateControllerFileL that is called when operation
    // finishes successfully. 

    const TTimeIntervalHours KExpirationDeltaInHours( 
        ConfigData().GridRefreshDeltaHours() );
    
    // Make sure that the most current data is gotten from the file.
    iControllerFile->ReadControllerDataL();

    TLanguage currentLanguage = User::Language();
    TLanguage lastTimeLanguage = iControllerFile->Language();
    if ( lastTimeLanguage != currentLanguage )
        {
        // Think nodes as expired because the language 
        // has been changed in the phone. By updating the nodes,
        // the items in UI will show correct language.
        // No need to set the language to the controller file here.
        // The value is set when the operation is successfully completed.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::LocalNodesExpiredL() ETrue end");
        return ETrue;
        }
        
    // Because the language check has passed,
    // check also the time expirations.

    // Get the last recorded refresh time from the controller file    
    TTime lastRefreshTime( iControllerFile->RefreshTime() );

    // Initialize expireTime with the last refresh time. Notice,
    // that this expire time will be increased below with the correct 
    // value
    TTime expireTime( lastRefreshTime );

    expireTime += KExpirationDeltaInHours;

    // Get the current time.
    TTime universalTime;
    universalTime.UniversalTime();

    if ( expireTime < universalTime
         || lastRefreshTime > universalTime )
        {
        // Database is expired because current time has passed the
        // expiration time. Also, sanity check is made. If
        // last refresh time is larger than current time, then the
        // last refresh value has been set wrong, and the database can
        // be thought as expired. This might be the case if the user has
        // changed the time in the phone.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::LocalNodesExpiredL() ETrue end");
        return ETrue;
        }
    else
        {
        // Database is not expired yet.
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::LocalNodesExpiredL() EFalse end");
        return EFalse;        
        } 
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::UpdateControllerFileL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::UpdateControllerFileL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateControllerFileL() begin");

    // This function is called after the refresh operation has
    // completed.

    // Check if local nodes were updated from the server.
    // Notice, that last refresh time and language are both checked
    // here when LocalNodesExpiredL() is called from AllowNetworkRefreshL.
    // AllowNetworkRefreshL is called here because autoupdate and roaming checks
    // should also be done when deciding if the nodes were updated.
    if ( AllowNetworkRefreshL() )
        {
        // Update controller file with current information 
        // because old values were expired.
        iControllerFile->SetCurrentData();

        // Because data is not up to date,
        // save new data into the file.
        iControllerFile->WriteControllerDataL();        
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateControllerFileL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::IsStartedByLauncher
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::IsStartedByLauncher()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::IsStartedByLauncher()");

    if ( iRequestType == IAUpdateUiDefines::ENoRequest )
        {
        IAUPDATE_TRACE("[IAUPDATE] Started by launcher");
        // This operation was started from the application that was
        // started by launcher
        // Currently it means that launching is originated 
        // from application grid or background checker. There is only
        // one iaupdate instance started by launcher
        return ETrue;
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] NOT started by launcher");
        // According to the request type, 
        // API was used to start the operation.
        return EFalse;        
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::StartPossibleSelfUpdateL
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::StartPossibleSelfUpdateL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartPossibleSelfUpdateL() begin");

    // If there exist some self updater files that need to be installed
    // separately, start self updating process.
    TBool started( 
        iController->
            StartPossibleSelfUpdateL( iNodeIndex, 
                                      iSelectedNodesArray.Count(),
                                      iSelectedNodesArray,
                                      EFalse ) );
            
    // Because self updater has gotten all the information it needs now,
    // we can reset the data that is meant for the updater.
    iController->ResetSelfUpdate();

    IAUPDATE_TRACE_1("[IAUPDATE] started: %d", started );
    if ( started )
        {
        // Because self update was started, save possible parameters data 
        // for the possible restart of IAD. The, IAD can continue correctly.
        ParamsWriteFileL();
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::StartPossibleSelfUpdateL() end");
    
    return started;  
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::UpdateFailedSelfUpdaterInfoToPurchaseHistoryL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::UpdateFailedSelfUpdaterInfoToPurchaseHistoryL( TInt aErrorCode )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateFailedSelfUpdaterInfoToPurchaseHistoryL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aErrorCode );

    // Check all the items that have been updated so far.
    // And, set the error info to purchase history accordingly.
    for ( TInt i = 0; 
          i < iSelectedNodesArray.Count()
          && i < iNodeIndex + 1;
          ++i )
        {
        MIAUpdateNode* node( iSelectedNodesArray[ i ] );
        if ( node->IsSelfUpdate() )
            {
            TBool installSuccess( node->IsInstalled() );
            if ( !installSuccess  )
                {
                // Notice, that we can just set the purchase history here for
                // the self updater items. If some of the items were installed,
                // before self updater items, then their information was already
                // correctly into the history.
                TInt error( aErrorCode );
                if ( aErrorCode == KErrNone )
                    {
                    error = KErrUnknown;
                    }

                // Because we are handling nodes that are visible in UI,
                // we can force the node as visible in history.                    
                node->Base().SetInstallStatusToPurchaseHistoryL( error, ETrue );
                }
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::UpdateFailedSelfUpdaterInfoToPurchaseHistoryL() end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::HandleUserCancelL
// 
// ---------------------------------------------------------------------------
//      
void CIAUpdateUiController::HandleUserCancelL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleUserCancelL() begin");

    // Get the current state.
    // CancelOperation will set a new value for the state and we do not 
    // want to use that.
    TState state( iState );
    
    CancelOperation();
    
    // Set the flag on. 
    // So, dialogs are handled correctly.
    iCancelling = ETrue;
    
    if ( state == EDownloading || state == EInstalling )
        {
        IAUPDATE_TRACE("[IAUPDATE] Cancel pressed while updating");
        // Update flow is completed here.
        // Notice, that this will also start the report sending,
        // but we do not wait the sending to be complete before continuing.
        UpdateCompleteL( KErrCancel );
        }
    else if ( state == ESendingReport )
        {
        IAUPDATE_TRACE("[IAUPDATE] Dialog cancel called while sending reports");
        // Reports are sent only when update flow has finished.
        // So, UpdateCompleteL has already been called and an update flow
        // dialog is still being shown. Because cancel is called, complete 
        // flow without waiting for the report sending to finish.
        EndUpdateFlowL( KErrCancel ); 
        }

    // Set the flag off because cancellation is over.
    iCancelling = EFalse;

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::HandleUserCancelL() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::RestartedFromSelfUpdate
// 
// ---------------------------------------------------------------------------
//      
TBool CIAUpdateUiController::RestartedFromSelfUpdate()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::RestartedFromSelfUpdate() begin");
	
    TBool restarted( EFalse );
    
    CIAUpdateRestartInfo* info( iController->SelfUpdateRestartInfo() ); 
    if ( info ) 
        { 
        // Restart info was available. 
        delete info; 
        info = NULL; 
        restarted = ETrue; 
        }    

	IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateUiController::RestartedFromSelfUpdate() end: %d", 
	                 restarted);
    
    return restarted;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::ParamsReadAndRemoveFileL
// 
// ---------------------------------------------------------------------------
//  
CIAUpdateParameters* CIAUpdateUiController::ParamsReadAndRemoveFileL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsReadAndRemoveFileL() begin");
	
    CIAUpdateParameters* params( NULL );

    CIAUpdateParametersFileManager* mgr( CIAUpdateParametersFileManager::NewLC() );
    params = mgr->ReadL();
    mgr->RemoveFile();
    CleanupStack::PopAndDestroy( mgr );
        
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsReadAndRemoveFileL() end");    
    
    return params;
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::ForcedRefresh
// 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::ForcedRefresh() const
    {
    return iForcedRefresh;
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::SetForcedRefresh
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::SetForcedRefresh( TBool aForcedRefresh )
    {
    iForcedRefresh = aForcedRefresh;
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::RefreshNodeList
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::RefreshNodeList()
    {
    // Restore list after update, that nodes are shown in original sequence
    iFilter->RestoreNodeListL( iNodes );
    // Remove installed nodes from the node array.
    // Set temp UI state to normal for remaining nodes 
    for ( TInt i = iNodes.Count() - 1; i >= 0; --i ) 
        {   
        MIAUpdateNode* node = iNodes[ i ];
        if ( node->IsInstalled() ) 
            {
            iNodes.Remove( i );
            }
        else
            {
            node->SetUiState( MIAUpdateNode::ENormal );        
            }
        }
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::ParamsWriteFileL
// 
// ---------------------------------------------------------------------------
//  
void CIAUpdateUiController::ParamsWriteFileL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsWriteFileL() begin");
	
	// Ownership is not transferred here.
	CIAUpdateParameters* params( iFilter->FilterParams() );
	if ( params )
	    {
        CIAUpdateParametersFileManager* mgr( 
            CIAUpdateParametersFileManager::NewLC() );
        mgr->WriteL( *params );
        CleanupStack::PopAndDestroy( mgr );
	    }

	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsWriteFileL() end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::ParamsRemoveFileL
// 
// ---------------------------------------------------------------------------
//  
void CIAUpdateUiController::ParamsRemoveFileL()
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsRemoveFileL() begin");
	
    CIAUpdateParametersFileManager* mgr( 
        CIAUpdateParametersFileManager::NewL() );
    mgr->RemoveFile();
    delete mgr;
    mgr = NULL;
            
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::ParamsRemoveFileL() end");    
    }


// ---------------------------------------------------------------------------
// CIAUpdateUiController::CreateSelectedNodesArrayL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateUiController::CreateSelectedNodesArrayL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CreateSelectedNodesArrayL() begin");
    iSelectedNodesArray.Reset();
    for ( TInt i = 0; i < iNodes.Count(); ++i )
        {
        if ( iNodes[ i ]->Base().IsSelected() )
            {
            iSelectedNodesArray.AppendL( iNodes[ i ] );
            }
        }

    // Dependency nodes to be updated before their dependants.
    // Also, self update nodes should be handled correctly.
    iSelectedNodesArray.Sort( 
          TLinearOrder< MIAUpdateNode >( &SortSelectedNodes ) );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::CreateSelectedNodesArrayL() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::IAUpdateEnabledL()
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::IAUpdateEnabledL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::IAUpdateEnabledL() begin");
    TBool enabled = EFalse;
    RFeatureControl featureControl;
    TInt error( KErrNone );

    error = featureControl.Connect();
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateUiController::IAUpdateEnabledL() Connection to the Feature Manager Server: %d", error);
    
    User::LeaveIfError( error );
    
    TUid iaupdatefeature;
    iaupdatefeature.iUid = KFeatureIdIAUpdate;
    
    TInt ret = featureControl.FeatureSupported( iaupdatefeature );
    if ( ret == KFeatureSupported )
        {
        enabled = ETrue;
        }
    featureControl.Close(); 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateUiController::IAUpdateEnabledL() end");
    return enabled;        
    }

// ---------------------------------------------------------------------------
// CIAUpdateUiController::AutomaticConnectionWhenRoamingL()
// ---------------------------------------------------------------------------
//
TBool CIAUpdateUiController::AutomaticConnectionWhenRoamingL() const
    {
    TBool automaticConnection = EFalse;
    RCmManager cmManager;
    cmManager.OpenLC();
    TCmGenConnSettings genConnSettings;
    cmManager.ReadGenConnSettingsL( genConnSettings );
    CleanupStack::PopAndDestroy( &cmManager );
    if ( genConnSettings.iCellularDataUsageVisitor == ECmCellularDataUsageAutomatic )
        {
        automaticConnection = ETrue;
        }
    return automaticConnection;
    }


// End of File  
