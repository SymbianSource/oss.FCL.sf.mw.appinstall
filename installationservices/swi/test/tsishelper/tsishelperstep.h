/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file tsishelperstep.h
*/
#ifndef __TSISHELPER_STEP_H__
#define __TSISHELPER_STEP_H__
#include <test/testexecutestepbase.h>
#include "tsishelperserver.h"
#include "sishelperclient.h"

class CTSISHelperStepBase : public CTestStep
	{
protected:
	CTSISHelperStepBase();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL() = 0;
	TInt startSisHelper(Swi::TSisHelperStartParams& aParams);
private:
	static TInt sisHelperThreadFunction(TAny *aPtr);
protected:
	RFs iFs;
	};

class CTSISHelperStepController : public CTSISHelperStepBase
	{
public:
	CTSISHelperStepController();
	TVerdict doTestStepL();
	};

class CTSISHelperStepData : public CTSISHelperStepBase
	{
public:
	CTSISHelperStepData();
	TVerdict doTestStepL();
	};

_LIT(KTSISHelperStepController,"Controller");
_LIT(KTSISHelperStepData,"Data");


#endif /* ndef __TSISHELPER_STEP_H__ */
