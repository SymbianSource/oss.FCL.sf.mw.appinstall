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
* archivestep.cpp
*
*/


#include <f32file.h>
#include <test/testexecutelog.h>
#include <apmstd.h>
#include <caf/caf.h>
#include "rightsDbStep.h"
#include "rtarights.h"

using namespace ReferenceTestAgent;

CClearRightsDb::~CClearRightsDb()
	{
	}

CClearRightsDb::CClearRightsDb(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KClearRightsDb);
	}


TVerdict CClearRightsDb::doTestStepL()
	{
	SetTestStepResult(EFail);

	__UHEAP_MARK;	
	
	INFO_PRINTF1(_L("Clearing Rights Database"));

	RRtaRights rightsServer;
	User::LeaveIfError(rightsServer.Connect());
	CleanupClosePushL(rightsServer);
	
	rightsServer.ClearAllRightsL();
	
	CleanupStack::PopAndDestroy(&rightsServer);

	SetTestStepResult(EPass);

	__UHEAP_MARKEND;

	return TestStepResult();
	}	


