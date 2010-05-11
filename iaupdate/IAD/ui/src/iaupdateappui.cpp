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
* Description:   This module contains the implementation of CIAUpdateAppUi class 
*                member functions.
*
*/



// INCLUDES
#include <avkon.hrh>
#include <StringLoader.h> 
#include <iaupdate.rsg>
#include <centralrepository.h>
#include <apgwgnam.h>
#include <f32file.h>
#include <ErrorUI.h>
#include <starterclient.h>
#include <iaupdateresult.h>
#include <iaupdateparameters.h>

#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginpacketdatadef.h>
#include <cmdestinationext.h>
#include <cmmanagerdef.h>
#include <rconnmon.h>

#include <hlplch.h>                   
          

#include "iaupdateappui.h"
#include "iaupdatemainview.h"
#include "iaupdatehistoryview.h"
#include "iaupdate.hrh"
#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"
#include "iaupdateuicontroller.h"
#include "iaupdaterequestobserver.h"
#include "iaupdateserver.h"
#include "iaupdatestatusdialog.h"
#include "iaupdatedetailsdialog.h"
#include "iaupdatefwdetailsdialog.h"
#include "iaupdatesettingdialog.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatestarter.h"
#include "iaupdateuiconfigdata.h"
#include "iaupdateagreement.h"
#include "iaupdateautomaticcheck.h"
#include "iaupdatequeryhistory.h"
#include "iaupdatedialogutil.h"
#include "iaupdategloballockhandler.h"
#include "iaupdatenodefilter.h"
#include "iaupdateuitimer.h"
#include "iaupdateagreement.h"
#include "iaupdatedebug.h"


