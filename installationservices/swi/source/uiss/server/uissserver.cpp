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
* UISS class implementation.
*
*/


/**
 @file
*/

#include <e32base.h>

#include "uiss.h"
#include "log.h"

#include "uissserver.h"
#include "uisssession.h"

namespace Swi
{
// All functions require TCB capability
const TInt CUissServer::iRanges[iRangeCount] = 
                {
                0, //Used by Client 
                CUissSession::KMsgSeparatorMinimumSwisMessage, // Used by SWIS
                };

const TUint8 CUissServer::iElementsIndex[iRangeCount] = 
                {
                0,  // Used by Client, so TrustedUI needed.                           
                1,  // Used by SWIS, so TCB needed.
                };

const CPolicyServer::TPolicyElement CUissServer::iPolicyElements[2] = 
                {
                {_INIT_SECURITY_POLICY_C1(ECapabilityTrustedUI), CPolicyServer::EFailClient},
                {_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient},
                };

const CPolicyServer::TPolicy CUissServer::iPolicy =
                {
                0,					//specifies all connect attempts need TCB
                iRangeCount,                                   
                iRanges,
                iElementsIndex,
                iPolicyElements,
                };


EXPORT_C CUissServer* CUissServer::NewL(CActive::TPriority aPriority)
	{
	CUissServer* self=NewLC(aPriority);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CUissServer* CUissServer::NewLC(CActive::TPriority aPriority)
	{
	DEBUG_PRINTF(_L8("UISS - Starting Server"));
	
	CUissServer* self=new(ELeave) CUissServer(aPriority);
	CleanupStack::PushL(self);

	// Start the server
	self->StartL(KUissServerName);

	// ConstructL() connects to SWIS
	self->ConstructL();
	return self;
	}

CUissServer::CUissServer(CActive::TPriority aPriority): 
	CPolicyServer(aPriority, iPolicy)
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	,iCompInfoBufPtr(0,0)
	#endif
	{
	}

void CUissServer::ConstructL()
	{
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iCompInfoBuffer = NULL;
	#endif
	}
	
CUissServer::~CUissServer()
	{
	ClearInstallRequest();
	ClearDialogRequest();

	if (iInstallWatcher)
		{
		iInstallWatcher->Cancel();
		delete iInstallWatcher;
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	delete iCompInfoBuffer;
	#endif
	}

void CUissServer::SignalInstallComplete(TInt aResult)
	{
	DEBUG_PRINTF2(_L8("UISS - Install Completed with code %d."), aResult);
	
	if (iInstallRequest)
		{
		iInstallRequest->Complete(aResult);
		ClearInstallRequest();
		}
	CActiveScheduler::Stop(); // close the server
	}

void CUissServer::CreateInstallWatcherL()
	{
	iInstallWatcher=CServerInstallWatcher::NewL(*this);
	}

CSession2* CUissServer::NewSessionL(const TVersion& aClientVersion,
	const RMessage2& /*aMessage*/) const
	{
	TVersion serverVersion(KUissServerVersionMajor, KUissServerVersionMinor, 
		KUissServerVersionBuild);
	if (!User::QueryVersionSupported(serverVersion, aClientVersion))
		{
		User::Leave(KErrNotSupported);
		}

	CUissSession* session=CUissSession::NewL(*const_cast<CUissServer*>(this));
	return session;
	}

void CUissServer::HandleDisconnect(const CUissSession* aSession)
	{
	if (iInstallRequestSession == NULL)
		{
		// If a session has disconnected before any install request, terminate the
		// server ASAP by stopping the active scheduler.
		CActiveScheduler::Stop();
		}
	else if (aSession == iInstallRequestSession)
		{
		// If the launcher session dropped, complete any outstanding dialog
		// request, cancel SWIS and clear both requests.
		if (iDialogRequest)
			{
			iDialogRequest->Complete(KErrSessionClosed);
			ClearDialogRequest();
			}
		if (iInstallWatcher)
			{
			iInstallWatcher->CancelSwis();
			}
		ClearInstallRequest();
		}
	else if (aSession == iDialogRequestSession)
		{
		// If the session from the install server dropped, only clear the dialog
		// request.  The install watcher's outstanding request should be completed
		// by the kernel if the install server has died, which will lead to
		// shutting down this server and completing any outstanding install
		// request.
		ClearDialogRequest();	
		}
	}
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CUissServer::SendBackComponentInfoL()
	{
	iInstallRequest->WriteL(0, *iCompInfoBuffer);
	}
#endif

} // namespace Swi


