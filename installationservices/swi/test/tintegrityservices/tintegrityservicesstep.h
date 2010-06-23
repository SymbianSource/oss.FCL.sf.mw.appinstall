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
 @file
*/
#ifndef __TINTEGRITYSERVICESSTEP_H__
#define __TINTEGRITYSERVICESSTEP_H__
#include <test/testexecutestepbase.h>
#include "tintegrityservicesclientserver.h"

namespace Swi
{
namespace Test
{
// Step names
_LIT(KInstall, "Install");
_LIT(KRecover, "Recover");
_LIT(KCleanup, "Cleanup");
_LIT(KOOM, "OOM");
_LIT(KCheck, "Check");

class CDummyIntegrityServices;

// Base class for all steps
class CIntegrityServicesStepBase : public CTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:
	void ReadFailureSettings();

protected:
	// Extension of config parser to allow you to get array of descriptors
	void GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray);

	TBool CheckFilesL();
	void doInstallL();
	void doRecoverL();
	void doCleanupL();

protected:
	RIntegrityServicesServerSession iIntegritySession;
	};

// Step which starts installation
class CInstallStep : public CIntegrityServicesStepBase
	{
public:
	CInstallStep();
	virtual TVerdict doTestStepL();
	};

// Step which recovers a previous failed installation
class CRecoverStep : public CIntegrityServicesStepBase
	{
public:
	CRecoverStep();
	virtual TVerdict doTestStepL();
	};

// Step which cleans up files between test cases
class CCleanupStep : public CIntegrityServicesStepBase
	{
public:
	CCleanupStep();
	virtual TVerdict doTestStepL();
	};

// OOM test step
class COOMStep : public CIntegrityServicesStepBase
	{
public:
	COOMStep();
	virtual TVerdict doTestStepL();
	};
	
// Step which checks for files
class CCheckStep : public CIntegrityServicesStepBase
	{
public:
	CCheckStep();
	virtual TVerdict doTestStepL();
	};

} //namespace
} //namespace
#endif
