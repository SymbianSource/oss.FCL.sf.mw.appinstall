/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "filldrive.h"
#include <test/testexecutelog.h>

const TInt64 KDefaultFileSize = 0;

// This test step creates a file either of a specific size or which
// leaves a specified amount of free space.
// Note that the maximum file size is KMaxTInt bytes.

TVerdict CFillDrive::doTestStepL()
	{
	_LIT(KDefaultFileName, "\\TempSpaceHog");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TFileName bigFile(sysDriveName);
	bigFile.Append(KDefaultFileName);
	
	TPtrC name;
	_LIT(KFileNameKey, "filename");
	
	if (GetStringFromConfig(ConfigSection(), KFileNameKey, name))
		{
		TParsePtrC parse(name);
		if (!parse.DrivePresent())
			{
			bigFile.Copy(sysDriveName);
			bigFile.Append(parse.Path());
			bigFile.Append(parse.NameAndExt());
			}
		else
			{
			bigFile.Copy(name);
			}
		}

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	INFO_PRINTF2(_L("File to create: %S"), &bigFile);

	// Delete file if it already exists, clearing hidden or read only attributes
	// first.
	TInt err = fs.SetAtt(bigFile, KEntryAttNormal, KEntryAttHidden | KEntryAttReadOnly );
	if(err == KErrNone)
		{
		INFO_PRINTF2(_L("Deleting existing file %S"), &bigFile);
		User::LeaveIfError(fs.Delete(bigFile));
		}
	else if (err != KErrNotFound)
		User::Leave(err);
	

	// Find free space on drive.
	TDriveUnit drive(bigFile);
	TInt driveNumber(drive);
	TVolumeInfo volInfo;

	User::LeaveIfError(fs.Volume(volInfo, driveNumber));

	TInt64 volSpace = volInfo.iFree; // bytes
	TPtrC driveName(drive.Name());
	INFO_PRINTF3(_L("Free space on drive %S %Ld bytes."), &driveName,
					volSpace);

	TInt64 fileSize(KDefaultFileSize);


	// Determine file size needed based on specified config and free space.
	
	_LIT(KSpaceToLeave, "spacetoleave");
	_LIT(KFileSize, "filesize");
	TInt num(0);
	
	if (GetIntFromConfig(ConfigSection(), KSpaceToLeave, num) && num >= 0)
		{
		// If the free space on the drive is already less than requested
		// generate 0 length file.
		fileSize = (num < volSpace) ? volSpace - num : 0;
		}
	else if (GetIntFromConfig(ConfigSection(), KFileSize, num) && num >= 0)
		{
		// If the requested size is larger than the amount of free space
		// limit file size to available size.
		fileSize = (num < volSpace) ? num : volSpace;
		}
	
	TVerdict testResult(EPass);
	if (fileSize <= KMaxTInt)
		{
		INFO_PRINTF3(_L("Creating file %S with size %LD bytes."), &bigFile,
					fileSize);
		
		RFile file;
		User::LeaveIfError(file.Create(fs, bigFile, EFileRead|EFileWrite));
		CleanupClosePushL(file);
		
		User::LeaveIfError(file.SetSize(fileSize));
		CleanupStack::PopAndDestroy(&file);
		}
	else
		{
		// File size can't be more than KMaxTInt bytes.
		ERR_PRINTF2(_L("Required file size %LD bytes is too large."),
					fileSize);
		testResult = ETestSuiteError;
		}

    CleanupStack::PopAndDestroy(&fs);
    SetTestStepResult(testResult);
	return testResult;
	}
