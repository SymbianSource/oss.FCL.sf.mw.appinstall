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
*
*/


/**
 @file
 @internalTechnology 
*/

#if (!defined TSIFCOMMONUNITTESTSTEP_H)
#define TSIFCOMMONUNITTESTSTEP_H
#include <test/testexecutestepbase.h>
#include "tsifsuitestepbase.h"

namespace Usif
	{

	class CSifCommonUnitTestStep : public CSifSuiteStepBase
		{
	public:
		virtual ~CSifCommonUnitTestStep();
		CSifCommonUnitTestStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();
		virtual void ImplTestStepPostambleL();

	private:
		void TestComponentInfoL();
		void TestOpaqueNamedParamsL();
		};

	_LIT(KSifCommonUnitTestStep,"SifCommonUnitTestStep");

	} // namespace Sif

#endif // TSIFCOMMONUNITTESTSTEP_H
