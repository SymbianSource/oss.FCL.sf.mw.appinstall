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


#include "uisssession.h"
#include "uissserver.h"
#include "installclientserver.h"
#include "log.h"
#include "swi/launcher.h"

#include <s32mem.h>

#include "sisregistrypackage.h"
#include "writestream.h"
#include "sisinstallerrors.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swi/nativecomponentinfo.h"
#endif

namespace Swi
{

CUissSession* CUissSession::NewL(CUissServer& aServer)
	{
	CUissSession* self=NewLC(aServer);
	CleanupStack::Pop(self);
	return self;
	}

CUissSession* CUissSession::NewLC(CUissServer& aServer)
	{
	CUissSession* self=new (ELeave) CUissSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUissSession::CUissSession(CUissServer& aServer)
	: iUissServer(aServer)
	{
	}

void CUissSession::ConstructL()
	{		
	}


CUissSession::~CUissSession()
	{
	}

void CUissSession::Disconnect(const RMessage2& aMessage)
	{
	iUissServer.HandleDisconnect(this);
	CSession2::Disconnect(aMessage);
	}

void CUissSession::ServiceL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("UISS - Servicing Message Type %d."), aMessage.Function());

	TInt func=aMessage.Function();
	if (func==KMessageInstall || func==KMessageUninstall
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	 		|| func==KMessageGetComponentInfo
	 	#endif
	 	)
		{
		// Check if already busy.
		if (iUissServer.InstallRequest() || iUissServer.InstallWatcher())
			{
			// Already have a main install, say we're busy
			aMessage.Complete(KErrServerBusy);
			return;
			}

		// Starting a new installation/uninstallation operation

		// Create a watcher and launch SWIS
		iUissServer.CreateInstallWatcherL();

		// Setup temporary stream for internalisation of arguments from parameter 1
		TInt length=aMessage.GetDesLengthL(1);
		HBufC8* buf=HBufC8::NewMaxLC(length);
		TPtr8 p=buf->Des();
		aMessage.ReadL(1, p);
		RMemReadStream readStream;
		readStream.Open(p.Ptr(),length);
		CleanupClosePushL(readStream);

		// Call into SWIS to start the Install or uninstall
		if (func==KMessageInstall)
			{
			// Internalise install preferences from readStream/param 1
			CInstallPrefs* prefs=CInstallPrefs::NewLC(readStream);
			RArray<TInt> deviceSupportedLanguages;
			TInt deviceLanguageCount = readStream.ReadInt32L();
			//Streaming set of languages that device supports
			for(TInt i=0;i<deviceLanguageCount;i++)
				{
				deviceSupportedLanguages.AppendL(readStream.ReadInt32L());
				}
			// Start installation with parameters.
			iUissServer.InstallWatcher()->StartInstallL(*prefs, deviceSupportedLanguages);
			CleanupStack::PopAndDestroy(prefs); // prefs
			deviceSupportedLanguages.Close();
			}
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// Call SWIS to get the component info.
		else if (func == KMessageGetComponentInfo)
			{
			// Internalise install preferences from readStream/param 1
			CInstallPrefs* prefs=CInstallPrefs::NewLC(readStream);
			
			delete iUissServer.iCompInfoBuffer;
			iUissServer.iCompInfoBuffer = NULL;
			iUissServer.iCompInfoBuffer = HBufC8::NewL(aMessage.GetDesMaxLength(0));
			iUissServer.iCompInfoBufPtr.Set(iUissServer.iCompInfoBuffer->Des());
			
			// Pass on the request and the buffer allocated in the uissclienthandler to the next level.			
			iUissServer.InstallWatcher()->StartGetComponentInfoL(*prefs, iUissServer.iCompInfoBufPtr);
			CleanupStack::PopAndDestroy(prefs); 			
			}
		#endif
		else if (func==KMessageUninstall)
			{
			// Internalise uninstall package from readStream/param 1
			CSisRegistryPackage* package=CSisRegistryPackage::NewLC(readStream);
			iUissServer.InstallWatcher()->StartUninstall(*package);
			CleanupStack::PopAndDestroy(package);
			}

		iUissServer.SetInstallRequestL(&aMessage, this);

		CleanupStack::PopAndDestroy(2,buf); // readStream, buf
		}
	else if (func==KMessageCompleteDialog)
		{
		if (iUissServer.InstallRequest())
			{
			// Already have an outstanding request, this should not happen
			aMessage.Complete(KErrServerBusy);
			return;
			}

		// We should have a dialog request outstanding.
		if (!iUissServer.DialogRequest())
			{
			// Internal error
			User::Leave(KErrInstallerLeave);
			}

		// This request completes the dialog request with the data
		// contained in the message.

		TInt err = aMessage.Int3(); // Read Leave Code from dialog handler
		if(err > 0)
			{
			// Positive Leave codes are not propogated because some code only considers <0 to be an error
			err = KErrGeneral;
			}
		
		if(err == KErrNone)
			{
			// Dialog handler did not leave so attempt to copy the response.
			// We use parameter 1 of the message for result reporting
			TRAP(err, TransferDescriptorL(aMessage, 0, *iUissServer.DialogRequest(), 1));
			}

		iUissServer.DialogRequest()->Complete(err == KErrNone ? iUissServer.DialogRequest()->Function() : err);
		iUissServer.ClearDialogRequest();

		iUissServer.SetInstallRequestL(&aMessage, this);

		if (iUissServer.DelayedCancel())
			{
			iUissServer.InstallWatcher()->CancelSwis();
			iUissServer.SetDelayedCancel(EFalse);
			}
		}
	else if (func==KMessageBufferReallocated)
		{
		if (iUissServer.InstallRequest())
			{
			// Already have an outstanding request, this should not happen
			aMessage.Complete(KErrServerBusy);
			return;
			}
		
		// We should have a dialog request outstanding.
		if (!iUissServer.DialogRequest())
			{
			// Internal error
			User::Leave(KErrInstallerLeave);
			}

		
		// The client has re-issued its request because it was not big
		// enough to copy the dialog information into. We should have
		// explicitly provided the size required.
		TRAPD(err, TransferDescriptorL(*iUissServer.DialogRequest(), 0, 
									   aMessage, 0));

		// If it was still not big enough something is very wrong.  We can't
		// complete the request with KErrOverflow again, since that will prompt
		// the client to repeat the request again.
		if (err == KErrOverflow)
			{
			User::Leave(KErrInstallerLeave);
			}
		else 
			{
			User::LeaveIfError(err);
			}

		aMessage.Complete(iUissServer.DialogRequest()->Function());
		return;
		}
	else if (func==KMessageCancel)
		{
		// Ignore if an operation is not in progress
		if (!iUissServer.InstallWatcher())
			{
			// Complete the request to cancel
			aMessage.Complete(KErrNone);
			return;
			}

		// If the cancel is issued from inside a dialog/event
		// callback, then we must delay the cancel until the dialog
		// returns and issues another KMessageCompleteDialog request
		// to us. An outstanding client KMessageCompleteDialog request
		// must be outsanding when the operation terminates, so we can
		// use it to return KErrCancel to the client.
		if(!iUissServer.InstallRequest())
			{
			// No outstanding client request, so delay cancel.
			iUissServer.SetDelayedCancel();
			}
		else
			{
			// We already have a client request, so can just cancel now.
			iUissServer.InstallWatcher()->CancelSwis();
			}

		// Complete the request to cancel
		aMessage.Complete(KErrNone);
		}
	else if (func < KMsgSeparatorMaximumSwisMessage &&
			 func > KMsgSeparatorMinimumSwisMessage)
		{
		// message from swis, try and pass it onto the launcher
		const RMessage2* installReq = iUissServer.InstallRequest();

		if (!installReq)
			{
			User::Leave(KErrInstallerLeave);
			}

		TRAPD(err, TransferDescriptorL(aMessage, 0, 
									   *installReq, 0));
		if(err == KErrOverflow)
			{
			TInt size = aMessage.GetDesLengthL(0);
			TPckg<TInt> theSize(size);
			installReq->WriteL(0, theSize);
			installReq->Complete(KErrOverflow);
			iUissServer.ClearInstallRequest();
			}
		else
			{
			iUissServer.InstallRequest()->Complete(err == KErrNone ? aMessage.Function() : err);
			iUissServer.ClearInstallRequest();
			User::LeaveIfError(err);
			}
		iUissServer.SetDialogRequestL(&aMessage, this);
		}
	}

