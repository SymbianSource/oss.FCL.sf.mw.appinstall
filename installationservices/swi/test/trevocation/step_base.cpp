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


#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include <f32file.h>

_LIT(KMetaDataFileName, "\\tswi\\tRevocation\\metadata%d");
_LIT(KBackupFileName, "\\tswi\\tRevocation\\backup%d.%d");

CTRevocationStepBase::~CTRevocationStepBase()
	{
	iFs.Close();
	}

CTRevocationStepBase::CTRevocationStepBase()
	{
	}

TVerdict CTRevocationStepBase::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTRevocationStepBase::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TBool CTRevocationStepBase::Exists(const TPtrC aFileName)
	{
	TEntry entry;
	TInt err = iFs.Entry(aFileName, entry);
	//INFO_PRINTF3(_L("Entry of %S returned %d"), &aFileName, err);
	return err == KErrNone ? ETrue : EFalse;
	}

void CTRevocationStepBase::CopyFileL(TPtrC aSourceFileName, TPtrC aDestFileName)
	{
	INFO_PRINTF3(_L("Copy %S to %S"), &aSourceFileName, &aDestFileName);
	RFile infile;
	RFile outfile;
	User::LeaveIfError(infile.Open(iFs, aSourceFileName, EFileRead));
	CleanupClosePushL(infile);
	if (KErrNone != outfile.Create(iFs, aDestFileName, EFileWrite))
		{
		User::LeaveIfError(outfile.Open(iFs, aDestFileName, EFileWrite));
		}
		
	CleanupClosePushL(outfile);
	
	HBufC8* buffer = HBufC8::NewLC(1024);
	TPtr8 ptr = buffer->Des();
	
	User::LeaveIfError(infile.Read(ptr));
	while(ptr.Length() != 0)
		{
		User::LeaveIfError(outfile.Write(*buffer));
		User::LeaveIfError(infile.Read(ptr));
		}
	
	CleanupStack::PopAndDestroy(3, &infile); // buffer, outfile
	}

TBool CTRevocationStepBase::NextPackage()
	{
	iCurrentPackageNumber++;
	iCurrentFileNumber=0;
	if (PackageUID().iUid != 0)
		{
		//INFO_PRINTF2(_L("Package %d"), iCurrentPackageNumber);
		return ETrue;
		}
	return EFalse;
	}

TUid CTRevocationStepBase::PackageUID()
	{
	TUid uid;
	TInt tmp;
	TBuf<20> token;
	token.Format(_L("UID%d"), iCurrentPackageNumber);
	if(GetHexFromConfig(ConfigSection(), token, tmp) == EFalse)
		{
		tmp = 0;
		}
	uid.iUid = tmp;
	return uid;
	}

TBool CTRevocationStepBase::NextFile()
	{
	iCurrentFileNumber++;
	if (InstalledFileName().Length() != 0)
		{
		//INFO_PRINTF2(_L("File %d"), iCurrentFileNumber);
		return ETrue;
		}
	return EFalse;
	}

TPtrC CTRevocationStepBase::SISFileNameL()
	{
	TBuf<20> token;
	token.Format(_L("SIS%d"), iCurrentPackageNumber);
	TPtrC sisfile;
	if (GetStringFromConfig(ConfigSection(), token, sisfile) == EFalse)
		{
		User::Leave(KErrNotFound);
		}
	return sisfile;
	}

TPtrC CTRevocationStepBase::WrongInstalledFileName()
	{
	TBuf<20> token;
	token.Format(_L("WrongFile%d.%d"), iCurrentPackageNumber, iCurrentFileNumber);
	TPtrC filename;
	GetStringFromConfig(ConfigSection(), token, filename);
	return filename;
	}

TPtrC CTRevocationStepBase::InstalledFileName()
	{
	TBuf<20> token;
	token.Format(_L("FILE%d.%d"), iCurrentPackageNumber, iCurrentFileNumber);
	TPtrC filename;
	GetStringFromConfig(ConfigSection(), token, filename);
	return filename;
	}


TFileName CTRevocationStepBase::BackupFileName()
	{
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KBackupFileName);
	TFileName backupFileName;
	backupFileName.Format(fileName, PackageUID().iUid, iCurrentFileNumber);
	return backupFileName;
	}

void CTRevocationStepBase::WriteMetaDataL(HBufC8* aMetaData)
	{
	RFile metadatafile;
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KMetaDataFileName);
	TBuf<256> metadataFileName;
	metadataFileName.Format(fileName, PackageUID().iUid);

	User::LeaveIfError(metadatafile.Replace(iFs, metadataFileName, EFileWrite));
	
	CleanupClosePushL(metadatafile);
	User::LeaveIfError(metadatafile.Write(*aMetaData));
	CleanupStack::PopAndDestroy(&metadatafile);
	}

HBufC8*	CTRevocationStepBase::ReadMetaDataL()
	{
	RFile metadatafile;
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KMetaDataFileName);
	TBuf<256> metadataFileName;
	metadataFileName.Format(fileName, PackageUID().iUid);

	User::LeaveIfError(metadatafile.Open(iFs, metadataFileName, EFileRead));
	CleanupClosePushL(metadatafile);
	
	TInt length(0);
	metadatafile.Seek(ESeekEnd, length);
	TInt dummy(0);
	metadatafile.Seek(ESeekStart, dummy);
	
	HBufC8* metadata = HBufC8::NewL(length);
	TPtr8 ptr = metadata->Des();
	TInt err = metadatafile.Read(ptr);
	if (err != KErrNone)
		{
		delete metadata;
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&metadatafile);
	return metadata;
	}
