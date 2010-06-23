/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "uninstallmachine.h"
#include "uninstallationplanner.h"
#include "uninstallationprocessor.h"
#include "log.h"
#include "sisregistrypackage.h"
#include "sisregistryentry.h"
#include "sisregistrysession.h"
#include "plan.h"
#include "swispubsubdefs.h"

#include <s32mem.h>

using namespace Swi;

//
// TUninstallState
//

CUninstallMachine::TUninstallState::TUninstallState(CUninstallMachine& aUninstallMachine)
	: iUninstallMachine(aUninstallMachine)
	{
	}

//
// TRegistrationState
//

CUninstallMachine::TRegistrationState::TRegistrationState(
	CUninstallMachine& aUninstallMachine)
/**
	Constructor.
 */
:   CUninstallMachine::TUninstallState(aUninstallMachine)
	{
	}

void CUninstallMachine::TRegistrationState::EnterL()
/**
	Send registration request to the SWI Observer and then activate the
	installation machine.
 */
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Entering Registration State"));

	//connect to the SWI Observer
	User::LeaveIfError(iUninstallMachine.Observer().Connect());
	
	//Register to the SWI Observer; which completes this request 
	//when the SWI Observer Processor is idle.
	iUninstallMachine.Observer().Register(iUninstallMachine.iStatus);
	iUninstallMachine.SetActive();
	}

CUninstallMachine::TState* CUninstallMachine::TRegistrationState::CompleteL()
/**
	Obtains the log file handle and its name from the SWI Observer.
	Adds the log file to the transaction
 */
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Registration State complete"));
	RBuf logFileName;
	logFileName.CreateL(KMaxFileName);
	logFileName.CleanupClosePushL();
	
	//Get created a log file and obtains its full name.
	iUninstallMachine.Observer().GetFileHandleL(logFileName);

	//Add the log file to the transaction
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iUninstallMachine.TransactionSession().RegisterNewL(logFileName);
#else
	iUninstallMachine.IntegrityServicesL().AddL(logFileName);
#endif

	CleanupStack::PopAndDestroy(&logFileName);
	// Get Controllers.
	return static_cast<TState*>(&iUninstallMachine.iConfirmationState);
	}
	
CUninstallMachine::TState* CUninstallMachine::TRegistrationState::ErrorL(TInt aCode)
/**
	If there is any error, closes the SWI Observer connection.
 */
	{
	DEBUG_PRINTF2(_L8("Uninstall Machine - Registration State failed with code '%d'"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CUninstallMachine::TRegistrationState::Cancel()
/**
	Cancels the registration request.
 */
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Registration State cancelled!"));
	iUninstallMachine.Observer().CancelRegistration();	
	}
	
//
// TConfirmationState
//

CUninstallMachine::TConfirmationState::TConfirmationState(
	CUninstallMachine& aUninstallMachine)
	: CUninstallMachine::TUninstallState(aUninstallMachine)
	{
	}

// Calls Uninstall dialog.
void CUninstallMachine::TConfirmationState::EnterL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Entering Confirmation State"));
	
	// Construct a TAppInfo object for the dialog. For that we need app name, 
	// vendor name, and version. The first two are already supplied in the 
	// \e iPackage member which is a pointer to CSisRegistryPackage object. 
	// Getting package version involves more work.
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	RSisRegistrySession regSession;
	User::LeaveIfError(regSession.Connect());
	CleanupClosePushL(regSession);
#endif
	RSisRegistryEntry regEntry;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	User::LeaveIfError(regEntry.OpenL(iUninstallMachine.RegistryWrapper().RegistrySession(), *iUninstallMachine.iPackage));
#else
	User::LeaveIfError(regEntry.OpenL(regSession, *iUninstallMachine.iPackage));
#endif
	CleanupClosePushL(regEntry);
	TVersion ver=regEntry.VersionL();
	HBufC* localizedVendorName=regEntry.LocalizedVendorNameL();
	CleanupStack::PushL(localizedVendorName);

	// At last we have everything to construct TAppInfo object.
	TAppInfo appInfo(iUninstallMachine.iPackage->Name(),
		*localizedVendorName,ver);
	
	// Construct and execute uninstall dialog command.
	CDisplayUninstall* cmd=CDisplayUninstall::NewLC(appInfo);
	iUninstallMachine.UiHandler().ExecuteL(*cmd);
	TBool rv=cmd->ReturnResult();
	CleanupStack::PopAndDestroy(cmd);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CleanupStack::PopAndDestroy(2, &regEntry); // regEntry, localizedVendorName
#else
	CleanupStack::PopAndDestroy(3, &regSession); // regEntry, localizedVendorName
#endif
	// Make the decision and leave if the user cancelled uninstallation.
	if (!rv)
		User::Leave(KErrCancel);
	
	// Continue uninstallation.	
	iUninstallMachine.CompleteSelf();
	iUninstallMachine.SetActive();
	
	// The user hasn't cancelled so mark the un-installation as confirmed. This
	// will allow the registry cache to be regenerated further on during the
	// un-installation process.
	iUninstallMachine.iOperationConfirmed = ETrue;
	}

// The next state will be planning uninstallation.
CUninstallMachine::TState* CUninstallMachine::TConfirmationState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Completed Confirmation State"));
	return static_cast<TState*>(&iUninstallMachine.iPlanUninstallationState);
	}

