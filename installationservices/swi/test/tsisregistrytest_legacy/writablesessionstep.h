/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __WRITABLESESSIONSTEP_H__
#define __WRITABLESESSIONSTEP_H__

#include "tsisregistryteststep.h"
#include "sisregistrywritablesession.h"
#include "sisregistrywritableentry.h"

using namespace Swi;

class CSisRegistryWritableSessionStep : public CSisRegistryTestStepBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:
	Swi::RSisRegistryWritableSession iSisRegistry;
	};

class CWritableEntryMethodsStep : public CSisRegistryWritableSessionStep 
	{
public:
	CWritableEntryMethodsStep();
	void PrintWritableEntryContents(Swi::RSisRegistryWritableEntry &aEntry);
	virtual TVerdict doTestStepL();
	};	

class CPoliceSidsStep : public CSisRegistryWritableSessionStep 
	{
public:
	CPoliceSidsStep();
	virtual TVerdict doTestStepL();
	};	
			
class CEntryDeleteStep : public CSisRegistryWritableSessionStep 
	{
public:
	CEntryDeleteStep();
	virtual TVerdict doTestStepL();
	};

#endif