const TUint KSWInstSvrUid = 0x101F875A;


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateAppUi::ConstructL()
    {    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ConstructL() begin");
     
    TInt ownWgId = iEikonEnv->RootWin().Identifier();
    RArray<RWsSession::TWindowGroupChainInfo> wgList;
    iEikonEnv->WsSession().WindowGroupList( &wgList );
    TBool chained = EFalse;
    for ( TInt i = 0; i < wgList.Count() && !chained ; i++ )
        {
        if ( wgList[i].iId == ownWgId && wgList[i].iParentId > 0 )
            {
            chained = ETrue;
            }
        }
    wgList.Reset();
    if ( chained )
        {
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        }
    else
        {
        iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        StatusPane()->MakeVisible( EFalse );
        }
    
    iController = CIAUpdateUiController::NewL( *this ); 
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ConstructL() end");   
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::CIAUpdateAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateAppUi::CIAUpdateAppUi(): iIsDMSet( EFalse ), iUiRefreshAllowed( ETrue )                              
    { 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::CIAUpdateAppUi()");   
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::~CIAUpdateAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateAppUi::~CIAUpdateAppUi()
    {    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::~CIAUpdateAppUi() begin");
    
    delete iGlobalLockHandler;
    iDestIdArray.Reset();
	delete iController;
	delete iIdle;
	delete iIdleAutCheck;
	delete iForegroundTimer;
	delete iBackgroundTimer;
	delete iDialogParam;
		        
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::~CIAUpdateAppUi() end");
    }        

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::StartedByLauncherL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateAppUi::StartedByLauncherL( MIAUpdateRequestObserver& aObserver, 
                                         TBool aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartedByLauncherL() begin");
    iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::ENoRequest;
    iController->SetRequestType( iRequestType );
    SetVisibleL( ETrue );
    CIAUpdateParameters* params = iController->ParamsReadAndRemoveFileL();
    iController->CheckUpdatesDeferredL( params, aRefreshFromNetworkDenied );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartedByLauncherL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::CheckUpdatesRequestL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateAppUi::CheckUpdatesRequestL( MIAUpdateRequestObserver& aObserver,
                                           CIAUpdateParameters* aFilterParams,
                                           TBool aForcedRefresh )
                                           
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::CheckUpdatesRequestL() begin");
    iStartedFromApplication  = ETrue;
    CleanupStack::PushL( aFilterParams );
    if ( iWgId > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
    CleanupStack::Pop( aFilterParams );
    
    if ( !aFilterParams->ShowProgress() )
        {
        iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );	
        }
           
    iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::ECheckUpdates; 
    iController->SetRequestType( iRequestType );
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateAppUi::CheckUpdatesRequestL() Forced refresh: %d", aForcedRefresh );
    iController->SetForcedRefresh( aForcedRefresh ); 
    iController->CheckUpdatesDeferredL( aFilterParams, EFalse );
    
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::CheckUpdatesRequestL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowUpdatesRequestL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::ShowUpdatesRequestL( MIAUpdateRequestObserver& aObserver,
                                          CIAUpdateParameters* aFilterParams )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdatesRequestL() begin"); 
    delete iBackgroundTimer;
    iBackgroundTimer = NULL;
    iStartedFromApplication  = ETrue;
    CleanupStack::PushL( aFilterParams );
    if ( iWgId > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
   
    StatusPane()->MakeVisible( ETrue );
    iEikonEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );
    
    iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::EShowUpdates;
    iController->SetRequestType( iRequestType );
     

    if ( !iMainView )
        {
    	iMainView  = CIAUpdateMainView::NewL( ClientRect() ); 
    	AddViewL( iMainView );
        }
    
      
    // by pushing object to cleanup stack its destructor is called if leave happens
    // so global lock issued by this instance can be released in destructor of CIAUpdateGlobalLockHandler
    CIAUpdateGlobalLockHandler* globalLockHandler = CIAUpdateGlobalLockHandler::NewLC();
    if ( !globalLockHandler->InUseByAnotherInstanceL() )
        {
        globalLockHandler->SetToInUseForAnotherInstancesL( ETrue );
        CleanupStack::Pop( globalLockHandler );
        CleanupStack::Pop( aFilterParams );
        CleanupStack::PushL( globalLockHandler );
        iController->CheckUpdatesDeferredL( aFilterParams, EFalse );
        CleanupStack::Pop( globalLockHandler );	
        delete iGlobalLockHandler;
        iGlobalLockHandler = globalLockHandler;
        //now possible deletion of iGlobalLockHandler in leave situation is handled
        //in HandleLeaveErrorL() and HandleLeaveErrorWithoutLeave methods. 
        }
    else
        {
        CleanupStack::PopAndDestroy( globalLockHandler );
        CleanupStack::PopAndDestroy( aFilterParams );
        // locked by another IAD instance, nothing else to do than just complete client's request.	
        InformRequestObserver( KErrNone );
        }  
  	

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdatesRequestL() end"); 
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowUpdateQueryRequestL
// 
// -----------------------------------------------------------------------------
//     
void CIAUpdateAppUi::ShowUpdateQueryRequestL( MIAUpdateRequestObserver& aObserver, 
                                              TUint aUid )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdateQueryRequestL begin");
    iStartedFromApplication  = ETrue;
    iUpdatequeryUid = aUid;
    iUpdateNow = EFalse;
    if ( iWgId > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
    
    iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::EUpdateQuery;
    
    
    delete iIdle;
    iIdle = NULL;
	iIdle = CIdle::NewL( CActive::EPriorityIdle ); 
    iIdle->Start( TCallBack( UpdateQueryCallbackL, this ) ); 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdateQueryRequestL end")
    }
    
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleObserverRemoval
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::HandleObserverRemoval()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleObserverRemoval() begin");
	iRequestObserver = NULL;
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleObserverRemoval() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::TextLinkL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::TextLinkL( TInt /*aLinkId*/ )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::TextLinkL() begin");
    TRAPD( err, HandleTextLinkL() );
    if ( err != KErrNone )
        {
    	HandleLeaveErrorL( err );
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::TextLinkL() end");
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleCommandL
// 
// -----------------------------------------------------------------------------
//         
void CIAUpdateAppUi::HandleCommandL( TInt aCommand )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() begin");   
    IAUPDATE_TRACE_1("[IAUPDATE] command id: %d", aCommand );

    switch(aCommand)
        {
        case EIAUpdateCmdStartUpdate:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdStartUpdate");
    

            // by pushing object to cleanup stack it's destructor is called if leave happens
            // so global lock issued by this instance can be released in destructor of CIAUpdateGlobalLockHandler
            CIAUpdateGlobalLockHandler* globalLockHandler = CIAUpdateGlobalLockHandler::NewLC();
            if ( !globalLockHandler->InUseByAnotherInstanceL() )
                {
            	globalLockHandler->SetToInUseForAnotherInstancesL( ETrue );
            	// No need to be totally silent since the updating is started
            	// by user.
            	SetDefaultConnectionMethodL( EFalse );
            	iController->StartUpdateL();
            	CleanupStack::Pop( globalLockHandler );	
            	delete iGlobalLockHandler;
            	iGlobalLockHandler = globalLockHandler;
            	//now possible deletion of iGlobalLockHandler in leave situation is handled
            	//in HandleLeaveErrorL() and HandleLeaveErrorWithoutLeave methods. 
                }
            else
                {
                CleanupStack::PopAndDestroy( globalLockHandler ); 	
                }
            break;	
            } 
            
       	case EIAUpdateCmdSettings:
       	    {
       	    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdSettings");
            CIAUpdateSettingDialog::ShowDialogL();
            break;	
       	    } 
       	    
       	case EIAUpdateCmdUpdateDetails:
       	     {
       	     IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdUpdateDetails");
       	            
       	     if ( iMainView->GetCurrentNode()->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
       	         {
       	         TIAUpdateDialogParam param;
       	         param.iNode = static_cast<MIAUpdateNode*>( iMainView->GetCurrentNode() );
       	         CIAUpdateDetailsDialog::ShowDialogL( &param );
       	         }
       	            
       	     if (iMainView->GetCurrentNode()->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
       	         {
       	         //this is changed because of the new requirement: when both NSU and FOTA available, only show FOTA in details
       	  
       	         MIAUpdateFwNode* node = static_cast<MIAUpdateFwNode*>( iMainView->GetCurrentNode() );
       	         CIAUpdateFwDetailsDialog::ShowDialogL( node );
       	         }

       	      break;      
       	      }
       	    
        case EIAUpdateCmdMarkedUpdateDetails:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdUpdateDetails");
            
            if ( iMainView->GetSelectedNode()->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
                {
                TIAUpdateDialogParam param;
                param.iNode = static_cast<MIAUpdateNode*>( iMainView->GetSelectedNode() );
                CIAUpdateDetailsDialog::ShowDialogL( &param );
                }
            
            if (iMainView->GetSelectedNode()->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                //this is changed because of the new requirement: when both NSU and FOTA available, only show FOTA in details
  
                MIAUpdateFwNode* node = static_cast<MIAUpdateFwNode*>( iMainView->GetSelectedNode() );
                CIAUpdateFwDetailsDialog::ShowDialogL( node );
                }

            break;		
            }
       	    
                
        // History view
        case EIAUpdateCmdUpdateHistory:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdUpdateHistory");
            if ( !iHistoryView )
                {
                iHistoryView = 
                    CIAUpdateHistoryView::NewL( iController->HistoryL() );
                AddViewL( iHistoryView );
                }
            ActivateLocalViewL( TUid::Uid( EIAUpdateHistoryViewId ) );
            break;	
            }
        
        case EIAUpdateCmdDisclaimer:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdDisclaimer");
            CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
            agreement->ShowAgreementL();
            CleanupStack::PopAndDestroy( agreement);
            break;	
            }
            
        case EAknCmdHelp:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EAknCmdHelp");
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppHelpContextL() );
            break;
            }
                      
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() Exit");
            if (iController )
                {
                iController->CancelOperation();
                if ( iController->Starter() )
                	 {
                	 iController->Starter()->BringToForegroundL();
                	 }
                }
            if ( iRequestObserver )
                {
   	            InformRequestObserver( KErrCancel );
                }
            Exit();	
            break;
            }
            
        case EIAUpdateCmdHistoryViewExit:
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() EIAUpdateCmdHistoryViewExit");
            ActivateLocalViewL( TUid::Uid( EIAUpdateMainViewId ) );
            break;

        default:
            {
            break;
            }
        }
       
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleCommandL() end")    
    }

