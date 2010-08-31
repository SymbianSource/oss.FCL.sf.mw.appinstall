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
 @test
 @internalComponent
*/
#ifndef __TSTSTESTSUITESTEP_H__
#define __TSTSTESTSUITESTEP_H__
#include <test/testexecutestepbase.h>
#include "tststestsuitestepcommon.h"
#include <scs/oomteststep.h>

class CStsTestServer;

class CStsBaseTestStep : public COomTestStep
	{
	public:
		CStsBaseTestStep(CStsTestServer &aStsTestServer);
	
	protected:
		CStsTestServer& Server();
		//ini access functions
		TInt GetIntFromConfigL(const TDesC& aName);
		TInt GetTransactionSlotFromConfigL();
		TInt GetTransactionSlotToReuseTransactionIdFromConfigL();
		TInt GetTransactionIdFromConfigL();
		TPtrC GetStringFromConfigL(const TDesC& aName);
		TPtrC GetTargetFilePathFromConfigL();
		
		//file handling
		void WriteToFileL(RFile &aFile, TDesC& aString);
		TBool FileExistsL(const TDesC &aFileName);
		void DeleteFileL(const TDesC &aFileName);
		void CopyFileL(const TDesC& aSourceFile, const TDesC& aDestinationFile);
		void RmDirL(const TDesC& aFileName);
		void MkDirAllL(const TDesC& aFileName);
		void DeleteL(const TDesC& aFileName);
		HBufC* CreateDataForNewFileCreatedByStsServerLC(const TDesC& aTestStepName, TNewFileType aType);
		void SetReadOnlyL(const TDesC& aFileName, TInt aSetReadOnly);
	protected:
		CStsTestServer &iStsTestServer;
	};


class CCreateTransactionStep : public CStsBaseTestStep
	{
	public:
		CCreateTransactionStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class COpenTransactionStep : public CStsBaseTestStep
	{
	public:
		COpenTransactionStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CCloseTransactionStep : public CStsBaseTestStep
	{
	public:
		CCloseTransactionStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};
	
class CRegisterNewFileStep : public CStsBaseTestStep
	{
	public:
		CRegisterNewFileStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CCreateNewFileStep : public CStsBaseTestStep
	{
	public:
		CCreateNewFileStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CRemoveStep : public CStsBaseTestStep
	{
	public:
		CRemoveStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CRegisterTemporaryStep : public CStsBaseTestStep
	{
	public:
		CRegisterTemporaryStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CCreateTemporaryStep : public CStsBaseTestStep
	{
	public:
		CCreateTemporaryStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class COverwriteStep : public CStsBaseTestStep
	{
	public:
		COverwriteStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CCommitStep : public CStsBaseTestStep
	{
	public:
		CCommitStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CRollbackStep : public CStsBaseTestStep
	{
	public:
		CRollbackStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CRollbackAllPendingStep : public CStsBaseTestStep
	{
	public:
		CRollbackAllPendingStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};

class CCheckFilesStep : public CStsBaseTestStep
	{
	public:
		CCheckFilesStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	private:
		void GetFileNamesForCheckL(RArray<TPtrC>& aFileNumExist,RArray<TPtrC>& aFileNumNonExist);
		void ExtractFileNameL(TInt aEntries, const TDesC& aEntryBase, RArray<TPtrC>& aFileArray);
		void CheckIfFilesExistL(const RArray<TPtrC>& aFileArray);
		void CheckIfFilesNotExistL(const RArray<TPtrC>& aFileArray);
	};

class CheckFileModeChangeStep : public CStsBaseTestStep
	{
	public:
		CheckFileModeChangeStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};


class CFileOperationsStep : public CStsBaseTestStep
	{
	public:
		CFileOperationsStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	private:
		void ProcessFileOperationsL();
		TPtrC GetNextCommandLineParameterL(const TDesC& aLine, TInt& aNextAvailablePos);
	};

class CFileModeTestStep : public CStsBaseTestStep
	{
	public:
		CFileModeTestStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	private:
		void TestCase1L();
		void TestCase2L();
		void TestCase3L();
	};

class CCreateLongFileNameTestStep : public CStsBaseTestStep
	{
	public:
		CCreateLongFileNameTestStep(CStsTestServer &aStsTestServer);
	protected:
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepPostambleL();
		virtual void ImplTestStepL();
	};
#endif

