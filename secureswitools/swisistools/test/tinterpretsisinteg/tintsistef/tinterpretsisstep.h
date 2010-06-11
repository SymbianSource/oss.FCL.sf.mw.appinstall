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
*
*/




/**
 @file tinterpretsisstep.h
*/
#ifndef __TINTERPRETSISSTEP_H__
#define __TINTERPRETSISSTEP_H__
#include <test/testexecuteserverbase.h>
#include <swi/sisregistrysession.h>

class CInterpretsisStep : public CTestStep
	{
public:
	CInterpretsisStep();
	~CInterpretsisStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	};

_LIT(KInterpretsisStep, "InterpretsisStep");

#endif
