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
* Class used by SWIS to handle certificate revocation checking
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#include <pkixcertchain.h>
#include <mctcertstore.h>

#include "revocationhandler.h"
#include "ocspsupporttransport.h"

using namespace Swi;

_LIT(KRevocationHandlerName, "_CRevocationHandler_");

const TInt KCertStoreUIDForSWInstallOCSPSigning = 268478646;     

EXPORT_C CRevocationHandler* CRevocationHandler::NewL(MCTCertStore& aCertStore)
	{
	CRevocationHandler* self = new (ELeave) CRevocationHandler(aCertStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRevocationHandler::~CRevocationHandler()
	{
	Deque(); // Calls cancel

	delete iParams;
	delete iClient;
	}

CRevocationHandler::CRevocationHandler(MCTCertStore& aCertStore)
: CActive(EPriorityNormal), iCertStore(aCertStore)
	{
	CActiveScheduler::Add(this);
	}

void CRevocationHandler::ConstructL()
	{
	iParams = COCSPParameters::NewL();
	}

EXPORT_C void CRevocationHandler::SetDefaultURIL(const TDesC8& aURI)
	{
	iParams->SetURIL(aURI, ETrue);
	}

EXPORT_C void CRevocationHandler::SetRetryCount(const TUint aRetryCount)
	{
	iParams->SetRetryCount(aRetryCount);
	}

EXPORT_C void CRevocationHandler::SetTimeout(const TInt aTimeout)
	{
	iParams->SetTimeout(aTimeout);
	}

EXPORT_C void CRevocationHandler::SendRequestL(RPointerArray<CPKIXCertChainBase>& aCertChainList, 
		TUint32& aIap, TRequestStatus& aClientStatus)
	{	
	iClientStatus = &aClientStatus;
	*iClientStatus = KRequestPending;
	iCertChainList = aCertChainList;

	// Chain of certs - will check all but the root
	for (TInt k = 0; k < iCertChainList.Count(); k++)
		{
		iParams->AddCertificatesL(*iCertChainList[k]);
		}

	// Set up authorisation scheme - we've a special UID registered with the certStore
	// for the appropriate certificates
	iParams->AddAllAuthorisationSchemesL(
		TUid::Uid(KCertStoreUIDForSWInstallOCSPSigning), iCertStore);

	COcspSupportTransport* iTransport = COcspSupportTransport::NewL(aIap);
	iParams->SetTransport(iTransport);

	iClient = COCSPClient::NewL(iParams);
	iParams = NULL; // Client takes ownership

	// We'll get our RunL called when the response comes back (or when it fails)
	SetActive();
	iStatus = KRequestPending;
	iState = EChecking;
	iClient->Check(iStatus);
	}


void CRevocationHandler::DoCancel()
	{
	iClient->CancelCheck();
	if(iClientStatus)
	   {
	   User::RequestComplete(iClientStatus, KErrCancel);	
	   }
	}


void CRevocationHandler::RunL()
	{
	switch (iState)
		{
		case EChecking:
			{
			User::RequestComplete(iClientStatus, iStatus.Int());
			iState = EFinished;				
			}
			break;

		default:
			{
			User::Panic(KRevocationHandlerName, 1);	
			}
		}
	}

TInt CRevocationHandler::RunError(TInt aError)
	{
	User::RequestComplete(iClientStatus, aError);	
	return KErrNone;
	}

EXPORT_C TInt CRevocationHandler::Error() const
   	{
  	ASSERT(iState == EFinished);
  	return iStatus.Int();
  	}
	
EXPORT_C OCSP::TResult CRevocationHandler::SummaryResult() const
	{
  	ASSERT(iState == EFinished);	
	return iClient->SummaryResult();
	}

EXPORT_C TInt CRevocationHandler::TransactionCount(void) const
	{
  	ASSERT(iState == EFinished);	
	return iClient->TransactionCount();
	}

EXPORT_C const TOCSPOutcome& CRevocationHandler::Outcome(TInt aIndex) const
	{
  	ASSERT(iState == EFinished);	
	return iClient->Outcome(aIndex);
	}