CUninstallMachine::TState* CUninstallMachine::TConfirmationState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Uninstall Machine - Confimation State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CUninstallMachine::TConfirmationState::Cancel()
	{
	}

//
// TPlanUninstallationState
//

CUninstallMachine::TPlanUninstallationState::TPlanUninstallationState(CUninstallMachine& aUninstallMachine)
	: CUninstallMachine::TUninstallState(aUninstallMachine)
	{
	}

void CUninstallMachine::TPlanUninstallationState::EnterL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Entering Uninstallation Planning State"));
	
	// Create result.
	iUninstallMachine.iResult=CInstallationResult::NewL();
	
	// Call uninstallation planner and plan the uninstallation
	iUninstallMachine.iPlanner=CUninstallationPlanner::NewL(
		iUninstallMachine.UiHandler(), *iUninstallMachine.iResult);

	// The following function may leave either because of an error or because 
	// the user cancelled uninstallation (a dialog), in which case it notifies 
	// the UI sothat we don't need to display anything (cancellation is not an
	// error). See RunError() for more details.
	iUninstallMachine.iPlanner->PlanUninstallationL(*iUninstallMachine.iPackage);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	iUninstallMachine.SetFinalProgressBarValue(iUninstallMachine.iPlanner->FinalProgressBarValue());
#endif	
	iUninstallMachine.iPlan=iUninstallMachine.iPlanner->TransferPlanOwnership();
	iUninstallMachine.CompleteSelf();
	iUninstallMachine.SetActive();
	}

// The next state will be interfacing with transaction support
CUninstallMachine::TState* CUninstallMachine::TPlanUninstallationState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Completed Uninstallation Planning State"));
	return static_cast<TState*>(&iUninstallMachine.iIntegritySupportState);
	}

CUninstallMachine::TState* CUninstallMachine::TPlanUninstallationState::ErrorL(TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Uninstall Machine - Uninstallation Planning State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CUninstallMachine::TPlanUninstallationState::Cancel()
	{
	}

//
// TIntegritySupportState
//

CUninstallMachine::TIntegritySupportState::TIntegritySupportState(
	CUninstallMachine& aUninstallMachine)
:   CUninstallMachine::TUninstallState(aUninstallMachine)
	{
	}

void CUninstallMachine::TIntegritySupportState::EnterL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Entering Integrity Support State"));
	
	// create a processor
	iUninstallMachine.iProcessor = CUninstallationProcessor::NewL(
		*iUninstallMachine.iPlan, iUninstallMachine.UiHandler(),
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iUninstallMachine.TransactionSession(), iUninstallMachine.RegistryWrapper(), 
#else
		iUninstallMachine.IntegrityServicesL(),
#endif
		iUninstallMachine.Observer());
	
	// execute the plan install files, update registry, all in one step
	iUninstallMachine.iProcessor->ProcessPlanL(iUninstallMachine.iStatus);
	
	//run the RBS executables , before entering the processor state.
	iUninstallMachine.iProcessor->RunBeforeShutdown();
	iUninstallMachine.SetActive();
	}

CUninstallMachine::TState* 
CUninstallMachine::TIntegritySupportState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Completed Integrity Support State"));
	return static_cast<TState*>(&iUninstallMachine.iFinalState);
	}
	
