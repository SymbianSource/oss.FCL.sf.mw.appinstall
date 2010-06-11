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
* Defines the basic test step class which implements OOM tests.
*
*/




/**
 @file 
 @internalComponent
 @test
*/

#ifndef OOMTESTSTEP_H
#define OOMTESTSTEP_H

#include <test/testexecutestepbase.h>

// Out of Memory Test attributes
_LIT(KConfigOOMTest,"OOMTest");
_LIT(KConfigOOMServerTest,"OOMServerTest");

// forward declaration
class RScsClientBase;

class COomTestStep : public CTestStep
/**
 	Base class used to provide handy functions available to test step classes.
 */
	{
public:
	IMPORT_C virtual ~COomTestStep();
	
	// The derived class must NOT override these three methods
	IMPORT_C TVerdict doTestStepPreambleL();
	IMPORT_C TVerdict doTestStepL();
	IMPORT_C TVerdict doTestStepPostambleL();
	IMPORT_C virtual RScsClientBase* ClientHandle();
	
protected:
	IMPORT_C COomTestStep();
	
private:
	// The derived class must implement these three classes
	// The actual test step will be implemented by ImplTestStepL
	virtual void ImplTestStepPreambleL() = 0;
	virtual void ImplTestStepL() = 0;
	virtual void ImplTestStepPostambleL() = 0;
	
	TVerdict ImplOomTestL();
	TVerdict ImplOomServerTestL();
	void ReadTestConfigurationL();
	
private:
	TBool iOOMTest;
	TBool iOOMServerTest;
	};
	
#endif /* OOMTESTSTEP_H */
