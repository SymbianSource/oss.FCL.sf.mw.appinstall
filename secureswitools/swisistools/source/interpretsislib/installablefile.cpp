/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#pragma warning (disable: 4786)

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <iostream>

#include "installablefile.h"
#include "sisfiledata.h"
#include "sisfiledescription.h"
#include "sisstring.h"
#include "is_utils.h"
#include "logger.h"
#include "stringutils.h"
#include "errors.h"


// SA SIS files
InstallableFile::InstallableFile(const CSISFileDescription& aFdes, const CSISFileData* aFdata, const std::wstring aDrivePath, int aInstallingDrive)
: isExecutable(aFdata->IsExecutable()),
  isExe(aFdata->IsExe()),
  iSid(aFdata->GetSid()),
  isStub(false),
  iFileDescription(&aFdes),
  iFileData(aFdata),
  iTargetFile(aFdes.Target().GetString()),
  iLocalTargetFile(aFdes.Target().GetString())
{
	// Transforming the file path into lower case to maintain consistency
	// between Windows and Linux as Linux path is case-sensitive.
	std::transform(iTargetFile.begin(), iTargetFile.end(), iTargetFile.begin(), tolower);
	std::transform(iLocalTargetFile.begin(), iLocalTargetFile.end(), iLocalTargetFile.begin(), tolower);

	// Update the installing file with the actual target drive letter
	ChangeTargetDrive(aDrivePath, aInstallingDrive);

	// warn the user if they are using a winscw emulator binary
	if (aFdata->IsEmulatorExecutable())
			LWARN(iTargetFile.c_str() << L" is an emulator binary!");
}

// PA SIS files		
InstallableFile::InstallableFile(const CSISFileDescription& aFdes, const std::wstring aDrivePath,
								 int aInstallingDrive)
: isExecutable(false),
  isExe(false),
  iSid(0),
  iFileDescription(&aFdes),
  iFileData(0),
  isStub(true),
  iTargetFile(aFdes.Target().GetString()),
  iLocalTargetFile(aFdes.Target().GetString())
  {
	// Transforming the file path into lower case to maintain consistency
	// between Windows and Linux as Linux path is case-sensitive.
	std::transform(iTargetFile.begin(), iTargetFile.end(), iTargetFile.begin(), tolower);
	std::transform(iLocalTargetFile.begin(), iLocalTargetFile.end(), iLocalTargetFile.begin(), tolower);

	// Update the installing file with the actual target drive letter
	ChangeTargetDrive(aDrivePath, aInstallingDrive);

		// retrieve the file attributes e.g. exe, dll, SID etc.
	const bool fileExists = FileExists( iLocalTargetFile );
	
	if  ( fileExists )
		{
		SBinarySecurityInfo info;
		
        const TInt err = ReadSecurityInfo( info, iLocalTargetFile );
        //
		if (!err)
			{
			int fileType = GetFileType(iLocalTargetFile);
			if(fileType & EFileExe)
				{
				isExe = true;
				isExecutable = true;
				iSid = info.iSecureId;
				
				if(fileType & EFileEmulatorExe)
					LWARN(iTargetFile.c_str() << L" is an emulator binary!");
				}
			else if (fileType & EFileDll)
				{
				isExecutable = true;
				}
			}
		}

	// files to be removed on un-installation (FN) should not
	// be treated as stub files
	if (aFdes.Operation() == CSISFileDescription::EOpNull)
		isStub = false;
	}

InstallableFile::~InstallableFile()
{
	delete iFileData;
}


void InstallableFile::ChangeTargetDrive(const std::wstring aDrivePath, int aInstallingDrive)
{
	// get the local path
	ConvertToLocalPath(iLocalTargetFile,aDrivePath);
#ifdef __TOOLS2_LINUX__
	ConvertToForwardSlash(iTargetFile);
#endif

	// change the drive letter
	if (StringUtils::StartsWithDrive(iTargetFile))
	{
		std::wstring target(iTargetFile);
		int targetFileDrive = tolower(target[0]);
		if (targetFileDrive != aInstallingDrive)
		{
			iTargetFile[0] = aInstallingDrive;
			LINFO(L"Disregarding drive selection. Installing "
				<< target.c_str() << L" to " << iTargetFile.c_str());
		}
	}
	else
	{
		std::wstring error = L"Invalid target file ";
		error.append(iTargetFile.c_str());
		throw InterpretSisError(error, INVALID_SIS);
	}
}


struct CollectSids 
{
	CollectSids(std::vector<TUint32>& aSids) 
		: iSids(aSids) {}

	void operator()(const InstallableFile* v)
	{
		if (v->IsExe())
		{
			iSids.push_back(v->Sid());
		}
	}

	std::vector<TUint32>&    iSids;
};

void GetSidsFromInstallable(const InstallableFiles& aFiles, 
			 std::vector<TUint32>& aSids)
{
	std::for_each(
		aFiles.begin(), 
		aFiles.end(),
		CollectSids(aSids));
}

void FreeInstallableFiles(InstallableFiles& aFiles)
{
	for( InstallableFiles::iterator curr = aFiles.begin(); curr != aFiles.end(); ++curr )
	{	
		delete *curr;				
	}
	aFiles.clear();
}