// -----------------------------------------------------------------------------
// CIAUpdateppUi::HandleWsEventL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleWsEventL() begin");   
    CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
  	if ( aEvent.Type() == EEventWindowGroupListChanged )
	    {
        IAUPDATE_TRACE("[IAUPDATE] Window group list changed"); 
          
		RWindowGroup owngroup;
		TInt ownWgId = iEikonEnv->RootWin().Identifier();
  		TApaTaskList tasklist( iEikonEnv->WsSession() );   
        
        TApaTask task = tasklist.FindApp( TUid::Uid( KSWInstSvrUid ) );
        
        if ( task.Exists() )
            {
            IAUPDATE_TRACE("[IAUPDATE] Installer task exists");
            
            TInt instUiWgId = task.WgId();
            TInt ownPosition = KErrNotFound;
            TInt instUiPosition = KErrNotFound;
            CArrayFixFlat<TInt>*  wgArray = new(ELeave)CArrayFixFlat<TInt>(10);  
            CleanupStack::PushL( wgArray );
            User::LeaveIfError( iEikonEnv->WsSession().WindowGroupList( 0, wgArray ) );   
            for ( TInt i = 0; i < wgArray->Count(); i++ )
                {
                if ( wgArray->At(i) == ownWgId )
                    {
                    IAUPDATE_TRACE_1("[IAUPDATE] ownPosition: %d", i);
                	ownPosition = i;
                    }
                else if ( wgArray->At(i) == instUiWgId )
                    {
                    IAUPDATE_TRACE_1("[IAUPDATE] instUiPosition: %d", i);
                	instUiPosition = i;
                    }
                }
            CleanupStack::PopAndDestroy( wgArray );
          	if ( ownPosition == 0 && iInstUiOnTop )
                {
                // IAD is now topmost. In the case Installer UI was previously on the top of IAD 
                // bring Installer UI foreground 
                IAUPDATE_TRACE("[IAUPDATE] instUi to foreground");
            	task.BringToForeground();
            	iInstUiOnTop = EFalse;
                }
            else
                {
                if ( instUiPosition < ownPosition  && 
                    instUiPosition != KErrNotFound && 
                    ownPosition != KErrNotFound )
        	        {
        	        // Installer UI is on the top of IAD, i.e. it's showing FILETEXT dialog 
                    IAUPDATE_TRACE("[IAUPDATE] instUi is on top of IAD");
        		    iInstUiOnTop = ETrue;
        	        }
        	    else
        	        {
                    IAUPDATE_TRACE("[IAUPDATE] iInstUiOnTop false");
        	        iInstUiOnTop = EFalse;	
        	        }
                }    
            }
        else
            {
            IAUPDATE_TRACE("[IAUPDATE] No task. iInstUiOnTop false.");
            iInstUiOnTop = EFalse;	
            }
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleWsEventL() end");           
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::StartupComplete
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateAppUi::StartupComplete( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartupComplete() begin"); 
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError);
    
    if( aError != KErrNone ) 
        {  
        HandleLeaveErrorWithoutLeave( aError );
        }
    else
        {
        TRAPD( err, StartupCompleteL() );
        if( err != KErrNone ) 
           {  
           HandleLeaveErrorWithoutLeave( err );
           }
        }
 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartupComplete() end");    
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::StartupCompleteL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateAppUi::StartupCompleteL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartupCompleteL() begin"); 
    
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    iGlobalLockHandler = CIAUpdateGlobalLockHandler::NewL();
    if ( !iGlobalLockHandler->InUseByAnotherInstanceL() )
        {
        TBool totalSilent( EFalse );
        if ( iRequestType == IAUpdateUiDefines::ECheckUpdates )
            {
            if ( iController->Filter() )
                {
                if ( iController->Filter()->FilterParams() )
                    {
                    if ( iController->Filter()->FilterParams()->Refresh() )
                        {
                        if ( !iController->ForcedRefresh() )
                            {
                            //from bgchecker, make it silent
                            totalSilent = ETrue;
                            }
                        }
                    }
                }
            }
        SetDefaultConnectionMethodL( totalSilent );
        iGlobalLockHandler->SetToInUseForAnotherInstancesL( ETrue );
        iController->StartRefreshL();  
        }
    else
        {
        RefreshCompleteL( ETrue, KErrServerBusy );
        }    
 
       
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartupCompleteL() end");    
    }
    
 
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleLeaveErrorL
// 
// -----------------------------------------------------------------------------
//        
void CIAUpdateAppUi::HandleLeaveErrorL( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleLeaveErrorL() begin");
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    // client request is completed before leave in case of leave error
    if ( aError != KErrNone ) 
        { 	
    	InformRequestObserver( aError );
    	User::Leave( aError );
        }
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleLeaveErrorL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleLeaveErrorWithoutLeave
// 
// -----------------------------------------------------------------------------
//  
void CIAUpdateAppUi::HandleLeaveErrorWithoutLeave( TInt aError )
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleLeaveErrorWithoutLeave() begin");
	//removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
	if ( aError != KErrNone ) 
        {
    	InformRequestObserver( aError );
        }
    if ( aError == KErrDiskFull )
        {
        TRAP_IGNORE( ShowGlobalErrorNoteL( aError ) );
        }
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleLeaveErrorWithoutLeave end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::RefreshCompleteL
// 
// -----------------------------------------------------------------------------
//      
void CIAUpdateAppUi::RefreshCompleteL( TBool aWithViewActivation, TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::RefreshCompleteL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;   
   
    if ( iRequestType == IAUpdateUiDefines::ECheckUpdates )
        {
  	    InformRequestObserver( aError );
        }
    else 
        {    
        RefreshL( aError );   
        if ( aWithViewActivation)
            {
        	ActivateLocalViewL( TUid::Uid( EIAUpdateMainViewId ) );
            }
        CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
        TBool agreementAccepted = agreement->AgreementAcceptedL();
        if ( iController->ForcedRefresh() )    
            {
            if ( !agreementAccepted )
                {
                agreement->SetAgreementAcceptedL();
                }
            }
        CleanupStack::PopAndDestroy( agreement );
        // By calling CIdle possible waiting dialog can be closed before
        // automatic check where a new dialog may be launched
        delete iIdleAutCheck;
        iIdleAutCheck = NULL;
        iIdleAutCheck = CIdle::NewL( CActive::EPriorityIdle ); 
        iIdleAutCheck->Start( TCallBack( AutomaticCheckCallbackL, this ) );
        } 
 
  
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::RefreshCompleteL() end");        
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::UpdateCompleteL
// 
// -----------------------------------------------------------------------------
//     
void CIAUpdateAppUi::UpdateCompleteL( TInt aError )    
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::UpdateCompleteL begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;  
    if ( iRequestType != IAUpdateUiDefines::ENoRequest )
        {
        InformRequestObserver( aError );
        }
    
    RefreshL( KErrNone ); 
      
    ShowStatusDialogDeferredL();
                
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::UpdateCompleteL end");
    }

   
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowStatusDialogL
// 
// -----------------------------------------------------------------------------
//   
void CIAUpdateAppUi::ShowStatusDialogL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogL() begin");
 
     
    if ( !iDialogParam )
        {
        iDialogParam = new( ELeave ) TIAUpdateDialogParam;
        }
    
    if ( !iShowStatusDialogAgain )
        {
        TIAUpdateResultsInfo resultsInfo( iController->ResultsInfo() );
        iDialogParam->iCountCancelled = resultsInfo.iCountCancelled;
        iDialogParam->iCountFailed = resultsInfo.iCountFailed;
        iDialogParam->iCountSuccessfull = resultsInfo.iCountSuccessfull;
        iDialogParam->iShowCloseAllText = resultsInfo.iFileInUseError;
        iRebootAfterInstall = resultsInfo.iRebootAfterInstall;
        iDialogParam->iLinkObserver = this;
        }
         
    iShowStatusDialogAgain = EFalse;
    iUiRefreshAllowed = ETrue;
    TInt ret = CIAUpdateStatusDialog::ShowDialogL( iDialogParam );
    if ( ret == EAknSoftkeyOk && iRebootAfterInstall )
        {
        TInt rebootRet = IAUpdateDialogUtil::ShowConfirmationQueryL( 
                                           R_IAUPDATE_PHONE_RESTART_QUERY, 
                                           R_AVKON_SOFTKEYS_OK_CANCEL__OK );
        if ( rebootRet == EAknSoftkeyOk )
            {
            RStarterSession startersession;
            if( startersession.Connect() == KErrNone )
                {
                startersession.Reset( RStarterSession::EUnknownReset );
                startersession.Close();
                return;
                }
            }
        }
    if ( ret == EAknSoftkeyOk && 
         iStartedFromApplication && 
         iDialogParam->iCountCancelled == 0 &&
         iDialogParam->iCountFailed == 0 )
        {
    	HandleCommandL( EEikCmdExit );
        }
    
    IAUPDATE_TRACE_1("[IAUPDATE] nodes count: %d", iController->Nodes().Count() );
    IAUPDATE_TRACE_1("[IAUPDATE] fw nodes: %d", iController->FwNodes().Count() );
    //exit from result view if there are no update left
    if ( ret == EAknSoftkeyOk && (iController->Nodes().Count() == 0) && (iController->FwNodes().Count() == 0) )
        {
        HandleCommandL( EEikCmdExit );
        }
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowStatusDialogDeferredL
// 
// -----------------------------------------------------------------------------
//       
void CIAUpdateAppUi::ShowStatusDialogDeferredL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogDeferredL() begin");
    delete iIdle;
    iIdle = NULL;
	iIdle = CIdle::NewL( CActive::EPriorityIdle ); 
    iIdle->Start( TCallBack( ShowStatusDialogCallbackL, this ) ); 
    iUiRefreshAllowed = EFalse;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogDeferredL() end");
    }
 


                       
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::RefreshL
// 
// -----------------------------------------------------------------------------
//  
void CIAUpdateAppUi::RefreshL( TInt aError ) 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::RefreshL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] Error code: %d", aError );
    iMainView->RefreshL( iController->Nodes(), iController->FwNodes(), aError );                    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::RefreshL() end");
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::InformRequestObserver
// 
// -----------------------------------------------------------------------------
//      
void CIAUpdateAppUi::InformRequestObserver( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::InformRequestObserver() begin");
        
    if ( iRequestObserver )
        {
        if ( iController->ClosingAllowedByClient() )
            {
            if ( iRequestType != IAUpdateUiDefines::ENoRequest )
                {
                if ( iRequestType == IAUpdateUiDefines::EUpdateQuery && iUpdateNow )
                    {
                    if ( !iBackgroundTimer )
                        {
                        iBackgroundTimer = CIAUpdateUITimer::NewL( *this, CIAUpdateUITimer::EBackgroundDelay );
                        }
                    if ( !iBackgroundTimer->IsActive() )
                        {
                        iBackgroundTimer->After( 500000 );
                        }
                    }
                else
                    {
                    iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );
                    }
                }
            }
        CIAUpdateResult* details( NULL );
        TRAPD( error, details = CIAUpdateResult::NewL() )
        if ( details )
            {
            TIAUpdateResultsInfo resultsInfo(
                iController->ResultsInfo() );
            IAUPDATE_TRACE_3("[IAUPDATE] succeed: %d failed: %d  cancelled: %d", 
                              resultsInfo.iCountSuccessfull, 
                              resultsInfo.iCountFailed, 
                              resultsInfo.iCountCancelled );
            IAUPDATE_TRACE_2("[IAUPDATE] update now: %d available updates: %d", 
                              iUpdateNow, iController->CountOfAvailableUpdates() );
            details->SetSuccessCount( resultsInfo.iCountSuccessfull );
            details->SetFailCount( resultsInfo.iCountFailed );
            details->SetCancelCount( resultsInfo.iCountCancelled );    
            
            // Ownership of details is transferred here.
            iRequestObserver->RequestCompleted( details, 
                                                iController->CountOfAvailableUpdates(), 
                                                iUpdateNow, 
                                                aError );
            details = NULL;        
            }
        else
            {
            iRequestObserver->RequestCompleted( details, 
                                                iController->CountOfAvailableUpdates(),
                                                iUpdateNow, 
                                                error );            
            }
        iRequestObserver = NULL;
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::InformRequestObserver() end");
    }


