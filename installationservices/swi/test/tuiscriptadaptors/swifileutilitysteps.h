/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SWIFILEUTILITYSTEPS_H
#define SWIFILEUTILITYSTEPS_H

#include <test/testexecutestepbase.h>
#include "testutilclientswi.h"
#include <test/testexecutelog.h>

_LIT(KSwiCopyFilesStep, "SwiCopyFilesStep");
_LIT(KSwiDeleteFilesStep, "SwiDeleteFilesStep");

class CSwiCopyFilesStep : public CTestStep
	{
public:
	CSwiCopyFilesStep();
	TVerdict doTestStepL();

	};

class CSwiDeleteFilesStep : public CTestStep
	{
public:
	CSwiDeleteFilesStep();
	TVerdict doTestStepL();

	};



#endif //SWIFILEUTILITYSTEPS_H
