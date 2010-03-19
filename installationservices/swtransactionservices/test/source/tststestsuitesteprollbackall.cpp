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
 @internalComponent
*/
#include "tststestsuitesteprollbackall.h"
#include <usif/sts/sts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

CRollbackAllPendingStep::CRollbackAllPendingStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRollbackAllPendingStep);
	}

void CRollbackAllPendingStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep Preamble"));
	SetTestStepResult(EPass);

	}

void CRollbackAllPendingStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep in test step"));

	Usif::RStsRecoverySession session;
	CleanupClosePushL(session);
	session.RollbackAllPendingL();
	CleanupStack::PopAndDestroy(&session);
	SetTestStepResult(EPass);

	}

void CRollbackAllPendingStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep Postamble"));

	}
