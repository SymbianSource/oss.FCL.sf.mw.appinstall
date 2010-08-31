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


#ifndef	INSTALLER_H
#define	INSTALLER_H

#include <vector>
#include <map>
#include "symbiantypes.h"
#include "installablefile.h"
#include "parameterlist.h"
#include "sisregistrydependency.h"
#include "expressionevaluator.h"

class SisRegistry;
class SisRegistryObject;
class CSISContents;
class SisFile;
class CSISFileDescription;
class CSISFileData;
class RomManager;
class ConfigManager;


/**
* @file INSTALLER.H
*
* @internalComponent
* @released
*/
class Installer
	{
public:
	typedef std::map<std::wstring, std::vector<SisRegistryDependency> > MissingDeps;

	Installer( SisRegistry& aReg, const CParameterList& aParamList, RomManager& aRomManager, ConfigManager& aConfigManager );

	TInt Install(const CParameterList::SISFileList& aList);

	TInt Install(const InstallSISFile& aFile);

	const MissingDeps& GetMissing() const
	{ return iMissing; }

	const SisRegistry& GetSisRegistry() const
	{ return iRegistry; }

private:

	typedef std::vector<std::wstring> Filenames;

	void SanitiseSISFileAttributes(const InstallSISFile& aInstallSISFile);

	void UninstallPkg(const SisFile& aSis); // Uninstall the same package (if found)
											// prior to installation
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void Backup(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag);
	void RemoveBackup(const SisFile& aFile, InstallableFiles& aInstallable);
	void Restore(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag);
	void RestoreAll(const SisFile& aFile, InstallableFiles& aInstallable, TBool& aBackupFlag);
#endif
	bool DependenciesOk(const SisFile& aFile);			// Check to see if the sis file
														// meets it's dependencies

	bool IsValidUpgrade(const SisFile& aFile, bool aSUFlag, bool aNonRemovable);	// Check to see if this is a valid upgrade
																	// to the existing packages
	void ValidateRegistry(const SisFile& aFile, TUint32 aPckgUid, TUint32 aInstallFlags, bool aRUFlag, bool aNonRemovable);
	void InitializeRegistryDetails	( const TUint32 aPckgUid, bool& aIsBaseRemovable, bool& aInRom, bool& aIsPreInstalled, TUint32& aUid, std::wstring& aPackageName );


	bool GetInstallableFiles(
		const SisFile& aFile, InstallableFiles& aList, ExpressionEvaluator& aEvaluator, int aInstallingDrive);

	void InstallFiles(const InstallableFiles& aList, const int aInstallDrive);	// Retrieves the list of files
														// to install from the sis file

	static void Install(const InstallableFile& aFile);

	void UpdateRegistry(const SisFile& aFile, const InstallableFiles& aInstalled, // Update the registry entries for
		const InstallSISFile& aInstallSISFile, const bool aSUFlag);	// PU sis files

	const SisRegistryObject& AddRegistry(const SisFile& aFile,	// Create the registry entries for
		const InstallableFiles& aInstalled, const bool aSUFlag);	// the installed sis file

	//void RemoveHashForFile(const std::wstring& aFile);

	bool CheckDependency(const CSISDependency* aDep);	

	void CheckDestinations(const SisFile& aFile, InstallableFiles& aFiles,
							const InstallSISFile& aInstallSISFile);	// Ensure installable files do not
																	// break platsec rules
	void CheckEmbedded(const SisFile& aFile);
	void SetupExpressionEnvironment( const SisFile& aFile,
                                     const SisRegistry& aSisRegistry,
                                     RomManager& aRomManager );

	void CreateStubSisFile(const InstallSISFile& aInstallSISFile, SisFile& aSis);
	
	// Eclipsing check methods
	void PrepareEclipsableFilesList(const SisFile& aSis);
	bool IsValidEclipsingUpgrade(const SisFile& aSis, const SisRegistryObject& registryEntry);
	bool ValidEclipse(const SisFile& aFile, const std::wstring& target, const InstallableFile& aInstallableFile, bool aSUFlag);
	void HandleFileOverwriting(const SisFile& aFile, const std::wstring& target, bool aSUFlag);
	#ifndef __TOOLS2_LINUX__
	bool IsEclipsable(std::wstring& aRomFile, bool aSUFlag);
	#else
	bool IsEclipsable(std::wstring& aRomFile, bool aSUFlag, const std::wstring& aDrivePath);
	#endif
	void FilterNonBlockingFilesOfFilename(const SisFile& aFile, const std::wstring& target);
	void WarnEclipseOverWrite(const SisFile& aFile);

	void ProcessConditionalBlockWarnings(const CSISInstallBlock& aInstallBlock, 
												  ExpressionEvaluator& aEvaluator,
												  const SisFile& aFile
												  ); 
	void ProcessInstallBlockWarnings(const CSISInstallBlock& aInstallBlock, const SisFile& aFile);

	// Adorned methods
	void AdornedProcessingOfFile(const std::wstring& aTarget, std::wstring& aUnadornedName, 
										std::list<std::wstring>& aAdornedFileNamesFound);

	friend struct CheckDependencyMet;
	friend struct InstallFile;

private:
	SisRegistry& iRegistry;		// The current sis registry
	const CParameterList& iParamList;	// options or parameters
	MissingDeps iMissing;		// List of missing dependencies
	ExpressionEnvironment* iExpressionEnvironment;	// Expresssion Environment for conditional statement
	ExpressionEvaluator* iExpressionEvaluator;		// Expression Evaluator
    RomManager& iRomManager;
    ConfigManager& iConfigManager;
	// list of potential blocking files which will prevent the SIS file from installing
	std::list<std::wstring> iEclipsableOverwriteFiles;
	// list of ROM files which are allowed to be eclipsed by the installing SIS file upgrade
	std::list<std::wstring> iEclipsableRomFiles;
	std::wstring iProblemFiles;
	ErrorCodes iError;
	};

#endif	/* INSTALLER_H */
