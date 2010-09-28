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

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

#include "dbhelper.h" 
#include "dirparse.h"
#ifndef __TOOLS2_LINUX__
#include <windows.h>
#endif 
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <exception>
#include <sstream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>

// SisX Library
#include "sisdependency.h"
#include "sisversionrange.h"
#include "sisversion.h"
#include "sisprerequisites.h"
#include "sisfiledescription.h"
#include "sisstring.h"
#include "sishash.h"

// User includes
#include "stringutils.h"
#include "parameterlist.h"
#include "rommanager.h"
#include "configmanager.h"
#include "errors.h"
#include "is_utils.h"
#include "installer.h"
#include "sisregistryobject.h"
#include "adornedutilities.h"

// Constants
const char KRomDriveLetter = 'z';
const TUint32 KSwiDaemonUid = 0x10202DCE;
#ifndef __TOOLS2_LINUX__
const std::wstring KSysBinPath = L"\\sys\\bin\\";
#else
const std::wstring KSysBinPath = L"/sys/bin/";
#endif

TInt CheckWildCard(TInt aVal, TInt aReplace)
{
	return
		aVal == -1
		? aReplace
		: aVal;
}

bool InRange(const Version& aVersion, const CSISVersionRange& aRange)
{
	Version from(
		CheckWildCard(aRange.FromVersion().Major(), aVersion.GetMajor()),
		CheckWildCard(aRange.FromVersion().Minor(), aVersion.GetMinor()),
		CheckWildCard(aRange.FromVersion().Build(), aVersion.GetBuild()));

	if(!aRange.ToVersion().WasteOfSpace())
	{
		Version to(
			CheckWildCard(aRange.ToVersion().Major(), aVersion.GetMajor()),
			CheckWildCard(aRange.ToVersion().Minor(), aVersion.GetMinor()),
			CheckWildCard(aRange.ToVersion().Build(), aVersion.GetBuild()));

		if (aVersion == from ||
			aVersion == to)
		{
			return true;
		}

		return (aVersion > from && aVersion < to);
	}
	else
	{
		return (aVersion >= from);
	}
}


Installer::Installer( SisRegistry& aReg, const CParameterList& aParamList, RomManager& aRomManager, ConfigManager& aConfigManager )
:   iParamList(aParamList), iRegistry(aReg), iRomManager( aRomManager ), iConfigManager( aConfigManager ),
    iExpressionEnvironment( NULL ), iExpressionEvaluator( NULL )
{
}

TInt Installer::Install(const CParameterList::SISFileList& aList)
{
	TInt result = 0;
	CParameterList::SISFileList sisFiles;

	// Get the SIS files
	for (CParameterList::SISFileList::const_iterator curr = aList.begin(); curr != aList.end(); ++curr)
	{
		std::wstring shortName(curr->iFileName);

		// Ensure formatting is correct
		if ( StringUtils::IsLastCharacter( shortName, KDirectorySeparator[ 0 ] ) ||
			 #ifndef __TOOLS2_LINUX__
             StringUtils::IsLastCharacter( shortName, L'/' ) )
             #else
			 StringUtils::IsLastCharacter( shortName, L'\\' ) )
			 #endif
		{
			shortName.erase(shortName.length()-1,1);
		}

		if ( IsDirectory(shortName) )
		{
			std::list<std::wstring> contents;
			GetDirContents(shortName, contents);

			for (std::list<std::wstring>::iterator currStr = contents.begin(); currStr != contents.end(); ++currStr)
			{
				if ((currStr->find(L".SIS",0) != std::wstring::npos)
					|| (currStr->find(L".sis",0) != std::wstring::npos))
				{
					InstallSISFile sisFileName(shortName + KDirectorySeparator + *currStr, 
												curr->iTargetDrive, curr->iGenerateStub, 
												curr->iNonRemovable, curr->iReadOnly, curr->iNotRegister, curr->iSUFlag);

					sisFiles.push_back(sisFileName);
				}
			}

			if (sisFiles.empty())
			{
				LWARN(L"No .SIS files found in directory" << shortName);
			}
		}
		else if ((shortName.find(L".SIS",0) != std::wstring::npos)
				|| (shortName.find(L".sis",0) != std::wstring::npos))
		{
			InstallSISFile sisFileName(shortName, curr->iTargetDrive, curr->iGenerateStub, 
										curr->iNonRemovable, curr->iReadOnly, curr->iNotRegister, curr->iSUFlag);

			sisFiles.push_back(sisFileName);
		}
		else
		{
			LWARN(L"Invalid SIS file: " << shortName);
		}
	}

	bool installed = false;

	do
		{
		installed = false;
		iMissing.clear();
		CParameterList::SISFileList::iterator curr = sisFiles.begin();

		while (curr != sisFiles.end())
			{
			// install it
			try 
				{
				LINFO(L"*** Installing " << (curr->iFileName) << L" ***");

				SanitiseSISFileAttributes(*curr);

				result = Install(*curr);

				if ( result == SUCCESS)
					{
                    LINFO(L"");
					LINFO(L"*** Installed " << (curr->iFileName) << L" ***");

					installed =  true;
					curr = sisFiles.erase(curr);
					}
				else
					{
					LERROR(L"Failed to install " << (curr->iFileName));
					++curr;
					}
				}
			catch(CSISException e)
				{
				// catch problems with individual sis files
				LERROR(L"Unable to install " << (curr->iFileName));
				LINFO(e.widewhat());
				curr = sisFiles.erase(curr);
				if (result == 0)
					result =  FILE_ERROR;
				}
			catch  (InterpretSisError& e)
				{
				LERROR(L"Failed installing " << (curr->iFileName));
				LERROR(L"\t" << string2wstring(e.what()));
				curr = sisFiles.erase(curr);
				
				if (result == 0)
					result =  e.GetErrorCode();
				}
			}
		}
	while (installed);

	if (result == 0 && !iMissing.empty())
		result = MISSING_DEPENDENCY;

	if(iParamList.RegistryVersionExists())
		{
	// Backup.lst regenerated at SisRegistry startup
	iRegistry.RemoveBackupLst();
		}
	return result;

}


void Installer::SanitiseSISFileAttributes(const InstallSISFile& aInstallSISFile)
{
	SisFile file(aInstallSISFile.iFileName);

	int targetDrive = aInstallSISFile.iTargetDrive;

	if (!iConfigManager.IsTargetDrivePresent(targetDrive))
	{
		std::wstring shortName(aInstallSISFile.iFileName);

		throw InterpretSisError(L"The installing target drive for " + shortName +
				L" is not defined", CMDLINE_ERROR);
	}

	if (aInstallSISFile.iGenerateStub && !iConfigManager.IsTargetDriveExt(targetDrive))
	{
		std::wstring shortName(aInstallSISFile.iFileName);

		throw InterpretSisError(L"The stub SIS file for " + shortName +
				L" cannot be created to an internal drive", CMDLINE_ERROR);
	}

	if (aInstallSISFile.iGenerateStub && file.GetInstallType() == CSISInfo::EInstPartialUpgrade)
	{
	std::wstring shortName(aInstallSISFile.iFileName);

	throw InterpretSisError(L"Generating stub SIS file for " + shortName +
				L" (PU type) is not supported", SIS_NOT_SUPPORTED);
	}
}

void Installer::ProcessConditionalBlockWarnings(const CSISInstallBlock& aInstallBlock, 
											  ExpressionEvaluator& aEvaluator,
											  const SisFile& aFile
											  ) 
	{
	const CSISArray<CSISIf, CSISFieldRoot::ESISIf>& ifs = aInstallBlock.Ifs();
	for (int i = 0; i < ifs.size(); ++i)
		{
		const CSISIf& ifBlock = ifs[i];

		if (ifBlock.WasteOfSpace())
			{
			return;
			}

		// Main expression
		if ( aEvaluator.Evaluate(ifBlock.Expression(),false).BoolValue() )
			{
			ProcessInstallBlockWarnings(ifBlock.InstallBlock(), aFile);
			ProcessConditionalBlockWarnings(ifBlock.InstallBlock(), aEvaluator, aFile);
			continue;
			}
		
		int elseCount = ifBlock.ElseIfCount();
		for (int i = 0; i < elseCount; ++i)
			{
			const CSISElseIf& elseIfBlock = ifBlock.ElseIf(i) ;
			if ( aEvaluator.Evaluate(elseIfBlock.Expression(),false).BoolValue())
				{
				ProcessInstallBlockWarnings(elseIfBlock.InstallBlock(), aFile);
				ProcessConditionalBlockWarnings(elseIfBlock.InstallBlock(), aEvaluator, aFile);
				break;	// Stop processing else if blocks
				}
			}
		} 
	}