// ---------------------------------------------------------------------------
// CIAUpdateAppUi::SetDefaultConnectionMethodL
// Sets the connection method for the update network connection.
// ---------------------------------------------------------------------------
//
void CIAUpdateAppUi::SetDefaultConnectionMethodL(TBool aTotalSilent )
    {
    if ( aTotalSilent )
        {
        // from back ground checker, choose the IAP to make the internet access silent
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDefaultConnectionMethodL() begin");

           TUint32 connectionMethodId( 0 );
           TInt connMethodId( 0 );

           // Let's first check whether cenrep contains SNAP id other than zero
           CRepository* cenrep( CRepository::NewLC( KCRUidIAUpdateSettings ) );
           User::LeaveIfError(  
               cenrep->Get( KIAUpdateAccessPoint, connMethodId ) );
           CleanupStack::PopAndDestroy( cenrep ); 
           cenrep = NULL;

           RCmManagerExt cmManagerExt;
           cmManagerExt.OpenL();
           CleanupClosePushL( cmManagerExt );
           
           if ( connMethodId == -1 )
               {
               IAUPDATE_TRACE("[IAUPDATE] user chooses default connection, use IAP logic");
               
               //check what is the default connection by users     
               
               TCmDefConnValue DCSetting;
               cmManagerExt.ReadDefConnL( DCSetting );
              
               
               switch ( DCSetting.iType )
                   {
                   case ECmDefConnAlwaysAsk:
                   case ECmDefConnAskOnce:
                       {
                       //go with the best IAP under internet snap
                       connectionMethodId = GetBestIAPInAllSNAPsL( cmManagerExt );
                       break;
                       }
                   case ECmDefConnDestination:
                       {
                       //go with the best IAP under this snap
                       connectionMethodId = GetBestIAPInThisSNAPL( cmManagerExt, DCSetting.iId );
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
           else if ( connMethodId == 0 )
               {
               //no choice from user, we go with the best IAP under Internent SNAP
               connectionMethodId = GetBestIAPInAllSNAPsL( cmManagerExt );
               }
           else
               {
               IAUPDATE_TRACE("[IAUPDATE] use chooses a snap");
               // It was some SNAP value
               connectionMethodId = GetBestIAPInThisSNAPL( cmManagerExt, connMethodId );
               }

           CleanupStack::PopAndDestroy( &cmManagerExt ); 
           
           if ( connectionMethodId != 0 )
               {
               TIAUpdateConnectionMethod connectionMethod( 
                   connectionMethodId, 
                   TIAUpdateConnectionMethod::EConnectionMethodTypeAccessPoint );

               iController->SetDefaultConnectionMethodL( connectionMethod );
               }
           else
               {
               //In the totally silent case, if no usable IAP, we complete the check update with 0 updates.
               //the bgchecker will try again later after 1 month. 
               //The LEAVE will be catched up later and complete the request from background checker.
               User::LeaveIfError( KErrNotFound );
               }
           


           IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDefaultConnectionMethodL() end");
        }
    else
        {
        // from grid, use the old logic
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDefaultConnectionMethodL() begin");
                  TUint32 connectionMethodId( 0 );
                  TInt connMethodId( 0 );

                  // Set initial value to always ask
                  TInt connectionMethodType( TIAUpdateConnectionMethod::EConnectionMethodTypeAlwaysAsk );
                  TBool needSaving(EFalse);

                  // Let's first check whether cenrep contains SNAP id other than zero
                  CRepository* cenrep( CRepository::NewLC( KCRUidIAUpdateSettings ) );
                  User::LeaveIfError(  
                      cenrep->Get( KIAUpdateAccessPoint, connMethodId ) );
                  CleanupStack::PopAndDestroy( cenrep ); 
                  cenrep = NULL;
                    
                  if ( connMethodId == -1 )
                      {
                      IAUPDATE_TRACE("[IAUPDATE] user chooses default connection, use IAP logic");
                                            
                      connectionMethodId = 0;
                      connectionMethodType = TIAUpdateConnectionMethod::EConnectionMethodTypeDefault;
                      }
                  else if ( connMethodId == 0 )
                      {
                      
                      IAUPDATE_TRACE("[IAUPDATE] use chooses nothing, use internal IAP logic");
                      //if nothing is set by user, use our new logic
                      //SetDefaultConnectionMethod2L();
                      //return;
                      // CenRep didn't contain any SNAP id. Let's try Internet SNAP then.
                          
                      RCmManagerExt cmManagerExt;
                      cmManagerExt.OpenL();
                      CleanupClosePushL( cmManagerExt );
                      iDestIdArray.Reset();
                      cmManagerExt.AllDestinationsL( iDestIdArray );

                      for ( TInt i = 0; i< iDestIdArray.Count(); i++ )
                          {
                          RCmDestinationExt dest = cmManagerExt.DestinationL( iDestIdArray[i] );
                          CleanupClosePushL( dest );
                           
                          if ( dest.MetadataL( CMManager::ESnapMetadataInternet ) )
                              {
                              // Check whether Internet SNAP contains any IAP.
                              if ( dest.ConnectionMethodCount() > 0 )
                                  {
                                  connectionMethodId = iDestIdArray[i];
                                  needSaving = ETrue;
                                  IAUPDATE_TRACE_1("[IAUPDATE] connectionMethodId: %d", connectionMethodId );
                                  }
                              CleanupStack::PopAndDestroy( &dest ); 
                              break;
                              }
                               
                          CleanupStack::PopAndDestroy( &dest ); 
                          }
                      iDestIdArray.Reset();
                      CleanupStack::PopAndDestroy( &cmManagerExt ); 
                      }
                  else
                      {
                      IAUPDATE_TRACE("[IAUPDATE] use chooses a snap");
                      // It was some SNAP value
                      connectionMethodId = connMethodId;
                      }
                  
                  if ( connectionMethodId > 0)
                      {
                      // We have now some valid SNAP id, either from CenRep or Internet SNAP
                      connectionMethodType = TIAUpdateConnectionMethod::EConnectionMethodTypeDestination;
                      // Save to cenrep if needed
                      if ( needSaving )
                          {
                          cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
                          TInt err = cenrep->StartTransaction( CRepository::EReadWriteTransaction );
                          User::LeaveIfError( err );
                          cenrep->CleanupCancelTransactionPushL();
                          
                          connMethodId = connectionMethodId;
                          err = cenrep->Set( KIAUpdateAccessPoint, connMethodId );
                          User::LeaveIfError( err );
                          TUint32 ignore = KErrNone;
                          User::LeaveIfError( cenrep->CommitTransaction( ignore ) );
                          CleanupStack::PopAndDestroy(); // CleanupCancelTransactionPushL()
                          CleanupStack::PopAndDestroy( cenrep );            
                          }
                      }

                  TIAUpdateConnectionMethod connectionMethod( 
                      connectionMethodId, 
                      static_cast< TIAUpdateConnectionMethod::TConnectionMethodType >( connectionMethodType ) );

                  iController->SetDefaultConnectionMethodL( connectionMethod );

                  IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDefaultConnectionMethodL() end");
        }

    }


// ---------------------------------------------------------------------------
// CIAUpdateAppUi::GetBestIAPInAllSNAPsL
// Sets the best IAP from all snaps
// ---------------------------------------------------------------------------
//
TUint32 CIAUpdateAppUi::GetBestIAPInAllSNAPsL( RCmManagerExt& aCmManagerExt  )
    { 
    //go with internet SNAP first.
    TUint32 IAPID = 0;
    IAPID = GetBestIAPInInternetSNAPL( aCmManagerExt );
    
    if ( IAPID )
        {
        return IAPID;
        }
    
    //select IAP from rest of the SNAPs
    iDestIdArray.Reset();
    aCmManagerExt.AllDestinationsL( iDestIdArray );
    
    for ( TInt i = 0; i< iDestIdArray.Count(); i++ )
        {
        TUint32 SNAPID = iDestIdArray[i];                   
        IAPID = GetBestIAPInThisSNAPL( aCmManagerExt, SNAPID );
        if ( IAPID )
            {
            break;
            }
         }
    iDestIdArray.Reset();
    return IAPID;
    }



// ---------------------------------------------------------------------------
// CIAUpdateAppUi::GetBestIAPInInternetSNAPL
// Sets the best IAP from internet snap
// ---------------------------------------------------------------------------
//
TUint32 CIAUpdateAppUi::GetBestIAPInInternetSNAPL( RCmManagerExt& aCmManagerExt  )
    {
    //select IAP from Internet SNAP
    iDestIdArray.Reset();
    aCmManagerExt.AllDestinationsL( iDestIdArray );
    TUint32 InternetSNAPID = 0;
    for ( TInt i = 0; i< iDestIdArray.Count(); i++ )
        {
        RCmDestinationExt dest = aCmManagerExt.DestinationL( iDestIdArray[i] );
        CleanupClosePushL( dest );
                                     
        if ( dest.MetadataL( CMManager::ESnapMetadataInternet ) )
            {
            InternetSNAPID = iDestIdArray[i];
            CleanupStack::PopAndDestroy( &dest ); 
            break;
            }                     
         CleanupStack::PopAndDestroy( &dest ); 
         }
    iDestIdArray.Reset();
    
    return GetBestIAPInThisSNAPL( aCmManagerExt, InternetSNAPID );
    }



// ---------------------------------------------------------------------------
// CIAUpdateAppUi::GetBestIAPInThisSNAPL
// Sets the best IAP from the given snap
// ---------------------------------------------------------------------------
//
TUint32 CIAUpdateAppUi::GetBestIAPInThisSNAPL( RCmManagerExt& aCmManagerExt, TUint32 aSNAPID  )
    {
    //get all usable IAPs
    TConnMonIapInfoBuf iapInfo;
    TRequestStatus status;
                       
    RConnectionMonitor connMon;
    connMon.ConnectL();
    CleanupClosePushL( connMon );
    
    connMon.GetPckgAttribute( EBearerIdAll, 0, KIapAvailability,iapInfo, status );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
    
    CleanupStack::PopAndDestroy( &connMon ); 
    
    RCmDestinationExt dest = aCmManagerExt.DestinationL( aSNAPID );
    CleanupClosePushL( dest );
    
    // Check whether the SNAP contains any IAP.
    for  (TInt i = 0; i < dest.ConnectionMethodCount(); i++ )
        {
        RCmConnectionMethodExt cm =  dest.ConnectionMethodL( i );
        CleanupClosePushL( cm );
        
        TUint32 iapid= cm.GetIntAttributeL( CMManager::ECmIapId );
        
        for ( TInt i = 0; i < iapInfo().iCount; i++ )
            {
            if ( iapInfo().iIap[i].iIapId == iapid )
                {
                CleanupStack::PopAndDestroy( 2 ); //cm & dest;
                return iapid;
                }
            }    
                                                                                 
        CleanupStack::PopAndDestroy( &cm );
        }
    
    CleanupStack::PopAndDestroy( &dest ); 
    return 0;
    }



// ---------------------------------------------------------------------------
// CIAUpdateAppUi::ShowStatusDialogCallbackL
// ---------------------------------------------------------------------------
//
TInt CIAUpdateAppUi::ShowStatusDialogCallbackL( TAny* aPtr )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogCallbackL() begin");
    CIAUpdateAppUi* appUI= static_cast<CIAUpdateAppUi*>( aPtr ); 
    TRAPD( err, appUI->ShowStatusDialogL() );
    appUI->HandleLeaveErrorL( err );
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowStatusDialogCallbackL() end");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::SetVisibleL
// 
// -----------------------------------------------------------------------------
//  
void CIAUpdateAppUi::SetVisibleL( TBool aVisible )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetVisibleL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] visible: %d", aVisible );
    if ( aVisible )
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
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetVisibleL() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::Controller()
// 
// -----------------------------------------------------------------------------
//      
CIAUpdateUiController* CIAUpdateAppUi::Controller() const
    {
	return iController;
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::PotentialCancel
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateAppUi::PotentialCancel() 
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::PotentialCancel() begin");
    iController->DoCancelIfAllowed();
    HandleObserverRemoval();
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::PotentialCancel() end");
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleClientToForegroundL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateAppUi::HandleClientToForegroundL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleClientToForegroundL() begin");
    if ( !iForegroundTimer )
        {
        iForegroundTimer = CIAUpdateUITimer::NewL( *this, CIAUpdateUITimer::EForegroundDelay );
        }
    if ( !iForegroundTimer->IsActive() )
        {
        iForegroundTimer->After( 700000 );
        }
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleClientToForegroundL() end");
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleAllClientsClosedL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateAppUi::HandleAllClientsClosedL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleAllClientsClosedL() begin");
	HideApplicationInFSWL( EFalse );
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleAllClientsClosedL() end");
    }
    
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowStatusDialogAgain
// 
// -----------------------------------------------------------------------------
//  
TBool CIAUpdateAppUi::ShowStatusDialogAgain() const
    {
    return iShowStatusDialogAgain;	
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::StartWGListChangeMonitoring
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::StartWGListChangeMonitoring()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartWGListChangeMonitoring() begin");
    iInstUiOnTop = EFalse;
	iEikonEnv->RootWin().EnableGroupListChangeEvents();
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StartWGListChangeMonitoring() end");
    }
 
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::StopWGListChangeMonitoring
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::StopWGListChangeMonitoring()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StopWGListChangeMonitoring() begin");
	iEikonEnv->RootWin().DisableGroupListChangeEvents();
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::StopWGListChangeMonitoring() end");
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::SetClientWgId
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::SetClientWgId( TInt aWgId )
    {
	IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateAppUi::SetClientWgId() wgId %d", aWgId );
	iWgId = aWgId;
    }

// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ClientInBackgroundL
// 
// -----------------------------------------------------------------------------
// 
TInt CIAUpdateAppUi::ClientInBackgroundL() const
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ClientInBackgroundL() begin");
	TBool inBackground = EFalse;
	if ( iWgId > 0 )
	    {
		CArrayFixFlat<TInt>*  wgArray = new( ELeave ) CArrayFixFlat<TInt>(10);  
        CleanupStack::PushL( wgArray );
        User::LeaveIfError( iEikonEnv->WsSession().WindowGroupList( 0, wgArray ) );  
        TInt ownWgId = iEikonEnv->RootWin().Identifier();
        if ( ( wgArray->At( 0 ) != ownWgId ) && ( wgArray->At( 0 ) != iWgId  ) )
            {
        	inBackground = ETrue;
            }
        CleanupStack::PopAndDestroy( wgArray );  
	    }
	IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateAppUi::ClientInBackgroundL() inBackground: %d", inBackground );
    return inBackground;  
    }



// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowUpdateQueryL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateAppUi::ShowUpdateQueryL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdateQueryL() begin");
	CIAUpdateQueryHistory* updateQueryHistory = CIAUpdateQueryHistory::NewL();
	// Get the delay information from the controller that has read it from
	// the config file.
	updateQueryHistory->SetDelay( iController->ConfigData().QueryHistoryDelayHours() );
    CleanupStack::PushL( updateQueryHistory );
    if ( !updateQueryHistory->IsDelayedL( iUpdatequeryUid ) )
        {
        if ( ClientInBackgroundL() )
            {
    	    iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNormal );
            }
        else
            {
            iEikonEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );	
            }
   	    TInt ret = IAUpdateDialogUtil::ShowConfirmationQueryL( 
   	                                  R_IAUPDATE_PROGRAM_UPDATE, 
	                                  R_IAUPDATE_SOFTKEYS_NOW_LATER__NOW );
	    switch ( ret )
	        {
	        case EIAUpdateSoftkeyNow:
	            IAUPDATE_TRACE("[IAUPDATE] EIAUpdateSoftkeyNow");
	            iUpdateNow = ETrue;
	            break;

	        case EIAUpdateSoftkeyLater:
	            IAUPDATE_TRACE("[IAUPDATE] EIAUpdateSoftkeyLater");
	        	updateQueryHistory->SetTimeL( iUpdatequeryUid );
	            break;
	            
	        default: //case of cancel (eq. End key)
	            break;
	        }     
        }
    CleanupStack::PopAndDestroy( updateQueryHistory );
	InformRequestObserver( KErrNone );	
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ShowUpdateQueryL() end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HideApplicationInFSWL
// 
// -----------------------------------------------------------------------------
//  
void CIAUpdateAppUi::HideApplicationInFSWL( TBool aHide )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HideApplicationInFSW() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] hide: %d", aHide );
    TInt id = iEikonEnv->RootWin().Identifier();

    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( 
                                                    iEikonEnv->WsSession(), id );
    
    wgName->SetHidden( aHide );
    wgName->SetWindowGroupName( iEikonEnv->RootWin() );    
    CleanupStack::PopAndDestroy( wgName ); 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HideApplicationInFSW() end");
    }
 
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::ShowGlobalErrorNoteL
// 
// -----------------------------------------------------------------------------
//     
void CIAUpdateAppUi::ShowGlobalErrorNoteL( TInt aError ) const
    {
    CErrorUI* errorUI = CErrorUI::NewLC( );     
    errorUI->ShowGlobalErrorNoteL( aError );        
    CleanupStack::PopAndDestroy( errorUI ); 	
    }



