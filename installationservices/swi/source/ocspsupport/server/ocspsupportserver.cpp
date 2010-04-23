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


/**
 @file
 @internalComponent
*/
#include <e32base.h>
#include <ocsptransport.h> 

#include "ocspsupportserver.h"
#include "ocspsupportsession.h"
#include "ocspsupport.h"
#include "log.h"

#include <ecom/ecom.h>

namespace Swi
{
// Shutdown after approx 2 seconds of inactivity.
static const TInt KServerShutdownDelay = 0x200000;

// All functions require ??? capability
const TInt COcspSupportServer::iRanges[iRangeCount] = 
                {
                0,
                COcspSupportSession::KMaxMessage, //range covers all valid messages
                };

const TUint8 COcspSupportServer::iElementsIndex[iRangeCount] = 
                {
                0,                            // All Functions in first range                              
                CPolicyServer::ENotSupported,  // Rest are unsupported
                };

const CPolicyServer::TPolicyElement COcspSupportServer::iPolicyElements[1] = 
                {
                {_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient}
                };

const CPolicyServer::TPolicy COcspSupportServer::iPolicy =
                {
                0,					//specifies all connect attempts need TCB
                iRangeCount,                                   
                iRanges,
                iElementsIndex,
                iPolicyElements,
                };



EXPORT_C COcspSupportServer* COcspSupportServer::NewL(CActive::TPriority aPriority)
	{
	COcspSupportServer* self=NewLC(aPriority);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C COcspSupportServer* COcspSupportServer::NewLC(CActive::TPriority aPriority)
	{
	COcspSupportServer* self=new(ELeave) COcspSupportServer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	// Start the server
	self->StartL(KOcspSupportServerName);

	return self;
	}

COcspSupportServer::COcspSupportServer(CActive::TPriority aPriority)
	: CPolicyServer(aPriority, iPolicy)
	{
	}

COcspSupportServer::~COcspSupportServer()
	{
	delete iTimer;
	}

void COcspSupportServer::ConstructL()
	{
	iTimer=CGenericTimer::NewL(*this);
	}

CSession2* COcspSupportServer::NewSessionL(const TVersion& aClientVersion,
												const RMessage2& /*aMessage*/) const
	{
	TVersion serverVersion(KOcspSupportServerVersionMajor, KOcspSupportServerVersionMinor, KOcspSupportServerVersionBuild);
	if (!User::QueryVersionSupported(serverVersion, aClientVersion))
		{
		User::Leave(KErrNotSupported);
		}

	// Cast is needed since NewSessionL is const member function
	return COcspSupportSession::NewL(const_cast<COcspSupportServer&>(*this));
	}

MOCSPTransport* COcspSupportServer::CreateTransportL(TUint32& aIap) const
	{
	// Currently only HTTP is supported
	return COCSPTransportDefault::NewL(aIap);
	}

void COcspSupportServer::AddSession()
	{
	// Cancel timer because we have at least one session
	iTimer->Cancel();
	++iSessionCount;
	DEBUG_PRINTF2(_L8("OCSP Support - Adding Session (%d active.)"), iSessionCount);
	}

void COcspSupportServer::DropSession()
	{
	// After the last session drops out, wait for the timer expiry and then
	// shut down
	if (--iSessionCount == 0)
		{
		DEBUG_PRINTF(_L8("OCSP Support - Starting Shutdown Timer."));
		iTimer->After(KServerShutdownDelay);
		};
	DEBUG_PRINTF2(_L8("OCSP Support - Dropping Session (%d active.)"), iSessionCount);
	}

void COcspSupportServer::HandleTimeout()
	{
	// Stop the active scheduler if there is no client for more than 2 seconds.
	// This in turn will stop the server
	CActiveScheduler::Stop();
	}

void StartServerL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	COcspSupportServer* server = NULL;

	DEBUG_PRINTF(_L8("OCSP Support - Starting Server"));

	TRAPD(err, server = COcspSupportServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	CActiveScheduler::Install(NULL);
	
	DEBUG_PRINTF(_L8("OCSP Support - Stopping Server"));
	
	delete server;
	delete sched;
	}

} // namespace Swi

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error,Swi::StartServerL()); 
	delete cleanup; 
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return(error);
	}