void Installer::ProcessInstallBlockWarnings(const CSISInstallBlock& aInstallBlock, const SisFile& aFile)
	{
	aFile.ProcessEmbeddedFileWarning(aInstallBlock);
	
	std::string error;
	bool result = aFile.ProcessInstallOptionsWarning(aInstallBlock,error);	
	if(result == false)
		{
		std::string x;
		throw InvalidSis(Ucs2ToUtf8(aFile.GetPackageName(),x),
			error, SIS_NOT_SUPPORTED);
		}
	}

TInt Installer::Install(const InstallSISFile& aInstallSISFile)
{
	SisFile file(aInstallSISFile.iFileName);

	// Setup the expression evaluator
	SetupExpressionEnvironment( file, GetSisRegistry(), iRomManager );

	// check presence of embedded sis files and 
	// display a warning if they are not installed before
	if (file.HasEmbedded())
		{
		CheckEmbedded(file);
		}

	// check file is acceptable
	file.CheckValid();

	const CSISController& ctrl = file.GetController();	
	ProcessConditionalBlockWarnings(ctrl.InstallBlock(), *iExpressionEvaluator, file);
	
	if (!DependenciesOk(file))
	{
		return MISSING_DEPENDENCY;
	}

	if (!IsValidUpgrade(file, aInstallSISFile.iSUFlag, aInstallSISFile.iNonRemovable))
	{
		return INVALID_UPGRADE;
	}

	InstallableFiles installable;
	GetInstallableFiles(file, installable, *iExpressionEvaluator, aInstallSISFile.iTargetDrive);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TBool iBackupFlag = false;
	Backup(file, installable, iBackupFlag);
#endif

	// Uninstall the same package (if found) prior to any installation
	UninstallPkg(file);

	CheckDestinations(file, installable, aInstallSISFile);

	InstallFiles(installable, iParamList.SystemDriveLetter());

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	UpdateRegistry(file, installable, aInstallSISFile, aInstallSISFile.iSUFlag);
#else
	try {

		UpdateRegistry(file, installable, aInstallSISFile, aInstallSISFile.iSUFlag);
	}
	catch  (InterpretSisError& e)
	{
		RestoreAll(file, installable, iBackupFlag);

		LERROR(L"\t" << string2wstring(e.what()));
		int result =  e.GetErrorCode();
		return result;
	
	}
	catch  (...)
	{//Update Registry Failed. UnInstall the files.

		RestoreAll(file, installable, iBackupFlag);
		return RSC_PARSING_ERROR;
	}

	if(iBackupFlag)
		RemoveBackup(file, installable);
#endif

	if (aInstallSISFile.iGenerateStub)
	{
		CreateStubSisFile(aInstallSISFile, file);
	}

	FreeInstallableFiles(installable);

	return SUCCESS;
}

/** 
 Prepares a list of ROM files which are allowed to be eclipsed from the right 
 source according to the SIS file upgrade type.

 If a SIS file wants to eclipse a file in ROM, it will fail unless there is an 
 associated stub SIS file present in the ROM that can be used by the Installer to 
 identify the replacement file as a valid upgrade of the original.  
 */
void Installer::PrepareEclipsableFilesList(const SisFile& aSis)
{
	iEclipsableRomFiles.clear();
	iEclipsableOverwriteFiles.clear();

	TUint32 pkgUid = aSis.GetPackageUid();

	// upgrade SIS file attributes
	bool puInstallation = aSis.GetInstallType() == CSISInfo::EInstPartialUpgrade;
	bool spInstallation = aSis.GetInstallType() == CSISInfo::EInstAugmentation;
	
	// base package
	const SisRegistryObject& registryEntry = iRegistry.GetRegistryObject(pkgUid);
	bool currentPkgIsInRom = registryEntry.GetInRom();
	bool stubExistsInRom = iRegistry.IsRomStubPackage(pkgUid); 

	// SA+RU, PU and SP are all allowed to eclipse ROM files. There must
	// exist a valid ROM stub in order to populate the eclipsable files list.
	if (IsValidEclipsingUpgrade(aSis, registryEntry))
	{
		// when an upgrade happens over an upgrade which has already upgraded the 
		// ROM based pacakge (i.e. ROM Stub -> SA+RU -> PU), don't populate the
		// eclipsable files from the SIS registry entry which may be in-complete.
		// Directly populate the eclipsable files list from the corresponding stub
		// SIS file.
		if (!currentPkgIsInRom && stubExistsInRom && (puInstallation || spInstallation))
		{
			iRegistry.GetStubFileEntries(pkgUid, iEclipsableRomFiles);
		}
		else
		{
			const FileDescriptions& files = registryEntry.GetFileDescriptions();
			FileDescriptions::const_iterator curr = files.begin();
			FileDescriptions::const_iterator end  = files.end();

			for ( ; curr != end ; ++curr)
			{
				std::wstring target((*curr)->GetTarget());
				iEclipsableRomFiles.push_back(target);
			}
		}
	}
}

/**
 Non-SU packages can Eclipse iff:
 a) The package uids match and 
 b) The package to be installed is immediate Upgrade to ROM (SP or PU over ROM)
      (SA is allowed based on next condition)
 c) (or) The package to be installed is (SA+RU type or PU or SP type) and it has a matching ROM stub SIS file
 are allowed to eclipse files.    
 */
bool Installer::IsValidEclipsingUpgrade(const SisFile& aSis, const SisRegistryObject& registryEntry)
{
	TUint32 pkgUid = aSis.GetPackageUid();

	// case a)
	if (pkgUid != registryEntry.GetUid())
		return false;

	bool stubExistsInRom = iRegistry.IsRomStubPackage(pkgUid);
	bool isBaseInRom = registryEntry.GetInRom();
	bool saInstall = aSis.GetInstallType() == CSISInfo::EInstInstallation;
	bool reg51 = true;
	if(iParamList.RegistryVersionExists())
		{
	int regMajor = iParamList.RegistryMajorVersion();
	int regMinor = iParamList.RegistryMinorVersion();
	bool reg51 = (regMajor >= SisRegistry::KSisRegistryMajorVersion) &&  
					(regMinor >= SisRegistry::KSisRegistryMinorVersion);
		}
	// RU flag was introduce when the SISRegistry was 5.1, otherwise, treat it as normal SA file
	bool romUpgradeFlag = (aSis.GetInstallFlags() & CSISInfo::EInstFlagROMUpgrade) && reg51;
	bool romUpgradableSA = saInstall && romUpgradeFlag;
	bool puInstall = aSis.GetInstallType() == CSISInfo::EInstPartialUpgrade;
	bool spInstall = aSis.GetInstallType() == CSISInfo::EInstAugmentation;
	
	// case b)
	if (isBaseInRom && !saInstall)
		return true;

	// case c)
	return (stubExistsInRom && (romUpgradableSA || puInstall || spInstall));    	
}

