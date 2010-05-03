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
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <rconnmon.h>
#include <apgwgnam.h>

#include "iaupdateengine.h"
#include "iaupdateserviceprovider.h"
#include "iaupdateuicontroller.h"
#include "iaupdategloballockhandler.h"
#include "iaupdatenodefilter.h"
#include "iaupdateresult.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdateuiconfigdata.h"
#include "iaupdatequeryhistory.h"
#include "iaupdateparameters.h"
#include "iaupdatedebug.h"

IAUpdateEngine::IAUpdateEngine(QObject *parent)
    : QObject(parent),
      iController(NULL),
      iGlobalLockHandler(NULL),
      iIdle(NULL),
      iUpdateNow(EFalse),
      iRequestIssued(EFalse),
      iStartedFromApplication(EFalse),
      iUpdatequeryUid(0)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::IAUpdateEngine() begin");
    iEikEnv = CEikonEnv::Static();
    mServiceProvider = new IAUpdateServiceProvider( *this );
    connect(mServiceProvider, SIGNAL(clientDisconnected()), this, SLOT(handleAllClientsClosed()));
    TRAP_IGNORE( iController = CIAUpdateUiController::NewL( *this ));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::IAUpdateEngine() end")
}


IAUpdateEngine::~IAUpdateEngine()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::~IAUpdateEngine() begin");
    InformRequestObserver( KErrCancel );
    delete iGlobalLockHandler;
    delete iController;
    delete mServiceProvider;    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::~IAUpdateEngine() end");
}

// -----------------------------------------------------------------------------
// IAUpdateEngine::StartedByLauncherL
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::StartedByLauncherL( TBool aRefreshFromNetworkDenied )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartedByLauncherL() begin");
    iRequestIssued = ETrue;
    iRequestType = IAUpdateUiDefines::ENoRequest;
    iController->SetRequestType( iRequestType );
    SetVisibleL( ETrue );
    CIAUpdateParameters* params = iController->ParamsReadAndRemoveFileL();
    iController->CheckUpdatesDeferredL( params, aRefreshFromNetworkDenied );
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartedByLauncherL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::CheckUpdatesRequestL
// 
// -----------------------------------------------------------------------------
//
void IAUpdateEngine::CheckUpdatesRequestL( int wgid, CIAUpdateParameters* aFilterParams )
                                           
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::CheckUpdatesRequestL() begin");
    SetClientWgId( wgid );
    iRequestIssued = ETrue;
    iStartedFromApplication  = ETrue;
    CleanupStack::PushL( aFilterParams );
    if ( wgid > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
    CleanupStack::Pop( aFilterParams );
    
    if ( !aFilterParams->ShowProgress() )
        {
        iEikEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront ); 
        }
           
    iRequestType = IAUpdateUiDefines::ECheckUpdates; 
    iController->SetRequestType( iRequestType );
    
    iController->CheckUpdatesDeferredL( aFilterParams, EFalse ); 
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::CheckUpdatesRequestL() end");
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowUpdatesRequestL
// 
// -----------------------------------------------------------------------------
// 
void IAUpdateEngine::ShowUpdatesRequestL( int wgid, CIAUpdateParameters* aFilterParams )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdatesRequestL() begin"); 
    SetClientWgId( wgid );
    iRequestIssued = ETrue;
//    delete iBackgroundTimer;
//    iBackgroundTimer = NULL;
    iStartedFromApplication  = ETrue;
    CleanupStack::PushL( aFilterParams );
    if ( wgid > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
   
    //StatusPane()->MakeVisible( ETrue );
    iEikEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );
    
    //iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::EShowUpdates;
    iController->SetRequestType( iRequestType );
     

    //if ( !iMainView )
    //    {
    //    iMainView  = CIAUpdateMainView::NewL( ClientRect() ); 
    //    AddViewL( iMainView );
    //    }
    
      
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

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdatesRequestL() end"); 
    }
    

