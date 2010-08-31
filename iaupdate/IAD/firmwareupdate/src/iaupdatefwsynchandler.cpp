/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDES
#include <StringLoader.h>
#include <e32property.h>
#include <data_caging_path_literals.hrh>  // for resource and bitmap directories
#include <SyncMLErr.h>      // sync error codes
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include <rconnmon.h>
#include <es_enum.h>

#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginpacketdatadef.h>

#include <cmdestination.h>
#include <cmmanagerdef.h>

#include <uikon.hrh>

#include "iaupdateconnectionmethod.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatefwfotamodel.h"
#include "iaupdatefwsyncappengine.h"
#include "iaupdatefwsynchandler.h"
#include "iaupdatefwsyncstate.h"
#include "iaupdatefwdebug.h"
#include "iaupdateuids.h"

#include "iaupdatefwconst.h"
#include "iaupdatefwnsmlcrkeys.h"
#include "iaupdatefwnsmlpskeys.h"

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncHandler* CIAUpdateFWSyncHandler::NewL( RSyncMLSession* aSyncSession,
                                              CIAUpdateFWSyncAppEngine* aAppEngine,
                                              CIAUpdateFWFotaModel* aFotaModel )
    {

    CIAUpdateFWSyncHandler* self =
        new (ELeave) CIAUpdateFWSyncHandler( aSyncSession,
                                         aAppEngine,
                                         aFotaModel );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncHandler::~CIAUpdateFWSyncHandler()
    {
    /*if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }*/
    
	delete iState;
	delete iActiveCaller;

	if ( iSyncRunning )
	    {
	    TRAP_IGNORE( iSyncJob.StopL() );
            iSyncJob.Close();
	    }
	    
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::ConstructL( void )
    {
      
	iState = CIAUpdateFWSyncState::NewL();
	iActiveCaller = CIAUpdateFWActiveCaller::NewL(this);
	
	iSyncRunning = EFalse;
	iSyncError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::CIAUpdateFWSyncHandler
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncHandler::CIAUpdateFWSyncHandler( RSyncMLSession* aSyncSession,
                                        CIAUpdateFWSyncAppEngine* aAppEngine,
                                        CIAUpdateFWFotaModel* aFotaModel  ) 
    : iSyncSession( aSyncSession ),
    iSyncAppEngine( aAppEngine ),
    iFotaModel( aFotaModel )
	{
	}


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aJobId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
	{

	iServerName = aServerName;
	iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iJobId = aJobId;
    iUseFotaProgressNote = aUseFotaProgressNote;
    
    iSyncJob.OpenL( Session(), iJobId );
    iSyncJobId = iSyncJob.Identifier();
    SynchronizeL();
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
    {
 
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL begin");
    
    iServerName = aServerName;
    iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iUseFotaProgressNote = aUseFotaProgressNote;
    
    //select the IAP which will be used for sync
    SelectIAPL();
    
    iSyncJob.CreateL( Session(), iProfileId );
    iSyncJobId = iSyncJob.Identifier();
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 1");
    SynchronizeL();
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 2");
    TInt dummyValue;
    TInt err = RProperty::Get( KPSUidNSmlDMSyncApp,
                               KNSmlDMSyncUiInitiatedJobKey,
                               dummyValue );
    if ( err == KErrNotFound )
        {
        _LIT_SECURITY_POLICY_S0( KWritePolicy, KIAUpdateUiUid );
        _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
        RProperty::Define( KPSUidNSmlDMSyncApp,
                           KNSmlDMSyncUiInitiatedJobKey,
                           RProperty::EInt,
                           KReadPolicy,
                           KWritePolicy );
        }
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KNSmlDMSyncUiInitiatedJobKey,
                    iSyncJobId );
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::SynchronizeL()
	{
	TInt err = KErrNone;
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 12");
	// for MSyncMLEventObserver events
	TRAP( err, Session().RequestEventL(*this) );
	if ( err != KErrNone )
		{
		FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 13");
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave(err);
		}
    
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 14");
    // for MSyncMLProgressObserver events
    TRAP( err, Session().RequestProgressL(*this) );
	if ( err != KErrNone )
		{
		FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 15");
		Session().CancelEvent();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 16");
	State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseConnecting );
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 17");
	
	TRAP( err, ShowProgressDialogL() );
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeL 18");
	if ( err != KErrNone )
		{
		Session().CancelEvent();
		Session().CancelProgress();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}

	iFotaModel->MarkFwUpdChangesStartL();
	
	iSyncRunning = ETrue;
	iSyncError = KErrNone;
	}


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SelectIAPL()
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::SelectIAPL()
    {
    CIAUpdateFWSyncProfile* profile = iSyncAppEngine->OpenProfileL( iProfileId, ESmlOpenReadWrite );
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL 15");
    
    //get the current access point in DM profile
    TInt dmIap = profile->AccessPointL();

         
    // if in the default DM profile, the IAP is set to Always ask. 
    // we should choose an IAP here. 
    // if the IAP is not Always ask, we don't choose but use it.
    
    if (dmIap < 0)
        {
        FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL Always ask is the IAP for default DM profile");
        //the current IAP is "Always ask"
        CRepository* centrep = CRepository::NewL(  KCRUidIAUpdateSettings );
        
        //Fetch a working IAP use our own logic
        TUint32 iapid = SelectConnectionMethodL();
        
        if ( iapid )
            {
            FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL we found working IAP");
            //found working IAP found
            //save it to the DM profile and mark the cenrep overwritten indicator
            if (centrep)
                { 
                FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL save original IAP (should be alway ask as always) to cen rep");
                //save the original IAP in cenrep also, so we can set DM profile back when FOTA is done
                centrep->Set(  KIAUpdateFWDefaultIAPId, dmIap );                    
                FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL Mark it as overwritten");
                 //Marked that default DM IAP was overwritten
                 centrep->Set(  KIAUpdateFWDefaultIAPIsOverwritten, 1 );                    
                 } 
            FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL save the chosen IAP into DM profile");
             //set the chosen one to DM profile
             profile->SetAccessPointL(iapid); 
             
             FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL 21");
             profile->SaveL();
             }
        else
            {
            //no working IAP found, access point dialog will pop to user later to select access point
            FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL DM profile has always ask setting for IAP and we didn't found any working one either");
            }

        delete centrep; 
        centrep = NULL;
        }       



    iSyncAppEngine->CloseProfile();
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SelectIAPL end");

          
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SynchronizeCompletedL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::SynchronizeCompletedL( TInt aError )
	{
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL");
	
	if ( !SyncRunning() )
		{
		FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL 1");
		return;  // sync has already completed
		}
	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL 2");	
    iSyncJob.Close();
    
    // Error code can be ignored.
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KNSmlDMSyncUiInitiatedJobKey,
                    KNSmlDMNoUserInitiatedJob );
	
	iSyncRunning = EFalse;
	iSyncError = aError;

    /*if ( iWaitDialog )
        {
        
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }*/

	iUseFotaProgressNote = EFalse;
    iSyncJob.Close();
    FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL 13");
	// handle error in HandleActiveCallL (when active caller completes)
	iActiveCaller->Start( aError );

	FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL 14");

	
	//the sync with DM server is done if no error we should get DM profile prepared since the real firmware download and
	//update are about to start.
    CIAUpdateFWSyncProfile* profile = 
                     iSyncAppEngine->OpenProfileL( iProfileId, ESmlOpenReadWrite );
	
	if ( aError == KErrNone )
	    {
	     FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::SynchronizeCompletedL 15");    

	     TBool bIsActive  = ETrue;

	     CRepository* centrep = NULL;
	     TRAPD( err, centrep = CRepository::NewL(  KCRUidDeviceManagementInternalKeys ) );
	     if (err == KErrNone)
	         {
	         centrep->Get(  KNSmlFotaProfileDefaultIsActive, bIsActive );                
	         delete centrep; 
	         centrep = NULL;         
	         }

	     if (!bIsActive)
	         {
	         profile->SetSASyncStateL(ESASyncStateDisable);      
	         }
	    }
	else
	    {
	    //sync with DM server failed for some reason
	    FLOG_NUM("[IAUPDATE] CIAUpdateFWSyncHandler Sync with DM failed with error = %d", aError );
	    }
	    

    //set the IAP back to the original one. After sync, the IAP is saved in FOTA side, 
    //no need to keep it anymore.	 
    CRepository* centrep = CRepository::NewLC(  KCRUidIAUpdateSettings );
	  //set the IAP back to the original one
    TInt wasDMDefaultIAPOverwritten = EFalse;
    centrep->Get(  KIAUpdateFWDefaultIAPIsOverwritten, wasDMDefaultIAPOverwritten );                    
        
    if (wasDMDefaultIAPOverwritten)
        {
        TInt initialDMIAPId = -1;
        centrep->Get(  KIAUpdateFWDefaultIAPId, initialDMIAPId );                     
        TInt currentIAP = profile->AccessPointL();              
            
        if (currentIAP != initialDMIAPId)
            {
            profile->SetAccessPointL(initialDMIAPId);                         
                
            }  
         centrep->Set(  KIAUpdateFWDefaultIAPIsOverwritten, EFalse );       
         }
   
  CleanupStack::PopAndDestroy( centrep );
	    
	//save the changes    
	profile->SaveL();   
	 
	iSyncAppEngine->CloseProfile();
	}


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SelectConnectionMethodL
// -----------------------------------------------------------------------------
//
TUint32 CIAUpdateFWSyncHandler::SelectConnectionMethodL()
    {
    // from back ground checker, choose the IAP to make the internet access silent
    TUint32 connectionMethodId( 0 );
    TInt connMethodId( 0 );

    // Let's first check whether cenrep contains SNAP id other than zero
    CRepository* cenrep( CRepository::NewLC( KCRUidIAUpdateSettings ) );
    User::LeaveIfError(  
        cenrep->Get( KIAUpdateAccessPoint, connMethodId ) );
    CleanupStack::PopAndDestroy( cenrep ); 
    cenrep = NULL;

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );
    
    if ( connMethodId == -1 )
        {
        //check what is the default connection by users     
        
        TCmDefConnValue DCSetting;
        cmManager.ReadDefConnL( DCSetting );
       
        
        switch ( DCSetting.iType )
            {
            case ECmDefConnAlwaysAsk:
            case ECmDefConnAskOnce:
                {
                //go with the best IAP under internet snap
                connectionMethodId = GetBestIAPInInternetSNAPL( cmManager );
                break;
                }
            case ECmDefConnDestination:
                {
                //go with the best IAP under this snap
                connectionMethodId = GetBestIAPInThisSNAPL( cmManager, DCSetting.iId );
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
        connectionMethodId = GetBestIAPInInternetSNAPL( cmManager );
        }
    else
        {

        // It was some SNAP value
        connectionMethodId = GetBestIAPInThisSNAPL( cmManager, connMethodId );
        }

    CleanupStack::PopAndDestroy( &cmManager ); 
    
    return connectionMethodId;
              
    }


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::GetBestIAPInInternetSNAPL
// -----------------------------------------------------------------------------
//
TUint32 CIAUpdateFWSyncHandler::GetBestIAPInInternetSNAPL( RCmManager& aCmManager )
    {
    //select IAP from Internet SNAP
    RArray<TUint32> destIdArray;
    aCmManager.AllDestinationsL( destIdArray );
    TUint32 InternetSNAPID = 0;
    for ( TInt i = 0; i< destIdArray.Count(); i++ )
        {
        RCmDestination dest = aCmManager.DestinationL( destIdArray[i] );
        CleanupClosePushL( dest );
                                     
        if ( dest.MetadataL( CMManager::ESnapMetadataInternet ) )
            {
            InternetSNAPID = destIdArray[i];
            CleanupStack::PopAndDestroy( &dest ); 
            break;
            }                     
         CleanupStack::PopAndDestroy( &dest ); 
         }
    destIdArray.Reset();
    
    return GetBestIAPInThisSNAPL( aCmManager, InternetSNAPID );
    }



// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::GetBestIAPInThisSNAPL
// -----------------------------------------------------------------------------
//
TUint32 CIAUpdateFWSyncHandler::GetBestIAPInThisSNAPL( RCmManager& aCmManager, TUint32 aSNAPID  )
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
    
    RCmDestination dest = aCmManager.DestinationL( aSNAPID );
    CleanupClosePushL( dest );
    
    // Check whether the SNAP contains any IAP.
    for  (TInt i = 0; i < dest.ConnectionMethodCount(); i++ )
        {
        RCmConnectionMethod cm =  dest.ConnectionMethodL( i );
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
// CIAUpdateFWSyncHandler::OnSyncMLSessionEvent
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::OnSyncMLSessionEvent( TEvent aEvent,
                                               TInt aIdentifier,
                                               TInt aError,
                                               TInt /*aAdditionalData*/ )
	{
	 FLOG_NUM(" aEvent %d", aEvent );
	 FLOG_NUM(" aError %d", aError );
	 FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::OnSyncMLSessionEvent ");
	if ( aEvent == EJobStop
	    || aEvent == EJobStartFailed
	    || aEvent == EJobRejected )
		{

		if ( iSyncJobId == aIdentifier )
			{

            TRAP_IGNORE( SynchronizeCompletedL( aError ) );
			}
		}
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::OnSyncMLSyncError
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::OnSyncMLSyncError( TErrorLevel aErrorLevel,
                                            TInt aError,
                                            TInt /*aTaskId*/,
                                            TInt /*aInfo1*/,
                                            TInt /*aInfo2*/)
	{
	FLOG_NUM(" aError %d", aError );
	 FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::OnSyncMLSyncError ");
	if ( aErrorLevel == ESmlFatalError )
		{
		 FLOG("[IAUPDATE] CIAUpdateFWSyncHandler::OnSyncMLSyncError fatal error ");
        TRAP_IGNORE( SynchronizeCompletedL( aError ) );
		}

	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::OnSyncMLSyncProgress
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::OnSyncMLSyncProgress( TStatus aStatus,
                                               TInt /*aInfo1*/,
                                               TInt /*aInfo2*/ )
	{
	switch (aStatus)
		{
		case ESmlConnecting:
		    FLOG("OnSyncMLSyncProgress::ESmlConnecting");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseConnecting );
		    break;
		case ESmlConnected:
		    FLOG("OnSyncMLSyncProgress::ESmlConnected");
		    break;
		case ESmlLoggingOn:
		    FLOG("OnSyncMLSyncProgress::ESmlLoggingOn");
		    break;
		case ESmlLoggedOn:
		    FLOG("OnSyncMLSyncProgress::ESmlLoggedOn");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseSynchronizing );
		    break;
		case ESmlDisconnected:
		    FLOG("OnSyncMLSyncProgress::ESmlDisconnected");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseDisconnecting );
		    break;
		case ESmlCompleted:
		    FLOG("OnSyncMLSyncProgress::ESmlCompleted");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseDisconnecting );
		    break;
		case ESmlProcessingServerCommands:
		    FLOG("OnSyncMLSyncProgress::ESmlProcessingServerCommands");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseSending );
		    break;
		case ESmlReceivingServerCommands:
		    FLOG("OnSyncMLSyncProgress::ESmlReceivingServerCommands");
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseReceiving );
		    break;
		default:
		    break;
		}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::OnSyncMLDataSyncModifications
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::OnSyncMLDataSyncModifications( TInt /*aTaskId*/,
                   const TSyncMLDataSyncModifications& /*aClientModifications*/,
                   const TSyncMLDataSyncModifications& /*aServerModifications*/ )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::HandleActiveCallL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::HandleActiveCallL()
	{

    FLOG("CIAUpdateFWSyncHandler::HandleActiveCallL 1")
    // HandleSyncErrorL will set this to true if
    // sync will be retried.
    iRetrySync = EFalse;

	if ( (iSyncError != KErrNone) && (iSyncError != KErrCancel) )
		{
		 FLOG("CIAUpdateFWSyncHandler::HandleActiveCallL 2")
		TRAP_IGNORE( HandleSyncErrorL() );
		}
	if ( iRetrySync == EFalse )
		{
		 FLOG("CIAUpdateFWSyncHandler::HandleActiveCallL 3")
	    // Inform parent that sync is done.
	    iSyncAppEngine->SyncCompleted( ENSmlSyncComplete );		
		 FLOG("CIAUpdateFWSyncHandler::HandleActiveCallL 4")
		}
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::HandleSyncErrorL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::HandleSyncErrorL()
	{

	FLOG("CIAUpdateFWSyncHandler::HandleSyncErrorL 1")
	if ( iSyncError != KDMErr )
		{
		FLOG("CIAUpdateFWSyncHandler::HandleSyncErrorL 2")
		}

	   FLOG("CIAUpdateFWSyncHandler::HandleSyncErrorL 4")
	if ( ( iSyncError == SyncMLError::KErrAuthenticationFailure ) ||
	    ( iSyncError == SyncMLError::KErrTransportAuthenticationFailure ) )
        {
        //we don't handle this at the moment
        FLOG("CIAUpdateFWSyncHandler::HandleSyncErrorL 5")
        }
	FLOG("CIAUpdateFWSyncHandler::HandleSyncErrorL 8")
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::ShowProgressDialogL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::ShowProgressDialogL( )
	{
        //iWaitDialog = new ( ELeave ) CAknWaitDialog(
        //    ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ) );
        //iWaitDialog->ExecuteLD( R_FOTA_CHECK_WAIT_NOTE );
        //iWaitDialog->SetCallback( this );
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::HideProgressDialogL
// -----------------------------------------------------------------------------
//
void CIAUpdateFWSyncHandler::HideProgressDialogL()
    {
    /*if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }*/
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession& CIAUpdateFWSyncHandler::Session()
	{
	__ASSERT_DEBUG(iSyncSession, TUtil::Panic(KErrGeneral));
	
	return *iSyncSession;
	}


// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::SyncRunning
// -----------------------------------------------------------------------------
//
TBool CIAUpdateFWSyncHandler::SyncRunning()
	{   
	return iSyncRunning;
	}

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::State
// -----------------------------------------------------------------------------
//
CIAUpdateFWSyncState* CIAUpdateFWSyncHandler::State()
	{
	__ASSERT_DEBUG(iState, TUtil::Panic(KErrGeneral));

	return iState;
	}
	
// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::CancelSynchronizeL
// -----------------------------------------------------------------------------
//		
void CIAUpdateFWSyncHandler::CancelSynchronizeL()
    { 
    if ( iSyncRunning )
        {
        iSyncJob.StopL();
        SynchronizeCompletedL( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateFWSyncHandler::DialogDismissedL
// -----------------------------------------------------------------------------
//		
void CIAUpdateFWSyncHandler::DialogDismissedL( TInt aButtonId )
    {
	if ( aButtonId == EEikBidCancel )
		{

		if ( SyncRunning() )
			{

			TRAP_IGNORE( iSyncJob.StopL() );
		    
		    State()->SetSyncPhase( CIAUpdateFWSyncState::EPhaseCanceling );
			}
		}
    }


// End of File
