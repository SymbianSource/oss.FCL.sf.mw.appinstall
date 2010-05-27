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


#include "tocspserver.h"
#include "tocspclientserver.h"

using Swi::Test::COcspServer;
using Swi::Test::COcspStateMachine;
using Swi::Test::COcspSession;
using Swi::Test::COcspServerShutdown;

static void PanicClient(const RMessagePtr2& aMessage, 
						Swi::Test::TOcspServerPanic aPanic)
	{
	aMessage.Panic(Swi::Test::KOcspServerName, aPanic);
	}

/////
///// TOCSP Server
/////

/*static*/ COcspServer* COcspServer::NewLC()
	{
	COcspServer* self = new(ELeave) COcspServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void COcspServer::AddSession()
	{
	++iSessionCount;
	iShutdown->Cancel();
	}
	
void COcspServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		}
	}

COcspServer::~COcspServer()
	{
	delete iShutdown;
	}

COcspServer::COcspServer() : CServer2(EPriorityNormal, ESharableSessions)
	{
	}

void COcspServer::ConstructL()
	{
	StartL(Swi::Test::KOcspServerName);
//	iConsole = Console::NewL(_L("SWI Console Server Test"), TSize(KDefaultConsWidth, KDefaultConsHeight));
	iShutdown = COcspServerShutdown::NewL();
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	}

CSession2* COcspServer::NewSessionL(const TVersion& /*aVersion*/, 
									   const RMessage2& /*aMessage*/) const
	{
	return new(ELeave) COcspSession();
	}

void COcspSession::CreateL()
	{
	Server().AddSession();
	}

COcspSession::COcspSession(/*CConsoleBase& aConsole*/) : CSession2() //, iConsole(aConsole)
	{
	}

COcspSession::~COcspSession()
	{
	Server().DropSession();
	}

COcspServer& COcspSession::Server()
	{
	return *static_cast<COcspServer*>(
		const_cast<CServer2*>(CSession2::Server()));
	}

/////
///// COcspSession		
/////
	
void COcspSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case ECheck:
			{
			if (iStateMachine)
				{
				delete iStateMachine;
				}
			iStateMachine = COcspStateMachine::NewL(aMessage);
			iStateMachine->DoCheck();
			}
			break;
			
		default:
			{
			PanicClient(aMessage, Swi::Test::EPanicOcspServerIllegalFunction);
			break;
			}
		}
	}

void COcspSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, Swi::Test::EPanicOcspServerBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}

/////
///// CInstallServerShutdown
/////
		
inline COcspServerShutdown::COcspServerShutdown() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void COcspServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void COcspServerShutdown::Start()
	{
	After(KShutdownDelay);
	}

/*static*/ COcspServerShutdown* COcspServerShutdown::NewL()
	{
	COcspServerShutdown* self = new(ELeave) COcspServerShutdown();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Initiate server exit when the timer expires
void COcspServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}



////
//// State Machine
////

/*static*/ COcspStateMachine* COcspStateMachine::NewL(const RMessage2& aMessage)
	{
	COcspStateMachine* self = new (ELeave) COcspStateMachine(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COcspStateMachine::COcspStateMachine(const RMessage2& aMessage) : CActive(EPriorityNormal), iClientMessage(aMessage)
	{
	CActiveScheduler::Add(this);
	}

COcspStateMachine::~COcspStateMachine()
	{
	if (iSecurityManager)
		{
		delete iSecurityManager;
		}

	iOcspOutcomes.ResetAndDestroy();
	iCerts.ResetAndDestroy();

	if (iOcspUri) 
		{
		delete iOcspUri;
		}
	}
	
void COcspStateMachine::ConstructL()
	{	
	}

void COcspStateMachine::DoCheck()
	{
	iSecurityManager = CSecurityManager::NewL();

	iState = EDone;	
	iIap = 0;
	iSecurityManager->PerformOcspL(*iOcspUri, 
								   iIap,
							   	   iRevocationMessage, 
							   	   iOcspOutcomes, 
							   	   iCerts,
							   	   iStatus);
	SetActive();
	}

	
void COcspStateMachine::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int()); // Hop into RunError()
		}

	switch (iState)
		{
		case EDone:
			{
			iClientMessage.Complete(static_cast<TInt>(*iRevocationMessage));
			}
			break;
			
		default:
			{
			User::Panic(_L("Test OCSP Server"), 1);
			}
		}
	}
	
void COcspStateMachine::DoCancel()
	{
	
	}
	
TInt COcspStateMachine::RunError(TInt aError)
	{
	iClientMessage.Complete(aError);
	
	return KErrNone;
	}

