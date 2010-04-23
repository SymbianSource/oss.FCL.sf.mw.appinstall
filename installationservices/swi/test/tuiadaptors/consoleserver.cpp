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


#include "consoleserver.h"
#include "consoleclientserver.h"

using Swi::Test::CConsoleServer;
using Swi::Test::CConsoleSession;
using Swi::Test::CConsoleServerShutdown;

static void PanicClient(const RMessagePtr2& aMessage, 
						Swi::Test::TConsoleServerPanic aPanic)
	{
	aMessage.Panic(Swi::Test::KConsoleServerName, aPanic);
	}

/////
///// Console Server
/////

/*static*/ CConsoleServer* CConsoleServer::NewLC()
	{
	CConsoleServer* self = new(ELeave) CConsoleServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CConsoleServer::AddSession()
	{
	++iSessionCount;
	iShutdown->Cancel();
	}
	
void CConsoleServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		}
	}

CConsoleServer::~CConsoleServer()
	{
	delete iShutdown;
	delete iConsole;
	}

CConsoleServer::CConsoleServer() : CServer2(EPriorityNormal, ESharableSessions)
	{
	}

void CConsoleServer::ConstructL()
	{
	StartL(Swi::Test::KConsoleServerName);
	iConsole = Console::NewL(_L("SWI Console Server Test"), TSize(KDefaultConsWidth, KDefaultConsHeight));
	iShutdown = CConsoleServerShutdown::NewL();
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	}

CSession2* CConsoleServer::NewSessionL(const TVersion& /*aVersion*/, 
									   const RMessage2& /*aMessage*/) const
	{
	return new(ELeave) CConsoleSession(*iConsole);
	}

void CConsoleSession::CreateL()
	{
	Server().AddSession();
	}

CConsoleSession::CConsoleSession(CConsoleBase& aConsole) : CSession2(), iConsole(aConsole)
	{
	}

CConsoleSession::~CConsoleSession()
	{
	Server().DropSession();
	}

CConsoleServer& CConsoleSession::Server()
	{
	return *static_cast<CConsoleServer*>(
		const_cast<CServer2*>(CSession2::Server()));
	}

/////
///// CConsoleSession		
/////
	
void CConsoleSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case EPrint:
		{
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
		aMessage.Complete(iConsole.Getch());
		break;
		}
	default:
		{
		PanicClient(aMessage, Swi::Test::EPanicConsoleServerIllegalFunction);
		break;
		}
		}
	}

void CConsoleSession::ServiceError(const RMessage2& aMessage, TInt aError)
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
		
inline CConsoleServerShutdown::CConsoleServerShutdown() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CConsoleServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CConsoleServerShutdown::Start()
	{
	After(KShutdownDelay);
	}

/*static*/ CConsoleServerShutdown* CConsoleServerShutdown::NewL()
	{
	CConsoleServerShutdown* self = new(ELeave) CConsoleServerShutdown();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Initiate server exit when the timer expires
void CConsoleServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}
