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
 @file INSTALLABLEFILE.H
 @internalComponent
 @released
*/
#ifndef	INSTALLABLE_FILE_H
#define	INSTALLABLE_FILE_H

#include <vector> 
#include <string>

#include "symbiantypes.h"


class CSISFileData;
class CSISFileDescription;


struct InstallableFile
{

public:
	// constructor for non stub files
	InstallableFile(const CSISFileDescription&, const CSISFileData*, const std::wstring aDrivePath, int aInstallingDrive);
	
	// constructor for stub files
	InstallableFile(const CSISFileDescription&, const std::wstring aDrivePath, int aInstallingDrive);
	~InstallableFile();

public:
	bool IsExecutable() const { return isExecutable;};
	bool IsExe() const { return isExe;};
	TUint32 Sid() const{ return iSid;};
	bool IsStub() const{ return isStub;};
	const CSISFileDescription* FileDescription() const{ return iFileDescription;};
	const CSISFileData* FileData() const{ return iFileData;};
	const std::wstring& GetTarget() const { return iTargetFile; };
	const std::wstring& GetLocalTarget() const { return iLocalTargetFile; };

private:
	void ChangeTargetDrive(const std::wstring aDrivePath, int aInstallingDrive);

private:
	bool isExecutable;
	bool isExe;
	TUint32 iSid;
	bool isStub;
	const CSISFileDescription* iFileDescription;	
	// this  can be NULL in case of stub
	const CSISFileData* iFileData;
	// Full file name to the device location
	std::wstring iTargetFile;
	// Full file name to the PC location
	std::wstring iLocalTargetFile;
};

typedef std::vector< InstallableFile* > InstallableFiles;

void GetSidsFromInstallable(const InstallableFiles& aFiles, 
							std::vector<TUint32>& aSids);

void FreeInstallableFiles(InstallableFiles& aFiles);
#endif	/* INSTALLABLE_FILE_H */
