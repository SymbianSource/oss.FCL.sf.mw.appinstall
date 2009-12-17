/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Application Processor.
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#include <hash.h>
#include "uninstallationprocessor.h"

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "integrityservices.h"
#endif

#include "sishelperclient.h"
#include "sisregistryfiledescription.h"
#include "sisstring.h"
#include "hashcontainer.h"
#include "siscontroller.h"
#include "application.h"
#include "log.h"
#include "secutils.h"
#include "sisuihandler.h"
#include "filesisdataprovider.h"
#include "securitymanager.h"
#include "sislauncherclient.h"
#include "sisinfo.h"
#include "sisuid.h"


using namespace Swi;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CUninstallationProcessor* CUninstallationProcessor::NewL(const CPlan& aPlan, 
	RUiHandler& aUiHandler, Usif::RStsSession& aStsSession, 
	CRegistryWrapper& aRegistryWrapper, RSwiObserverSession& aObserver)
	{
	CUninstallationProcessor* self = CUninstallationProcessor::NewLC(aPlan, aUiHandler, aStsSession, 
			aRegistryWrapper, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
#else
CUninstallationProcessor* CUninstallationProcessor::NewL(const CPlan& aPlan, 
	RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver)
	{
	CUninstallationProcessor* self = CUninstallationProcessor::NewLC(aPlan, aUiHandler, aIntegrityServices, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
#endif

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CUninstallationProcessor* CUninstallationProcessor::NewLC(const CPlan& aPlan, 
	RUiHandler& aUiHandler, Usif::RStsSession& aStsSession, 
	CRegistryWrapper& aRegistryWrapper, RSwiObserverSession& aObserver)
	{
	CUninstallationProcessor* self = new(ELeave) CUninstallationProcessor(aPlan, 
		aUiHandler, aStsSession, aRegistryWrapper, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
#else
CUninstallationProcessor* CUninstallationProcessor::NewLC(const CPlan& aPlan, 
	RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver)
	{
	CUninstallationProcessor* self = new(ELeave) CUninstallationProcessor(aPlan, 
		aUiHandler, aIntegrityServices, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
#endif

CUninstallationProcessor* CUninstallationProcessor::NewL(CUninstallationProcessor& aProcessor)
	{
	return CUninstallationProcessor::NewL(aProcessor.Plan(), aProcessor.UiHandler(), 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			aProcessor.TransactionSession(), aProcessor.iRegistryWrapper,
#else
			aProcessor.IntegrityServices(), 
#endif
			aProcessor.Observer());
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CUninstallationProcessor::CUninstallationProcessor(const CPlan& aPlan, 
	RUiHandler& aUiHandler, Usif::RStsSession& aStsSession, CRegistryWrapper& aRegistryWrapper,
	RSwiObserverSession& aObserver)
	: CProcessor(aPlan, aUiHandler, aStsSession, aRegistryWrapper, aObserver)
	{
	}
#else
CUninstallationProcessor::CUninstallationProcessor(const CPlan& aPlan, 
	RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices, RSwiObserverSession& aObserver)
	: CProcessor(aPlan, aUiHandler, aIntegrityServices, aObserver)
	{
	}
#endif

CUninstallationProcessor::~CUninstallationProcessor()
	{
	Cancel();
	
	delete iEmbeddedProcessor;
	Reset(); // reset filename arrays
	}

void CUninstallationProcessor::ConstructL()
	{
	CProcessor::ConstructL();
	}

void CUninstallationProcessor::Reset()
	{
	}


void CUninstallationProcessor::DoCancel()
	{
	CProcessor::DoCancel();
	if (iEmbeddedProcessor && iEmbeddedProcessor->IsActive())
		{
		iEmbeddedProcessor->Cancel();
		}
	}
	
// State processing functions

TBool CUninstallationProcessor::DoStateInitializeL()
	{
	iUiState = EInitialize;
	iCurrent = 0;
	return ETrue;
	}
	
TBool CUninstallationProcessor::DoStateProcessEmbeddedL()
	{
	if (iCurrent < ApplicationL().EmbeddedApplications().Count())
		{
		EmbeddedProcessorL().ProcessApplicationL(*ApplicationL().EmbeddedApplications()[iCurrent++], iStatus);
		WaitState(ECurrentState);
		return EFalse;
		}
	else
		{
		iCurrent=0;
		return ETrue;
		}
	}

TBool CUninstallationProcessor::DoStateExtractFilesL()
	{
	return ETrue;	// Nothing to do
	}

TBool CUninstallationProcessor::DoStateVerifyPathsL()
	{
	return ETrue;	// Nothing to do
	}
	
TBool CUninstallationProcessor::DoStateInstallFilesL()
	{
	return ETrue;	// Nothing to do
	}

TBool CUninstallationProcessor::DoStateDisplayFilesL()
	{
	return ETrue;	// Nothing to do
	}

TBool CUninstallationProcessor::DoStateProcessSkipFilesL()
	{
	return ETrue;	// Nothing to do
	}

TBool CUninstallationProcessor::DoStateUpdateRegistryL()
	{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Now that we are ready to make changes to the registry so we start a transaction
	// Note that the commit/rollback action is subsequently taken by the later steps of the state machine
	iRegistryWrapper.StartMutableOperationsL();
	iRegistryWrapper.RegistrySession().DeleteEntryL(ApplicationL().PackageL(), TransactionSession().TransactionIdL());
#else
	RSisRegistryWritableSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	session.DeleteEntryL(ApplicationL().PackageL(), IntegrityServices().TransactionId());
	CleanupStack::PopAndDestroy(&session);
#endif
	return ETrue;
	}

CUninstallationProcessor& CUninstallationProcessor::EmbeddedProcessorL()
	{
	if (!iEmbeddedProcessor)
		{
		iEmbeddedProcessor=CUninstallationProcessor::NewL(*this);
		}
	return *iEmbeddedProcessor;
	}