void Installer::UninstallPkg(const SisFile& aSis)
{
	TUint32 uid = aSis.GetPackageUid();
	TUint32 installType = aSis.GetInstallType();

	// Check to see the SA is installed, otherwise, RemovePkg() will throw an exception
	if (iRegistry.IsInstalled(uid) && (installType == CSISInfo::EInstInstallation))
	{
		LINFO(L"Removing package \"" << aSis.GetPackageName() << L"\" prior to re-installation");

		// Remove all installed files for this Uid's packages and all the SisRegistry Entries
		iRegistry.RemovePkg(uid, true);
	}

	if (installType == CSISInfo::EInstAugmentation)
	{
		// Clean up the installed files from the specified SP package and the SisRegistry Entry
		iRegistry.RemoveEntry(uid, aSis.GetPackageName().c_str(), aSis.GetVendorName().c_str());
	}

	// Regenerate the ROM stub registry entries for eclipsing check later.
	iRegistry.GenerateStubRegistry();
}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
// Backup all installed files for this Uid's packages and all the SisRegistry Entries
void Installer::Backup(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag)
{
	TUint32 uid = aFile.GetPackageUid();
	TUint32 installType = aFile.GetInstallType();

	// Check to see the SA is installed, else backup is not required.
	if (iRegistry.IsInstalled(uid) && ((installType == CSISInfo::EInstInstallation) || (installType == CSISInfo::EInstPartialUpgrade)))
	{
		aBackupFlag = true;
		int err = 0;
		LINFO(L"Backup package \"" << aFile.GetPackageName() << L"\" prior to re-installation");

		// Backup all installed files for this Uid's packages and all the SisRegistry Entries
		iRegistry.BackupCtl(uid);

		//Backup SCR DB
		std::string DbPath = iRegistry.GetDbPath();
		if (FileExists(string2wstring(DbPath)))
		{	
			std::string BackupDb(DbPath);
			BackupDb.append("_backup");

		 	err=FileCopyA(DbPath.c_str(),BackupDb.c_str(),0);
			if (err != 0)
				LERROR(L"Failed to Backup Database scr.db ");
		}

		// Backup all installed files for this Uid's packages and all the SisRegistry Entries
		InstallableFiles::const_iterator filedesIter; 
		//Backup files
		for(filedesIter = aInstallable.begin() ; filedesIter != aInstallable.end(); ++filedesIter)
		{
			std::wstring itargetLocalFile = (*filedesIter)->GetLocalTarget();
			if (FileExists(itargetLocalFile))
			{
				std::wstring itargetBackupFile = (*filedesIter)->GetLocalTarget();
				std::wstring temp = itargetBackupFile.c_str();
				temp.append(L"_backup");
				itargetBackupFile = temp;


				int pos =0;
				#ifdef __LINUX__
				std::wstring sysBin = L"/sys/bin/";
				#else
				std::wstring sysBin = L"\\sys\\bin\\";
				#endif

				if((pos = itargetBackupFile.find(sysBin, 0)) != std::wstring::npos)
				{
					BackupHashForFile(itargetLocalFile, iParamList.SystemDriveLetter(), iParamList.SystemDrivePath());
				}

				std::string iLocalFile = wstring2string(itargetLocalFile);
				std::string iBackupFile = wstring2string(itargetBackupFile);

				err=FileCopyA(iLocalFile.c_str(),iBackupFile.c_str(),0);
				if (err != 0)
					LERROR(L"Failed to Backup installable file ");
			}
		}
	}
}

// Remove Backup of all installed files for this Uid's packages and all the SisRegistry Entries
void Installer::RemoveBackup(const SisFile& aFile, InstallableFiles& aInstallable)
{
	TUint32 uid = aFile.GetPackageUid();
	TUint32 installType = aFile.GetInstallType();

	// Check to see the SA is installed, else backup is not required.
	if (iRegistry.IsInstalled(uid) && ((installType == CSISInfo::EInstInstallation) || (installType == CSISInfo::EInstPartialUpgrade)))
	{
		LINFO(L"Remove Backup of package \"" << aFile.GetPackageName() << L"\" after re-installation success");
		// Remove all installed files for this Uid's packages and all the SisRegistry Entries
		iRegistry.RemoveCtlBackup(uid);

		//Remove Backup SCR DB
		std::string DbPath = iRegistry.GetDbPath();
		std::string BackupDb(DbPath);
		BackupDb.append("_backup");
		std::wstring DbBackup = string2wstring(BackupDb);

		if (FileExists(DbBackup))
			RemoveFile(DbBackup);
	
		// Remove all installed files for this Uid's packages and all the SisRegistry Entries
		InstallableFiles::const_iterator filedesIter; 
		std::wstring itargetLocalFile;
		std::wstring itargetBackupFile;

		for(filedesIter = aInstallable.begin() ; filedesIter != aInstallable.end(); ++filedesIter)
		{
		    itargetLocalFile = (*filedesIter)->GetLocalTarget();
			itargetBackupFile = (*filedesIter)->GetLocalTarget();
			std::wstring temp = itargetBackupFile.c_str();
			temp.append(L"_backup");
			itargetBackupFile = temp;
			
			if (FileExists(itargetBackupFile))
			{
				RemoveFile(itargetBackupFile);
				RemoveHashForFile(itargetBackupFile, iParamList.SystemDriveLetter(), iParamList.SystemDrivePath());
			}
		}
	}
}

// Restore all installed files for this Uid's packages and all the SisRegistry Entries
void Installer::Restore(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag)
{
	TUint32 uid = aFile.GetPackageUid();
	TUint32 installType = aFile.GetInstallType();

	// Check to see the SA is installed, else backup is not required.
	if((installType == CSISInfo::EInstInstallation) || (installType == CSISInfo::EInstPartialUpgrade))
	{
		LINFO(L"Restoring Installed Package ");

		// Restore all .ctl file for this Uid's packages and all the SisRegistry Entries
		iRegistry.RestoreCtl(uid, aBackupFlag);

		// Restore all installed files for this Uid's packages and all the SisRegistry Entries
		InstallableFiles::const_iterator filedesIter; 
		for(filedesIter = aInstallable.begin() ; filedesIter != aInstallable.end(); ++filedesIter)
		{
			std::wstring itargetLocalFile = (*filedesIter)->GetLocalTarget();
			std::wstring itargetBackupFile = (*filedesIter)->GetLocalTarget();
			itargetBackupFile.append(L"_backup");
			if (FileExists(itargetBackupFile))
			{
				int pos =0;
				#ifdef __LINUX__
				std::wstring sysBin = L"/sys/bin/";
				#else
				std::wstring sysBin = L"\\sys\\bin\\";
				#endif

				if((pos = itargetBackupFile.find(sysBin, 0)) != std::wstring::npos)
				{
					RestoreHashForFile(itargetLocalFile, iParamList.SystemDriveLetter(), iParamList.SystemDrivePath());
				}

				std::string iLocalFile = wstring2string(itargetLocalFile);
				std::string iBackupFile = wstring2string(itargetBackupFile);
				
				int err = FileMoveA(iBackupFile.c_str(),iLocalFile.c_str());
				if (err != 0)
					LERROR(L"Failed to Restore file ");
			}
		}
	}
}

void Installer::RestoreAll(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag)
{
	InstallableFiles::const_iterator filedesIter; 
	std::wstring itargetLocalFile;

	for(filedesIter = aInstallable.begin() ; filedesIter != aInstallable.end(); ++filedesIter)
	{
		itargetLocalFile = (*filedesIter)->GetLocalTarget();
		
		if (FileExists(itargetLocalFile))
		{
			RemoveFile(itargetLocalFile);
			RemoveHashForFile(itargetLocalFile, iParamList.SystemDriveLetter(), iParamList.SystemDrivePath());
		}
	}
	Restore(aFile, aInstallable, aBackupFlag);
	FreeInstallableFiles(aInstallable);
}

#endif

struct CheckDependencyMet
{
	CheckDependencyMet(
		Installer& aInstaller,
		std::vector<SisRegistryDependency>& aMissing)
		: iInstaller(aInstaller),
		iMissingDependencies(aMissing)
	{}

	void operator()(const CSISDependency* dep)
	{
		if (!iInstaller.CheckDependency(dep))
		{
			iMissingDependencies.push_back(SisRegistryDependency(*dep));
		}
	}
	Installer& iInstaller;
	std::vector<SisRegistryDependency>& iMissingDependencies;
};

bool Installer::CheckDependency(const CSISDependency* aDep)
{
	bool result = false;

	try
	{
		const SisRegistryObject& pkg =
			iRegistry.GetRegistryObject(aDep->SisUID().UID1());

		const CSISVersionRange& range = aDep->VersionRange();
		if (!range.WasteOfSpace() && InRange(pkg.GetVersion(), range))
			{
			result = true;
			}
		else
			{
			result = false;
			}
	}

	catch(...)
	{
		result = false;
	}
	return result ;
}

bool Installer::DependenciesOk(const SisFile& aFile)
	{
	const CSISPrerequisites* reqs = aFile.GetDependencies();
	const CSISPrerequisites::TDependencyList& deps = reqs->DependencyList();

	CheckDependencyMet checkDependencyMet(*this, iMissing[aFile.GetPackageName()]);
	for(int i = 0; i < deps.size(); ++i)
		{
		checkDependencyMet(&deps[i]);
		}

	return iMissing[aFile.GetPackageName()].size() == 0;
	}


