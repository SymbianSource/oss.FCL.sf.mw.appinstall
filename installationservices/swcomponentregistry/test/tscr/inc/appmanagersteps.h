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
* Defines test steps for application manager related interface in the Software Component Registry
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef APPMANAGERSTEPS_H
#define APPMANAGERSTEPS_H

#include "tscrstep.h"
class CScrTestServer;

_LIT(KScrIsMediaPresentStep, "SCRIsMediaPresent");
_LIT(KScrIsComponentOrphaned, "SCRIsComponentOrphaned");
_LIT(KScrCompareVersions, "SCRCompareVersions");
_LIT(KScrIsComponentOnReadOnlyDrive, "SCRIsComponentOnReadOnlyDrive");
_LIT(KScrComponentPresence, "ScrComponentPresence");
_LIT(KScrComponentOnEMMC, "ScrComponentOnEMMC");
_LIT(KScrComponentPresentForNameVendor, "ScrComponentPresentForNameVendor");

class CScrIsMediaPresentStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR IsMediaPresent interface
 */
	{
public:
	CScrIsMediaPresentStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrIsComponentOrphanedStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR IsComponentOrphanedL interface
 */
	{
public:
	CScrIsComponentOrphanedStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrCompareVersionsStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR CompareVersionsL interface
 */
	{
public:
	CScrCompareVersionsStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrIsComponentOnReadOnlyDriveStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR IsComponentOnReadOnlyDriveL interface
 */
	{
public:
	CScrIsComponentOnReadOnlyDriveStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrComponentPresenceStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR IsComponentPresentL interface
 */
	{
public:
	CScrComponentPresenceStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();		
	};

class CScrEMMCComponentStep : public CScrTestStep
/**
    TEF test step which checks if any components are present on EMMC
 */
    {
public:
    CScrEMMCComponentStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();      
    };

class CScrComponentPresentForNameVendorStep : public CScrTestStep
/**
    TEF test step which checks if any components are present with the given name and vendor
 */
    {
public:
    CScrComponentPresentForNameVendorStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();      
    };
#endif /* APPMANAGERSTEPS_H */