// ---------------------------------------------------------------------------
// CIAUpdateAppUi::UpdateQueryCallbackL
// ---------------------------------------------------------------------------
//
TInt CIAUpdateAppUi::UpdateQueryCallbackL( TAny* aPtr )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::UpdateQueryCallbackL() begin");
    CIAUpdateAppUi* appUI= static_cast<CIAUpdateAppUi*>( aPtr ); 
    TRAPD( err, appUI->ShowUpdateQueryL() );
    if ( err != KErrNone )
        {
    	appUI->HandleLeaveErrorL( err );
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::UpdateQueryCallbackL() end");
    return KErrNone;
    }    

    
// ---------------------------------------------------------------------------
// CIAUpdateAppUi::AutomaticCheckCallbackL
// ---------------------------------------------------------------------------
//    
    
TInt CIAUpdateAppUi::AutomaticCheckCallbackL( TAny* aPtr )    
    {
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::AutomaticCheckCallbackL() begin");
	CIAUpdateAppUi* appUI= static_cast<CIAUpdateAppUi*>( aPtr ); 
	
	TInt err = KErrNone;
	CIAUpdateAutomaticCheck* automaticCheck = NULL;
	TRAP( err, automaticCheck = CIAUpdateAutomaticCheck::NewL() ); 
    if ( err != KErrNone )
        {
    	appUI->HandleLeaveErrorL( err );
        }
    else
        {
        CleanupStack::PushL( automaticCheck );
        TRAP( err, automaticCheck->AcceptAutomaticCheckL() );
        if ( err != KErrNone )
            {
    	    appUI->HandleLeaveErrorL( err );
            }	
        } 
    
    CleanupStack::PopAndDestroy( automaticCheck );
	
	IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::AutomaticCheckCallbackL() end");
	return KErrNone;
    }
    
    
    
