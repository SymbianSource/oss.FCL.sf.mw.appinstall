/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tintegrityservicesserver.h"
#include "tintegrityservicesclientserver.h"
#include "integrityservices.h"
#include "log.h"
#include <e32debug.h>


_LIT(KJournalPath, "\\IntegrityServices\\");

using Swi::Test::CIntegrityServicesServer;
using Swi::Test::CIntegrityServicesSession;
using Swi::Test::CShutdownTimer;

static void PanicClient(const RMessagePtr2& aMessage, 
						Swi::Test::TIntegrityServicesServerPanic aPanic)
	{
	aMessage.Panic(Swi::Test::KIntegrityServicesServerName, aPanic);
	}

/////
///// CIntegrityServicesServer
/////

CIntegrityServicesServer* CIntegrityServicesServer::NewLC()
	{
	CIntegrityServicesServer* self = new(ELeave) CIntegrityServicesServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CIntegrityServicesServer::AddSession()
	{
	++iSessionCount;
	iShutdown->Cancel();
	}
	
void CIntegrityServicesServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		iShutdown->Start();
		RDebug::Print(_L("Server shutting down"));
		}
	}

CIntegrityServicesServer::~CIntegrityServicesServer()
	{
	delete iShutdown;
	RDebug::Print(_L("Server destructor"));
	}

CIntegrityServicesServer::CIntegrityServicesServer() : CServer2(EPriorityNormal, ESharableSessions)
	{
	}

void CIntegrityServicesServer::ConstructL()
	{
	StartL(Swi::Test::KIntegrityServicesServerName);
	
	iShutdown = CShutdownTimer::NewL();
	
	// Ensure that the server still exits even if the 1st client fails to
	// connect
	iShutdown->Start();
	}

CSession2* CIntegrityServicesServer::NewSessionL(const TVersion& /*aVersion*/, 
						 const RMessage2& /*aMessage*/) const
	{
	  return CIntegrityServicesSession::NewL();
	}

CIntegrityServicesSession* CIntegrityServicesSession::NewL()
	{
	CIntegrityServicesSession* self = CIntegrityServicesSession::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
	
CIntegrityServicesSession* CIntegrityServicesSession::NewLC()
	{
	CIntegrityServicesSession* self = new(ELeave) CIntegrityServicesSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CIntegrityServicesSession::CIntegrityServicesSession() : CSession2()
	{
	}
	
void CIntegrityServicesSession::ConstructL()
	{
	iIntegrityServices = CDummyIntegrityServices::NewL(0, KJournalPath);
	}
	
void CIntegrityServicesSession::CreateL()
	{
	RDebug::Print(_L("Session CreateL"));
	Server().AddSession();
	}



CIntegrityServicesSession::~CIntegrityServicesSession()
	{
	delete iIntegrityServices;
	Server().DropSession();
	}

CIntegrityServicesServer& CIntegrityServicesSession::Server()
	{
	return *static_cast<CIntegrityServicesServer*>(
		const_cast<CServer2*>(CSession2::Server()));
	}

/////
///// CIntegrityServicesSession		
/////
	
void CIntegrityServicesSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case EAdd:
			{
			RDebug::Print(_L("ServiceL EAdd"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->AddL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ERemove:
			{
			RDebug::Print(_L("ServiceL ERemove"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->RemoveL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			
			break;
			}
		case ETemporary:
			{
			RDebug::Print(_L("ServiceL ETemporary"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->TemporaryL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ECommit:
			{
			RDebug::Print(_L("ServiceL ECommit"));
			TRAPD(err, iIntegrityServices->CommitL());
			aMessage.Complete(err);
			break;
			}
		case ERollBack:
			{
			RDebug::Print(_L("ServiceL ERollBack"));
			TBool all;
			TPckg<TBool> allTransactions(all);
			aMessage.ReadL(0, allTransactions);
			
			TRAPD(err, iIntegrityServices->RollBackL(all));
			
			aMessage.Complete(err);
			
			break;
			}
			
		case ESetSimulatedFailure:
			{
			HBufC16* aFailType = HBufC16::NewLC(256);
			HBufC16* aPosition = HBufC16::NewLC(256);
			HBufC16* aFailFileName = HBufC16::NewLC(256);
			TPtr16 failType(aFailType->Des());
			TPtr16 position(aPosition->Des());
			TPtr16 failFileName(aFailFileName->Des());
			
			aMessage.ReadL(0, failType);
			aMessage.ReadL(1, position);
			aMessage.ReadL(2, failFileName);
			RDebug::Print(_L("ServiceL ESetSimulatedFailure %S %S %S"), &aFailType, &aPosition, &aFailFileName);
			TRAPD(err, iIntegrityServices->SetSimulatedFailure(failType, position, failFileName));
			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(3, aFailType);
			break;
			}
		case ECreateNewTestFile:
			{
			RDebug::Print(_L("ServiceL ECreateNewTestFile"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->CreateNewTestFileL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
		case ECreateTempTestFile:
			{
			RDebug::Print(_L("ServiceL ECreateTempTestFile"));
			HBufC16* fileName = HBufC16::NewLC(aMessage.GetDesLength(0));
		  	TPtr16 ptr(fileName->Des());
		  	aMessage.Read(0, ptr);

		  	TRAPD(err, iIntegrityServices->CreateTempTestFileL(*fileName));

			aMessage.Complete(err);
			CleanupStack::PopAndDestroy(fileName);	
			break;
			}
	default:
		{
		PanicClient(aMessage, Swi::Test::EPanicIntegrityServicesServerIllegalFunction);
		break;
		}
		}
	}

void CIntegrityServicesSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, Swi::Test::EPanicIntegrityServicesServerIllegalFunction);
		}
	CSession2::ServiceError(aMessage, aError);
	}

/////
///// CShutdownTimer
/////
inline CShutdownTimer::CShutdownTimer() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CShutdownTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CShutdownTimer::Start()
	{
	After(KShutdownDelay);
	}

CShutdownTimer* CShutdownTimer::NewL()
	{
	CShutdownTimer* self = new(ELeave) CShutdownTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Initiate server exit when the timer expires
void CShutdownTimer::RunL()
	{
	CActiveScheduler::Stop();
	}
