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
*
*/


/**
 @file
 @test
*/

#include "stsrollbackalltest.h"

#include <usif/sts/sts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KStsRollBackAllSecName, "STS RollBAckAllPending APIs test");

using namespace Usif;

CStsRollbackAllSecTest* CStsRollbackAllSecTest::NewL()
	{
	CStsRollbackAllSecTest* self=new(ELeave) CStsRollbackAllSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CStsRollbackAllSecTest::CStsRollbackAllSecTest()
	{
	SetSidRequired(TUid::Uid(0x10202DCE)); //SWI daemon's UID3 (ie: SID)
	}
	
void CStsRollbackAllSecTest::ConstructL()
	{
	SetNameL(KStsRollBackAllSecName);
	}

void CStsRollbackAllSecTest::RunTestL()
	{
	RStsRecoverySession stsRecoverySession;
	CleanupClosePushL(stsRecoverySession);
	TRAPD(err, stsRecoverySession.RollbackAllPendingL());
	CheckFailL(err, _L("RollbackAllPendingL"));
	CleanupStack::PopAndDestroy(&stsRecoverySession);
	}
