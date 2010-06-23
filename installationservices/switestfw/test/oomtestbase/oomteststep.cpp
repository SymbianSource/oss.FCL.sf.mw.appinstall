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
* Implements the basic test step which implements OOM tests.
*
*/




#include "oomteststep.h"
#include <scs/scsclient.h>

EXPORT_C COomTestStep::COomTestStep()
// Constructor.
	: iOOMTest(EFalse)
	{
	//empty
	}

EXPORT_C COomTestStep::~COomTestStep()
// Destructor.
	{
	//empty
	}
	
	
EXPORT_C TVerdict COomTestStep::doTestStepPreambleL()
/**
 	From CTestStep. Do some boilerplate pre-test configuration. 
 */
	{
	__UHEAP_MARK;
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	ReadTestConfigurationL();
	SetTestStepResult(EPass);
	
	ImplTestStepPreambleL();
	
	return TestStepResult();
	}


EXPORT_C TVerdict COomTestStep::doTestStepL()
/**
	From CTestStep. Default behaviour of ImplTestStepL() allows for the test case to be run both
	under 'Normal' and 'Out of Memory' Conditions. 
  
    Implementation of the test case itself is called from the ImplTestStepL() of the derived test step.
  
    The state of the iOOMTest member variable determines the type of test conditons:
    - EFalse : Normal Test 
    - ETrue : Out of Memory Test
 */
	{
	
	if (iOOMTest)
		{
		ImplOomTestL();
		}
	else if (iOOMServerTest)
		{
		ImplOomServerTestL();
		}
	else
		{
		ImplTestStepL();
		}	
	
	return TestStepResult();
	}


EXPORT_C TVerdict COomTestStep::doTestStepPostambleL()
/**
 	From CTestStep. Destroys the active scheduler of the test step.
 */
	{
	ImplTestStepPostambleL();
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"),User::CountAllocCells());
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}


TVerdict COomTestStep::ImplOomTestL()
/**
    Runs the test step under OOM Conditions checking that each heap allocation is fail safe.
   
 */
	{	
	// Pre and Post test heap cell allocation counts
 	TInt cellCountAfter = 0;
	TInt cellCountBefore = 0;
	
	// The loop tests each heap allocation under out of memory conditions to determine whether
	// the test framework cleans up correctly without leaking memory.
	// 
	// The 'for' loop terminates as soon as any of the following events occur:
	// a) The pre and post heap cell counts mismatch signalling a memory leakage
	// b) Any leave with an error code other than 'KErrNoMemory'
	// c) All heap allocations have been tested and the test returns 'KErrNone'
	
	for (TInt testCount = 0; ; ++testCount)
 		{
 		__UHEAP_MARK;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, testCount+1);
 		cellCountBefore = User::CountAllocCells();
 		TRAPD(err, ImplTestStepL());
 		cellCountAfter = User::CountAllocCells();
 		__UHEAP_MARKEND;
 		
 		INFO_PRINTF3(_L("OOM Test %d: Status = %d"),testCount,err);
 		
 		if (err == KErrNone)
 			{
			INFO_PRINTF1(_L("OOM Test Finished"));
 			break;
 			}
 		else if(err == KErrNoMemory)
 			{
 			if (cellCountBefore != cellCountAfter)
 				{
 				ERR_PRINTF2(_L("OOM Test Result: Failed - Memory leakage on iteration %d"), testCount);
 				ERR_PRINTF2(_L("Pre-Test Heap Cell Count: %d"), cellCountBefore);
 				ERR_PRINTF2(_L("Post-Test Heap Cell Count: %d"), cellCountAfter);
 				SetTestStepResult(EFail);
 				break;
 				}
 			}
 		else
 			{
 			User::Leave(err);
 			break;
 			}
		}
	
	return TestStepResult();
	}


TVerdict COomTestStep::ImplOomServerTestL()
	{
	RScsClientBase *client = ClientHandle();
	if(!client)
		{
		ERR_PRINTF1(_L("ClientHandle has returned NULL. The server cannot be tested without the client handle!"));
		User::Leave(KErrArgument);
		}
	TInt err(0);
	for (TInt testCount=0; ; ++testCount)
 		{
 		INFO_PRINTF2(_L("OOM Server Test %d"), testCount);
 		err = client->SetServerHeapFail(testCount+1);
 		if(KErrNone != err)
 			{
 			ERR_PRINTF3(_L("OOM Server Test Result: Failed to set heap fail on iteration %d. Error:%d"), testCount, err);
 			SetTestStepResult(EFail);
 			break;
 			}
 		TRAPD(retStepVal, ImplTestStepL());
 		err = client->ResetServerHeapFail();
 		
 		if(KErrNone != err)
 			{
 			ERR_PRINTF3(_L("ResetServerHeapFail failed on iteration %d with error %d"), testCount, err);
 			SetTestStepResult(EFail);
 			break;
 			}
 					
 		if(KErrNoMemory == retStepVal)
 			{
 			INFO_PRINTF2(_L("Received correct out of memory error on iteration %d"), testCount);
 			}
 		else if (KErrNone == retStepVal)
 			{
 			INFO_PRINTF1(_L("Server OOM Test Finished"));
 			break;
 			}
 		else 
 			{
 			// Propagate all errors apart from KErrNoMemory
 			User::Leave(retStepVal);
 			}
 		}// for
	
	return TestStepResult();
	}


void COomTestStep::ReadTestConfigurationL()
	{
	// Read OOM Test Flag
	GetBoolFromConfig(ConfigSection(), KConfigOOMTest, iOOMTest);
	// Read OOM Server Test Flag
	GetBoolFromConfig(ConfigSection(), KConfigOOMServerTest, iOOMServerTest);
	}

EXPORT_C RScsClientBase* COomTestStep::ClientHandle()
/**
 	Returns a pointer to RScsClientBase class. This default implementation returns NULL.
 	If a server is tested with OOM conditions, the derived class must return a pointer to
 	the RScsClientBase-derived object.
 */
	{
	return NULL;
	}
