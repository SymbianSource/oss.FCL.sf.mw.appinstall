/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file provides implementation for CTSwiConsoleStep & CTSwiConsoleStepBase
*
*/


#ifndef __TSWICONSOLE_STEP_BASE_H__
#define __TSWICONSOLE_STEP_BASE_H__

#include <test/testexecutestepbase.h>

class CTSwiConsoleStepBase : public CTestStep
	{
protected:
	CTSwiConsoleStepBase();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL() = 0;
protected:
	RFs iFs;
	};

#endif //__TSWICONSOLE_STEP_BASE_H__
