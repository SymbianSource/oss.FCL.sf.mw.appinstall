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
* Implementation of Swi::CUissClientHandler class which is the client-side 
* part of the reverse completion mechanism used by SWI to communicate with 
* the UI
*
*/


/**
 @file
*/
#include "uissclienthandler.h"
#include "uisscommand.h"
#include "uisssession.h"
#include "../source/uiss/server/uissserver.h"
#include "sishelper.h"
#include "sisregistrypackage.h"
#include "writestream.h"
// UI Support Server Commands
#include "commands/installdialog.h"
#include "commands/grantcapabilitiesdialog.h"
#include "commands/languagedialog.h"
#include "commands/applicationsinusedialog.h"
#include "commands/drivedialog.h"
#include "commands/cannotoverwritefiledialog.h"
#include "commands/dependencybreakdialog.h"
#include "commands/deviceincompatibility.h"
#include "commands/missingdependency.h"
#include "commands/errordialog.h"
#include "commands/handlecancellableinstallevent.h"
#include "commands/handleinstallevent.h"
#include "commands/ocspresultdialog.h"
#include "commands/optionsdialog.h"
#include "commands/questiondialog.h"
#include "commands/upgradedialog.h"
#include "commands/uninstalldialog.h"
#include "commands/securitywarningdialog.h"
#include "commands/textdialog.h"
#include "log.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "cleanuputils.h"
#include <usif/sif/sifcommon.h>
const TInt KCompInfoBufferSize=4*1024;
#endif

