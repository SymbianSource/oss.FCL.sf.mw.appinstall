/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:   This module contains the implementation of IAUpdateEngine
 *                class member functions.
 *
 */

#include <qapplication.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <eikenv.h>
#include <centralrepository.h>
#include <cmmanager.h>
#include <cmdestination.h>
#include <rconnmon.h>
#include <apgwgnam.h>
#include <starterclient.h>

#include "iaupdateengine.h"
#include "iaupdateserviceprovider.h"
#include "iaupdateuicontroller.h"
#include "iaupdatefwupdatehandler.h"
#include "iaupdategloballockhandler.h"
#include "iaupdatenodefilter.h"
#include "iaupdateresult.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdateuiconfigdata.h"
#include "iaupdatequeryhistory.h"
#include "iaupdateparameters.h"
#include "iaupdateagreement.h"
#include "iaupdateautomaticcheck.h"
#include "iaupdateresultsdialog.h"
#include "iaupdatedebug.h"

IAUpdateEngine::IAUpdateEngine(QObject *parent) :
    QObject(parent)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::IAUpdateEngine() begin");
    iController = NULL;
    iFwUpdateHandler = NULL;
    iGlobalLockHandler = NULL;
    iAutomaticCheck = NULL;
    iIdle = NULL;
    iIdleAutCheck = NULL;
    iEikEnv = CEikonEnv::Static();
    mUpdateNow = false;
    mRequestIssued = false;
    mStartedFromApplication = false;
    mUiRefreshAllowed = true;
    mWgId = 0;
    mUpdatequeryUid = 0;
    mDialogState = NoDialog;
    mResultsDialog = NULL;
    mServiceProvider = NULL;
    mServiceProvider = new IAUpdateServiceProvider(*this);
    connect(mServiceProvider, SIGNAL(clientDisconnected()), this,
            SLOT(handleAllClientsClosed()));
    TRAP_IGNORE( iController = CIAUpdateUiController::NewL( *this ));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::IAUpdateEngine() end");
    }

