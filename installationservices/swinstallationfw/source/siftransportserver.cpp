/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This file implements the SIF Transport Server.
*
*/


#include "siftransportserver.h"
#include "siftransportcommon.h"
#include <e32property.h>
#include "sifnotification_internal.h"
#include "scrclient.inl"

using namespace Usif;

CSifTransportServer* CSifTransportServer::NewLC(const TDesC& aServerName, const TVersion& aVersion,
									TransportTaskFactory::GenerateTask aTaskFactory, TInt aShutdownPeriodUs)
/**
	Factory function allocates new, initialized instance of
	CSifServer which is left on the cleanup stack.

	@return					New, initialized instance of CSifServer
							which is left on the cleanup stack.
 */
	{
	CSifTransportServer* self = new (ELeave) CSifTransportServer(aVersion, aTaskFactory);
	CleanupStack::PushL(self);
	self->ConstructL(aServerName, aShutdownPeriodUs);
	return self;
	}

CSifTransportServer::CSifTransportServer(const TVersion& aVersion, TransportTaskFactory::GenerateTask aTaskFactory)
/**
	Initializes the superclass with this server's version.
 */
	:	CScsServer(aVersion), iTaskFactory(aTaskFactory)
	{
	}

void CSifTransportServer::ConstructL(const TDesC& aServerName, TInt aShutdownPeriodUs)
/**
	Second-phase construction initializes the superclass and
	starts the server.
 */
	{
	// Define a key (KSifOperationKey) which would be used to notify the client of any new operations.
    TInt ret = RProperty::Define(KUidSystemCategory, KSifOperationKey, RProperty::EByteArray, KSecurityPolicyWDD, KSecurityPolicyNone,(sizeof(TInt) * KMaxNumberOfOperations));
    if (ret != KErrAlreadyExists && ret != KErrNone)
        {
        User::Leave(ret);
        }

    if(ret == KErrNone)
        {
        // Create a empty CSifOperationKey object and publish it.
        CSifOperationKey* nullKey = CSifOperationKey::NewL();
        CleanupStack::PushL(nullKey);
        RBuf8 nullKeyBuffer;
        nullKeyBuffer.CleanupClosePushL();
        ExternalizeRefObjectL(*nullKey, nullKeyBuffer);
        User::LeaveIfError(RProperty::Set(KUidSystemCategory, KSifOperationKey, nullKeyBuffer));
		CleanupStack::PopAndDestroy(2, nullKey);
        }
    
    CScsServer::ConstructL(aShutdownPeriodUs);
	StartL(aServerName);
	}


CSifTransportServer::~CSifTransportServer()
/**
	Cleanup the server, in particular close the RFs session.
 */
	{
	RProperty::Delete(Usif::KSifOperationKey);
	}

CScsSession* CSifTransportServer::DoNewSessionL(const RMessage2& /*aMessage*/)
/**
	Implement CScsServer by allocating a new instance of CSifSession.

	@param	aMessage	Standard server-side handle to message.	 Not used.
	@return			New instance of CSifSession which is owned by the
					caller.
 */
	{
	return CSifTransportSession::NewL(*this, iTaskFactory);
	}

namespace
	{
	struct TServerStartupInfo
		{
		const TDesC* iServerName;
		const TVersion* iVersion;
		TransportTaskFactory::GenerateTask iTaskFactory;
		TInt iShutdownPeriodUs;
		};
	}

EXPORT_C CScsServer* CSifTransportServer::NewSifTransportServerLC()
/**
	This factory function is called by SCS.  It allocates
	the server object and leaves it on the cleanup stack.

	@return	New initialized instance of CScsTestServer.
			On return this is on the cleanup stack.
*/ 
	{
	TServerStartupInfo* ssi = static_cast<TServerStartupInfo*>(Dll::Tls());
	return CSifTransportServer::NewLC(*ssi->iServerName, *ssi->iVersion, ssi->iTaskFactory, ssi->iShutdownPeriodUs);
	}

namespace Usif
	{
	EXPORT_C TInt StartTransportServer(const TDesC& aServerName, const TVersion& aVersion, TransportTaskFactory::GenerateTask aTaskFactory,  TInt aShutdownPeriodUs)
		{
		// A local object can be used with TLS here because it is read by CSifTransportServer::NewSifTransportServerLC()
		// before we leave the scope of this function. This is because CSifTransportServer::NewSifTransportServerLC() is a callback
		// function called by the SCS Server in response to StartScsServer().
		TServerStartupInfo ssi = {&aServerName, &aVersion, aTaskFactory, aShutdownPeriodUs};
		Dll::SetTls(&ssi);
		TInt err = StartScsServer(CSifTransportServer::NewSifTransportServerLC);
		return err;
		}
	}
