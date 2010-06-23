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

#include "tintegrityservicescommon.h"
#include "tststestsuitestep.h"
#include "tintegrityservicesclientserver.h"
#include "journal.h"
#include "integrityservices.h"


// Base class for all steps
class CIntegrityServicesStepBase : public CStsBaseTestStep
	{
public:
	CIntegrityServicesStepBase(CStsTestServer &aStsTestServer);
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();

private:
	void ReadFailureSettingsL();

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
	CInstallStep(CStsTestServer &aStsTestServer);
	virtual void ImplTestStepL();
	};

// Step which recovers a previous failed installation
class CRecoverStep : public CIntegrityServicesStepBase
	{
public:
	CRecoverStep(CStsTestServer &aStsTestServer);
	virtual void ImplTestStepL();
	};

// Step which cleans up files between test cases
class CCleanupStep : public CIntegrityServicesStepBase
	{
public:
	CCleanupStep(CStsTestServer &aStsTestServer);
	virtual void ImplTestStepL();
	};
	
// Step which checks for files
class CCheckStep : public CIntegrityServicesStepBase
	{
public:
	CCheckStep(CStsTestServer &aStsTestServer);
	virtual void ImplTestStepL();
	};

//---------- new unit tests developed as part of the STS framework ----------
union TIntValue
	{
	TInt8  int8;
	TInt16 int16;
	TInt32 int32;
	};

enum TIntMode
	{
	Mode8Bit  =1,
	Mode16Bit =2,
	Mode32Bit =4
	};

_LIT(KEmptyString, "");
_LIT(KDriveLetterC, "c");
_LIT(KDriveLetterE, "e");
_LIT(KKournalFileExt, ".log");

class CTestFile : public CBase
	{
	public:
		CTestFile(RFs& aFs);
		~CTestFile();
		static CTestFile* NewL(RFs& aFs);
		void OpenJournalFileL(const TDesC& aFileName, TFileMode aMode);
		void CloseJournalFile();
		void CreateJournalFileL(const TDesC& aFileName, TFileMode aMode);
		void WriteIntToJournalFileL(TIntValue value, TIntMode mode);
		TIntValue ReadIntFromJournalFileL(TIntMode mode);
		void SeekInJournalFileL(TSeek aSeekMode, TInt &aPos);

	protected:
		RFs&          iFs;
		RFile         journalFile;
	};

class CJournalFileUnitTest : public CStsBaseTestStep
	{
public:
	CJournalFileUnitTest(CStsTestServer &aStsTestServer);
	~CJournalFileUnitTest();
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();
	virtual void ImplTestStepL();
	
protected:
	void OpenJournalFileL(const TDesC& aFileName, TFileMode aMode);
	void CloseJournalFile();
	void CreateJournalFileL(const TDesC& aFileName, TFileMode aMode);
	void WriteIntToJournalFileL(TIntValue value, TIntMode mode);
	TIntValue ReadIntFromJournalFileL(TIntMode mode);
	void SeekInJournalFileL(TSeek aSeekMode, TInt &aPos);
	
protected:
	RFs          		iFs;
	RLoader      		iLoader;
	CTestFile*  	testJournalFile;
	};

class CJournalFileUnitTestScenarioBase : public CStsBaseTestStep
	{
public:
	CJournalFileUnitTestScenarioBase(CStsTestServer &aStsTestServer);
	~CJournalFileUnitTestScenarioBase();
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();
	virtual void ImplTestStepL();

private:
	static void InTestCleanup(TAny* aObject);
	virtual void ImplTestCaseScenarioL()=0; //implements the real test scenario executed in OOM test

protected:
	RFs          		iFs;
	RLoader      		iLoader;
	TPtrC               iJournalFileName;
	HBufC* 				iTargetFileName;
	HBufC*				iBackupFileName;
	TDriveUnit          iJournaldrive;
	TBool				iCleanupNeeded;
	};

class CJournalFileUnitTestAdd : public CJournalFileUnitTestScenarioBase
	{
public:
	CJournalFileUnitTestAdd(CStsTestServer &aStsTestServer);
	virtual void ImplTestCaseScenarioL();
	};

class CJournalFileUnitTestTemp : public CJournalFileUnitTestScenarioBase
	{
public:
	CJournalFileUnitTestTemp(CStsTestServer &aStsTestServer);
	virtual void ImplTestCaseScenarioL();
	};