namespace Swi
{
//
// A cancel handler
//
class InternalCancelHandler : public MCancelHandler
	{
public:
	InternalCancelHandler(CUissClientHandler& aUissClientHandler);
	void HandleCancel();
private:
	CUissClientHandler& iUissClientHandler;
	};

InternalCancelHandler::InternalCancelHandler(
	CUissClientHandler& aUissClientHandler)
:	iUissClientHandler(aUissClientHandler)
	{
	}

void InternalCancelHandler::HandleCancel()
	{
	iUissClientHandler.CancelOperation();
	}

CUissClientHandler* CUissClientHandler::NewLC(MUiHandler& aUiHandler, TBool aActiveObjectMode)
	{
	CUissClientHandler* self=new(ELeave) CUissClientHandler(aUiHandler, aActiveObjectMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUissClientHandler* CUissClientHandler::NewL(MUiHandler& aUiHandler, TBool aActiveObjectMode)
	{
	CUissClientHandler* self=NewLC(aUiHandler, aActiveObjectMode);
	CleanupStack::Pop(self);
	return self;
	}

CUissClientHandler::CUissClientHandler(MUiHandler& aUiHandler, TBool aActiveObjectMode)
        : CActive(EPriorityStandard),
          iUiHandler(aUiHandler),
		  iPtrIntoBuf(0,0),
          iActiveObjectMode(aActiveObjectMode),
		  iPtrIntoArgsStream(0,0)
		  #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		  ,iCompInfoBufPtr(0,0)
		  #endif
	{
    if (iActiveObjectMode)
		{
		CActiveScheduler::Add(this);
		}
	}

void CUissClientHandler::WaitForSisHelperShutdown()
	{	
	if(iSisHelper.Handle() > 0)
		{
		TRequestStatus reqStatus;
		iSisHelper.Logon(reqStatus);
		User::WaitForRequest(reqStatus);
		iSisHelper.Close();
		}		
	}
	
CUissClientHandler::~CUissClientHandler()
	{
	//Cancel any outstanding request
	CancelOperation();

	WaitForSisHelperShutdown();
    if (iActiveObjectMode)
		{
		CActive::Cancel(); // Make sure we are cancelled before deletion
		}

	delete iCancelHandler;
    delete iArgsStream;
	iUissSession.Close();
	delete iBuf;
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    delete iCompInfoBuffer;
	#endif
	}

/**
 * Allocates a buffer for reverse-completion commands. The buffer is going to 
 * be resized in case it is not sufficient for a dialog command.
 */
void CUissClientHandler::ConstructL()
	{
	iCancelHandler=new(ELeave) InternalCancelHandler(*this);
	AllocBufL(KBufSize);// Allocate the initial r/c buffer
	User::LeaveIfError(StartUiss()); // Start UISS
	User::LeaveIfError(iUissSession.Connect()); // Connect to UISS
	}

void CUissClientHandler::AllocBufL(TInt aSize)
	{
    HBufC8* buf=HBufC8::NewL(aSize);
    delete iBuf;
    iBuf=buf;
    }

void CUissClientHandler::HandleOverflowL()
	{
	// Reallocate the buffer to the size received in parameter 1
	TInt size=0;
	TPckg<TInt> theSize(size);
	theSize.Copy(iBuf->Left(sizeof(TInt)));
	AllocBufL(size);
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CUissClientHandler::AllocCompInfoBufL(TInt aSize)
	{
    HBufC8* buf = HBufC8::NewL(aSize);
    delete iCompInfoBuffer;
    iCompInfoBuffer = buf;
    }
#endif

///\short Creates a command handler object for the specified dialog request
CUissCmdHandler* CUissClientHandler::UissCmdHandlerFactoryL(TInt aCommand) const
	{
	switch (aCommand)
		{
		case CUissSession::KMessageApplicationsInUseDialog:
			return new(ELeave) CApplicationsInUseDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageCannotOverwriteFileDialog:
			return new(ELeave) CCannotOverwriteFileDialogCmdHandler(iUiHandler);
		
		case CUissSession::KMessageDependencyBreakDialog:
			return new(ELeave) CDependencyBreakDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageDeviceIncompatibility:
			return new(ELeave) CDeviceIncompatibilityDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageMissingDependency:
			return new(ELeave) CMissingDependencyDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageDriveDialog:
			return new(ELeave) CDriveDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageErrorDialog:
			return new(ELeave) CErrorDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageGrantCapabilitiesDialog:
			return new(ELeave) CGrantCapabilitiesDialogCmdHandler(iUiHandler);	
		case CUissSession::KMessageHandleCancellableInstallEvent:
			return new(ELeave) CHandleCancellableInstallEventCmdHandler(iUiHandler, 
				*iCancelHandler);
		case CUissSession::KMessageHandleInstallEvent:
			return new(ELeave) CHandleInstallEventCmdHandler(iUiHandler);
		case CUissSession::KMessageInstallDialog:
			return new(ELeave) CInstallDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageLanguageDialog:
			return new(ELeave) CLanguageDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageOcspResultDialog:
			return new(ELeave) COcspResultDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageOptionsDialog:
			return new(ELeave) COptionsDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageQuestionDialog:
			return new(ELeave) CQuestionDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageSecurityWarningDialog:
			return new(ELeave) CSecurityWarningDialogCmdHandler(iUiHandler);
		case CUissSession::KMessageUninstallDialog:
			return new(ELeave) CUninstallDialogCmdHandler(iUiHandler);
		
		case CUissSession::KMessageUpgradeDialog:
			return new(ELeave) CUpgradeDialogCmdHandler(iUiHandler);
		
		case CUissSession::KMessageTextDialog:
			return new(ELeave) CTextDialogCmdHandler(iUiHandler);
					
		default:
			return NULL;
		}
	}

void CUissClientHandler::InitializeArgStreamL(const CInstallPrefs& aInstallPrefs)
	{
	// Stream out install parameters. Cannot do this in UISSCLIENT because 
	// the code is in LAUNCHER which depends on UISSCLIENT.
    delete iArgsStream;
    iArgsStream = 0;
	iArgsStream = CWriteStream::NewL();
	*iArgsStream << aInstallPrefs;
    // Save ptr for args (must persist whilst server is processing)
	iPtrIntoArgsStream.Set(iArgsStream->Ptr());	
	}
	
void CUissClientHandler::InstallL(const CInstallPrefs& aInstallPrefs, const RArray<TInt>& aDeviceSupportedLanguages, TRequestStatus& aRequestStatus, RThread& aServer)
	{
    iState = KUissClientInstalling;
    iClientStatus = &aRequestStatus;
    
    // Save ptr for data returned by request (must persist whilst server is processing)
	iPtrIntoBuf.Set(iBuf->Des());

	InitializeArgStreamL(aInstallPrefs);
	iArgsStream->Stream().WriteInt32L(aDeviceSupportedLanguages.Count());
	//Streaming set of languages that device supports
	TInt noOfDeviceSupportedLanguages = aDeviceSupportedLanguages.Count();
	for(TInt i=0;i<noOfDeviceSupportedLanguages;i++)
		{
		iArgsStream->Stream().WriteInt32L(aDeviceSupportedLanguages[i]);
		}
	// Save ptr for args (must persist whilst server is processing)
	iPtrIntoArgsStream.Set(iArgsStream->Ptr());	
		
    // Issue initial request
    iUissSession.Install(iPtrIntoArgsStream, iPtrIntoBuf, iStatus);
    if (iActiveObjectMode)
		{
		SetActive();
		}

    // Update client's TRequestStatus object
    *iClientStatus = KRequestPending;
    iSisHelper = aServer;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CUissClientHandler::GetComponentInfoL(const CInstallPrefs& aInstallPrefs, Usif::CComponentInfo& aComponentInfo, TRequestStatus& aRequestStatus, RThread& aServer)
	{	
    iState = KUissClientGettingCompInfo;
    iClientStatus = &aRequestStatus;
    	
    // Store the component info reference to the class reference. So that, the same will be 
    // populated after getting the asynchronous method completed.
    iComponentInfo = &aComponentInfo;
    
	InitializeArgStreamL(aInstallPrefs);
	
	AllocCompInfoBufL(KCompInfoBufferSize);	
	
	// Save the pointer for component info collection buffer
	iCompInfoBufPtr.Set(iCompInfoBuffer->Des());

    // Issue get component info request
    iUissSession.GetComponentInfo(iPtrIntoArgsStream, iCompInfoBufPtr, iStatus);

	// There is no synchronous API for GetComponentInfo
	__ASSERT_ALWAYS(iActiveObjectMode, User::Invariant());
	SetActive();

    // Update client's TRequestStatus object
    *iClientStatus = KRequestPending;
    iSisHelper = aServer;	
	}
#endif

void CUissClientHandler::UninstallL(const CSisRegistryPackage& aPackage, TRequestStatus& aRequestStatus)
	{
    iState = KUissClientUninstalling;
    iClientStatus = &aRequestStatus;

    // Save ptr for data returned by request (must persist whilst server is processing)
    iPtrIntoBuf.Set(iBuf->Des());

    delete iArgsStream;
    iArgsStream = 0;
	iArgsStream = CWriteStream::NewL();
    *iArgsStream << aPackage;
    // Save ptr for args (must persist whilst server is processing)
	iPtrIntoArgsStream.Set(iArgsStream->Ptr());

	// Issue initial request
    iUissSession.Uninstall(iPtrIntoArgsStream, iPtrIntoBuf, iStatus);
    if (iActiveObjectMode)
		{
		SetActive();
		}

    // Update client's TRequestStatus object
    *iClientStatus = KRequestPending;
	}

void CUissClientHandler::CancelOperation()
	{
	if (iState == KUissClientIdle)
		{
		return;
		}
	
	// User called this so must have an outstanding request with us.

	// First tell the Uiss that we want to cancel the current
	// operation.
	//
	// If we have an outstanding Uiss request, this will complete (with
	// KErrCancel) when the operation has terminated.
	//
	// If we are called inside a dialog callback, then there is no
	// outstanding Uiss request at the moment. When the dialog
	// returns, we will issue a request, which will complete (with
	// KErrCancel) when the operation has terminated.
	(void)iUissSession.Cancel();
	}

void CUissClientHandler::WorkUntilCompleteL()
	{
	// Keep processing UISS responses and issuing new requests
	// until we update the client status to non-pending.
	while(iClientStatus && *iClientStatus == KRequestPending) 
		{
		User::WaitForRequest(iStatus);
		TRAPD(err,RunL());
		if(err != KErrNone)
			{	
			RunError(err);
			}
		}
	}

TBool CUissClientHandler::IsBusy()
	{
	return iState != KUissClientIdle;
	}


//
// Code necessary to run UISS in the same process but in a different thread
//
TInt CUissClientHandler::StartUiss()
	{
	const TInt KUissServerStackSize=0x2000;
	const TInt KUissServerInitHeapSize=0x1000;
	const TInt KUissServerMaxHeapSize=0x1000000;
	
	
	TThreadFunction entryPoint=UissThreadFunction;
	//TUiSupportStartParams uiSupportParams(aUiHandler);
	// The owner of the new thread will be the process, otherwise if the 
	// current thread dies, the server thread will die, too.
	RThread server;
	TInt err = KErrNone;
		
	for (TInt retry=0; retry < 2; ++retry)
		{
		err = server.Create(KUissServerName, entryPoint, 
		KUissServerStackSize, KUissServerInitHeapSize, KUissServerMaxHeapSize,
		NULL, EOwnerThread);
		
		if (err == KErrAlreadyExists)
			{
			User::After(30000);	
			}
		else
			{
			break;
			}
		}
		
	if (err==KErrAlreadyExists)
		{
		return KErrServerBusy;
		}
	if (err != KErrNone)
		{
		return err;
		}
	
	// Synchronise with the process to make sure it hasn't died straight away
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		server.Kill(0); // Abort startup
		}
	else
		{
		// logon OK - start the server
		server.Resume();
		}
	// Wait to synchronise with server - if it dies in the meantime, it
	// also gets completed
	User::WaitForRequest(stat);	
	// We can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	TInt r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// Entry point for the thread the UISS runs in
TInt CUissClientHandler::UissThreadFunction(TAny *)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CUissServer* server=NULL;
	
	TRAPD(err, server=CUissServer::NewL());
	if (err==KErrNone)
		{
		RThread::Rendezvous(KErrNone);
		scheduler->Start();
		}
	
	delete server;
	
	CActiveScheduler::Install(NULL);
	delete scheduler;
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

void CUissClientHandler::RunL()
    {
    TInt err = iStatus.Int();
	iPtrIntoBuf.Set(iBuf->Des()); // Get ptr to our return buffer

	DEBUG_PRINTF2(_L8("Sis Helper - UISS Client Handler, RunL(). Status: %d."), err);

	
    if (err==KErrOverflow 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
		&& iState != KUissClientGettingCompInfo // We don't support overflow management for component info
#endif
		)
        {
        // Grow the respective buffer buffer and re-issue "request".
        // There should now be space for the UISS server to copy in its dialogue message.
        HandleOverflowL();
        iPtrIntoBuf.Set(iBuf->Des());
		iUissSession.BufferReallocated(iPtrIntoBuf, iStatus);

		if (iActiveObjectMode)
			{
			SetActive();
			}
		return;
		}
	else
		{
		if (err>CUissSession::KMsgSeparatorMinimumSwisMessage && 
			err<CUissSession::KMsgSeparatorMaximumSwisMessage)
			{
			// this is a dialog request, unmarshal parameters and display 
			// the dialog
			CUissCmdHandler* cmdHandler=UissCmdHandlerFactoryL(err);
			if (!cmdHandler)
				{
				User::Leave(KErrNotSupported);
				}
			
			CleanupStack::PushL(cmdHandler);
			// Note that the callback might call CancelOperation which
			// would update iState...
			cmdHandler->HandleMessageL(iPtrIntoBuf, iPtrIntoBuf);
			CleanupStack::PopAndDestroy(cmdHandler);
			}
		else
			{
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Request has been completed successfully. So, now construct the 
			// component info from the buffer which is populated from the SWI server.
			if (err == KErrNone  && iState == KUissClientGettingCompInfo)
				{
				ConstructCompInfoFromBufferL();
				}			
			#endif
			// Either KErrNone or some sort of error status - in any case the processing has finished
			iState = KUissClientIdle;			
			}
		}
	
	// Re-issue request, if we are still installing/uninstalling
	switch(iState)
		{
	case KUissClientInstalling:
	case KUissClientUninstalling:
		iUissSession.CompleteDialog(KErrNone, iPtrIntoBuf, iStatus);
		if (iActiveObjectMode)
			{
			SetActive();
			}
		return;
			
	case KUissClientIdle:
		// All done, or failed...
		delete iArgsStream;
		iArgsStream = 0;
		//Wait for the death of SisHelper
		WaitForSisHelperShutdown();	
		// Complete user request (also sets iClientStatus to 0)
		ASSERT(iClientStatus);
		User::RequestComplete(iClientStatus, err);
		return;
		}
	ASSERT(false);
    }


TInt CUissClientHandler::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Sis Helper - UISS Client Handler, RunError. Error: %d."), aError);
	// Pass failure code on to our client.
	iPtrIntoBuf.Zero();
	iUissSession.CompleteDialog(aError, iPtrIntoBuf, iStatus);
    if (iActiveObjectMode)
		{
		SetActive();
		}
	return KErrNone; // Do not crash the CActiveScheduler.
	}

void CUissClientHandler::DoCancel()
    {
    DEBUG_PRINTF(_L8("Sis Helper - UISS Client Handler, Cancelling."));
    
	// Normally NEVER called because the application should have
	// waited for the original request to complete!
	
	// We can NOT simply call CancelOperation, because when we return
	// into the framework Cancel function it will block on our
	// iStatus, which would stop the active scheduler and hence stop
	// the CancelOperation from being actioned.

	// Do an emergency abort.....

	// First kill our helper threads
	
	// SIS helper thread/server
	CSisHelperServer::Abort();
	
	// UI helper thread/server
	TFullName fullName = RProcess().FullName();
	fullName.Append(':');
	fullName.Append(':');
	fullName.Append(KUissServerName);

	RThread server;
	TInt err = server.Open(fullName);
	if (err == KErrNone)
		{
		server.Terminate(KErrAbort);
		server.Close();
		}
	
	// Now complete any client request
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, err);
		}
    }
    
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CUissClientHandler::ConstructCompInfoFromBufferL()
    {    
    // create a stream based on the buffer
	RDesReadStream stream(*iCompInfoBuffer);
	CleanupClosePushL(stream);
	
	CNativeComponentInfo* nativeCompInfo = CNativeComponentInfo::NewLC();
	nativeCompInfo->InternalizeL(stream);
		
	// UISS and SWI cannot use Usif::CComponentInfo directly, as it is implemented in a non-TCB DLL. For this reason, a private structure maintained (CNativeComponentInfo),
	// which is returned by SWI and is converted here to the CComponentInfo according to the USIF interface
	Usif::CComponentInfo::CNode* rootNode = MapToComponentInfoL(*nativeCompInfo);
	iComponentInfo->SetRootNodeL(rootNode);

	CleanupStack::PopAndDestroy(nativeCompInfo);
	CleanupStack::PopAndDestroy(&stream);
    }

 void CUissClientHandler::MapToApplicationInfoL(RCPointerArray<Swi::CNativeComponentInfo::CNativeApplicationInfo>& aNativeApplicationsInfo, RPointerArray<Usif::CComponentInfo::CApplicationInfo>& aApplicationsInfo)
    {        
        TInt count = aNativeApplicationsInfo.Count();
        for (TInt i = 0; i < count; ++i)
            {
            Usif::CComponentInfo::CApplicationInfo* app ;
            app =  Usif::CComponentInfo::CApplicationInfo::NewLC(aNativeApplicationsInfo[i]->AppUid(), aNativeApplicationsInfo[i]->Name(), aNativeApplicationsInfo[i]->GroupName(), aNativeApplicationsInfo[i]->IconFileName());                       
            DEBUG_PRINTF2(_L("App Uid 0x%08x"),app->AppUid());
            DEBUG_PRINTF2(_L("File Name %S"),&app->Name());
            DEBUG_PRINTF2(_L("Group Name %S"),&app->GroupName());
            DEBUG_PRINTF2(_L("Icon File Name %S"),&app->IconFileName());            
            aApplicationsInfo.AppendL(app);
            CleanupStack::Pop(app);
            }        
    }

Usif::CComponentInfo::CNode* CUissClientHandler::MapToComponentInfoL(CNativeComponentInfo& aNativeComponentInfo)
	{
	// Create the array to store the children nodes.
	RPointerArray<Usif::CComponentInfo::CNode> children;
	CleanupResetAndDestroyPushL(children);
	
	// If there is any child for the current node, call this method with that child object.
	// Continue this (recursively) until we get the leaf node in the embedded tree (with no children)
	// and add the resultant node as one of the children and pass it to create the parent node further.
	TInt count = aNativeComponentInfo.iChildren.Count();
	for (TInt i = 0; i < count; ++i)
		{
		Usif::CComponentInfo::CNode* node = MapToComponentInfoL(*aNativeComponentInfo.iChildren[i]);
		CleanupStack::PushL(node);
		children.AppendL(node);
		CleanupStack::Pop(node);
		}		
	
	RPointerArray<Usif::CComponentInfo::CApplicationInfo> applicationInfo;	
	CleanupResetAndDestroyPushL(applicationInfo);
	MapToApplicationInfoL(aNativeComponentInfo.iApplications, applicationInfo);	
	// Create the CNode object using the appropriate parameters.
	// children for leaf nodes (bottom most nodes in the embedded tree) will be null.
	Usif::CComponentInfo::CNode* node = Usif::CComponentInfo::CNode::NewLC(
										Usif::KSoftwareTypeNative(),
										*(aNativeComponentInfo.iComponentName), 
										*(aNativeComponentInfo.iVersion),
										*(aNativeComponentInfo.iVendor),
										static_cast<Usif::TScomoState>(aNativeComponentInfo.iScomoState),
										static_cast<Usif::TInstallStatus>(aNativeComponentInfo.iInstallStatus),
										aNativeComponentInfo.iComponentId,
										*(aNativeComponentInfo.iGlobalComponentId),
										static_cast<Usif::TAuthenticity>(aNativeComponentInfo.iAuthenticity),
										aNativeComponentInfo.iUserGrantableCaps,
										aNativeComponentInfo.iMaxInstalledSize,
										aNativeComponentInfo.iHasExe,
										aNativeComponentInfo.iIsDriveSelectionRequired,
										&applicationInfo,
										&children);
	CleanupStack::Pop(node);	
	CleanupStack::Pop(&applicationInfo);
	applicationInfo.Close();
	CleanupStack::Pop(&children);
	children.Close();
	return (node);
	}
#endif
} // namespace Swi
