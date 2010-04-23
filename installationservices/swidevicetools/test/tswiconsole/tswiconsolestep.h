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


#ifndef __TSWICONSOLE_STEP_H__
#define __TSWICONSOLE_STEP_H__

// User includes
#include "tswiconsolestepbase.h"

// Constants and literals
_LIT(KTSwiConsoleStep, "TestSwiConsole");

// Class declaration
class CTSwiConsoleStep : public CTSwiConsoleStepBase
	{
public:
	CTSwiConsoleStep();
	TVerdict doTestStepL();
	};



#endif //__TSWICONSOLE_STEP_H__
