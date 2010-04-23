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
 @released
 @internalTechnology
*/

#include "certificateretriever.h"

#include <securitydefs.h>
#include <ccertattributefilter.h>
#include <swicertstore.h>

#include "log.h"

_LIT(KCertificateRetriever, "_CCertificateRetriever_");

using namespace Swi;

/*static*/ CCertificateRetriever* CCertificateRetriever::NewL(CSWICertStore& aCerstore)
	{
	CCertificateRetriever* self = CCertificateRetriever::NewLC(aCerstore);
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ CCertificateRetriever* CCertificateRetriever::NewLC(CSWICertStore& aCerstore)
	{
	CCertificateRetriever* self = new(ELeave) CCertificateRetriever(aCerstore);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCertificateRetriever::CCertificateRetriever(CSWICertStore& aCerstore) : CActive(EPriorityNormal), iCertStore(aCerstore)
	{
	CActiveScheduler::Add(this);
	}

CCertificateRetriever::~CCertificateRetriever()
	{
	Deque();
	
	delete iFilter;
	}

void CCertificateRetriever::ConstructL()
	{
	iFilter = CCertAttributeFilter::NewL();
	iFilter->SetOwnerType(ECACertificate); // We only want root certs!	
	}

//
// From CActive
//

void CCertificateRetriever::RunL()
	{
	DEBUG_PRINTF3(_L8("Security Manager - CCertificateRetriever::RunL(). State: %d, Status: %d."),
		iState, iStatus.Int());
	
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());     // Hop into RunError()
		}
		
	switch (iState)
		{ 	
		case EBuildCAList:
			{
			// Nothing else to do
			User::RequestComplete(iClientStatus, KErrNone);
			break;
			}
		default:
			{
			User::Panic(KCertificateRetriever, 1); 
			}
		}
	}

void CCertificateRetriever::DoCancel()
	{
	DEBUG_PRINTF2(_L8("Security Manager - Cancelling certificate retriever in state %d."), iState);
	
	switch (iState)
		{
		case EBuildCAList:
			{
			iCertStore.CancelList();
			break;
			}
		default:
			{
			// mandatory call is syncronoue so no need for cancel
			// Do Nothing
			}
		}	
	}

TInt CCertificateRetriever::RunError(TInt aError)
	{
	DEBUG_PRINTF2(_L8("Security Manager - CCertificateRetriever::RunError(). Error code: %d."), aError);
	
	User::RequestComplete(iClientStatus, aError);	
	return KErrNone;
	}	


//
// Business methods
//
void CCertificateRetriever::RetrieveCACertificates(RMPointerArray<CCTCertInfo>& aCertificateListOut, TRequestStatus& aClientStatus)
	{	
	iState = EBuildCAList;
	DoInitialize(aCertificateListOut, aClientStatus);
	}

void CCertificateRetriever::DoInitialize(RMPointerArray<CCTCertInfo>& aCertificateListOut, TRequestStatus& aClientStatus)
	{
	iClientStatus = &aClientStatus;
	*iClientStatus = KRequestPending;

	iCertificateList = &aCertificateListOut;
	SetActive();
	iCertStore.List(*iCertificateList, *iFilter, iStatus); // Get all the root certs		
	}


