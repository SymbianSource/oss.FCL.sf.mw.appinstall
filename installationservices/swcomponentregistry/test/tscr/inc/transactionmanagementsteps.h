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
* Defines the basic test step for the Software Component Registry test harness
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef TRANSACTIONMANAGEMENTSTEPS_H
#define TRANSACTIONMANAGEMENTSTEPS_H

#include "tscrstep.h"
class CScrTestServer;

_LIT(KScrApplyTransactionStep, "SCRApplyTransaction");
_LIT(KScrCreateTransactionStep, "SCRCreateTransaction");
_LIT(KScrManageAnotherTransactionStep, "SCRManageAnotherTransaction");

class CScrApplyTransactionStep : public CScrTestStep
/**
 	TEF test step which exercises all SCR transaction interfaces
 */
	{
public:
	CScrApplyTransactionStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};

class CScrCreateTransactionStep : public CScrTestStep
/**
 	TEF test step which exercises transaction management logic
 */
	{
public:
	CScrCreateTransactionStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};		

class CScrManageAnotherTransactionStep : public CScrTestStep
/**
 	TEF test step which exercises transaction management logic
 */
	{
public:
	CScrManageAnotherTransactionStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

#endif /* TRANSACTIONMANAGEMENTSTEPS_H */