// -----------------------------------------------------------------------------
// CIAUpdateAppUi::HandleTextLinkL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::HandleTextLinkL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleTextLinkL() begin");
    if ( !iHistoryView )
        {
        iHistoryView = CIAUpdateHistoryView::NewL( iController->HistoryL() );
         
        AddViewL( iHistoryView );
        }
    ActivateLocalViewL( TUid::Uid( EIAUpdateHistoryViewId ) );
    iShowStatusDialogAgain = ETrue;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::HandleTextLinkL() end");
    }



// -----------------------------------------------------------------------------
// CIAUpdateAppUi::SetDMSupport
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateAppUi::SetDMSupport( TBool aDMSupport )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDMSupport begin");
    iIsDMSupported  = aDMSupport;
    iIsDMSet = ETrue;;
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::SetDMSupport end");
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::IsDMSupport
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateAppUi::IsDMSupport()
    {
    return iIsDMSupported;
    }


// -----------------------------------------------------------------------------
// CIAUpdateAppUi::IsDMSupportSet
// 
// -----------------------------------------------------------------------------
//
TBool CIAUpdateAppUi::IsDMSupportSet()
    {
    return iIsDMSet;
    }

TBool CIAUpdateAppUi::UiRefreshAllowed()
    {
    return iUiRefreshAllowed;
    }

// ---------------------------------------------------------------------------
// CIAUpdateAppUi::ForegroundDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAppUi::ForegroundDelayComplete( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateAppUi::ForegroundDelayComplete() begin: %d",
                     aError);

    delete iForegroundTimer;
    iForegroundTimer = NULL;
    
    iEikonEnv->RootWin().SetOrdinalPosition( 0 );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::ForegroundDelayComplete() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateAppUi::BackgroundDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAppUi::BackgroundDelayComplete( TInt aError )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateAppUi::BackgroundDelayComplete() begin: %d",
                     aError);

   delete iBackgroundTimer;
    iBackgroundTimer = NULL;
    
    iEikonEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateAppUi::BackgroundDelayComplete() end");
    }

// ---------------------------------------------------------------------------
// CIAUpdateAppUi::ProcessStartDelayComplete
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateAppUi::ProcessStartDelayComplete( TInt /*aError*/ )
    {
    }
    

// End of File  
