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
* tocsp.cpp
*
*/


#include "log.h"
#include "tocspserver.h"
#include "tocspclientserver.h"

using Swi::Test::COcspServer;
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
	FLOG(_L("Console Server added session"));
	++iSessionCount;
	iShutdown->Cancel();
	}
	
void COcspServer::DropSession()
	{
	FLOG(_L("Console Server dropped session"));
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		FLOG(_L("Console Server shutting down"))
		}
	}

COcspServer::~COcspServer()
	{
	delete iShutdown;
	delete iConsole;
	FLOG(_L("Console Server destructor"))
	}

COcspServer::COcspServer() : CServer2(EPriorityNormal, ESharableSessions)
	{
	}

void COcspServer::ConstructL()
	{
	StartL(Swi::Test::KConsoleServerName);
	iConsole = Console::NewL(_L("SWI Console Server Test"), TSize(KDefaultConsWidth, KDefaultConsHeight));
	iShutdown = COcspServerShutdown::NewL();
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	}

CSession2* COcspServer::NewSessionL(const TVersion& /*aVersion*/, 
									   const RMessage2& /*aMessage*/) const
	{
	return new(ELeave) COcspSession(*iConsole);
	}

void COcspSession::CreateL()
	{
	Server().AddSession();
	}

COcspSession::COcspSession(CConsoleBase& aConsole) : CSession2(), iConsole(aConsole)
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
		/*case EPrint:
		{
		FLOG(_L("COcspSession::ServiceL(EPrint)"));
		TInt length = User::LeaveIfError(aMessage.GetDesMaxLength(0));
		HBufC* string = HBufC::NewMaxLC(length);
		TPtr stringBuffer(string->Des());
		aMessage.Read(0, stringBuffer);
		iConsole.Printf(*string); 
		aMessage.Complete(KErrNone);
		break;
		}
	case EGet:
		{
		FLOG(_L("COcspSession::ServiceL(EGet)"));
		aMessage.Complete(iConsole.Getch());
		break;
		}*/
	default:
		{
		FLOG(_L("COcspSession::ServiceL(): unknown function"));
		PanicClient(aMessage, Swi::Test::EPanicOcspServerIllegalFunction);
		break;
		}
		}
	}

void COcspSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, Swi::Test::EPanicConsoleServerBadDescriptor);
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
	FLOG(_L("COcspServerShutdown::RunL(): stopping Test Console Server"));
	CActiveScheduler::Stop();
	}
