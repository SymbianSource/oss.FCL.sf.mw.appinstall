/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file implements the SIF Transport Session
*
*/


#include "siftransportserver.h"
#include <scs/ipcstream.h>
#include <scs/nullstream.h>
#include <usif/sif/sif.h>
#include "siftransportcommon.h"
#include "usiflog.h"

using namespace Usif;

CSifTransportSession* CSifTransportSession::NewL(CSifTransportServer &aServer, TransportTaskFactory::GenerateTask aTaskFactory)
/**
	Factory function allocates new instance of CSifTransportSession.

	@return	New, initialized instance of CSifTransportSession
			which is owned by the caller.
 */
	{
	CSifTransportSession* self = new (ELeave) CSifTransportSession(aServer, aTaskFactory);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSifTransportSession::CSifTransportSession(CSifTransportServer &aServer, TransportTaskFactory::GenerateTask aTaskFactory)
/**
	This private constructor prevents direct instantiation.
 */
	: CScsSession(aServer), iTaskFactory(aTaskFactory), iExclusiveOperation(ETrue)
	{
	DEBUG_PRINTF3(_L8("0x%x CSifSession(server %x)\n"), this, &aServer);
	}

CSifTransportSession::~CSifTransportSession()
/**
	The base class destructor destroys any remaining subsessions
	or outstanding requests.
 */
	{
	DEBUG_PRINTF2(_L8("0x%x ~CSifTransportSession)\n"), this);
	}

TBool CSifTransportSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	Note the subsession creation command is automatically sent to
	DoCreateSubsessionL, and not this function.

	@param	aFunction	Function identifier without SCS code.
	@param	aMessage	Standard server-side handle to message.
 */
	{

	DEBUG_PRINTF3(_L8("0x%x CSifSession::DoServiceL function %d\n"), this, aFunction);

	// Check if aFunction falls into the expected range
	if (aFunction < EGetComponentInfoByFileName || aFunction > EDeactivate)
		{
		aMessage.Panic(KSifTransportServerRequestError, KErrNotSupported);
		return ETrue;
		}

	// Check if this is an Install or Uninstall request, if yes read the ExclusiveOperation flag from aMessage
	if (aFunction == EGetComponentInfoByFileName || aFunction == EGetComponentInfoByFileHandle)
		{
		// GetComponentInfo requests are always processed regardless of the state of the SIF server.
		iExclusiveOperation = EFalse;
		}
	else if (aFunction >= EInstallByFileName && aFunction <= EUninstallWithOpaqueData)
		{
		// Read the ExclusiveOperation flag from aMessage for a synchronous request
		if (aFunction == EInstallByFileHandleWithOpaqueDataPreamble)
			{
			iExclusiveOperation = aMessage.Int0() != EFalse;
			return ETrue;
			}

		// Read the ExclusiveOperation flag from aMessage for asynchronous requests
		if (aFunction == EInstallByFileName ||
			aFunction == EInstallByFileNameWithOpaqueData ||
			aFunction == EUninstall ||
			aFunction == EUninstallWithOpaqueData)
			{
			iExclusiveOperation = aMessage.Int1() != EFalse;
			}
		else if (aFunction == EInstallByFileHandle)
			{
			iExclusiveOperation = aMessage.Int2() != EFalse;
			}
		}
	else
		{
		// We set iExclusiveOperation to ETrue in the constructor. However, we have to
		// set it here again because the client may send more requests within the same
		// session.
		iExclusiveOperation = ETrue;
		}

	// If the  ExclusiveOperation flag is set to ETrue and there is another pending request we leave with KErrServerBusy
	if (iExclusiveOperation && iServer.iAsyncRequests.Count() > 0)
		{
		User::Leave(KErrServerBusy);
		}

	// Create new request and execute it
	CSifTransportRequest::CreateAndExecuteL(aFunction, this, iTaskFactory, aMessage);

	return EFalse;
	}