bool Installer::IsValidUpgrade(const SisFile& aFile, bool aSUFlag, bool aNonRemovable)
{
	TUint32 pkg = aFile.GetPackageUid();
	TUint32 installFlags = aFile.GetInstallFlags();

	if(aNonRemovable)
	{
		TUint8 iFlag = 0;
		iFlag |= CSISInfo::EInstFlagNonRemovable;
		aFile.AddInstallFlags(iFlag);
		installFlags = aFile.GetInstallFlags();
	}
	
	bool RUFlag = ((installFlags & CSISInfo::EInstFlagROMUpgrade) &&  CSISInfo::EInstFlagROMUpgrade);

	if(iParamList.RegistryVersionExists())
		{
	int regMajor = iParamList.RegistryMajorVersion();
	int regMinor = iParamList.RegistryMinorVersion();
	bool reg51 = (regMajor >= SisRegistry::KSisRegistryMajorVersion) &&  
					(regMinor >= SisRegistry::KSisRegistryMinorVersion);
	// RU flag was introduce when the SISRegistry was 5.1, otherwise, treat it as normal SA file
		RUFlag = RUFlag && reg51;
		}
	if ( aSUFlag && !RUFlag)
	{
		std::stringstream err;
		err << "Illegal system upgrade (SU) of ROM package 0x" << std::hex << pkg << " without setting the RU flag";
		throw InterpretSisError(err.str(), ATTEMPT_TO_UPGRADE_ROM_PKG);
	}

	// Check is this package has been installed before

	TUint32 installType = aFile.GetInstallType();

	if (iRegistry.IsInstalled(pkg))
		{
		ValidateRegistry(aFile,pkg,installFlags,RUFlag,aNonRemovable);	
		}
	else if (installType == CSISInfo::EInstAugmentation || installType == CSISInfo::EInstPartialUpgrade)
		{
		// Installing SP and PU without the base package
		std::stringstream err;
		err << "Could not perform upgrade - the base package 0x" << std::hex 
			<< pkg << " is missing";

		throw InterpretSisError(err.str(), MISSING_BASE_PACKAGE);
		}

	return true;
	}

void Installer::ValidateRegistry(const SisFile& aFile, TUint32 aPckgUid, TUint32 aInstallFlags, bool aRUFlag, bool aNonRemovable)
	{
	bool isSisNonRemovable = aInstallFlags & CSISInfo::EInstFlagNonRemovable;
	bool isBaseRemovable = false;
	bool inRom = false;
	bool isPreInstalled = false;
	TUint32 uid = 0;
	std::wstring packageName;
	
	InitializeRegistryDetails(aPckgUid, isBaseRemovable, inRom, isPreInstalled, uid, packageName );
	
	// Check is this package has been installed before
	TUint32 installType = aFile.GetInstallType();

	if (installType == CSISInfo::EInstInstallation)
		{
		if (inRom && !aRUFlag)
			{
			std::stringstream err;
			err << "Illegal SA upgrade to ROM package 0x" << std::hex << aPckgUid;
			throw InterpretSisError(err.str(), ATTEMPT_TO_UPGRADE_ROM_PKG);
			}
		// This is not a ROM base package, check is there a SIS stub base package present
		else if (iRegistry.IsRomStubPackage(aPckgUid) && !aRUFlag)
			{			
			std::stringstream err;
			err << "Indirect SA upgrade to ROM package 0x" << std::hex << aPckgUid
				<< " missing RU flag";
			throw InterpretSisError(err.str(), ATTEMPT_TO_UPGRADE_ROM_PKG);
			}
		}
	else if (installType == CSISInfo::EInstAugmentation)
		{
		if (isBaseRemovable && isSisNonRemovable)
			{
			std::stringstream err;
			err << "Not allowed to install SP + NR to the removable base package 0x" << std::hex << aPckgUid;

			throw InterpretSisError(err.str(), INVALID_UPGRADE);
			}

		if (packageName == aFile.GetPackageName() && uid == aFile.GetPackageUid())
			{
			std::stringstream err;
			err << "Cannot augment package 0x" << std::hex << aFile.GetPackageUid() << " - package names match";

			throw InterpretSisError(err.str(), MISSING_BASE_PACKAGE);
			}
		}
	else if (installType == CSISInfo::EInstPartialUpgrade)
		{
		// Do not allow partial upgrades to preinstalled applications
		if (isPreInstalled)
			{
			std::stringstream err;
			err << "Cannot install PU (0x" << std::hex << aPckgUid << ") to a PA";

			throw InterpretSisError(err.str(), INVALID_UPGRADE);
			}

		// Partial Upgrade can only be installed to a base package of the same install flag type.
		// The only exception is a ROM stub which is upgradable and non-removable by definition.
		if ((isBaseRemovable == isSisNonRemovable) && !inRom )
			{
				if(!aNonRemovable)
				{
				std::stringstream err;
				err << "Cannot install PU (0x" << std::hex << aPckgUid << ") to a base package with a different removable flag type";

				throw InterpretSisError(err.str(), INVALID_UPGRADE);
				}
			}
		}
	else if (installType == CSISInfo::EInstPreInstalledApp)
		{
		std::stringstream err;
		err << "Cannot install PA (0x" << std::hex << aPckgUid << ") over top of another package";

		throw InterpretSisError(err.str(), INVALID_UPGRADE);
		}
	}