void CUissSession::TransferDescriptorL(
	const RMessage2& aMessageFrom, TInt aParameterFrom,
	const RMessage2& aMessageTo, TInt aParameterTo)
	{
	TInt lengthFrom=aMessageFrom.GetDesLengthL(aParameterFrom);
	
	// copy data only if there is anything to copy
	if (lengthFrom)
		{
		HBufC8* buffer=HBufC8::NewLC(lengthFrom);
		TPtr8 bufferPtr(buffer->Des());
		aMessageFrom.ReadL(aParameterFrom, bufferPtr);
		aMessageTo.WriteL(aParameterTo, bufferPtr);
		CleanupStack::PopAndDestroy(buffer);
		}
	}


// CUissServer::CServerInstallWatcher 

/*static*/ CServerInstallWatcher* CServerInstallWatcher::NewL(CUissServer& aServer)
	{
	CServerInstallWatcher* self=NewLC(aServer);
	CleanupStack::Pop(self);
	return self;
	}
/*static*/ CServerInstallWatcher* CServerInstallWatcher::NewLC(CUissServer& aServer)
	{
	CServerInstallWatcher* self=new (ELeave) CServerInstallWatcher(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CServerInstallWatcher::CServerInstallWatcher(CUissServer& aServer)
	: CActive(CActive::EPriorityStandard), iUissServer(aServer)
	
	{
	CActiveScheduler::Add(this);
	}

CServerInstallWatcher::~CServerInstallWatcher()
	{
	Deque();
	iSwisSession.Close();
	delete iWriteStream;	
	}
	
void CServerInstallWatcher::ConstructL()
	{
	User::LeaveIfError(iSwisSession.Connect());
	}

void CServerInstallWatcher::SerializePrefsL(const CInstallPrefs& aInstallPrefs)
	{
	delete iWriteStream;	
	iWriteStream=NULL;
	iWriteStream=CWriteStream::NewL();
	*iWriteStream << aInstallPrefs; // This can leave.	
	iPtr.Set(iWriteStream->Ptr());		
	}
	
void CServerInstallWatcher::StartInstallL(
	const CInstallPrefs& aInstallPrefs, const RArray<TInt>& aDeviceSupportedLanguages)
	{

	SerializePrefsL(aInstallPrefs);

	iWriteStream->Stream().WriteInt32L(aDeviceSupportedLanguages.Count());
	for(TInt i=0;i<aDeviceSupportedLanguages.Count();i++)
		{
		iWriteStream->Stream().WriteInt32L(aDeviceSupportedLanguages[i]);
		}
	iPtr.Set(iWriteStream->Ptr());
	
	TIpcArgs args(&iPtr);
	iSwisSession.InstallL(args,iStatus);
	SetActive();
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CServerInstallWatcher::StartGetComponentInfoL(
							const CInstallPrefs& aInstallPrefs, 
							TDes8& aCompInfoBuffer)
	{
	iIsComponentInfoRequest = ETrue;

	SerializePrefsL(aInstallPrefs);
	
	TIpcArgs args(&iPtr, &aCompInfoBuffer);
	iSwisSession.GetComponentInfoL(args, iStatus);
	SetActive();
	}
#endif

void CServerInstallWatcher::StartUninstall(CSisRegistryPackage& aPackage)
	{	
	SetActive();
	
	delete iWriteStream;
	iWriteStream=0;
	iWriteStream=CWriteStream::NewL();
	*iWriteStream << aPackage;
	
	iPtr.Set(iWriteStream->Ptr());
	TIpcArgs args(&iPtr);
	
	iSwisSession.Uninstall(args, iStatus);
	}


void CServerInstallWatcher::DoCancel()
	{
	}
	
void CServerInstallWatcher::CancelSwis()
	{
	iSwisSession.Cancel();
	}

void CServerInstallWatcher::RunL()
	{
	// If the session runs for component information collection,
	// transfer the details collected from the install server to sis helper.
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	if (iIsComponentInfoRequest)
		{
		iUissServer.SendBackComponentInfoL();
		}
	#endif			
	iUissServer.SignalInstallComplete(iStatus.Int());
	}

} // namespace Swi
