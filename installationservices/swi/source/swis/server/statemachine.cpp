/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "statemachine.h"
#include "log.h"
#include "plan.h"
#include "swispubsubdefs.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK  
#include <usif/scr/appregentries.h>
#endif

namespace Swi
{

//
// CSwisStateMachine
//

CSwisStateMachine::CSwisStateMachine(const RMessage2& aMessage)
:	CActive(EPriorityNormal),
	iMessage(aMessage)
	{
	CActiveScheduler::Add(this);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CSwisStateMachine::CSwisStateMachine(const RMessage2& aMessage,TBool aInInfoMode)
:	CActive(EPriorityNormal),
	iMessage(aMessage)
	{
	CActiveScheduler::Add(this);
	// Set the state machine to operate on component information extraction mode 
	// (simply extract and return the info don't install the component).
	iIsInInfoMode = aInInfoMode;
	}
#endif

CSwisStateMachine::~CSwisStateMachine()
	{
	Cancel();
	// close UISS session
	iUiHandler.Close();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.Close();
	delete iRegistryWrapper;
	delete iProgressPublisher;
#else
	delete iIntegrityServices;
#endif
	
	// close SWI Observer session
	iObserver.Close();
	}

void CSwisStateMachine::ConstructL()
	{
	// mark the installation/un-installation operation as unconfirmed to start with
	iOperationConfirmed = EFalse;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.CreateTransactionL();	
	iRegistryWrapper = CRegistryWrapper::NewL();
	iProgressPublisher = CProgressBarValuePublisher::NewL();
	iUiHandler.SetProgressBarValuePublisher(iProgressPublisher);
#else
	// Create integrity services, use the current time as transaction ID
	TTime currentTime;
	currentTime.UniversalTime();
	_LIT(KIntegrityServicesPath, "\\sys\\install\\integrityservices\\");
	iIntegrityServices=CIntegrityServices::NewL(currentTime.Int64(), 
		KIntegrityServicesPath);
	// attempt to recover any previously failed installations
	// do not let failed rollback prevent this installation from taking place
	TRAP_IGNORE(iIntegrityServices->RollBackL(ETrue));
	if (iIntegrityServices->StartedJournalRollback())
		{
		// At least one journal file started being rolled back.  In case any
		// registry entries were removed, the cache must be regenerated.
		ResetRegistryCache();
		}
#endif
	}

TInt CSwisStateMachine::Start()
	{
	// connect to the UISS
	TInt err=iUiHandler.Connect();
	if (err!=KErrNone)
		{
		return err;
		}	
	// kick off
	CompleteSelf();
	SetActive();
	return KErrNone;
	}

void CSwisStateMachine::CompleteSelf()
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CSwisStateMachine::ChangeStateL(TState* aNextState)
	{
	// Enter aNextState and make it our current state
	TInt err=KErrNone;
	while (aNextState)
		{
		// State change required
		iState=aNextState;
		TRAP(err, iState->EnterL());
		aNextState=err ? ErrorOnStateEntryL(err) : NULL;
		}

	if (err!=KErrNone && aNextState==NULL)
		{
		// There was an error and we couldn't recover from it so leave with the error
		User::Leave(err);
		}
	}

void CSwisStateMachine::RunL()
	{
	if (iCancelled)
		{
		FinalizeJournalsL(KErrCancel);
		PostJournalFinalizationL(KErrCancel);
		CompleteClientL(KErrCancel);
		return;
		}
	
	if (iState==NULL)
		{
		ChangeStateL(FirstState());
		}
	else
		{
		// See how the last SetActive() ended up
		TInt err=iStatus.Int();
		// Decide what to do next
		TState* nextState=NULL;
		if (err==KErrNone)
			{
			// Everything was OK, leave this state
			nextState=iState->CompleteL();
			}
		else
			{
			// Something went wrong
			nextState=iState->ErrorL(err);
			}

		// Move on to the next state (could be NULL, in which case we're done)
		ChangeStateL(nextState);

		if (nextState==NULL)
			{
			FinalizeJournalsL(KErrNone);
			PostJournalFinalizationL(KErrNone);
			CompleteClientL(KErrNone);
			}
		}
	}

void CSwisStateMachine::PostJournalFinalizationL(TInt /*aError*/ )
	{
	
	}

void CSwisStateMachine::CompleteClientL(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Completing Install/Uninstall client with code '%d'"), aError);
	iMessage.Complete(aError);
	}

/**
 * If there is no error signals the client that the (un)install has finished before 
 * commiting the journals and kicking off the observer.
 * If there is an error the journals are rolled back.  
 */
void CSwisStateMachine::FinalizeJournalsL(TInt aError)
	{	
	if(aError == KErrNone)
		{		
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// When the state machine runs in non info collection (ie,. installation / uninstallation) mode,
		// We should signal the client before committing the transactions, as the client can still
		// abort the operation at the callback.
		if (!IsInInfoMode())
			{			
			SignalCompletedL();
	
			// NOTE: There is a caveat here as we need to commit two independant transactions and there 
			// exists a possibility that the second commit fails and this would leave the device in an 
			// inconsistent state - there is no known solution for this
			// Due to the fact that SCR allows only one transaction at a time (one writer, many readers), 
			// the possibility of the registry commit (SCR commit) failing is less likely than an STS commit 
			// failure - therefore we commit STS first and then the registry 
			iStsSession.CommitL();
			iRegistryWrapper->CommitMutableOperationsL();
			//If the operation is successfull, call Commit of the SWI Observer.
			//Otherwise, do not commit to allow SWI Observer to delete the observation file.
			iObserver.CommitL();
			}
		else
			{
			// We are runing in information collection mode. So, simply ignore the transaction details.
			// Close SWI Observer session to release the observation log file handle. 
			// Otherwise, the rollback would not be able to delete the swi observer log file. 
			iObserver.Close();
			iStsSession.RollBackL();
			iRegistryWrapper->RollbackMutableOperationsL();
			}
		#else
			SignalCompletedL();
			iIntegrityServices->CommitL();
			//If the operation is successfull, call Commit of the SWI Observer.
			//Otherwise, do not commit to allow SWI Observer to delete the observation file.
			iObserver.CommitL();
		#endif
		}
	else
		{
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
			// Close SWI Observer session to release the observation log file handle. 
			// Otherwise, the rollback would not be able to delete the swi observer log file. 
			iObserver.Close();
			iStsSession.RollBackL();
			iRegistryWrapper->RollbackMutableOperationsL();		
		#else
			// Close SWI Observer session to release the observation log file handle. 
			// Otherwise, the rollback would not be able to delete the swi observer log file. 
			iObserver.Close();
			iIntegrityServices->RollBackL(EFalse);
			ResetRegistryCache();
		#endif
		}
	}

// Inform the user of a fatal error. 	
void CSwisStateMachine::InformUserOfFatalErrorL(TInt aError)
	{
	// Try to inform the user
	TAppInfo appInfo(KNullDesC, KNullDesC, TVersion());
	switch (aError)
		{
		// We can get "file corrupt" error when
		// getting the controller, at which point the planner is not yet 
		// available.
		case KErrCorrupt: // file corrupt
			{
			CDisplayError* cmd=CDisplayError::NewLC(appInfo, EUiFileCorrupt,
				KNullDesC);
			iUiHandler.ExecuteL(*cmd);
			CleanupStack::PopAndDestroy(cmd);
			break;
			}
			
		// If the disk is not ready on reading controller or logo, etc.
		// then handle this with a disk not present error note			
		case KErrNotReady:
			{
			CDisplayError* event=CDisplayError::NewLC(appInfo, EUiDiskNotPresent, KNullDesC);
			iUiHandler.ExecuteL(*event);
			CleanupStack::PopAndDestroy(event);
			break;
			}
			
		case KErrDiskFull:
			{
			CDisplayError* event=CDisplayError::NewLC(appInfo, EUiInsufficientSpaceOnDrive, KNullDesC);
			iUiHandler.ExecuteL(*event);
			CleanupStack::PopAndDestroy(event);
			break;
			}	
				
		default:
			{
			break;
			}
		}					
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iStsSession.RollBackL();
	iRegistryWrapper->RollbackMutableOperationsL();
#else
	iIntegrityServices->RollBackL(EFalse);	
#endif
	}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CSwisStateMachine::ResetRegistryCache(void)
	{
 	// Need to get the registry to reload its cache since we may have deleted
	// files which were in it.
	// Nothing we can do about errors during rollback here
	RSisRegistryWritableSession session;
	if (KErrNone==session.Connect())
		{
		TRAP_IGNORE(session.RegenerateCacheL());
		session.Close();
		}
	}
#endif

// This is called if RunL() leaves.
TInt CSwisStateMachine::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Install/Uninstall Statemachine RunL() leave with code '%d'"), aError);
	
	TInt property = 0;
 	if (RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property) == KErrNone)
		{
		RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, property | ESwisStatusAborted);
		}	
	// Close SWI Observer session to release the observation log file handle. 
	// Otherwise, the rollback would not be able to delete the swi observer log file. 
	iObserver.Close();

	// RunError cannot leave so ignore UI problems when attempting 
	// to display fatal error messages.
	TRAPD(err, InformUserOfFatalErrorL(aError));
	if(err != KErrNone)
		{
		// Dialog failed so use its error code instead?!
		aError = err;
		}
		
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// don't want to regenerate the sisregistry if the user has declined the install/un-install
	// and the registry has not been altered, as this makes the UI unresponsive.
	if (iOperationConfirmed)
		{			
		// re-generate the sisregistry cache .. very time consuming!
		ResetRegistryCache();
		}
