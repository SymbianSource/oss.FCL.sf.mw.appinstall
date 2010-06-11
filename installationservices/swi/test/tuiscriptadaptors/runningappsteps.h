/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __RUNNINGAPPSTEPS_H__
#define __RUNNINGAPPSTEPS_H__

#include <test/testexecutestepbase.h>

_LIT(KSwisShutdownApp, "ShutdownApp");
_LIT(KSwisFindRunningExe, "FindRunningExe");
_LIT(KEnsureSwisNotRunningStep, "SwisNotRunning");

class CShutdownApp : public CTestStep
	{
public:
	TVerdict doTestStepL();
	};

class CFindRunningExe : public CTestStep
	{
public:
	TVerdict doTestStepL();
	};
	
class CEnsureSwisNotRunning : public CTestStep
	{
public:
	TVerdict doTestStepL();
	};

#endif /* __RUNNINGAPPSTEPS_H__ */