void Installer::InitializeRegistryDetails(  const TUint32 aPckgUid, bool& aIsBaseRemovable, 
											bool& aInRom, bool& aIsPreInstalled, 
											TUint32& aUid, std::wstring& aPackageName 
										)
	{
	if(iParamList.RegistryVersionExists() )
		{
		const SisRegistryObject& obj = iRegistry.GetRegistryObject(aPckgUid);
		aIsBaseRemovable = obj.GetIsRemovable();
		aInRom = obj.GetInRom();
		aIsPreInstalled = obj.IsPreInstalled();
		aUid = obj.GetUid();
		aPackageName = obj.GetPackageName();
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		const DbHelper* dbHelper( iRegistry.GetDbHelper());
		TInt32 componentId = dbHelper->GetComponentId(aPckgUid);
		aIsBaseRemovable = dbHelper->GetIsRemovable(componentId);
		aInRom = dbHelper->GetInRom(componentId);
		if(dbHelper->GetInstallType(componentId) == CSISInfo::EInstPreInstalledApp )
			{
			aIsPreInstalled = true;
			}
		aUid = dbHelper->GetUid(componentId);
		aPackageName = dbHelper->GetPackageName(componentId);
		}
	#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	}

bool Installer::GetInstallableFiles(const SisFile& aFile, InstallableFiles& aList, ExpressionEvaluator& aEvaluator, 
									int aInstallingDrive)
{
	return aFile.GetInstallableFiles(aList, aEvaluator, 
		iConfigManager.GetLocalDrivePath(aInstallingDrive), aInstallingDrive);
}


struct InstallFile
{
	InstallFile(const std::wstring& aCDrive, const int aSystemDrive)
		: iSystemDrivePath(aCDrive), iSystemDrive(aSystemDrive) {}

	const std::wstring& iSystemDrivePath;
	const int iSystemDrive;

	void operator()(const InstallableFile* aFile)
	{
		std::fstream output;
		std::wstring target(aFile->GetTarget());
		std::wstring localTarget(aFile->GetLocalTarget());

		bool fileNullOption = (aFile->FileDescription()->Operation() == CSISFileDescription::EOpNull);
				
        if (!target.length())
            {
			LINFO(L"Skipping file with empty destination filename");
            }
        else
            {
			// Unicode characters can not be displayed on DOS prompt
 			std::string temporary = wstring2string(target);
			std::wstring targetDisplay = string2wstring( temporary );
		    LINFO(L"Installing file: " << targetDisplay);

            std::wstring targetDirectory = localTarget.substr( 0, localTarget.rfind( KDirectorySeparator ) );    		
			const unsigned char* buffer = NULL;
			TUint32 len;

			if (aFile->IsStub()) // PA
			{
				// check the presence of file at target location 
				if (!FileExists(localTarget))
				{
					throw InterpretSisError(L"PA Install error - No target file- "+target, PA_NO_TARGET_FILE);
				}
			}
			else // SA 
			{
				// if the FN option specified, leave the file creation
				if (fileNullOption)
				{
					LINFO(L"File " << target << L" contains \"Delete-File-On-Uninstall\" option." );
				}
				else 
				{
					if ( !MakeDir( targetDirectory ) )
					{
						throw InterpretSisError(L"Directory Creation Error - "+targetDirectory,
													DIRECTORY_CREATION_ERROR);
					}

					HANDLE hFile = MakeSISOpenFile(localTarget.c_str(), GENERIC_WRITE, CREATE_ALWAYS);
					if( INVALID_HANDLE_VALUE == hFile )
					{
						throw InterpretSisError(L"FileOpenError - "+target, FILE_ERROR);
					}
					buffer = aFile->FileData()->Data();
					len = aFile->FileData()->UncompressedSize();
					DWORD bytesWritten;
					BOOL error = WriteFile(hFile, buffer, len, &bytesWritten, NULL);
					if( error != TRUE )
					{
						throw InterpretSisError(L"FileWriteError - "+target, FILE_ERROR);
					}
					CloseHandle(hFile);
				}
			}

			if (aFile->IsExecutable() && !fileNullOption)
			{
				// register the hash
				std::wstring basename = localTarget.substr( localTarget.rfind( KDirectorySeparator ) + 1 );
				#ifndef __TOOLS2_LINUX__
				// Remove the null character at the end and.
				std::wstring tempStr = iSystemDrivePath.c_str();
				std::wstring hashdir =  tempStr + L"\\sys\\hash\\";				
				std::wstring reghashdir = L"$:\\sys\\hash\\";
				#else
				std::wstring hashdir = iSystemDrivePath + L"/sys/hash/";
				std::wstring reghashdir = L"$:/sys/hash/";
				#endif

				if ( !MakeDir( hashdir ) )
				{
					throw InterpretSisError(L"Directory Creation Error - "+hashdir,
													DIRECTORY_CREATION_ERROR);
				}

				// hash file is always created on the system drive
				reghashdir[0] = iSystemDrive;

				LINFO(L"\tCreating hash: " << reghashdir << basename.c_str());				
				std::wstring hash = hashdir + basename;
				HANDLE hFile = MakeSISOpenFile(hash.c_str(), GENERIC_WRITE, CREATE_ALWAYS);		
				if( INVALID_HANDLE_VALUE == hFile )
				{
					throw InterpretSisError(L"FileOpenError - " + hashdir + basename, FILE_ERROR);
				}

				buffer = aFile->FileDescription()->Hash().Blob().Data();
				len = aFile->FileDescription()->Hash().Blob().Size();
				DWORD bytesWritten;
				BOOL error = WriteFile(hFile, buffer, len, &bytesWritten, NULL);
				if( error != TRUE )
				{
					throw InterpretSisError(L"FileWriteError - " + hashdir + basename, FILE_ERROR);
				}
				CloseHandle(hFile);
			}
        }
	}
};


void Installer::InstallFiles(const InstallableFiles& aList, const int aInstallDrive)
{
	std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(aInstallDrive);

	std::for_each(aList.begin(), aList.end(), InstallFile(localTargetPath, aInstallDrive));
}


void Installer::UpdateRegistry(const SisFile& aFile, const InstallableFiles& aList, 
												   const InstallSISFile& aInstallSISFile, const bool aSUFlag)
{
	if (aFile.GetInstallType() == CSISInfo::EInstPartialUpgrade)
	{
		iRegistry.UpdateRegistryEntry(aFile, aList, aInstallSISFile);
	}
	else if (!aInstallSISFile.iNotRegister)
	{
		iRegistry.AddRegistryEntry(aFile, aList, aInstallSISFile);
	}
}

/** This function takes a fully qualified name and searches the filesystem through for all possible adorned filename matches.
 Besides that it calculates the unadorned version of the input filename as well and return it through the corresponding given 
 input reference variable.
 @param aTarget the fully qualified filename (full path and name)
 @param aUnadornedName the calulated unadorned name is returned through this variable(full path and unadorned name calculated from aTarget)
 @param aAdornedFileNamesFound all the found adorned name matches are returned through this list
*/
void Installer::AdornedProcessingOfFile(const std::wstring& aTarget, std::wstring& aUnadornedName, 
										std::list<std::wstring>& aAdornedFileNamesFound)
{
	std::wstring targetNameAndExt(StringUtils::NameAndExt(aTarget));
	std::wstring targetDriveAndPath(StringUtils::DriveAndPath(aTarget));

	std::wstring unadornedName;
	std::wstring searchNameWild;

	// create the unadorned version of the target file e.g. c:\sys\bin\a.dll
	GetUnadornedFileName(targetNameAndExt, unadornedName);
	aUnadornedName = targetDriveAndPath;
	aUnadornedName.append(unadornedName);

	// create a wildcard version of the target file e.g. c:\sys\bin\a{????????}.dll
	GenerateSearchNameWild(targetNameAndExt, searchNameWild);

	const DrivesMap& driveMap = iConfigManager.GetDrivesMap();
	FindAllAdornedVariants(searchNameWild, KSysBinPath, aAdornedFileNamesFound, driveMap);

	// find all adorned variants in the ROM/ROFS logs
	if (iParamList.IsFlagSet(CParameterList::EFlagsRomRofsLogFilesSet))
	{
	#ifndef __TOOLS2_LINUX__
		std::wstring romSearchNameWild = L"z:\\sys\\bin\\";
	#else
		std::wstring romSearchNameWild = L"z:/sys/bin/";
	#endif
		romSearchNameWild.append(searchNameWild);
		iRomManager.FindAllAdornedVariants(romSearchNameWild, aAdornedFileNamesFound);
	}
}

/**
 This function ensures the SIS file to be installed does not illegally
 eclipse a file already installed in the ROM.
 @param aFile the SIS file to be installed
 @param aTarget the fully qualified filename (full path and name)
 @param aInstallableFile the current installing files properties
 @param aSUFlag specifies if the installing SIS file has been signed by a SU certificate
*/
bool Installer::ValidEclipse(const SisFile& aFile, const std::wstring& aTarget, const InstallableFile& aInstallableFile, bool aSUFlag)
{
	bool result = true;
	std::wstring searchNameUnadorned = L"";
	std::wstring localDir = L"";
	std::list<std::wstring> adornedFileNamesFound;

	int targetDisk = tolower(aTarget[0]);
	const DrivesMap& driveMap = iConfigManager.GetDrivesMap();

	int count = driveMap.size();

	// check for adorned (versioned) files e.g. dummy{12345678}.dll
	if (aTarget.find(KSysBinPath) != std::wstring::npos)
	{
		AdornedProcessingOfFile(aTarget, searchNameUnadorned, adornedFileNamesFound);
	}
	else
	{
		searchNameUnadorned = aTarget;
	}

	DrivesMap::const_iterator it = driveMap.begin();
	DrivesMap::const_iterator end = driveMap.end();

	// search on all of defined drives (including the ROM and system drive) for
	// the target file
	for ( ; it != end ; ++it)
	{
		// drive letter
		int disk = tolower(it->first);
		searchNameUnadorned[0] = disk;
		std::wstring unadornedLocalPath(searchNameUnadorned);

		// check if adorned file version is found on this disk
		bool adornedFound = false;

		std::list<std::wstring>::iterator index = adornedFileNamesFound.begin();
		std::list<std::wstring>::iterator adornedEnd = adornedFileNamesFound.end();
		int nrOfAdornedFilenamesFound = adornedFileNamesFound.size();

		for ( ; index != adornedEnd ; ++index)
		{
			std::wstring adornedFile = *index;
			int drive = tolower(adornedFile[0]);
			if (drive == disk)
			{
				adornedFound = true;
				break;
			}
		}
		
		// the actual directory
		localDir = it->second->iDir;
		bool unadornedFound;

		// check to see if we are using ROM/ROFS files to represent the z drive.
		// If so, check to see if the ROM file exists in the logs.
		if (disk == 'z' && localDir.empty())
		{
			unadornedFound = iRomManager.RomFileExists(searchNameUnadorned);
		}
		else
		{
			// convert to the local path and see if the file exists on the current drive
			ConvertToLocalPath( unadornedLocalPath, localDir );
			unadornedFound = FileExists(unadornedLocalPath);
		}

		bool matchFound = unadornedFound || adornedFound;
		if (!matchFound)
			continue;

		bool processEclipsingSuspectsFoundOnThisDrive = false;
		
		// The logic varies whether we find an eclipsable/overwritable file on: (1) The same disk as the one we are installing to
		// (2) The Z drive (ROM), (3) Any other drive
		if (disk == targetDisk)  // case (1)
		{
			bool isOverwrite = FileExists(aInstallableFile.GetLocalTarget());
			
			// there are 3 possible cases: (1.1) We are correctly overwriting the already present file (a PU), 
			// (1.2) We are incorrectly overwriting the file present, (1.3) We are eclipsing the file (if one of the file names is adorned)
			if (isOverwrite)
			{
				// case 1.1 and 1.2 are handled here - overwrite the file if we have the right to do so.
				HandleFileOverwriting(aFile, aTarget, aSUFlag);
			}
			else
			{
				// case 1.3 is handled here: 
				// we have found a matching filename (either unadorned or adorned); add it to the eclipsing suspects
				processEclipsingSuspectsFoundOnThisDrive = true;
			}
		}
		else if (disk == 'z') // case (2)
		{
			// we are eclipsing a ROM file, check if this is authorised:
			// see implementation of IsEclipsable()
			#ifndef __TOOLS2_LINUX__
			if (!IsEclipsable(searchNameUnadorned, aSUFlag))
			#else
			if (!IsEclipsable(searchNameUnadorned, aSUFlag, localDir))
			#endif
			{
				result = false;
				break;
			}
		}
		else // case (3)
		{
			processEclipsingSuspectsFoundOnThisDrive = true;
		}

		// now check to see whether the unadorned or the adorned filename found on 
		// this drive should be added to iEclipsableOverwriteFiles.
		if (processEclipsingSuspectsFoundOnThisDrive && unadornedFound)
		{
			iEclipsableOverwriteFiles.push_back(searchNameUnadorned.c_str());
		}

		if (processEclipsingSuspectsFoundOnThisDrive && adornedFound)
		{
			nrOfAdornedFilenamesFound = adornedFileNamesFound.size();
			std::list<std::wstring>::iterator index = adornedFileNamesFound.begin();
			std::list<std::wstring>::iterator adornedEnd = adornedFileNamesFound.end();

			// add the adorned filenames found to the list of eclipsable/overwrite candidates
			for ( ; index != adornedEnd ; ++index)
			{
				std::wstring adornedFile = *index;
				int drive = tolower(adornedFile[0]);
				if (drive == disk)
				{
					iEclipsableOverwriteFiles.push_back(adornedFile.c_str());
				}
			}
		}
	}
	// end of loop

	// the final step is to try and remove all the files from iEclipsableOverwriteFiles that
	// will prevent this SIS file from installing.
	if (result && (iEclipsableOverwriteFiles.size() > 0))
	{
		FilterNonBlockingFilesOfFilename(aFile, aTarget);
	}

	return result;
}

/** Checks the list of eclipsable candidate files against those that are owned by the 
 installing package (if any). Blocking files are removed if a match is found.
 @param aFile the SIS file to be installed
 @param aTarget the fully qualified filename (full path and name)
*/
void Installer::FilterNonBlockingFilesOfFilename(const SisFile& aFile, const std::wstring& aTarget)
{
	// if the package aleady exists
	TUint32 pkgUid = aFile.GetPackageUid();
	if (iRegistry.IsInstalled(pkgUid))
	{
		const SisRegistryObject& registryEntry = iRegistry.GetRegistryObject(pkgUid);
		const FileDescriptions& installedFiles = registryEntry.GetFileDescriptions();			
	
		FileDescriptions::const_iterator inCurr = installedFiles.begin();
		FileDescriptions::const_iterator inEnd  = installedFiles.end();

		for ( ; inCurr != inEnd ; ++inCurr)
		{
			std::wstring baseFile((*inCurr)->GetTarget());

			std::list<std::wstring>::iterator eCurr = iEclipsableOverwriteFiles.begin();
			std::list<std::wstring>::iterator eEnd = iEclipsableOverwriteFiles.end();

			for ( ; eCurr != eEnd; ++eCurr)
			{
				std::wstring eclipseFile(*eCurr);
				
				// the installed base package owns the eclipse/overwrite candidate
				if (FoldedCompare(baseFile, eclipseFile) == 0)
				{
					// we are installing a new adorned variant of a file, so remove the blocking
					// file from the list
					if (StringUtils::NameAndExt(baseFile) != StringUtils::NameAndExt(aTarget))
					{
						eCurr = iEclipsableOverwriteFiles.erase(eCurr);
					}
				}
			}
		}
	}
}

/** This function examines the list of ROM files we may eclipse to see if the supplied name is among them.
 Here we have 2 options:
 1)   file is not to be installed to \sys\bin\
      in this case we don't invoke special handling of adorned filenames so we are only
      interested in if that particular file is allowed to be eclipsed
 2)   file is to be installed to \sys\bin therefore we have to handle adorned filenames.
      Eclipsing is allowed if the filename that we are trying to install is a variant of any ROM filename 
      marked as eclipsable i.e: if we are trying to install c:\sys\bin\A{000A0001}.dll
      and we have z:\sys\bin\A.dll as an eclipsable file or any z:\sys\bin\A{????????}.dll
      then we are allowed to eclipse.
 @param aRomFile fully qualified filename in ROM that is to be checked for eclipising (i.e: z:\sys\bin\some.dll)
 @param aSUFlag specifies if the installing SIS file has been signed by a SU certificate
*/
#ifndef __TOOLS2_LINUX__
bool Installer::IsEclipsable(std::wstring& aRomFile, bool aSUFlag)
#else
bool Installer::IsEclipsable(std::wstring& aRomFile, bool aSUFlag, const std::wstring& aDrivePath)
#endif
{
	// SA/SP/PU sis + RU flag + signed by SU root cert is allowed
	if (aSUFlag)
		return true;

	bool isEclipsable = false;
	bool goesToSysBin = (aRomFile.compare(KSysBinPath) != std::wstring::npos);

	// go through the list of ROM files which are allowed to be eclipsed
	std::list<std::wstring>::const_iterator end = iEclipsableRomFiles.end();
	for (std::list<std::wstring>::const_iterator curr = iEclipsableRomFiles.begin() ;
		 curr != end; ++curr)
	{
		std::wstring eclipsableRomFile = *curr;
		
		// support wildcard characters in ROM stub files
		if (StringUtils::WildcardCompare(eclipsableRomFile,aRomFile)) // case 1
		{
			isEclipsable = true;
			break;
		}
		else if (goesToSysBin) // case 2
		{
			#ifndef __TOOLS2_LINUX__
			if (IsAdornedVariationOf(eclipsableRomFile,aRomFile))
			#else
			if (IsAdornedVariationOf(eclipsableRomFile,aRomFile,aDrivePath))
			#endif
			{
				isEclipsable = true;
				break;
			}
		}
	}

	return isEclipsable;
}

/** This function handles overwriting scenarios (i.e: we are trying to install c:\somedir\somename.ext however it already exists)
 In this case we have 2 possibilities:
 1) We are correctly overwriting the already present file (a PU)
 2) We are incorrectly overwriting the file present
 @param aFile the SIS file to be installed
 @param aTarget The fully qualified filename (full path and name)
 @param aSUFlag specifies if the installing SIS file has been signed by a SU certificate
*/
void Installer::HandleFileOverwriting(const SisFile& aFile, const std::wstring& aTarget, bool aSUFlag)
{
	// find out which package this file belongs to
	TUint32 owningUid = 0;
	const SisRegistryObject* owningObj = iRegistry.OwningPackage(aTarget,owningUid);
	bool isSp = aFile.GetInstallType() == CSISInfo::EInstAugmentation;
	bool isPu = aFile.GetInstallType() == CSISInfo::EInstPartialUpgrade;
	bool isSa = aFile.GetInstallType() == CSISInfo::EInstInstallation;

	// no package owns this file. Always allow orphaned file overwriting!
	if (owningUid == 0)
	{
		// do not display a warning if the installing file is a PA. The user
		// needs to copy the installing files to the installing directory
		// prior to installing the SIS file.
		std::string tempTarget = wstring2string(aTarget);
  	  	std::wstring targetDisplay = string2wstring( tempTarget );
		if (aFile.GetInstallType() == CSISInfo::EInstPreInstalledApp)
		{
			LINFO(targetDisplay << L" found for PA installation");
		}
		else
		{
			LWARN(targetDisplay << L" overwrites orphaned file");
		}
	}
	else if (aFile.GetPackageUid() == owningUid)
	{
		// SP can not overwrite a file in the base pkg
		if (isSp)
		{
			std::wostringstream os;
			os << aTarget.c_str() << L" overwrites file from base package 0x" << std::hex << owningUid << L" \"" << owningObj->GetPackageName() << L"\"" << std::endl;
			iProblemFiles.append(os.str());
			iError = ECLIPSING_VIOLATION;
		}
	}
	else
	{
		// SP can not overwrite a file belonging to a different base pkg
		// PU/SA can not overwrite a file belonging to a different base pkg unless it has been signed by
		// a SU cert
		if (isSp || (!aSUFlag && (isPu || isSa)))
		{
			std::wostringstream os;
			os << aTarget.c_str() << L" overwrites file from base package 0x" << std::hex << owningUid << L" \"" << owningObj->GetPackageName() << L"\"" << std::endl;
			iProblemFiles.append(os.str());
			iError = ECLIPSING_VIOLATION;
		}
		else
		{
		    LWARN(aTarget.c_str() << L" overwrites file from base package 0x" << std::hex << owningUid << L" \"" << owningObj->GetPackageName() << L"\"");
		}
	}
}

void Installer::WarnEclipseOverWrite(const SisFile& aFile)
{
	// we still have some blocking files left after we have checked the destination
	// of the installing file.
	if (iEclipsableOverwriteFiles.size()>0)
	{
		// InterpretSIS allows orphan file eclipsing/overwriting
		std::list<std::wstring>::const_iterator curr = iEclipsableOverwriteFiles.begin();
		std::list<std::wstring>::const_iterator end = iEclipsableOverwriteFiles.end();
		
		for ( ; curr != end; ++curr)
		{
			std::wstring eclipseFile(*curr);

			TUint32 owningUid = 0;
			const SisRegistryObject* owningObj = iRegistry.OwningPackage(eclipseFile.c_str(), owningUid);
			
			// always allow orphaned file overwriting
			if (owningUid == 0)
			{
			    LWARN(eclipseFile.c_str() << L" eclipses orphaned file.");
			}
			else
			{
				// SP/PU can not overwrite a file belonging to a different base pkg or
				// SP/PU can not eclipse a base package which is not in the ROM.
				std::wostringstream os;
				os << eclipseFile.c_str() << L" eclipses file from package 0x"  << std::hex << owningUid << L" \"" << owningObj->GetPackageName().c_str() << L"\""<< std::endl;
				iProblemFiles.append(os.str());
				iError = ECLIPSING_VIOLATION;
				continue;
			}
		}		
	}
}

void Installer::CheckDestinations(const SisFile& aFile, InstallableFiles& aFiles, 
								  const InstallSISFile& aInstallSISFile)
{
	bool suFlag = aInstallSISFile.iSUFlag;
	
	// if the -e option or InstallSISFile::iNotRegister is set, disable the eclipsing checks
	bool isEclipseAllowed = (iParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet)) || aInstallSISFile.iNotRegister;

	iError = SUCCESS;

	Sids sids;
	GetSidsFromInstallable(aFiles, sids);

	#ifndef __TOOLS2_LINUX__
	const std::wstring privatePath = L":\\private\\";
	const std::wstring sysPath = L":\\sys\\";
	const std::wstring sysBinPath = L":\\sys\\bin\\";
	const std::wstring resourcePath = L":\\resource\\";
	#else
	const std::wstring privatePath = L":/private/";
	const std::wstring sysPath = L":/sys/";
	const std::wstring sysBinPath = L":/sys/bin/";
	const std::wstring resourcePath = L":/resource/";
	#endif
	const std::wstring exeType = L".exe";

    int num_of_files = aFiles.size();
	TUint32 pkgUid = aFile.GetPackageUid();
	TUint32 installType = aFile.GetInstallType();
	std::list<std::wstring> stubFileEntries;

	iRegistry.GetStubFileEntries(pkgUid, stubFileEntries);

	// if a base package exists, check to see if it chains back to a ROM stub. If so, populate
	// iEclipsableRomFiles with the files owned by the ROM stub. Only ROM files can be eclipsed.
	//
	// if the package has been signed by a SU cert, then there is no need to generate the eclipsable
	// file list - it is allowed to eclipse any file in the ROM.
	if (iRegistry.IsInstalled(pkgUid) && !suFlag)
		{
		PrepareEclipsableFilesList(aFile);
		}

	// loop through the files to be installed
	for( int ii=num_of_files-1; ii>=0; ii-- )
	{   
		const InstallableFile* file = aFiles[ ii ];
        const CSISFileDescription* sisDescription = file->FileDescription();

		// file to be installed
		std::wstring target( file->GetTarget().c_str() );

		if (sisDescription->Operation() == CSISFileDescription::EOpText)
			break;
		if (sisDescription->Operation() == CSISFileDescription::EOpNull)
			{
			// a FN file can not remove a file from \sys or \resource
			if ((target.find(sysPath,0) != std::wstring::npos) || (target.find(resourcePath,0) != std::wstring::npos))
				{
				// eclipsing problem
				std::wostringstream os;
				os << L"FN file \"" << target.c_str() << L"\" can not remove a file from \\sys or \\resource" << std::endl;
				iProblemFiles.append(os.str());
				iError = DATA_CAGE_VIOLATION;
				continue;
				}
			}
        else 
            {
			std::wstring realTarget(file->GetLocalTarget().c_str());

		    std::wstring romTarget(target);
			romTarget[0] = KRomDriveLetter;

			// check to see if we are legally allowed to eclipse a ROM file 
			if (!isEclipseAllowed)
			{
				if (!ValidEclipse(aFile, target, *file, suFlag))
				{
					// eclipsing problem
					std::wostringstream os;
					os << target.c_str() << L" eclipses file from ROM" << std::endl;
					iProblemFiles.append(os.str());

					iError = ECLIPSING_VIOLATION;
					continue;
				}
			}
			
			// additional platsec checks
			if (target.find(privatePath) != std::wstring::npos)
		    {
				// add an extra one for the drive letter
			    TInt start = privatePath.size()+1;
			    TInt end   = target.find( KDirectorySeparator[ 0 ], start);

			    std::wistringstream uidStr(target.substr(start,end-start));

			    TUint32 sid = 0;
			    uidStr >> std::hex >> sid;
				
				if (sid == 0)
					break;

				// if a file in a private directory is registered, check to see if
				// it belongs to this package
			    if (std::find(sids.begin(), sids.end(), sid) == sids.end())
			    {
					TUint32 sidOwnerPkgUId = KNullUid;

					if (iRegistry.SidExists(sid))
					{
						if(iParamList.RegistryVersionExists())
							{
						// SID belongs to some package, lets find out its UID
						const SisRegistryPackage& sisRegistryPkg = iRegistry.SidToPackage(sid);

						sidOwnerPkgUId = sisRegistryPkg.GetUid();
							}
						else
							{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
							sidOwnerPkgUId = iRegistry.GetUid(sid);
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
							}
					}

					if (sidOwnerPkgUId != pkgUid)
					{
						// this package is trying to write to a SID's private directory 
						// which does not belong to this package

						const int KUidStringLength = 8;
						std::wstring importPath = privatePath;
						#ifndef __TOOLS2_LINUX__
						importPath.append( uidStr.str() + L"\\import" );
						#else
						importPath.append( uidStr.str() + L"/import" );
						#endif

						if ((target.find(importPath,0) == std::wstring::npos) && !suFlag)
						{
							// only SA with RU + SU and PU with RU + SU flag can override other private directory
							std::wostringstream os;
							os << target.c_str() << L" cannot be written to a private directory which "
								<< L"does not belong to any exe in this package" << std::endl;
							iProblemFiles.append(os.str());
							iError = DATA_CAGE_VIOLATION;
							continue;
						}
					}
			    }
		    }
		    else if (file->IsExecutable())
		    {
				TUint32 sid = file->Sid();

				std::wstring romFileWithDuplicateSid(L"");
				bool sidExistsInRom = iRomManager.SidExistsInRom(romFileWithDuplicateSid, sid);
				bool sidExistsInRegistry = iRegistry.SidExists(sid);

				if (sidExistsInRegistry && file->IsExe())
				{
					// the .EXE which owns the SID
					std::wstring owningSidFile;
					iRegistry.SidToFileName(sid,owningSidFile);

					bool owningPkgInRom = false;
					bool samePkgUid = false;
					TUint32 owningPkg = 0;
					if(iParamList.RegistryVersionExists())
						{
						// the SisRegistryObject which owns the .EXE
						
						const SisRegistryObject* owningObj = iRegistry.OwningPackage(owningSidFile,owningPkg);

						owningPkgInRom = owningObj->GetInRom();
						samePkgUid = (owningPkg == pkgUid);
						}
					else
						{
						#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
							owningPkg = iRegistry.GetUid(owningSidFile);
							owningPkgInRom = iRegistry.GetInRom(owningPkg);
							samePkgUid = (owningPkg == pkgUid);
						#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
						}
					std::wstring targetName = StringUtils::NameAndExt(file->GetTarget());
					std::wstring owningSidFileName = StringUtils::NameAndExt(owningSidFile);

					bool sameTargetName = (FoldedCompare(targetName,owningSidFileName) == 0);
					bool romStubExists = iRegistry.IsRomStubPackage(pkgUid);
					bool validInstallType = (installType == CSISInfo::EInstInstallation || installType == CSISInfo::EInstPartialUpgrade || installType == CSISInfo::EInstAugmentation);

					// allow SID eclipsing in the following scenarios:
					// 1) SIS file has been signed by a SU cert
					// 2) eclipsing checks have been disabled by the user (using -e)
					// 3) SP/PU/SA+RU eclipsing of a ROM stub
					bool allowSidEclipse = false;

					// case 1 and 2
					if (suFlag || isEclipseAllowed)
						allowSidEclipse = true;

					// case 3
					if (validInstallType && sameTargetName && samePkgUid && (owningPkgInRom || romStubExists))
					{
						allowSidEclipse = true;
					}
					
					if (!allowSidEclipse)
					{
						std::wostringstream os;
										
						os << L"The existing file " << owningSidFile.c_str() << L" already has the SID 0x" << std::hex << sid
						   << std::dec << L", cannot install " << target.c_str() << std::endl;
						iProblemFiles.append(os.str());
						iError = DUPLICATE_SID;
						continue;
					}
				}
				// .EXE exists in the ROM, but no ROM stub has ownership of it
				else if (sidExistsInRom)
				{
					// A ROM stub could have referenced this .EXE using wildcards - in this case the SID will not
					// be registered in the SIS registry.
					bool isInstalled = iRegistry.IsInstalled(pkgUid);
					bool ownedByRomStub = false;
					if (isInstalled)
					{
						if (iRegistry.IsRomStubPackage(pkgUid))
						{
							CSISController* sisController = iRegistry.GetStubController(pkgUid);
							TFileDescList fileDesList;
							sisController->InstallBlock().GetFileList(fileDesList);
							for(TFileDescListConstIter iter = fileDesList.begin(); iter != fileDesList.end(); ++iter)
							{
								const CSISFileDescription* fD = *iter;

								std::wstring romStubTarget(fD->Target().GetString().c_str());

								if (StringUtils::WildcardCompare(romStubTarget,romFileWithDuplicateSid))
								{
									ownedByRomStub = true;
									break;
								}
							}
						}
					}

					bool allowSidEclipse = false;

					if (suFlag || isEclipseAllowed)
						allowSidEclipse = true;

					if (!allowSidEclipse && !ownedByRomStub)
					{
						std::wostringstream os;
											
						os << L"A ROM file already has the SID 0x" << std::hex << file->Sid()
						   << std::dec << L", cannot install " << target.c_str() << std::endl;
						iProblemFiles.append(os.str());
						iError = DUPLICATE_SID;
						continue;
					}
				}
		    }
		    else if (realTarget.find(sysBinPath) != std::wstring::npos)
		    {
				std::wostringstream os;
				os << target.c_str() << L" cannot be installed to a data caged area" << std::endl;
				iProblemFiles.append(os.str());
				iError = DATA_CAGE_VIOLATION;
				continue;
		    }
        }
	}

	// we still may have blocking files left. Handle them here.
	WarnEclipseOverWrite(aFile);

	iEclipsableOverwriteFiles.clear();
	iEclipsableRomFiles.clear();

	if (!iProblemFiles.empty())
	{
		std::string x = wstring2string(iProblemFiles);
		throw InvalidSis("", x, iError);
	}
}

