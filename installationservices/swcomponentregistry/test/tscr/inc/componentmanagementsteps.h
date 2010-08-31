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
* Defines test steps for component management APIs in the SCR
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef COMPONENTMANAGEMENTSTEPS_H
#define COMPONENTMANAGEMENTSTEPS_H

#include "tscrstep.h"
class CScrTestServer;

_LIT(KScrAddComponentStep, "SCRAddComponent");
_LIT(KScrSetComponentPropertyStep, "SCRSetComponentProperty");
_LIT(KScrRegisterFileStep, "SCRRegisterFile");
_LIT(KScrAddFilePropertyStep, "SCRAddFileProperty");
_LIT(KScrSetCommonPropertyStep, "SCRSetCommonProperty");
_LIT(KScrDeleteComponentPropertyStep, "SCRDeleteComponentProperty");
_LIT(KScrDeleteFilePropertyStep, "SCRDeleteFileProperty");
_LIT(KScrUnregisterFileStep, "SCRUnregisterFile");
_LIT(KScrDeleteComponentStep, "SCRDeleteComponent");
_LIT(KScrDeleteAllComponentsStep, "SCRDeleteAllComponents");
_LIT(KScrInstallComponentStep, "SCRInstallComponent");
_LIT(KScrAddComponentDependencyStep, "SCRAddComponentDependency");
_LIT(KScrDeleteComponentDependencyStep, "SCRDeleteComponentDependency");

class CScrAddComponentStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR AddComponent interface
 */
	{
public:
	CScrAddComponentStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	
private:
	Usif::TComponentId AddLocalisableComponentL();	
	void VerifyComponentLocalizablesL(Usif::TComponentId aComponentId);		
	};

class CScrSetComponentPropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR SetComponentProperty interface.
 */
	{
public:
	CScrSetComponentPropertyStep(CScrTestServer& aParent);
		
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};

class CScrRegisterFileStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR RegisterComponentFile interface
 */
	{
public:
	CScrRegisterFileStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};		
	
class CScrAddFilePropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR SetFilePropertyL interface
 */
	{
public:
	CScrAddFilePropertyStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};	

class CScrSetCommonPropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR SetComponentNameL, SetVendorNameL and SetVersionL interface
 */
	{
public:
	CScrSetCommonPropertyStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrDeleteComponentPropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR DeleteComponentPropertyL interface
 */
	{
public:
	CScrDeleteComponentPropertyStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();	
	};	

class CScrDeleteFilePropertyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR DeleteFilePropertyL interface
 */
	{
public:
	CScrDeleteFilePropertyStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrUnregisterFileStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR UnregisterComponentFileL interface
 */
	{
public:
	CScrUnregisterFileStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrDeleteComponentStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR DeleteComponentL interface
 */
	{
public:
	CScrDeleteComponentStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrDeleteAllComponentsStep : public CScrTestStep
/**
 	TEF test step which deletes all components added by the harness.
 */
	{
public:
	CScrDeleteAllComponentsStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrInstallComponentStep : public CScrTestStep
/**
 	TEF test step which installs a typical SWI component.
 */
	{
public:
	CScrInstallComponentStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};	

class CScrAddComponentDependencyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR AddComponentDependencyL interface
 */
	{
public:
	CScrAddComponentDependencyStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();	
	};

class CScrDeleteComponentDependencyStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR DeleteComponentDependencyL interface
 */
	{
public:
	CScrDeleteComponentDependencyStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();	
	};

#endif /* COMPONENTMANAGEMENTSTEPS_H */
