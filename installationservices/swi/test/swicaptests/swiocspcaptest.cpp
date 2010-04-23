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

#include "swiocspcaptest.h"

#include "ocspsupport.h"

#include <e32def.h>

_LIT(KSwiOcspCapTestName, "Swi Ocsp capability test");


CSwiOcspCapTest* CSwiOcspCapTest::NewL()
	{
	CSwiOcspCapTest* self=new(ELeave) CSwiOcspCapTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiOcspCapTest::CSwiOcspCapTest()
	{
	SetCapabilityRequired(ECapabilityTCB);
	}
	
void CSwiOcspCapTest::ConstructL()
	{
	SetNameL(KSwiOcspCapTestName);
	}

void CSwiOcspCapTest::RunTestL()
	{
	Swi::ROcspHandler ocsp;
	CleanupClosePushL(ocsp);
		
	if (KErrNone != CheckFailL(ocsp.Connect(), _L("Connect")))
		{
		// if we get here we didn't connect and expected not to, but cannot carry on with tests
		CleanupStack::PopAndDestroy(&ocsp);
		return;
		}

	TInt err=KErrNone;
	TRequestStatus s;
	TRequestStatus& rs=s;
	
	TInt timeout(0);
	TUint32 iap(0);
	ocsp.SendRequest(_L8("uri"), _L8("request"), timeout, iap, rs);
	User::WaitForRequest(rs);	// Acceptable since we know what SendRequest does. 
	CheckFailL(rs.Int(), _L("SendRequest"));
	
	if (rs.Int()==KErrNone)
		{
		TRAP(err, ocsp.GetResponseL(iap));
		CheckFailL(err, _L("GetResponseL"));
		}

	CleanupStack::PopAndDestroy(&ocsp);
	}

