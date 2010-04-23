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
*
*/


/**
 @file
*/

#include "ocspsupport.h"
#include "ocspsupportsession.h"


namespace Swi
{

// ROcspHandler

const TInt ROcspHandler::KDefaultBufferSize=100;

TInt ROcspHandler::Connect()
	{
	// Set result data to empty
	iResultData=0;
	
	// Only retry up to three times before giving up
	TInt retry = 3;
	for (;;)
		{
		TInt r = CreateSession(KOcspSupportServerName, TVersion(KOcspSupportServerVersionMajor, KOcspSupportServerVersionMinor, KOcspSupportServerVersionBuild));
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

TInt ROcspHandler::StartServer()
	{
	RProcess server;
	TInt r = server.Create(KOcspSupportServerExeName, KNullDesC);

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
	
void ROcspHandler::Close()
	{
	RSessionBase::Close();
	delete iResultData;
	}

void ROcspHandler::CancelRequest()
	{
	SendReceive(COcspSupportSession::KMessageCancelRequest, TIpcArgs());
	}

TPtrC8 ROcspHandler::GetResponseL(TUint32& aIapOut) const
	{
	if (!iResultData)
		{
		iResultData=HBufC8::NewL(KDefaultBufferSize);
		}
	TPtr8 pDataBuffer(iResultData->Des());
	TPckg<TUint32> pkg(aIapOut);
	
	TInt result=SendReceive(COcspSupportSession::KMessageGetResponse, TIpcArgs(&pDataBuffer, &pkg));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*iResultData);
		
		// Re-allocate buffer
		delete iResultData;
		iResultData=0;
		iResultData=HBufC8::NewL(sizeNeeded);

		TPtr8 pResizedDataBuffer(iResultData->Des());
		
		result=SendReceive(COcspSupportSession::KMessageGetResponse, TIpcArgs(&pResizedDataBuffer, &pkg));
		}

	User::LeaveIfError(result);
	return TPtrC8(*iResultData);
	}

void ROcspHandler::SendRequest(const TDesC8& aUri, const TDesC8& aRequest, const TInt aTimeout, TUint32 aIap, TRequestStatus& aStatus)
	{
	SendReceive(COcspSupportSession::KMessageSendRequest, TIpcArgs(&aUri, &aRequest, aTimeout, aIap), aStatus);
	}

} // namespace Swi