IAUpdateEngine::~IAUpdateEngine()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::~IAUpdateEngine() begin");
    InformRequestObserver(KErrCancel);
    if (iIdle)
        {
        delete iIdle;
        }
    if (iIdleAutCheck)
        {
        delete iIdleAutCheck;
        }
    if (iGlobalLockHandler)
        {
        delete iGlobalLockHandler;
        }
    if (iAutomaticCheck)
        {
        delete iAutomaticCheck;
        }
    if (iController)
        {
        delete iController;
        }
    if (iFwUpdateHandler)
        {
        delete iFwUpdateHandler;
        }
    if (mServiceProvider)
        {
        delete mServiceProvider;
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::~IAUpdateEngine() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::StartedByLauncherL
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::StartedByLauncherL(bool aRefreshFromNetworkDenied)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartedByLauncherL() begin");
    mRequestIssued = true;
    mRequestType = IAUpdateUiDefines::ENoRequest;
    iController->SetRequestType(mRequestType);
    SetVisibleL(true);
    CIAUpdateParameters* params = iController->ParamsReadAndRemoveFileL();
    iController->CheckUpdatesDeferredL(params, aRefreshFromNetworkDenied);
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::CheckUpdatesRequestL
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::CheckUpdatesRequestL(int wgid,
        CIAUpdateParameters* aFilterParams, bool aForcedRefresh)

    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::CheckUpdatesRequestL() begin");
    SetClientWgId(wgid);
    mRequestIssued = true;
    mStartedFromApplication = true;
    CleanupStack::PushL(aFilterParams);
    if (wgid > 0)
        {
        HideApplicationInFSWL(true);
        }
    CleanupStack::Pop(aFilterParams);

    if (!aFilterParams->ShowProgress())
        {
        iEikEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNeverAtFront);
        }

    mRequestType = IAUpdateUiDefines::ECheckUpdates;
    iController->SetRequestType(mRequestType);
    iController->SetForcedRefresh(aForcedRefresh);

    iController->CheckUpdatesDeferredL(aFilterParams, false);

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::CheckUpdatesRequestL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowUpdatesRequestL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateEngine::ShowUpdatesRequestL(int wgid,
        CIAUpdateParameters* aFilterParams)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdatesRequestL() begin");
    SetClientWgId(wgid);
    mRequestIssued = true;
    //    delete iBackgroundTimer;
    //    iBackgroundTimer = NULL;
    mStartedFromApplication = true;
    CleanupStack::PushL(aFilterParams);
    if (wgid > 0)
        {
        HideApplicationInFSWL(true);
        }

    //StatusPane()->MakeVisible( true );
    iEikEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityNormal);

    //iRequestObserver = &aObserver;
    mRequestType = IAUpdateUiDefines::EShowUpdates;
    iController->SetRequestType(mRequestType);

    //if ( !iMainView )
    //    {
    //    iMainView  = CIAUpdateMainView::NewL( ClientRect() ); 
    //    AddViewL( iMainView );
    //    }


    // by pushing object to cleanup stack its destructor is called if leave happens
    // so global lock issued by this instance can be released in destructor of CIAUpdateGlobalLockHandler
    CIAUpdateGlobalLockHandler* globalLockHandler =
            CIAUpdateGlobalLockHandler::NewLC();
    if (!globalLockHandler->InUseByAnotherInstanceL())
        {
        globalLockHandler->SetToInUseForAnotherInstancesL(true);
        CleanupStack::Pop(globalLockHandler);
        CleanupStack::Pop(aFilterParams);
        CleanupStack::PushL(globalLockHandler);
        iController->CheckUpdatesDeferredL(aFilterParams, false);
        CleanupStack::Pop(globalLockHandler);
        delete iGlobalLockHandler;
        iGlobalLockHandler = globalLockHandler;
        //now possible deletion of iGlobalLockHandler in leave situation is handled
        //in HandleLeaveErrorL() and HandleLeaveErrorWithoutLeave methods. 
        }
    else
        {
        CleanupStack::PopAndDestroy(globalLockHandler);
        CleanupStack::PopAndDestroy(aFilterParams);
        // locked by another IAD instance, nothing else to do than just complete client's request.  
        InformRequestObserver(KErrNone);
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdatesRequestL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowUpdateQueryRequestL
// 
// -----------------------------------------------------------------------------
//     
void IAUpdateEngine::ShowUpdateQueryRequestL(int wgid, uint aUid)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryRequestL begin");
    SetClientWgId(wgid);
    mRequestIssued = true;
    mStartedFromApplication = true;
    mUpdatequeryUid = aUid;
    mUpdateNow = false;
    if (wgid > 0)
        {
        HideApplicationInFSWL(true);
        }
    mRequestType = IAUpdateUiDefines::EUpdateQuery;

    delete iIdle;
    iIdle = NULL;
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    iIdle->Start(TCallBack(UpdateQueryCallbackL, this));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryRequestL end")
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::StartUpdate
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::StartUpdate(bool aFirmwareUpdate)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartUpdate() begin");
    if (aFirmwareUpdate)
        {
        if (!iFwUpdateHandler)
            {
            TRAP_IGNORE( CIAUpdateFWUpdateHandler::NewL() );
            }
        if (iFwUpdateHandler)
            {
            iFwUpdateHandler->FirmWareUpdatewithFOTA();
            }
        }
    else
        {
        // by pushing object to cleanup stack it's destructor is called if leave happens
        // so global lock issued by this instance can be released in destructor of CIAUpdateGlobalLockHandler
        CIAUpdateGlobalLockHandler* globalLockHandler =
                CIAUpdateGlobalLockHandler::NewLC();
        if (!globalLockHandler->InUseByAnotherInstanceL())
            {
            globalLockHandler->SetToInUseForAnotherInstancesL(true);
            // No need to be totally silent since the updating is started
            // by user.
            SetDefaultConnectionMethodL(false);
            iController->StartUpdateL();
            CleanupStack::Pop(globalLockHandler);
            delete iGlobalLockHandler;
            iGlobalLockHandler = globalLockHandler;
            //now possible deletion of iGlobalLockHandler in leave situation is handled
            //in HandleLeaveErrorL() and HandleLeaveErrorWithoutLeave methods. 
            }
        else
            {
            CleanupStack::PopAndDestroy(globalLockHandler);
            }
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartUpdate() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::SetVisibleL
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::SetVisibleL(bool /*aVisible*/)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetVisibleL() begin");
    //IAUPDATE_TRACE_1("[IAUPDATE] visible: %d", aVisible );
    /*if ( aVisible )
     {
     if ( iEikonEnv->RootWin().OrdinalPosition() != 0 || iEikonEnv->RootWin().OrdinalPriority() != ECoeWinPriorityNormal )
     {
     iEikonEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );
     }
     
     StatusPane()->MakeVisible( ETrue );
     iMainView  = CIAUpdateMainView::NewL( ClientRect() ); 
     AddViewL( iMainView );
     ActivateLocalViewL( TUid::Uid( EIAUpdateMainViewId ) );
     }
     else
     {
     iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );
     StatusPane()->MakeVisible( EFalse );
     }*/
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetVisibleL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::SetClientWgId
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateEngine::SetClientWgId(int aWgId)
    {
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateEngine::SetClientWgId() wgId %d", aWgId );
    mWgId = aWgId;
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ClientInBackgroundL
// 
// -----------------------------------------------------------------------------
// 
bool IAUpdateEngine::ClientInBackgroundL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ClientInBackgroundL() begin");
    bool inBackground = false;
    if (mWgId > 0)
        {
        CArrayFixFlat<int>* wgArray = new (ELeave) CArrayFixFlat<int> (10);
        CleanupStack::PushL(wgArray);
        User::LeaveIfError(iEikEnv->WsSession().WindowGroupList(0, wgArray));
        int ownWgId = iEikEnv->RootWin().Identifier();
        if ((wgArray->At(0) != ownWgId) && (wgArray->At(0) != mWgId))
            {
            inBackground = true;
            }
        CleanupStack::PopAndDestroy(wgArray);
        }
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateEngine::ClientInBackgroundL() inBackground: %d", inBackground );
    return inBackground;
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::handleAllClientsClosed()
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::handleAllClientsClosed()
    {
    qApp->quit();
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::dialogFinished
// Called when dialog is finished.
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::dialogFinished(HbAction *action)
    {
    DialogState dialogState = mDialogState;
    mDialogState = NoDialog;

    switch (dialogState)
        {
        case Results:
            mUiRefreshAllowed = true;
            if (iController->ResultsInfo().iRebootAfterInstall)
                {
                ShowRebootDialogL();
                }
            else
                {
                if (!DoPossibleApplicationClose())
                    {
                    iController->RefreshNodeList();
                    RefreshUI();
                    }
                }
            break;
        case RebootQuery:
            if (action == mPrimaryAction)
                {
                RStarterSession startersession;
                if (startersession.Connect() == KErrNone)
                    {
                    startersession.Reset(RStarterSession::EUnknownReset);
                    startersession.Close();
                    }
                }
            else
                {
                if (!DoPossibleApplicationClose())
                    {
                    iController->RefreshNodeList();
                    RefreshUI();
                    }
                }
            break;
        case ShowUpdateQuery:
            if (action == mPrimaryAction)
                {
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::dialogFinished() Now");
                mUpdateNow = true;
                }
            else if (action == mSecondaryAction)
                {
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::dialogFinished() Later");
                CIAUpdateQueryHistory* updateQueryHistory =
                        CIAUpdateQueryHistory::NewL();
                CleanupStack::PushL(updateQueryHistory);
                updateQueryHistory->SetTimeL(mUpdatequeryUid);
                CleanupStack::PopAndDestroy(updateQueryHistory);
                }
            InformRequestObserver(KErrNone);
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::StartupComplete
// 
// -----------------------------------------------------------------------------
//    
void IAUpdateEngine::StartupComplete(TInt aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupComplete() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError);

    if (aError != KErrNone)
        {
        HandleLeaveErrorWithoutLeave(aError);
        }
    else
        {
        TRAPD( err, StartupCompleteL() );
        if (err != KErrNone)
            {
            HandleLeaveErrorWithoutLeave(err);
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupComplete() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::StartupCompleteL
// 
// -----------------------------------------------------------------------------
//    
void IAUpdateEngine::StartupCompleteL()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupCompleteL() begin");

    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    iGlobalLockHandler = CIAUpdateGlobalLockHandler::NewL();
    if (!iGlobalLockHandler->InUseByAnotherInstanceL())
        {
        bool totalSilent(false);
        if (mRequestType == IAUpdateUiDefines::ECheckUpdates)
            {
            if (iController->Filter())
                {
                if (iController->Filter()->FilterParams())
                    {
                    if (iController->Filter()->FilterParams()->Refresh())
                        {
                        if (!iController->ForcedRefresh())
                            {
                            //from bgchecker, make it silent
                            totalSilent = true;
                            }
                        }
                    }
                }
            }
        SetDefaultConnectionMethodL(totalSilent);
        iGlobalLockHandler->SetToInUseForAnotherInstancesL(true);
        iController->StartRefreshL();
        }
    else
        {
        RefreshCompleteL(true, KErrServerBusy);
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupCompleteL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::HandleLeaveErrorL
// 
// -----------------------------------------------------------------------------
//        
void IAUpdateEngine::HandleLeaveErrorL(TInt aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorL() begin");
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    // client request is completed before leave in case of leave error
    if (aError != KErrNone)
        {
        InformRequestObserver(aError);
        User::Leave(aError);
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::HandleLeaveErrorWithoutLeave
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::HandleLeaveErrorWithoutLeave(TInt aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorWithoutLeave() begin");
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    if (aError != KErrNone)
        {
        InformRequestObserver(aError);
        }
    if (aError == KErrDiskFull)
        {
        //TRAP_IGNORE( ShowGlobalErrorNoteL( aError ) );
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorWithoutLeave end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::RefreshUI
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateEngine::RefreshUI()
    {
    emit refresh(iController->Nodes(), iController->FwNodes(), KErrNone);
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::RefreshCompleteL
// 
// -----------------------------------------------------------------------------
//      
void IAUpdateEngine::RefreshCompleteL(TBool /*aWithViewActivation*/,
        TInt aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::RefreshCompleteL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;

    if (mRequestType == IAUpdateUiDefines::ECheckUpdates)
        {
        InformRequestObserver(aError);
        }
    else
        {
        emit refresh(iController->Nodes(), iController->FwNodes(), aError);
        
        // inform bgchecker to clear indicator menu
        User::LeaveIfError( 
                RProperty::Set(KPSUidBgc, KIAUpdateBGNotifyIndicatorRemove, 0) );
        
        //if ( aWithViewActivation)
        //  {
        //  ActivateLocalViewL( TUid::Uid( EIAUpdateMainViewId ) );
        //  }
        CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
        bool agreementAccepted = agreement->AgreementAcceptedL();
        if (iController->ForcedRefresh())
            {
            if (!agreementAccepted)
                {
                agreement->SetAgreementAcceptedL();
                }
            }
        CleanupStack::PopAndDestroy(agreement);
        // By calling CIdle possible waiting dialog can be closed before
        // automatic check where a new dialog may be launched
        delete iIdleAutCheck;
        iIdleAutCheck = NULL;
        iIdleAutCheck = CIdle::NewL(CActive::EPriorityIdle);
        iIdleAutCheck->Start(TCallBack(AutomaticCheckCallbackL, this));
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::RefreshCompleteL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::UpdateCompleteL
// 
// -----------------------------------------------------------------------------
//     
void IAUpdateEngine::UpdateCompleteL(TInt aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateCompleteL begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    if (mRequestType != IAUpdateUiDefines::ENoRequest)
        {
        InformRequestObserver(aError);
        }

    ShowResultsDialogL();

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateCompleteL end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowResultsDialogL
// 
// -----------------------------------------------------------------------------
//   
void IAUpdateEngine::ShowResultsDialogL()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowResultsDialogL() begin");

    mUiRefreshAllowed = false;
    mResultsDialog = new IAUpdateResultsDialog(this);
    mResultsDialog->showResults(iController->ResultsInfo(), this,
            SLOT(dialogFinished(HbAction*)));
    mDialogState = Results;

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowResultsDialogL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowRebootDialogL
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::ShowRebootDialogL()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowRebootDialogL() begin");

    HbMessageBox *messageBox = new HbMessageBox(
            HbMessageBox::MessageTypeQuestion);
    messageBox->setText(QString("Phone restart needed. Restart now?"));
    int actionCount = messageBox->actions().count();
    for (int i = actionCount - 1; i >= 0; i--)
        {
        messageBox->removeAction(messageBox->actions().at(i));
        }
    mPrimaryAction = NULL;
    mPrimaryAction = new HbAction("Ok");
    HbAction *secondaryAction = NULL;
    secondaryAction = new HbAction("Cancel");

    messageBox->addAction(mPrimaryAction);
    messageBox->addAction(secondaryAction);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->open(this, SLOT(dialogFinished(HbAction*)));
    mDialogState = RebootQuery;

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowRebootDialogL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::InformRequestObserver
// 
// -----------------------------------------------------------------------------
//      
void IAUpdateEngine::InformRequestObserver(int aError)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::InformRequestObserver() begin");

    //if ( iRequestObserver )
    if (mRequestIssued)
        {
        if (iController->ClosingAllowedByClient())
            {
            if (mRequestType != IAUpdateUiDefines::ENoRequest)
                {
                //                if ( iRequestType == IAUpdateUiDefines::EUpdateQuery && iUpdateNow )
                //                    {
                //                  if ( !iBackgroundTimer )
                //                        {
                //                        iBackgroundTimer = CIAUpdateUITimer::NewL( *this, CIAUpdateUITimer::EBackgroundDelay );
                //                        }
                //                 if ( !iBackgroundTimer->IsActive() )
                //                        {
                //                        iBackgroundTimer->After( 500000 );
                //                        }
                //                    }
                //                 else
                //                    {
                iEikEnv->RootWin().SetOrdinalPosition(-1,
                        ECoeWinPriorityNeverAtFront);
                //                    }
                }
            }

        switch (mRequestType)
            {
            case IAUpdateUiDefines::ENoRequest:
                {
                mServiceProvider->completeLauncherLaunch(aError);
                break;
                }
            case IAUpdateUiDefines::ECheckUpdates:
                {
                mServiceProvider->completeCheckUpdates(
                        iController->CountOfAvailableUpdates(), aError);
                break;
                }
            case IAUpdateUiDefines::EShowUpdates:
                {
                CIAUpdateResult* result(NULL);
                TRAPD( error, result = CIAUpdateResult::NewL() )
                if (result)
                    {
                    TIAUpdateResultsInfo resultsInfo(
                            iController->ResultsInfo());
                    IAUPDATE_TRACE_3("[IAUPDATE] IAUpdateEngine::InformRequestObserver succeed: %d failed: %d  cancelled: %d",
                            resultsInfo.iCountSuccessfull,
                            resultsInfo.iCountFailed,
                            resultsInfo.iCountCancelled );
                    result->SetSuccessCount(resultsInfo.iCountSuccessfull);
                    result->SetFailCount(resultsInfo.iCountFailed);
                    result->SetCancelCount(resultsInfo.iCountCancelled);
                    mServiceProvider->completeShowUpdates(result, aError);
                    // Ownership of result is transferred here.
                    }
                else
                    {
                    mServiceProvider->completeShowUpdates(NULL, error);
                    }
                break;
                }
            case IAUpdateUiDefines::EUpdateQuery:
                {
                mServiceProvider->completeUpdateQuery(mUpdateNow, aError);
                break;
                }
            default:
                {
                break;
                }
            }

        mRequestIssued = false;
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::InformRequestObserver() end");
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::SetDefaultConnectionMethodL
// Sets the connection method for the update network connection.
// ---------------------------------------------------------------------------
//
void IAUpdateEngine::SetDefaultConnectionMethodL(bool aTotalSilent)
    {
    if (aTotalSilent)
        {
        // from back ground checker, choose the IAP to make the internet access silent
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() begin");

        uint connectionMethodId(0);
        int connMethodId(0);

        // Let's first check whether cenrep contains SNAP id other than zero
        CRepository* cenrep(CRepository::NewLC(KCRUidIAUpdateSettings));
        User::LeaveIfError(cenrep->Get(KIAUpdateAccessPoint, connMethodId));
        CleanupStack::PopAndDestroy(cenrep);
        cenrep = NULL;

        RCmManager cmManager;
        cmManager.OpenL();
        CleanupClosePushL(cmManager);

        if (connMethodId == -1)
            {
            IAUPDATE_TRACE("[IAUPDATE] user chooses default connection, use IAP logic");

            //check what is the default connection by users     

            TCmDefConnValue DCSetting;
            cmManager.ReadDefConnL(DCSetting);

            switch (DCSetting.iType)
                {
                case ECmDefConnAlwaysAsk:
                case ECmDefConnAskOnce:
                    {
                    //go with the best IAP under internet snap
                    connectionMethodId = GetBestIAPInAllSNAPsL(cmManager);
                    break;
                    }
                case ECmDefConnDestination:
                    {
                    //go with the best IAP under this snap
                    connectionMethodId = GetBestIAPInThisSNAPL(cmManager,
                            DCSetting.iId);
                    break;
                    }
                case ECmDefConnConnectionMethod:
                    {
                    //go with the best IAP under this snap
                    connectionMethodId = DCSetting.iId;
                    break;
                    }
                }
            }
        else if (connMethodId == 0)
            {
            //no choice from user, we go with the best IAP under Internent SNAP
            connectionMethodId = GetBestIAPInAllSNAPsL(cmManager);
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] use chooses a snap");
            // It was some SNAP value
            connectionMethodId = GetBestIAPInThisSNAPL(cmManager,
                    connMethodId);
            }

        CleanupStack::PopAndDestroy(&cmManager);

        if (connectionMethodId != 0)
            {
            TIAUpdateConnectionMethod
                    connectionMethod(
                            connectionMethodId,
                            TIAUpdateConnectionMethod::EConnectionMethodTypeAccessPoint);

            iController->SetDefaultConnectionMethodL(connectionMethod);
            }
        else
            {
            //In the totally silent case, if no usable IAP, we complete the check update with 0 updates.
            //the bgchecker will try again later after 1 month. 
            //The LEAVE will be catched up later and complete the request from background checker.
            User::LeaveIfError(KErrNotFound);
            }
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() end");
        }
    else
        {
        // from grid, use the old logic
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() begin");
        uint connectionMethodId(0);
        int connMethodId(0);

        // Set initial value to always ask
        int connectionMethodType(
                TIAUpdateConnectionMethod::EConnectionMethodTypeAlwaysAsk);
        bool needSaving(false);

        // Let's first check whether cenrep contains SNAP id other than zero
        CRepository* cenrep(CRepository::NewLC(KCRUidIAUpdateSettings));
        User::LeaveIfError(cenrep->Get(KIAUpdateAccessPoint, connMethodId));
        CleanupStack::PopAndDestroy(cenrep);
        cenrep = NULL;

        if (connMethodId == -1)
            {
            IAUPDATE_TRACE("[IAUPDATE] user chooses default connection, use IAP logic");

            connectionMethodId = 0;
            connectionMethodType
                    = TIAUpdateConnectionMethod::EConnectionMethodTypeDefault;
            }
        else if (connMethodId == 0)
            {
            IAUPDATE_TRACE("[IAUPDATE] use chooses nothing, use internal IAP logic");
            //if nothing is set by user, use our new logic
            //SetDefaultConnectionMethod2L();
            //return;
            // CenRep didn't contain any SNAP id. Let's try Internet SNAP then.

            RCmManager cmManager;
            cmManager.OpenL();
            CleanupClosePushL(cmManager);
            iDestIdArray.Reset();
            cmManager.AllDestinationsL(iDestIdArray);

            for (int i = 0; i < iDestIdArray.Count(); i++)
                {
                RCmDestination dest = cmManager.DestinationL(iDestIdArray[i]);
                CleanupClosePushL(dest);

                if (dest.MetadataL(CMManager::ESnapMetadataInternet))
                    {
                    // Check whether Internet SNAP contains any IAP.
                    if (dest.ConnectionMethodCount() > 0)
                        {
                        connectionMethodId = iDestIdArray[i];
                        needSaving = true;
                        IAUPDATE_TRACE_1("[IAUPDATE] connectionMethodId: %d", connectionMethodId );
                        }
                    CleanupStack::PopAndDestroy(&dest);
                    break;
                    }

                CleanupStack::PopAndDestroy(&dest);
                }
            iDestIdArray.Reset();
            CleanupStack::PopAndDestroy(&cmManager);
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] use chooses a snap");
            // It was some SNAP value
            connectionMethodId = connMethodId;
            }

        if (connectionMethodId > 0)
            {
            // We have now some valid SNAP id, either from CenRep or Internet SNAP
            connectionMethodType
                    = TIAUpdateConnectionMethod::EConnectionMethodTypeDestination;
            // Save to cenrep if needed
            if (needSaving)
                {
                cenrep = CRepository::NewLC(KCRUidIAUpdateSettings);
                int err = cenrep->StartTransaction(
                        CRepository::EReadWriteTransaction);
                User::LeaveIfError(err);
                cenrep->CleanupCancelTransactionPushL();

                connMethodId = connectionMethodId;
                err = cenrep->Set(KIAUpdateAccessPoint, connMethodId);
                User::LeaveIfError(err);
                TUint32 ignore = KErrNone;
                User::LeaveIfError(cenrep->CommitTransaction(ignore));
                CleanupStack::PopAndDestroy(); // CleanupCancelTransactionPushL()
                CleanupStack::PopAndDestroy(cenrep);
                }
            }

        TIAUpdateConnectionMethod
                connectionMethod(
                        connectionMethodId,
                        static_cast<TIAUpdateConnectionMethod::TConnectionMethodType> (connectionMethodType));

        iController->SetDefaultConnectionMethodL(connectionMethod);

        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() end");
        }
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::GetBestIAPInAllSNAPsL
// Sets the best IAP from all snaps
// ---------------------------------------------------------------------------
//
uint IAUpdateEngine::GetBestIAPInAllSNAPsL(RCmManager& aCmManager)
    {
    //go with internet SNAP first.
    uint IAPID = 0;
    IAPID = GetBestIAPInInternetSNAPL(aCmManager);

    if (IAPID)
        {
        return IAPID;
        }

    //select IAP from rest of the SNAPs
    iDestIdArray.Reset();
    aCmManager.AllDestinationsL(iDestIdArray);

    for (int i = 0; i < iDestIdArray.Count(); i++)
        {
        uint SNAPID = iDestIdArray[i];
        IAPID = GetBestIAPInThisSNAPL(aCmManager, SNAPID);
        if (IAPID)
            {
            break;
            }
        }
    iDestIdArray.Reset();
    return IAPID;
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::GetBestIAPInInternetSNAPL
// Sets the best IAP from internet snap
// ---------------------------------------------------------------------------
//
uint IAUpdateEngine::GetBestIAPInInternetSNAPL(RCmManager& aCmManager)
    {
    //select IAP from Internet SNAP
    iDestIdArray.Reset();
    aCmManager.AllDestinationsL(iDestIdArray);
    uint InternetSNAPID = 0;
    for (int i = 0; i < iDestIdArray.Count(); i++)
        {
        RCmDestination dest = aCmManager.DestinationL(iDestIdArray[i]);
        CleanupClosePushL(dest);

        if (dest.MetadataL(CMManager::ESnapMetadataInternet))
            {
            InternetSNAPID = iDestIdArray[i];
            CleanupStack::PopAndDestroy(&dest);
            break;
            }
        CleanupStack::PopAndDestroy(&dest);
        }
    iDestIdArray.Reset();

    return GetBestIAPInThisSNAPL(aCmManager, InternetSNAPID);
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::GetBestIAPInThisSNAPL
// Sets the best IAP from the given snap
// ---------------------------------------------------------------------------
//
uint IAUpdateEngine::GetBestIAPInThisSNAPL(RCmManager& aCmManager,
        uint aSNAPID)
    {
    //get all usable IAPs
    TConnMonIapInfoBuf iapInfo;
    TRequestStatus status;

    RConnectionMonitor connMon;
    connMon.ConnectL();
    CleanupClosePushL(connMon);

    connMon.GetPckgAttribute(EBearerIdAll, 0, KIapAvailability, iapInfo,
            status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());

    CleanupStack::PopAndDestroy(&connMon);

    RCmDestination dest = aCmManager.DestinationL(aSNAPID);
    CleanupClosePushL(dest);

    // Check whether the SNAP contains any IAP.
    for (int i = 0; i < dest.ConnectionMethodCount(); i++)
        {
        RCmConnectionMethod cm = dest.ConnectionMethodL(i);
        CleanupClosePushL(cm);

        uint iapid = cm.GetIntAttributeL(CMManager::ECmIapId);

        for (int i = 0; i < iapInfo().iCount; i++)
            {
            if (iapInfo().iIap[i].iIapId == iapid)
                {
                CleanupStack::PopAndDestroy(2); //cm & dest;
                return iapid;
                }
            }

        CleanupStack::PopAndDestroy(&cm);
        }

    CleanupStack::PopAndDestroy(&dest);
    return 0;
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowUpdateQueryL
// 
// -----------------------------------------------------------------------------
//    
void IAUpdateEngine::ShowUpdateQueryL()
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryL() begin");
    CIAUpdateQueryHistory* updateQueryHistory = CIAUpdateQueryHistory::NewL();
    // Get the delay information from the controller that has read it from
    // the config file.
    updateQueryHistory->SetDelay(
            iController->ConfigData().QueryHistoryDelayHours());
    CleanupStack::PushL(updateQueryHistory);
    bool isDelayed(updateQueryHistory->IsDelayedL(mUpdatequeryUid));
    CleanupStack::PopAndDestroy(updateQueryHistory);
    if (!isDelayed)
        {
        if (ClientInBackgroundL())
            {
            iEikEnv->RootWin().SetOrdinalPosition(-1, ECoeWinPriorityNormal);
            }
        else
            {
            iEikEnv->RootWin().SetOrdinalPosition(0, ECoeWinPriorityNormal);
            }

        HbMessageBox *messageBox = new HbMessageBox(
                HbMessageBox::MessageTypeQuestion);
        messageBox->setText(QString(
                "Application update is available from Nokia. Update?"));
        int actionCount = messageBox->actions().count();
        for (int i = actionCount - 1; i >= 0; i--)
            {
            messageBox->removeAction(messageBox->actions().at(i));
            }
        mPrimaryAction = NULL;
        mPrimaryAction = new HbAction("Now");
        mSecondaryAction = NULL;
        mSecondaryAction = new HbAction("Later");
        messageBox->addAction(mPrimaryAction);
        messageBox->addAction(mSecondaryAction);
        messageBox->setTimeout(HbPopup::NoTimeout);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->open(this, SLOT(dialogFinished(HbAction*)));
        mDialogState = ShowUpdateQuery;
        }
    else
        {
        InformRequestObserver(KErrNone);
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::HideApplicationInFSWL
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::HideApplicationInFSWL(bool aHide) const
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HideApplicationInFSW() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] hide: %d", aHide );
    int id = iEikEnv->RootWin().Identifier();

    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC(
            iEikEnv->WsSession(), id);

    wgName->SetHidden(aHide);
    wgName->SetWindowGroupName(iEikEnv->RootWin());
    CleanupStack::PopAndDestroy(wgName);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HideApplicationInFSW() end");
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::UpdateQueryCallbackL
// ---------------------------------------------------------------------------
//
TInt IAUpdateEngine::UpdateQueryCallbackL(TAny* aPtr)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateQueryCallbackL() begin");
    IAUpdateEngine* engine = static_cast<IAUpdateEngine*> (aPtr);
    //TRAPD( err, engine->ShowUpdateQueryL() );
    TRAP_IGNORE( engine->ShowUpdateQueryL() );
    //if ( err != KErrNone )
    //    {
    //    appUI->HandleLeaveErrorL( err );
    //    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateQueryCallbackL() end");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::AutomaticCheckCallbackL
// ---------------------------------------------------------------------------
//    

TInt IAUpdateEngine::AutomaticCheckCallbackL(TAny* aPtr)
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::AutomaticCheckCallbackL() begin");
    IAUpdateEngine* engine = static_cast<IAUpdateEngine*> (aPtr);

    int err = KErrNone;
    if (!engine->iAutomaticCheck)
        {
        TRAP( err, engine->iAutomaticCheck = CIAUpdateAutomaticCheck::NewL() );
        }
    if (err != KErrNone)
        {
        engine->HandleLeaveErrorL(err);
        }
    else
        {
        TRAP( err, engine->iAutomaticCheck->AcceptAutomaticCheckL() );
        if (err != KErrNone)
            {
            engine->HandleLeaveErrorL(err);
            }
        }

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::AutomaticCheckCallbackL() end");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::DoPossibleApplicationClose()
// ---------------------------------------------------------------------------
//  
bool IAUpdateEngine::DoPossibleApplicationClose()
    {
    //exit from result view if there are no update left
    bool toBeClosed = false;
    if (iController->Nodes().Count() == 0 && iController->FwNodes().Count()
            == 0)
        {
        toBeClosed = true;
        }
    else if (mStartedFromApplication
            && iController->ResultsInfo().iCountCancelled == 0
            && iController->ResultsInfo().iCountFailed == 0)
        {
        toBeClosed = true;
        }
    if (toBeClosed)
        {
        qApp->quit();
        }
    return toBeClosed;
    }

