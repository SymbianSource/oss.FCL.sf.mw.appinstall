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

#ifndef SIFSTEPS_H
#define SIFSTEPS_H

#include "tscrstep.h"
class CScrTestServer;

_LIT(KScrGetPluginStep, "SCRGetPlugin");
_LIT(KScrSetScomoStateStep, "SCRSetScomoState");

class CScrGetPluginStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR GetPluginUidL interfaces
 */
	{
public:
	CScrGetPluginStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();

private:
	TUid GetPluginUidFromConfigL();
	TBool GetMimeTypeFromConfigL(TPtrC& aMimeType);
	};	

class CScrSetScomoStateStep : public CScrTestStep
/**
 	TEF test step which exercises the SCR SetScomoStateL interface
 */
	{
public:
	CScrSetScomoStateStep(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();			
	};

#endif /* SIFSTEPS_H */
