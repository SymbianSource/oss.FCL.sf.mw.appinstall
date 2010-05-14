/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef COMPONENTQUERYSTEPS_H
#define COMPONENTQUERYSTEPS_H

#include "tscrstep.h"

class CScrTestServer;

_LIT(KScrGetComponentStep, "SCRGetComponent");
_LIT(KScrGetComponentLocalizedStep, "SCRGetComponentLocalizedInfo");
_LIT(KScrGetFilePropertiesStep, "SCRGetFileProperties");
_LIT(KScrGetFilePropertyStep, "SCRGetFileProperty");
_LIT(KScrGetFileComponentsStep, "SCRGetFileComponents");
_LIT(KScrGetComponentPropertyStep, "SCRGetComponentProperty");
_LIT(KScrGetComponentPropertiesStep, "SCRGetComponentProperties");
_LIT(KScrGetComponentIdsStep, "SCRGetComponentIds");
_LIT(KScrGetComponentFilesCountStep, "SCRGetComponentFilesCount");
_LIT(KScrGetComponentWithGlobalIdStep, "SCRGetComponentWithGlobalId");
_LIT(KScrGetComponentIdStep, "SCRGetComponentId");
_LIT(KScrGetGlobalIdList, "SCRGetGlobalIdList");
_LIT(KScrValuesNegativeStep, "SCRValuesNegative");
_LIT(KScrOutOfMemoryStep, "SCROutOfMemory");

class CScrGetComponentStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponent interface
 */
	{
public:
	CScrGetComponentStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrGetComponentLocalizedStep : public CScrTestStep
/**
    TEF test step which exercises the SCR GetComponentLocalized interface
 */
    {
public:
    CScrGetComponentLocalizedStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetFilePropertiesStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetFileProperties interface
 */
	{
public:
	CScrGetFilePropertiesStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrGetFilePropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetFileProperties interface
 */
	{
public:
	CScrGetFilePropertyStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};
		
class CScrGetFileComponentsStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetFileComponents interface
 */
	{
public:
	CScrGetFileComponentsStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrGetComponentPropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentPropertyL interface
 */
	{
public:
	CScrGetComponentPropertyStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrGetComponentPropertiesStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentProperties interface
 */
	{
public:
	CScrGetComponentPropertiesStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrGetComponentIdsStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentIdsL interface
 */
	{
public:
	CScrGetComponentIdsStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrGetComponentFilesCountStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentFilesCountL interface
 */
	{
public:
	CScrGetComponentFilesCountStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrGetComponentWithGlobalIdStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentL interface
 */
	{
public:
	CScrGetComponentWithGlobalIdStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrGetComponentIdStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetComponentIdL interface
 */
	{
public:
	CScrGetComponentIdStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrGetGlobalIdListStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetSupplierComponentsL and GetDependantComponentsL interfaces
 */
	{
public:
	CScrGetGlobalIdListStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrValuesNegativeStep : public CScrTestStep
/**
 	TEF test step which checks boundaries for values in SCR.
 */
	{
public:
	CScrValuesNegativeStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class SCROufOfMemory : public CScrTestStep
/**
 	TEF test step which checks OOM conditions in SCR.
 */
	{
public:
	SCROufOfMemory(CScrTestServer& aParent);
	SCROufOfMemory();
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	
private:
	void ExecuteL();
	void ApplicationRegistrationOperationsL(Usif::RSoftwareComponentRegistry& scrSession);
	TInt iFailRate;	
	};	

#endif //COMPONENTQUERYSTEPS_H
