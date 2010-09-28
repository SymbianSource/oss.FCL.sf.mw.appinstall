/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "plan.h"
#include "sisregistrypackage.h"

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

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TBool CUninstallationProcessor::DoParseApplicationRegistrationFilesL()
    {
    return ETrue;   // Nothing to do
    }
#endif

TBool CUninstallationProcessor::DoStateUpdateRegistryL()
    {
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK   
    const CApplication& application = ApplicationL();
    RArray<TAppUpdateInfo> affectedApps;    
    CleanupClosePushL(affectedApps);        
    RArray<Usif::TComponentId> componentIds;
    CleanupClosePushL(componentIds);
    RArray<TUid> existingAppUids;
    CleanupClosePushL(existingAppUids);
    RArray<TUid> newAppUids;    
    CleanupClosePushL(newAppUids);
    
    TAppUpdateInfo existingAppInfo, newAppInfo;     
    TUid packageUid = application.PackageL().Uid();   
    Plan().GetAffectedApps(affectedApps);
    
    // Get all existing componentsIds for the package to to be uninstalled
    TRAPD(err,iRegistryWrapper.RegistrySession().GetComponentIdsForUidL(packageUid, componentIds));            
    TInt componentCount = componentIds.Count();
    if(0 == componentCount)
        {
        DEBUG_PRINTF(_L("ComponentIDs not found for the base package"));
        User::Leave(KErrNotFound);
        }
    
    /* 
     * Find the apps of the corrosponding component and if they exist in affected list then mark them 'EAppUninstalled' and if not present
     * append it to the affected apps with EAppUninstalled.
     */
    for(TInt i = 0 ; i < componentCount ; ++i)
        {
        existingAppUids.Reset();
        TRAP(err,iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentIds[i], existingAppUids));  
        TInt currentAppCount = existingAppUids.Count();
        for(TInt i = 0 ; i < currentAppCount; ++i)
            {
            existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppInstalled);
            TInt index(0);
            index = affectedApps.Find(existingAppInfo);
            if(KErrNotFound != index)
                {                
                affectedApps.Remove(index);
                }
            existingAppInfo = TAppUpdateInfo(existingAppUids[i], EAppUninstalled);
            affectedApps.Append(existingAppInfo);
            }
        }
        
    // Now that we are ready to make changes to the registry so we start a transaction
    // Note that the commit/rollback action is subsequently taken by the later steps of the state machine   
    iRegistryWrapper.StartMutableOperationsL();
    iRegistryWrapper.RegistrySession().DeleteEntryL(ApplicationL().PackageL(), TransactionSession().TransactionIdL()); 
    
    componentIds.Reset();
    TRAP(err,iRegistryWrapper.RegistrySession().GetComponentIdsForUidL(packageUid, componentIds));            
    TInt currentComponentCount = componentIds.Count();          
    
    // Mark the apps which are still in scr as 'EAppInstalled'(remove first, if present in affected apps) 
    for(TInt i = 0 ; i < currentComponentCount; ++i)
       {
       newAppUids.Reset();                    
       TRAP(err,iRegistryWrapper.RegistrySession().GetAppUidsForComponentL(componentIds[i], newAppUids));          
       for(TInt i = 0 ; i < newAppUids.Count(); ++i)
           {
           existingAppInfo = TAppUpdateInfo(newAppUids[i], EAppUninstalled);
           TInt index = 0;
           index = affectedApps.Find(existingAppInfo);
           if(KErrNotFound != index)
               {
               affectedApps.Remove(index);
               }                 
           existingAppInfo = TAppUpdateInfo(newAppUids[i],EAppInstalled);
           affectedApps.Append(existingAppInfo);
           }        
       }
    
    for(TInt i = 0; i < affectedApps.Count(); ++i)
        {
        DEBUG_PRINTF2(_L("AppUid is 0x%x"), affectedApps[i].iAppUid);
        DEBUG_PRINTF2(_L("Action is %d"), affectedApps[i].iAction);
        }   
            
    const_cast<CPlan&>(Plan()).ResetAffectedApps();
    const_cast<CPlan&>(Plan()).SetAffectedApps(affectedApps);
    
    CleanupStack::PopAndDestroy(4, &affectedApps);    
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



