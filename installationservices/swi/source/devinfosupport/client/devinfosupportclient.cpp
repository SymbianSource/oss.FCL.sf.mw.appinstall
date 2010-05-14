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
* Device information client interface
* @internalComponent
* @released
*
*/


#include "deviceidlist.h"
#include "devinfosupportclient.h"
#include "devinfosupportsession.h"

namespace Swi
{

static const TInt KDefaultBufferSize=100;

EXPORT_C RDeviceInfo::RDeviceInfo() : iDeviceIdList(0)
	{
	}

EXPORT_C TInt RDeviceInfo::Connect()
	{
	// Only retry up to three times before giving up
	TInt retry = 3;
	for (;;)
		{
		TInt r = CreateSession(KDeviceInfoServerName, TVersion(KDeviceInfoServerVersionMajor, KDeviceInfoServerVersionMinor, KDeviceInfoServerVersionBuild));
		if ((r != KErrNotFound) && (r != KErrServerTerminated))
			{
			return r;
			}
		if (--retry == 0)
			{
			return r;
			}
		r = StartServer();
		if ((r != KErrNone) && (r != KErrAlreadyExists))
			{
			return r;
			}
		}
	}

EXPORT_C TInt RDeviceInfo::StartServer()
	{
	RProcess server;
	TInt r = server.Create(KDeviceInfoServerExeName, KNullDesC);

	// check server started
	if (r != KErrNone)
		{
		return r;
		}

	// Synchronise with the process to make sure it hasn't died straight away
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		server.Kill(0);				// Abort startup
		}
	else
		{
		// logon OK - start the server
		server.Resume();
		}

	// Wait to synchronise with server - if it dies in the meantime, it
	// also gets completed
	User::WaitForRequest(stat);	

	// We can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r= (server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}
	
EXPORT_C void RDeviceInfo::Close()
	{
	RSessionBase::Close();
	delete iDeviceIdList;
	iDeviceIdList = NULL;
	}

EXPORT_C const RPointerArray<HBufC>& RDeviceInfo::DeviceIdsL()
	{
	delete iDeviceIdList;
	iDeviceIdList = NULL;

	HBufC8* dataBuffer=HBufC8::NewLC(KDefaultBufferSize);
	TPtr8 pDataBuffer=dataBuffer->Des();
	TInt result=SendReceive(CDeviceInfoSession::KMessageGetDeviceIds, TIpcArgs(&pDataBuffer));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(pDataBuffer);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(dataBuffer);
		dataBuffer=HBufC8::NewL(sizeNeeded);
		pDataBuffer=dataBuffer->Des();
		
		result=SendReceive(CDeviceInfoSession::KMessageGetDeviceIds, TIpcArgs(&pDataBuffer));
		}
	User::LeaveIfError(result);

	iDeviceIdList = CDeviceIdList::NewL(pDataBuffer);
	CleanupStack::PopAndDestroy(dataBuffer);

	return iDeviceIdList->DeviceIds();
	}

} // namespace Swi
