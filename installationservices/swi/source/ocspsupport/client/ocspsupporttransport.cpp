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

#include "ocspsupporttransport.h"

namespace Swi
{
	
EXPORT_C /*static*/ COcspSupportTransport* COcspSupportTransport::NewL(TUint32& aIap)
	{
	COcspSupportTransport* self=COcspSupportTransport::NewLC(aIap);	
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C /*static*/ COcspSupportTransport* COcspSupportTransport::NewLC(TUint32& aIap)
	{
	COcspSupportTransport* self=new(ELeave) COcspSupportTransport(aIap);	
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

COcspSupportTransport::COcspSupportTransport(TUint32& aIap)
	: iIap(aIap)
	{
	}

void COcspSupportTransport::ConstructL()
	{
	User::LeaveIfError(iOcspHandler.Connect());
	}

COcspSupportTransport::~COcspSupportTransport()
	{
	delete iResultData;
	iOcspHandler.Close();
	}

// from MOCSPTransport
void COcspSupportTransport::SendRequest (const TDesC8 &aUri, const TDesC8 &aRequest, const TInt aTimeout, TRequestStatus &aStatus)
	{
	aStatus=KRequestPending;
	iOcspHandler.SendRequest(aUri, aRequest, aTimeout, iIap, aStatus);
	}

void COcspSupportTransport::CancelRequest ()
	{
	iOcspHandler.CancelRequest();
	}

TPtrC8 COcspSupportTransport::GetResponse () const
	{
	TPtrC8 pResultData;
	
	TRAPD(err, pResultData.Set(iOcspHandler.GetResponseL(iIap)));
	if (err!=KErrNone)
		{
		return TPtrC8();
		}
	else
		{
		return pResultData;
		}
	}
} // namespace Swi