CUninstallMachine::TState* CUninstallMachine::TIntegritySupportState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Uninstall Machine - Integrity Support State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CUninstallMachine::TIntegritySupportState::Cancel()
	{
	}

//
// TFinalState
//

CUninstallMachine::TFinalState::TFinalState(
	CUninstallMachine& aUninstallMachine)
:   CUninstallMachine::TUninstallState(aUninstallMachine)
	{
	}

void CUninstallMachine::TFinalState::EnterL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Entering Final State"));
	
	TUid pubsubCategory;
	pubsubCategory.iUid = KUidSystemCategoryValue;
	TInt err = RProperty::Set(pubsubCategory, KUidSoftwareInstallKey, ESwisStatusSuccess | ESwisUninstall);
	User::LeaveIfError(err);
	
	iUninstallMachine.CompleteSelf();
	iUninstallMachine.SetActive();
	}

CUninstallMachine::TState* CUninstallMachine::TFinalState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Uninstall Machine - Completed Final State"));
	return NULL;
	}
	
CUninstallMachine::TState* CUninstallMachine::TFinalState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Uninstall Machine - Final State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CUninstallMachine::TFinalState::Cancel()
	{
	}

//
// CUninstallMachine
//

// Constructor, sets this pointer into the state objects
CUninstallMachine::CUninstallMachine(const RMessage2& aMessage)
:   CSwisStateMachine(aMessage),
	iRegistrationState(*this),
	iConfirmationState(*this),
    iPlanUninstallationState(*this),
    iIntegritySupportState(*this),
    iFinalState(*this)
	{
	}

void CUninstallMachine::ConstructL()
	{
	DEBUG_PRINTF(_L8("Constructing new Uninstall Machine"));
	
	// Call base class' ConstructL() which inits transaction service
	CSwisStateMachine::ConstructL();
	
	TInt length=Message().GetDesLengthL(0);
	HBufC8* packageBuffer=HBufC8::NewMaxLC(length);
	TPtr8 p=packageBuffer->Des();
	
	Message().ReadL(0, p); 
	RMemReadStream readStream;
	readStream.Open(p.Ptr(), length);
	CleanupClosePushL(readStream);
	
	iPackage=CSisRegistryPackage::NewL(readStream);

	CleanupStack::PopAndDestroy(2, packageBuffer);
	}

/*static*/ CUninstallMachine* CUninstallMachine::NewL(const RMessage2& aMessage)
	{
	CUninstallMachine* self=new(ELeave) CUninstallMachine(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TInt CUninstallMachine::Start()
	{
	DEBUG_PRINTF(_L8("Starting Uninstall"));
	return CSwisStateMachine::Start();
	}

// Delete objects, close connection to the servers
CUninstallMachine::~CUninstallMachine()
	{
	DEBUG_PRINTF(_L8("Destroying Uninstall Machine"));
	delete iPlan;
	delete iPlanner;
	delete iResult;
	delete iProcessor;
	delete iPackage;
	}

CUninstallMachine::TState* CUninstallMachine::FirstState()
	{
	return &iRegistrationState;
	}

// This is called from RunL() whenever a state leaves
CUninstallMachine::TState* CUninstallMachine::ErrorOnStateEntryL(TInt aError)
	{
	return CurrentState()->ErrorL(aError);
	}

void CUninstallMachine::SignalCompletedL()
	{
	HandleInstallationEventL(iPlan, EEventCompletedUnInstall);
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CUninstallMachine::PostJournalFinalizationL(TInt)
    {    
    if (!iPlan)
        {
        return;
        }
    
    DEBUG_PRINTF(_L8("Uninstall Machine - PostJournalFinalization"));
    RSisLauncherSession launcher;
        
    if (launcher.Connect() != KErrNone)
        {
        DEBUG_PRINTF(_L8("Uninstall Machine - Failed to connect to SisLauncher"));
        return;
        }
    CleanupClosePushL(launcher);
        
    //Notify apparc for the the change in the Applications
    RArray<TAppUpdateInfo> affectedApps;
    CleanupClosePushL(affectedApps);
    iPlan->GetAffectedApps(affectedApps);
    if (affectedApps.Count() > 0)
        {
        launcher.NotifyNewAppsL(affectedApps);
        }
    affectedApps.Close();
    CleanupStack::PopAndDestroy(2, &launcher);  //affectedApps
    }
#endif