class CJournalFileUnitTestRemove : public CJournalFileUnitTestScenarioBase
	{
public:
	CJournalFileUnitTestRemove(CStsTestServer &aStsTestServer);
	virtual void ImplTestCaseScenarioL();
	};

class CJournalFileUnitTestRead : public CJournalFileUnitTestScenarioBase
	{
public:
	CJournalFileUnitTestRead(CStsTestServer &aStsTestServer);
	virtual void ImplTestCaseScenarioL();
	};

class CJournalFileUnitTestEvent : public CJournalFileUnitTestScenarioBase
	{
public:
	CJournalFileUnitTestEvent(CStsTestServer &aStsTestServer);
	virtual void ImplTestCaseScenarioL();
	};

class CJournalUnitTest : public CStsBaseTestStep
	{
public:
	CJournalUnitTest(CStsTestServer &aStsTestServer);
	~CJournalUnitTest();
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();
	virtual void ImplTestStepL();

private:
	void TestCase1L();
	
protected:
	RFs          		iFs;
	RLoader      		iLoader;
	CTestFile*  	    iTestFileInterface;
	TInt 				iTrID;
	TPtrC				iJournalPathInIni;
	};


class CJournalUnitTestScenarioBase : public CStsBaseTestStep
	{
public:
	CJournalUnitTestScenarioBase(CStsTestServer &aStsTestServer);
	~CJournalUnitTestScenarioBase();
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();
	virtual void ImplTestStepL();

protected:
	virtual void ScenarioEndL(Usif::CJournal* aJournal)=0;
	static void InTestCleanUp(TAny* aObject);
	
protected:
	RFs          		iFs;
	RLoader      		iLoader;
	TInt 				iTrID;
	TPtrC				iJournalPathInIni;
	HBufC* 				iTargetFileName;
	TDriveUnit          iDriveUnitC;	
	TDriveUnit          iDriveUnitE;
	HBufC* 				iJournalFileName;
	HBufC* 				iDrvFileName;
	HBufC* 				iBackupFileName;
	};

class CJournalUnitTestInstall : public CJournalUnitTestScenarioBase
	{
public:
	CJournalUnitTestInstall(CStsTestServer &aStsTestServer);
protected:	
	virtual void ScenarioEndL(Usif::CJournal* aJournal);
	};

class CJournalUnitTestRollback : public CJournalUnitTestScenarioBase
	{
public:
	CJournalUnitTestRollback(CStsTestServer &aStsTestServer);
protected:	
	virtual void ScenarioEndL(Usif::CJournal* aJournal);
	};

_LIT(KTransactionPath, "\\sys\\install\\integrityservices\\");

class CIntegrityServicesTestScenarioBase : public CStsBaseTestStep
	{
public:
	CIntegrityServicesTestScenarioBase(CStsTestServer &aStsTestServer);
	~CIntegrityServicesTestScenarioBase();
	virtual void ImplTestStepPreambleL();
	virtual void ImplTestStepPostambleL();
	virtual void ImplTestStepL();

protected:
	virtual void ScenarioEndL(Usif::CIntegrityServices* aIntegrServ)=0;
	static void InTestCleanUp(TAny* aObject);
	void NormalizeDirectoryName(TDes& aFileName);
	
protected:
	RFs          		iFs;
	RLoader      		iLoader;
	TInt 				iTrID;
	TPtrC				iTargetPathInIni;
	HBufC* 				iTargetFileName;
	TDriveUnit          iDriveUnitC;	
	TDriveUnit          iDriveUnitE;
	HBufC* 				iBackupFileName;
	HBufC* 				iTransactionBackupDirectoryPath;
	HBufC* 				iJournalFileName;
	HBufC* 				iDrvFileName;
	};

class CIntegrityServicesTestInstall : public CIntegrityServicesTestScenarioBase
	{
public:
	CIntegrityServicesTestInstall(CStsTestServer &aStsTestServer);
protected:	
	virtual void ScenarioEndL(Usif::CIntegrityServices* aIntegrServ);
	};

class CIntegrityServicesTestRollback : public CIntegrityServicesTestScenarioBase
	{
public:
	CIntegrityServicesTestRollback(CStsTestServer &aStsTestServer);
protected:	
	virtual void ScenarioEndL(Usif::CIntegrityServices* aIntegrServ);
	};

#endif