// -----------------------------------------------------------------------------
// IAUpdateEngine::ShowUpdateQueryRequestL
// 
// -----------------------------------------------------------------------------
//     
void IAUpdateEngine::ShowUpdateQueryRequestL( int wgid, TUint aUid )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryRequestL begin");
    SetClientWgId( wgid );
    iRequestIssued = ETrue;
    iStartedFromApplication  = ETrue;
    iUpdatequeryUid = aUid;
    iUpdateNow = EFalse;
    if ( wgid > 0 )
        {
        HideApplicationInFSWL( ETrue );
        }
    
    //iRequestObserver = &aObserver;
    iRequestType = IAUpdateUiDefines::EUpdateQuery;
    
    
    delete iIdle;
    iIdle = NULL;
    iIdle = CIdle::NewL( CActive::EPriorityIdle ); 
    iIdle->Start( TCallBack( UpdateQueryCallbackL, this ) ); 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryRequestL end")
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::SetVisibleL
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::SetVisibleL( TBool aVisible )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetVisibleL() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] visible: %d", aVisible );
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
void IAUpdateEngine::SetClientWgId( TInt aWgId )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateEngine::SetClientWgId() wgId %d", aWgId );
    iWgId = aWgId;
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::ClientInBackgroundL
// 
// -----------------------------------------------------------------------------
// 
TInt IAUpdateEngine::ClientInBackgroundL() const
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ClientInBackgroundL() begin");
    TBool inBackground = EFalse;
    if ( iWgId > 0 )
        {
        CArrayFixFlat<TInt>*  wgArray = new( ELeave ) CArrayFixFlat<TInt>(10);  
        CleanupStack::PushL( wgArray );
        User::LeaveIfError( iEikEnv->WsSession().WindowGroupList( 0, wgArray ) );  
        TInt ownWgId = iEikEnv->RootWin().Identifier();
        if ( ( wgArray->At( 0 ) != ownWgId ) && ( wgArray->At( 0 ) != iWgId  ) )
            {
            inBackground = ETrue;
            }
        CleanupStack::PopAndDestroy( wgArray );  
        }
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateEngine::ClientInBackgroundL() inBackground: %d", inBackground );
    return inBackground;  
    }



// -----------------------------------------------------------------------------
// IAUpdateEngine::refresh
// 
// -----------------------------------------------------------------------------
//  
/*void IAUpdateEngine::refresh(int error) 
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::refresh() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] Error code: %d", error );
    //iMainView->RefreshL( iController->Nodes(), iController->FwNodes(), aError );                    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::refresh() end");
    }*/


void IAUpdateEngine::handleAllClientsClosed()
{
    qApp->quit(); 
}


// -----------------------------------------------------------------------------
// IAUpdateEngine::StartupComplete
// 
// -----------------------------------------------------------------------------
//    
void IAUpdateEngine::StartupComplete( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupComplete() begin"); 
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
                        //from bgchecker, make it silent
                        totalSilent = ETrue;
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
 
       
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::StartupCompleteL() end");    
    }
    
 
// -----------------------------------------------------------------------------
// IAUpdateEngine::HandleLeaveErrorL
// 
// -----------------------------------------------------------------------------
//        
void IAUpdateEngine::HandleLeaveErrorL( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorL() begin");
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    // client request is completed before leave in case of leave error
    if ( aError != KErrNone ) 
        {   
        InformRequestObserver( aError );
        User::Leave( aError );
        }
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorL() end");
    }


// -----------------------------------------------------------------------------
// IAUpdateEngine::HandleLeaveErrorWithoutLeave
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::HandleLeaveErrorWithoutLeave( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorWithoutLeave() begin");
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;
    if ( aError != KErrNone ) 
        {
        InformRequestObserver( aError );
        }
    if ( aError == KErrDiskFull )
        {
        //TRAP_IGNORE( ShowGlobalErrorNoteL( aError ) );
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HandleLeaveErrorWithoutLeave end");
    }


// -----------------------------------------------------------------------------
// IAUpdateEngine::RefreshCompleteL
// 
// -----------------------------------------------------------------------------
//      
void IAUpdateEngine::RefreshCompleteL( TBool /*aWithViewActivation*/, TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::RefreshCompleteL() begin");
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
        emit refresh( iController->Nodes(), iController->FwNodes(), aError );   
        //if ( aWithViewActivation)
          //  {
          //  ActivateLocalViewL( TUid::Uid( EIAUpdateMainViewId ) );
          //  }
 
        // By calling CIdle possible waiting dialog can be closed before
        // automatic check where a new dialog may be launched
        //delete iIdleAutCheck;
        //iIdleAutCheck = NULL;
        //iIdleAutCheck = CIdle::NewL( CActive::EPriorityIdle ); 
        //iIdleAutCheck->Start( TCallBack( AutomaticCheckCallbackL, this ) );*/
        } 
 
  
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::RefreshCompleteL() end");        
    }

