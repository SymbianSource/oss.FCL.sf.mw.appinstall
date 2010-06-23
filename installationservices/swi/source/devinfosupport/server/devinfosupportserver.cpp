/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Device information server
* @internalComponent
* @released
*
*/


#include <e32base.h>

#include "devinfosupportserver.h"
#include "devinfosupportsession.h"
#include "devinfosupportclient.h"
#include "deviceidlist.h"
#include "log.h"


namespace Swi
{
// Shutdown after approx 2 seconds of inactivity.
static const TInt KServerShutdownDelay = 2000000;

// All functions require TCB capability
const TInt CDeviceInfoServer::iRanges[iRangeCount] = 
                {
                0,
                CDeviceInfoSession::KMaxMessage, //range covers all valid messages
                };

const TUint8 CDeviceInfoServer::iElementsIndex[iRangeCount] = 
                {
                0,                             // All Functions in first range                              
                CPolicyServer::ENotSupported,  // Rest are unsupported
                };

const CPolicyServer::TPolicyElement CDeviceInfoServer::iPolicyElements[1] = 
                {
                {_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient}
                };

const CPolicyServer::TPolicy CDeviceInfoServer::iPolicy =
                {
                0,					//specifies all connect attempts need TCB
                iRangeCount,                                   
                iRanges,
                iElementsIndex,
                iPolicyElements,
                };



EXPORT_C CDeviceInfoServer* CDeviceInfoServer::NewL(CActive::TPriority aPriority)
	{
	CDeviceInfoServer* self=NewLC(aPriority);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDeviceInfoServer* CDeviceInfoServer::NewLC(CActive::TPriority aPriority)
	{
	CDeviceInfoServer* self=new(ELeave) CDeviceInfoServer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	// Start the server
	self->StartL(KDeviceInfoServerName);

	return self;
	}

CDeviceInfoServer::CDeviceInfoServer(CActive::TPriority aPriority)
	: CPolicyServer(aPriority, iPolicy)
	{
	}

CDeviceInfoServer::~CDeviceInfoServer()
	{
	delete iTimer;
	delete iDeviceIdList;
	delete iPackedDeviceIdList;
	delete iDeviceInfoFetcher;
	iOutstandingDeviceIdRequests.Close();
	}

void CDeviceInfoServer::ConstructL()
	{
	iDeviceInfoFetcher = CDeviceInfoFetcher::NewL(*this);
	iTimer=CGenericTimer::NewL(*this);
	}

CSession2* CDeviceInfoServer::NewSessionL(const TVersion& aClientVersion,
												const RMessage2& /*aMessage*/) const
	{
	TVersion serverVersion(KDeviceInfoServerVersionMajor, KDeviceInfoServerVersionMinor, KDeviceInfoServerVersionBuild);
	if (!User::QueryVersionSupported(serverVersion, aClientVersion))
		{
		User::Leave(KErrNotSupported);
		}

	// Cast is needed since NewSessionL is const member function
	return CDeviceInfoSession::NewL(const_cast<CDeviceInfoServer&>(*this));
	}

void CDeviceInfoServer::AddSession()
	{
	// Cancel timer because we have at least one session
	iTimer->Cancel();
	++iSessionCount;
	DEBUG_PRINTF2(_L8("Device Info Server - Adding Session (%d sessions active.)"), iSessionCount);
	}

void CDeviceInfoServer::DropSession()
	{
	// After the last session drops out, wait for the timer expiry and then
	// shut down
	if (--iSessionCount == 0)
		{
		ASSERT(iDeviceInfoFetcher);
		
		 // cancel any outstanding requests on the fetcher
    	iDeviceInfoFetcher->Cancel();
    	
		DEBUG_PRINTF(_L8("Device Info Server - Starting Shutdown Timer"));
		iTimer->After(KServerShutdownDelay);
		}

	DEBUG_PRINTF2(_L8("Device Info Server - Dropping Session (%d sessions active.)"), iSessionCount);
	}

void CDeviceInfoServer::HandleTimeout()
	{
	// Stop the active scheduler if there is no client for more than 2 seconds.
	// This in turn will stop the server
	CActiveScheduler::Stop();
	}

void CDeviceInfoServer::AcceptDeviceIdsL(CDeviceIdList* aDeviceIdList)
	{
	iDeviceIdList = aDeviceIdList;
	iPackedDeviceIdList = iDeviceIdList->PackL();
	CompleteOutstandingRequestsL();
	}

void CDeviceInfoServer::HandleDeviceIdErrorL(TInt aErrCode)
	{
	iDeviceIdFetchError = aErrCode;
	CompleteOutstandingRequestsL();
	}

void CDeviceInfoServer::CompleteOutstandingRequestsL()
	{
	// Complete any outstanding requests.  Note that if this leaves
	// then the server will be panic-ed because its unhandled, so it
	// doesn't matter that the requests are left on the queue until
	// the reset at the end.
	TInt count = iOutstandingDeviceIdRequests.Count();
	for (TInt i = 0; i < count; i++)
		{
		const RMessagePtr2 message = iOutstandingDeviceIdRequests[i];
		CompleteDeviceIdRequestL(message);
		}
	iOutstandingDeviceIdRequests.Reset();
	}

void CDeviceInfoServer::CompleteDeviceIdRequestL(const RMessagePtr2& aMessage)
	{	
	if (iDeviceIdList == NULL)
		{
		DEBUG_PRINTF2(_L8("Device Info Server - Device ID fetch failed with error code %d"), iDeviceIdFetchError);
		
		// error case - complete with error
		aMessage.Complete(iDeviceIdFetchError);
		}
	else
		{
		DEBUG_PRINTF(_L8("Device Info Server - Attempting to sent device ID list back to client"));
		
		TInt packedDeviceIdListLength = iPackedDeviceIdList->Length();
		if (aMessage.GetDesMaxLengthL(0) < packedDeviceIdListLength)
			{
			if (aMessage.GetDesMaxLengthL(0) < static_cast<TInt>(sizeof(TUint)))
				{
				DEBUG_PRINTF(_L8("Device Info Server - Invalid IPC descriptor length."));
				aMessage.Complete(KErrArgument);
				}
			else
				{
				DEBUG_PRINTF(_L8("Device Info Server - IPC descriptor overflow, must re-request."));
				
				TPckgC<TInt> responseSizePackage(packedDeviceIdListLength);
				aMessage.WriteL(0, responseSizePackage);
				aMessage.Complete(KErrOverflow);
				}
			}
		else
			{
			aMessage.WriteL(0, iPackedDeviceIdList->Des());
			aMessage.Complete(KErrNone);
			
			DEBUG_PRINTF(_L8("Device Info Server - ID list sent sucessfully."));
			}
		}
	}

void CDeviceInfoServer::HandleGetDeviceIdsL(const RMessagePtr2& aMessage)
	{
	DEBUG_PRINTF(_L8("Device Info Server - Handling Get Device IDs Request"));
	
	if (iDeviceIdList == NULL && iDeviceIdFetchError == KErrNone)
		{
		DEBUG_PRINTF(_L8("Device Info Server - Device ID fetch still pending. Awaiting outcome...")); 
		
		TInt err = iOutstandingDeviceIdRequests.Append(aMessage);
		if (err != KErrNone)
			{
			aMessage.Complete(err);
			}
		}
	else
		{
		CompleteDeviceIdRequestL(aMessage);
		}
	}

void StartServerL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CDeviceInfoServer* server = NULL;

	DEBUG_PRINTF(_L8("Device Info Server - Starting Server"));

	TRAPD(err, server = CDeviceInfoServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	CActiveScheduler::Install(NULL);
	
	DEBUG_PRINTF(_L8("Device Info Server - Stopping Server"));
	
	delete server;
	delete sched;
	}

} // namespace Swi

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, Swi::StartServerL()); 
	delete cleanup; 
	__UHEAP_MARKEND;
	return error;
	}