void Installer::SetupExpressionEnvironment(const SisFile& aFile, const SisRegistry& aSisRegistry, RomManager& aRomManager )
{
	delete iExpressionEnvironment;
	iExpressionEnvironment = new ExpressionEnvironment( aFile,
                                                        aSisRegistry,
                                                        iRomManager,
                                                        iConfigManager,
                                                        iParamList.SystemDrivePath() );
    
    delete iExpressionEvaluator;
    iExpressionEvaluator = new ExpressionEvaluator( *iExpressionEnvironment );
}

void Installer::CheckEmbedded(const SisFile& aFile)
{
	PackageUids pkgs = aFile.GetEmbeddedPackageUids();

	for (PackageUids::const_iterator curr = pkgs.begin() ;
		 curr != pkgs.end() ;
		 ++curr)
	{
		 if (!iRegistry.IsInstalled(*curr))
		 {
			 
			 LWARN(L" Embedded Package not installed: UID " << std::hex << *curr );
		 }
	}
}


void Installer::CreateStubSisFile(const InstallSISFile& aInstallSISFile, SisFile& aSis)
{
	int targetDrive = aInstallSISFile.iTargetDrive;
	std::wstring drivePath = iConfigManager.GetLocalDrivePath(targetDrive);
	#ifndef __TOOLS2_LINUX__
	drivePath.append(L"\\private\\");
	#else
	drivePath.append(L"/private/");
	#endif

	// build SwiDaemon Pathname
	std::wstring ctrl = StringUtils::MakePathFromSID(drivePath, KSwiDaemonUid);

	if ( !MakeDir( ctrl ) )
	{
		throw InterpretSisError(L"Directory Creation Error - "+ctrl, DIRECTORY_CREATION_ERROR);
	}

	std::wstringstream s;
	s << std::hex << aSis.GetPackageUid();

	ctrl.append(s.str());

	switch(aSis.GetInstallType())
    {
	case CSISInfo::EInstInstallation:
	case CSISInfo::EInstPreInstalledApp:
		{
			// If an Installation type is SA/PA then append _0 after the stub UID.
			ctrl.append(L"_0");
			break;
		}

	case CSISInfo::EInstAugmentation:
		{
			// If an Installation type is SP then append an index after the stub UID.

			int num = 0;

			ctrl.append( L"_");
			
			if (aInstallSISFile.iNotRegister)
			{
				// In this case, we can only assume all the previous SP installations 
				// (of a particular package) are consistently installed to the same target 
				// drive and all of the SP installation's attribute are set to either 
				// +mcardalone OR +mcardalonenr. Otherwise, the generated index will be wrong.

				// Get the index from the daemon folder
				num = GetAugmentationsNumber(ctrl);
			}
			else
			{
				// In this case, we can only assume all the previous SP installations 
				// (of a particular package) is consistent with attributes set to either 
				// +mcard OR +mcardnr. Otherwise, the generated index will be wrong.

				// Get the index from the sisregistry entries
				num = iRegistry.GetAugmentationsNumber(aSis.GetPackageUid());
			}

			std::wstringstream s2;
			s2 << std::hex << num;
			ctrl.append( s2.str() );
			break;
		}

	default:
		{
			std::wstring err(L"SIS file cannot be propagated: ");
			err.append(aInstallSISFile.iFileName);
			throw InterpretSisError(err, SIS_NOT_SUPPORTED);
		}
	}

	ctrl.append(L".sis");

	// Display the target SIS file. Format the stub SIS file string so it displays correctly.
	std::wstring ctrlDrive(L"$:");
	ctrlDrive[0] = targetDrive;
	std::wstring ctrlTarget = ctrl.substr(iConfigManager.GetLocalDrivePath(targetDrive).length(),ctrl.length());
	ctrlDrive.append(ctrlTarget);

	LINFO(L"\tCreating stub: " << ctrlDrive);

	aSis.MakeSISStub(ctrl);
	// If the NR flag is set, change the file attribute to RO
	if (aInstallSISFile.iReadOnly)
	{
		_wchmod(ctrl.c_str(),_S_IREAD);
	}
}
