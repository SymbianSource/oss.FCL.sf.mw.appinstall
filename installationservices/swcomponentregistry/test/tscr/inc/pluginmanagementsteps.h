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
* Defines test steps for post-manufacture plugin management related interface in the Software Component Registry
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef PLUGINMANAGEMENTSTEPS_H
#define PLUGINMANAGEMENTSTEPS_H

#include "tscrstep.h"
#include "tscraccessor_client.h"
class CScrTestServer;

_LIT(KScrAddSoftwareTypeStep, "SCRAddSoftwareType");
_LIT(KScrDeleteSoftwareTypeStep, "SCRDeleteSoftwareType");

class CScrAddSoftwareTypeStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR AddSoftwareTypeL interface
 */
	{
public:
	CScrAddSoftwareTypeStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();

private:
	RScrAccessor iScrAccessor;
	};

class CScrDeleteSoftwareTypeStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR DeleteSoftwareTypeL interface
 */
	{
public:
	CScrDeleteSoftwareTypeStep(CScrTestServer& aParent);
	
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();	
	
private:
	RScrAccessor iScrAccessor;
	};
	
#endif /* PLUGINMANAGEMENTSTEPS_H */