// -----------------------------------------------------------------------------
// IAUpdateEngine::UpdateCompleteL
// 
// -----------------------------------------------------------------------------
//     
void IAUpdateEngine::UpdateCompleteL( TInt aError )    
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateCompleteL begin");
    IAUPDATE_TRACE_1("[IAUPDATE] error code: %d", aError );
    //removal of iGlobalLockHandler releases possible global operation lock to other IAD instances
    delete iGlobalLockHandler;
    iGlobalLockHandler = NULL;  
    if ( iRequestType != IAUpdateUiDefines::ENoRequest )
        {
        InformRequestObserver( aError );
        }
    
    //RefreshL( KErrNone ); 
      
    //ShowStatusDialogDeferredL();
                
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateCompleteL end");
    }



// -----------------------------------------------------------------------------
// IAUpdateEngine::InformRequestObserver
// 
// -----------------------------------------------------------------------------
//      
void IAUpdateEngine::InformRequestObserver( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::InformRequestObserver() begin");
        
    //if ( iRequestObserver )
    if ( iRequestIssued )    
        {
        if ( iController->ClosingAllowedByClient() )
            {
            if ( iRequestType != IAUpdateUiDefines::ENoRequest )
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
                      iEikEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNeverAtFront );
//                    }
                }
            }
        
        switch ( iRequestType )
            {
            case IAUpdateUiDefines::ENoRequest:
                {
                mServiceProvider->completeLauncherLaunch( aError );
                break;
                }
            case IAUpdateUiDefines::ECheckUpdates:
                {
                mServiceProvider->completeCheckUpdates( iController->CountOfAvailableUpdates(), aError );
                break;
                }
            case IAUpdateUiDefines::EShowUpdates:
                {
                CIAUpdateResult* result( NULL );
                TRAPD( error, result = CIAUpdateResult::NewL() )
                if ( result )
                    {
                    TIAUpdateResultsInfo resultsInfo( iController->ResultsInfo() );
                    IAUPDATE_TRACE_3("[IAUPDATE] IAUpdateEngine::InformRequestObserver succeed: %d failed: %d  cancelled: %d", 
                                                  resultsInfo.iCountSuccessfull, 
                                                  resultsInfo.iCountFailed, 
                                                  resultsInfo.iCountCancelled );
                    result->SetSuccessCount( resultsInfo.iCountSuccessfull );
                    result->SetFailCount( resultsInfo.iCountFailed );
                    result->SetCancelCount( resultsInfo.iCountCancelled );    
                    mServiceProvider->completeShowUpdates( result, aError );
                    // Ownership of result is transferred here.
                    }
                else
                    {
                    mServiceProvider->completeShowUpdates( NULL, error );
                    }
                break;
                }
            case IAUpdateUiDefines::EUpdateQuery:
                {
                mServiceProvider->completeUpdateQuery( iUpdateNow, aError );
                break;
                }
            default:
                {
                break;
                }
            }

        //iRequestObserver = NULL; 
        iRequestIssued = EFalse;
        }
        
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::InformRequestObserver() end");
    }


// ---------------------------------------------------------------------------
// IAUpdateEngine::SetDefaultConnectionMethodL
// Sets the connection method for the update network connection.
// ---------------------------------------------------------------------------
//
void IAUpdateEngine::SetDefaultConnectionMethodL( TBool aTotalSilent )
    {
    if ( aTotalSilent )
        {
        // from back ground checker, choose the IAP to make the internet access silent
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() begin");

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
               
               //the following code will pop up dialog to ask from user, just for proto
              /* connectionMethodId = 0;               
               TIAUpdateConnectionMethod connectionMethod( 
                   connectionMethodId, TIAUpdateConnectionMethod::EConnectionMethodTypeDefault );

               iController->SetDefaultConnectionMethodL( connectionMethod );*/
               }
           


           IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() end");
        }
    else
        {
        // from grid, use the old logic
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() begin");
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

                  IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::SetDefaultConnectionMethodL() end");
        }

    }