#else
	// Deregister the force registered applications from AppArc
	DEBUG_PRINTF(_L8("Deregistering the force registered applications with AppArc"));
	RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
	User::LeaveIfError(launcher.Connect());
	RArray<TAppUpdateInfo> emptyAppRegDataArray;
	launcher.NotifyNewAppsL(emptyAppRegDataArray);
	CleanupStack::PopAndDestroy(&launcher);
#endif

	iMessage.Complete(aError);
	return KErrNone;
	}

void CSwisStateMachine::DoCancel()
	{
	}

// called when an install is cancelled whilst files are being copied
void CSwisStateMachine::CancelInstallation()
	{
	DEBUG_PRINTF(_L8("Install/Uninstall cancelled"));
	
	if (!iCancelled)
		{
		iCancelled=ETrue;
		if(iState)
			{
			// cancel install
			iState->Cancel();
			}
		}
	}

void CSwisStateMachine::SetNextState(TState* aState)
	{
	iState=aState;
	}

void CSwisStateMachine::HandleInstallationEventL(CPlan* plan, TInstallEvent aEvent, TInt aValue)
	{
	TInt err=KErrGeneral;
	if (plan)
		{
		TRAP(err, plan->AppInfoL());
		}

	if (err==KErrNone)
		{
		CHandleInstallEvent* cmd = CHandleInstallEvent::NewLC(plan->AppInfoL(), aEvent, aValue, KNullDesC);
		UiHandler().ExecuteL(*cmd);

		if (!cmd->ReturnResult())
			{
			User::Leave(KErrCancel);
			}

		CleanupStack::PopAndDestroy(cmd);
		}
	else
		{
		TAppInfo appInfo(KNullDesC, KNullDesC, TVersion());
		CHandleInstallEvent* cmd = CHandleInstallEvent::NewLC(appInfo, aEvent, aValue, KNullDesC);
		UiHandler().ExecuteL(*cmd);

		if (!cmd->ReturnResult())
			{
			User::Leave(KErrCancel);
			}

		CleanupStack::PopAndDestroy(cmd);
		}
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TBool CSwisStateMachine::IsInInfoMode()
	{
	return iIsInInfoMode;
	}
	
void CSwisStateMachine::SetIsInInfoMode(TBool aOperationalMode)
	{
	iIsInInfoMode = aOperationalMode;
	}

void CSwisStateMachine::SetFinalProgressBarValue(TInt aValue)
	{
	iProgressPublisher->SetFinalProgressBarValue(aValue);
	}
#endif
} // namespace Swi
