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


#ifndef T_REVOCATION_STEP_H
#define T_REVOCATION_STEP_H

#include <test/testexecutestepbase.h>
#include "trevocationServer.h"

_LIT(KTStepInstall,					"INSTALL");
_LIT(KTStepUninstall,				"UNINSTALL");
//Following 5 lines will be removed
_LIT(KTStepBackup,					"BACKUP");
_LIT(KTStepRestore,					"RESTORE");
_LIT(KTStepRestoreNoCommit,			"RESTORENOCOMMIT");
_LIT(KTStepRestoreCorruptController,"RESTORECORRUPTCONTROLLER");
_LIT(KTStepRestoreExtraExecutable,	"RESTOREEXTRAEXECUTABLE");

_LIT(KTStepCheckPresent,			"PRESENT");
_LIT(KTStepCheckAbsent,				"ABSENT");
_LIT(KTStepClean,					"CLEAN");
_LIT(KTStepPirCheck,				"PIRCHECK");
_LIT(KTStepSisRegCheck,				"SISREGCHECK");


class CTRevocationStepBase : public CTestStep
	{
protected:
	CTRevocationStepBase();
	~CTRevocationStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	// Iterates through SIS files within a step
	TBool	NextPackage();
	// The UID of the current SIS file
	TUid	PackageUID();
	// The filename of the current SIS file
	TPtrC	SISFileNameL();
	// Iterates through files within a SIS file
	TBool	NextFile();
	TFileName	BackupFileName();
	// The location where the current file is [to be] installed
	TPtrC	InstalledFileName();
	// The location where the current file is not [to be] installed
	TPtrC	WrongInstalledFileName();
	// Utility method
	TBool	Exists(TPtrC aFileName);
	// Utility method
	void	CopyFileL(TPtrC aSourceFileName, TPtrC aDestFileName);
	// Utility method for storing backup metadata
	void	WriteMetaDataL(HBufC8* aMetaData);
	// Utility method for recovering backup metadata
	HBufC8*	ReadMetaDataL();
	
private:
	TInt	iCurrentPackageNumber;
	TInt	iCurrentFileNumber;

	RFs		iFs;
	};

class CTStepInstall : public CTRevocationStepBase
	{
public:
	CTStepInstall();
	virtual TVerdict doTestStepL();
	};

class CTStepUninstall : public CTRevocationStepBase
	{
public:
	CTStepUninstall();
	virtual TVerdict doTestStepL();
	};

class CTStepBackup : public CTRevocationStepBase
	{
public:
	CTStepBackup();
	virtual TVerdict doTestStepL();
	};

class CTStepRestore : public CTRevocationStepBase
	{
public:
	CTStepRestore();
	virtual TVerdict doTestStepL();
	};

class CTStepRestoreNoCommit : public CTRevocationStepBase
	{
public:
	CTStepRestoreNoCommit();
	virtual TVerdict doTestStepL();
	};

class CTStepRestoreCorruptController : public CTRevocationStepBase
	{
public:
	CTStepRestoreCorruptController();
	virtual TVerdict doTestStepL();
	};

class CTStepRestoreExtraExecutable : public CTRevocationStepBase
	{
public:
	CTStepRestoreExtraExecutable();
	virtual TVerdict doTestStepL();
	};

class CTStepCheckPresent : public CTRevocationStepBase
	{
public:
	CTStepCheckPresent();
	virtual TVerdict doTestStepL();
	};

class CTStepCheckAbsent : public CTRevocationStepBase
	{
public:
	CTStepCheckAbsent();
	virtual TVerdict doTestStepL();
	};

class CTStepClean : public CTRevocationStepBase
	{
public:
	CTStepClean();
	virtual TVerdict doTestStepL();
	};
	
class CTStepPirCheck : public CTRevocationStepBase
	{
public:
	CTStepPirCheck();
	virtual TVerdict doTestStepL();
	};
	
class CTStepSisRegCheck : public CTRevocationStepBase
	{
public:
	CTStepSisRegCheck();
	virtual TVerdict doTestStepL();
	//Utility method for converting the ValidationValue to a string
	TBufC<40> GetEnumValidationValue (TInt input);
	//Utility method for converting the RevocationValue to a string
	TBufC<40> GetEnumRevocationValue (TInt input);
	};

#endif	/* T_REVOCATION_STEP_H */