// ---------------------------------------------------------------------------
// IAUpdateEngine::GetBestIAPInAllSNAPsL
// Sets the best IAP from all snaps
// ---------------------------------------------------------------------------
//
TUint32 IAUpdateEngine::GetBestIAPInAllSNAPsL( RCmManagerExt& aCmManagerExt  )
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
// IAUpdateEngine::GetBestIAPInInternetSNAPL
// Sets the best IAP from internet snap
// ---------------------------------------------------------------------------
//
TUint32 IAUpdateEngine::GetBestIAPInInternetSNAPL( RCmManagerExt& aCmManagerExt  )
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
// IAUpdateEngine::GetBestIAPInThisSNAPL
// Sets the best IAP from the given snap
// ---------------------------------------------------------------------------
//
TUint32 IAUpdateEngine::GetBestIAPInThisSNAPL( RCmManagerExt& aCmManagerExt, TUint32 aSNAPID  )
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
    updateQueryHistory->SetDelay( iController->ConfigData().QueryHistoryDelayHours() );
    CleanupStack::PushL( updateQueryHistory );
    if ( !updateQueryHistory->IsDelayedL( iUpdatequeryUid ) )
        {
        if ( ClientInBackgroundL() )
            {
            iEikEnv->RootWin().SetOrdinalPosition( -1, ECoeWinPriorityNormal );
            }
        else
            {
            iEikEnv->RootWin().SetOrdinalPosition( 0, ECoeWinPriorityNormal );    
            }
 
        HbMessageBox messageBox(HbMessageBox::MessageTypeQuestion); 
        messageBox.setText(QString("Application update is available from Nokia. Update?"));
        HbAction nowAction("Now");
        HbAction laterAction("Later");
        messageBox.setPrimaryAction(&nowAction);
        messageBox.setSecondaryAction(&laterAction);
        messageBox.setTimeout(HbPopup::NoTimeout);
        messageBox.show();
        HbAction *selectedAction = messageBox.exec();
        
        if (selectedAction == messageBox.primaryAction())
            {
            IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryL() Now");
            iUpdateNow = ETrue;
            }
        else if (selectedAction == messageBox.secondaryAction())
            {
            IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryL() Later");
            updateQueryHistory->SetTimeL( iUpdatequeryUid );
            }
        }
    CleanupStack::PopAndDestroy( updateQueryHistory );
    InformRequestObserver( KErrNone );  
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::ShowUpdateQueryL() end");
    }



// -----------------------------------------------------------------------------
// IAUpdateEngine::HideApplicationInFSWL
// 
// -----------------------------------------------------------------------------
//  
void IAUpdateEngine::HideApplicationInFSWL( TBool aHide ) const
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HideApplicationInFSW() begin");
    IAUPDATE_TRACE_1("[IAUPDATE] hide: %d", aHide );
    TInt id = iEikEnv->RootWin().Identifier();

    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( 
            iEikEnv->WsSession(), id );
    
    wgName->SetHidden( aHide );
    wgName->SetWindowGroupName( iEikEnv->RootWin() );    
    CleanupStack::PopAndDestroy( wgName ); 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::HideApplicationInFSW() end");
    }

// ---------------------------------------------------------------------------
// IAUpdateEngine::UpdateQueryCallbackL
// ---------------------------------------------------------------------------
//
TInt IAUpdateEngine::UpdateQueryCallbackL( TAny* aPtr )
    {
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateQueryCallbackL() begin");
    IAUpdateEngine* engine = static_cast<IAUpdateEngine*>( aPtr ); 
    //TRAPD( err, engine->ShowUpdateQueryL() );
    TRAP_IGNORE( engine->ShowUpdateQueryL() );
    //if ( err != KErrNone )
    //    {
    //    appUI->HandleLeaveErrorL( err );
    //    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateEngine::UpdateQueryCallbackL() end");
    return KErrNone;
    }    





