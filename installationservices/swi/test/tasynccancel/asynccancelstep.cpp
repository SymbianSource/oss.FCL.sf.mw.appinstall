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


#include "asynccancelstep.h"


	
CAsyncCancelStep::~CAsyncCancelStep()
	{
	}	
	
CAsyncCancelStep::CAsyncCancelStep()
	{
	SetTestStepName(KAsyncCancelStep);
	}

TVerdict CAsyncCancelStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
TVerdict CAsyncCancelStep::doTestStepL()
	{
	
	
	// read sis filename
	TPtrC sisToInstall;
	GetStringFromConfig(ConfigSection(), _L("sisToInstall"), sisToInstall);
	
	// read uid
	TInt tmpUid;
	if(GetHexFromConfig(ConfigSection(), _L("uidToInstall"), tmpUid) == EFalse)
		{
		tmpUid = 0;
		}
	
	// read data related to cancelling intervals
	TInt whenToCancelLow;
	GetIntFromConfig(ConfigSection(),_L("whenToCancelLow"),whenToCancelLow);
	TInt whenToCancelHigh;
	GetIntFromConfig(ConfigSection(),_L("whenToCancelHigh"),whenToCancelHigh);
	TInt howManyCancelations;
	GetIntFromConfig(ConfigSection(),_L("howManyCancelations"),howManyCancelations);
	TInt minWhenToCancelInc;
	GetIntFromConfig(ConfigSection(),_L("minWhenToCancelInc"),minWhenToCancelInc);

	CInstallPrefs* prefs=CInstallPrefs::NewLC();
	
	
	// OCSP related options	
	TPtrC ocspUri;
	if (GetStringFromConfig(ConfigSection(), _L("ocspUri"), ocspUri))
		{
		const TUint KMaxUriLength=512;
		TBuf8<KMaxUriLength> temp;
		temp.Copy(ocspUri);
		prefs->SetRevocationServerUriL(temp);
		}

	TInt useOcsp=0;
	if (GetIntFromConfig(ConfigSection(), _L("useOcsp"), useOcsp))
		{
		prefs->SetPerformRevocationCheck(useOcsp ? ETrue : EFalse);
		}
	
	
	// run test
	TInt err= DoTestL(
					 whenToCancelLow, whenToCancelHigh, howManyCancelations, minWhenToCancelInc,
					 sisToInstall, TUid::Uid(tmpUid),
					 prefs
					 );
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
		
		
	CleanupStack::PopAndDestroy(prefs);
		
	return TestStepResult();
	}		
		
TVerdict CAsyncCancelStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

// End of file
