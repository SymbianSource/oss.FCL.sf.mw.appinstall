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
#include "dblayer.h"
#include "dbconstants.h"
#include "xmlgenerator.h"
#include "xmlparser.h"
#include "util.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <iostream>
#include "dirparse.h"
#include "parse.h"
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

// constants 
#define IGNORE_FORMATTING -1
#define ENABLE 1
#define DISABLE 0
// System Includes
#include <list>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>

#ifndef __TOOLS2_LINUX__
#include <windows.h>
#endif 

// SisX Library Includes
#include "sisfiledescription.h"
#include "sisstring.h"
#include "utility.h"

// User Includes
#include "sisregistry.h"
#include "errors.h"
#include "rommanager.h"
#include "stringutils.h"
#include "is_utils.h"
#include "sisfile.h"
#include "sisregistrypackage.h"
#include "sisregistryobject.h"
#include "configmanager.h"

// Constants
#ifndef __TOOLS2_LINUX__
const std::wstring SisRegistry::KPathToRegistry(L"\\sys\\install\\sisregistry/");
const std::wstring SisRegistry::KPathToRomStubs(L"\\system\\install\\");
const std::wstring SisRegistry::KPathToRegistryVersionMarker(L"\\system\\data\\");
#else
const std::wstring SisRegistry::KPathToRegistry(L"/sys/install/sisregistry/");
const std::wstring SisRegistry::KPathToRomStubs(L"/system/install/");
const std::wstring SisRegistry::KPathToRegistryVersionMarker(L"/system/data/");
#endif
const std::wstring KRegistryEntry(L"00000000.reg");
const std::wstring KControllerEntry(L"00000000_0000.ctl");
const std::wstring KBackupLst(L"backup.lst");

const int SisRegistry::KSisRegistryMajorVersion = 5;
const int SisRegistry::KSisRegistryMinorVersion = 1;

// Supported SisRegistry versions
const std::wstring SisRegistry::KRegistryFormat(L"sisregistry_?.?.txt");
const std::wstring SisRegistry::KRegistryV40(L"sisregistry_4.0.txt");
const std::wstring SisRegistry::KRegistryV51(L"sisregistry_5.1.txt");
const std::wstring SisRegistry::KRegistryV52(L"sisregistry_5.2.txt");
const std::wstring SisRegistry::KRegistryV53(L"sisregistry_5.3.txt");
const std::wstring SisRegistry::KRegistryV54(L"sisregistry_5.4.txt");
#ifndef __TOOLS2_LINUX__
const std::wstring SisRegistry::KRomStubDir(L"z:\\system\\install\\");
const std::wstring SisRegistry::KRomRegistryVersionMarker(L"z:\\system\\data\\");
#else
const std::wstring SisRegistry::KRomStubDir(L"z:/system/install/");
const std::wstring SisRegistry::KRomRegistryVersionMarker(L"z:/system/data/");
#endif
const std::string SisRegistry::KRegistryV40string("4.0");
const std::string SisRegistry::KRegistryV50string("5.0");
const std::string SisRegistry::KRegistryV51string("5.1");
const std::string SisRegistry::KRegistryV52string("5.2");
const std::string SisRegistry::KRegistryV53string("5.3");
const std::string SisRegistry::KRegistryV54string("5.4");

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
const std::wstring SisRegistry::NativeSoftwareType(L"native");

#ifdef _MSC_VER
// "." and ".ctl"
// VC does not have ".." entry
const int KDirFileCount = 2;

#else
// "." ".." and ".ctl"
const int KDirFileCount = 3;

#endif

#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

// KRegDirFileCount is the number of directory entries present
// in the reg directory when there is no upgrade is made to the
// package. i.e. one .reg and one .ctl file
//
#ifdef _MSC_VER
// "." ".reg" and ".ctl"
// VC does not have ".." entry
const int KRegDirFileCount = 3;

#else
// "." ".." ".reg" and ".ctl"
const int KRegDirFileCount = 4;

#endif

// function object for determining if all characters are valid hex chars
struct AllHexDigits
    {
	bool result;

	AllHexDigits() : result(true) {}
	void operator()(char a)
	    {
		if (!((a >= '0' && a <= '9') ||
			(a >= 'a' && a <= 'f') ||
			(a >= 'A' && a <= 'F')))
		    {
			result = false;
		    }
	    }
    };

// function object for retreiving keys from the sisregistry entry map
struct CopyUids
    {
	CopyUids(std::vector<TUint32>& uids) : iUids(uids) {}
	std::vector<TUint32>& iUids;

    void operator()(SisRegistry::EntryMap::value_type& a)
	{ iUids.push_back(a.first); }
    };


struct FindFile
    {
	FindFile(const std::wstring& aFile) : iFile(aFile), iFileDescr(0) {}

	bool operator()(const SisRegistry::EntryMap::value_type&  val)
	    {
	    const SisRegistryObject& obj = *val.second;
	    return (iFileDescr = obj.GetFile(iFile)) != 0;
	    }

    std::wstring iFile;
	const FileDescription* iFileDescr;
    };


struct FindFile2
    {
	FindFile2(TUint32 aSid, const FileDescription*& aFd) :
		iSid(aSid), iFileDescr(aFd) {}

	bool operator()(const SisRegistry::EntryMap::value_type& val)
	    {
		const SisRegistryObject& obj = *val.second;
		if (iFileDescr = obj.GetFile(iSid))
		    {
			return true;
		    }
		return false;
	    }

	TUint32 iSid;
	const FileDescription*& iFileDescr;
    };


struct Contains
    {
	const std::wstring& iFile;

	Contains(const std::wstring& aFilename) : iFile(aFilename)
	{}

	SisRegistryObject* operator()(const SisRegistry::EntryMap::value_type& aObj)
	    {
		if (aObj.second->GetFile(iFile) != 0)
			{
			return aObj.second;
			}
    	}
    };

SisRegistry::SisRegistry( CParameterList& aOptions, RomManager& aRomManager, ConfigManager& aConfigManager)
:   iParamList( aOptions ), iRomManager( aRomManager ), iConfigManager( aConfigManager )
    {
	// Determine which version of the SisRegistry to generate
	if (iParamList.RegistryVersionExists() == false)
		{
		ReadSisRegistryVersion();
		}
		
	if(iParamList.RegistryVersionExists())
		{
		LINFO(L"Using SIS Registry v" << iParamList.RegistryMajorVersion() << L"." << iParamList.RegistryMinorVersion());
    
		std::wstring registryPathDriveC = KPathToRegistry;

		ConvertToLocalPath( registryPathDriveC, iParamList.SystemDrivePath() );
	
		ReadRegistry( registryPathDriveC );
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// this code is for handling the database registry implementation
	else
		{
		LINFO(L"Using database implementation of SIS Registry");
		
		int val = iParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet);
		if	( 
			(iParamList.RomLogFileNames().size() != 0 && iParamList.RomDrivePath().size() == 0 ) ||
			iParamList.IsFlagSet( CParameterList::EFlagsDisableZDriveChecksSet) 
			)
			{
			iDbHelper = new DbHelper(iParamList.SystemDrivePath(), true);	
			}
		else
			{
			iDbHelper = new DbHelper(iParamList.RomDrivePath(), false);			
			}		
		}
	#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

	GenerateStubRegistry();
    }


SisRegistry::~SisRegistry()
    {
	if(iParamList.RegistryVersionExists())
		{
		EntryMap::const_iterator end = iEntries.end();
		for (EntryMap::const_iterator curr = iEntries.begin() ;
			curr != end; ++curr)
			{
			delete 	curr->second;
			}
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		delete iDbHelper;
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    }
/**
 * If the ROM drive has been defined, find the marker file in the stub directory 
 * e.g. z:\system\data\. If using the ROM/ROFS logs, find the marker file in the
 * logs. If not found, then check to see if the marker file exists in the ROM stub directory
 * specified using the -t option. If still not found, use the default version of the Sis Registry.
 */
void SisRegistry::ReadSisRegistryVersion()
	{
	// ROM drive defined
	if (iParamList.IsFlagSet(CParameterList::EFlagsZDriveSet)) 
		{
		// ROM SisRegistry marker file
		std::wstring stubPathZ = KPathToRegistryVersionMarker;
		ConvertToLocalPath( stubPathZ, iParamList.RomDrivePath() );

		FindMarkerAndSetVersion(stubPathZ);
		}
	// ROM/ROFS logs defined
	else if (iParamList.IsFlagSet(CParameterList::EFlagsRomRofsLogFilesSet))
		{
			// Check the stub dir
			if (iParamList.IsFlagSet(CParameterList::EFlagsStubDirectorySet))
				{
				// ROM SisRegistry marker file
				std::wstring customStubPath = iParamList.SisStubPath();

				FindMarkerAndSetVersion(customStubPath);
				}
			
			// Search in the ROM/ROFS logs. This takes precedence over the stub dir
			if(iRomManager.RomFileExists(KPathToRegistryVersionMarker+KRegistryV40))
			{
				iParamList.SetRegistryMajorVersion(4);
				iParamList.SetRegistryMinorVersion(0);
			}
			else if(iRomManager.RomFileExists(KRomRegistryVersionMarker+KRegistryV51))
			{
				iParamList.SetRegistryMajorVersion(5);
				iParamList.SetRegistryMinorVersion(1);
			}
			else if(iRomManager.RomFileExists(KRomRegistryVersionMarker+KRegistryV52))
			{
				iParamList.SetRegistryMajorVersion(5);
				iParamList.SetRegistryMinorVersion(2);
			}
			else if(iRomManager.RomFileExists(KRomRegistryVersionMarker+KRegistryV53))
			{
				iParamList.SetRegistryMajorVersion(5);
				iParamList.SetRegistryMinorVersion(3);
			}
			else if(iRomManager.RomFileExists(KRomRegistryVersionMarker+KRegistryV54))
			{
				iParamList.SetRegistryMajorVersion(5);
				iParamList.SetRegistryMinorVersion(4);
			}
		}
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// No valid ROM stub marker (sisregistry_x.x.txt) found!
	// Revert to the default (5.1) SisRegistry version as defined 
	// by KSisRegistryMajorVersion and KSisRegistryMinorVersion
	if (iParamList.RegistryMajorVersion() == 0 && iParamList.RegistryMinorVersion() == 0)
		{
		iParamList.SetRegistryMajorVersion(KSisRegistryMajorVersion);
		iParamList.SetRegistryMinorVersion(KSisRegistryMinorVersion);
		}
	#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	}


void SisRegistry::FindMarkerAndSetVersion(const std::wstring& aPath)
	{
	// Make sure the system\data directory name is properly terminated.
	std::wstring basePath = StringUtils::EnsureDirectoryTerminated( aPath );

	std::list<std::wstring> systemDir;
	GetDirContents( basePath, systemDir );

	// Loop on the system\data files until we find a SisRegistry marker file
	std::list<std::wstring>::iterator curr = systemDir.begin();
	while (curr != systemDir.end())
		{
		std::wstring fileFound ( *curr );

		// Found the SisRegistry marker file
		if (fileFound.compare(KRegistryV40) == 0)
			{
			iParamList.SetRegistryMajorVersion(4);
			iParamList.SetRegistryMinorVersion(0);
			}
		else if (fileFound.compare(KRegistryV51) == 0) // introduced isRemovable flag
			{
			iParamList.SetRegistryMajorVersion(5);
			iParamList.SetRegistryMinorVersion(1);
			}
		else if (fileFound.compare(KRegistryV52) == 0) // fixed DEF114705 - SIS controller incorrect
			{
			iParamList.SetRegistryMajorVersion(5);
			iParamList.SetRegistryMinorVersion(2);
			}
		else if (fileFound.compare(KRegistryV53) == 0) // introduced SU cert flag
			{
			iParamList.SetRegistryMajorVersion(5);
			iParamList.SetRegistryMinorVersion(3);
			}
		else if (fileFound.compare(KRegistryV54) == 0) // Support for exact & equivalent device supported languages
			{
			iParamList.SetRegistryMajorVersion(5);
			iParamList.SetRegistryMinorVersion(4);
			}

			++curr;
		}
	}




bool SisRegistry::IsPackageDir(const std::wstring& aString)
    {
	AllHexDigits digits;
	std::for_each(aString.begin(), aString.end(), digits);
	return aString.length() == 8 && digits.result;
    }

TBool SisRegistry::IsInstalled(TUint32 aUid) const
    {
	if(iParamList.RegistryVersionExists())
		{
		return iEntries.find(aUid) != iEntries.end();;
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		return iDbHelper->IsInstalled(aUid);
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	return false;
    }


const SisRegistryObject& SisRegistry::GetRegistryObject(TUint32 aUid) const
    {
	if(iParamList.RegistryVersionExists())
		{
		EntryMap::const_iterator end = iEntries.end();
		for (EntryMap::const_iterator curr = iEntries.begin() ;
			 curr != end; ++curr)
			{
			if ((curr->first == aUid) && ((*curr->second).GetIndex() == 0))
				{
				return *curr->second;
				}
			}
		}
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		SisRegistryObject* sisRegistryObject = new SisRegistryObject(iConfigManager);
		if(iDbHelper->InternalizeIfExists(*sisRegistryObject, aUid))
			{
			return *sisRegistryObject;
			}
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	throw std::runtime_error("Couldn't find specified package");
    }

SisRegistryObject* SisRegistry::GetRegistryObjectPtr(TUint32 aUid)
    {
	if(iParamList.RegistryVersionExists())
		{
		EntryMap::const_iterator end = iEntries.end();
		for (EntryMap::const_iterator curr = iEntries.begin() ;
			 curr != end; ++curr)
			{
			if ((curr->first == aUid) && ((curr->second)->GetIndex() == 0))
				{
				return curr->second;
				}
			}
		}
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		SisRegistryObject* sisRegistryObject = new SisRegistryObject(iConfigManager);
		if(iDbHelper->InternalizeIfExists(*sisRegistryObject, aUid))
			{
			return sisRegistryObject;
			}
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

	throw std::runtime_error("Couldn't find specified package");
    }

const SisRegistryPackage& SisRegistry::SidToPackage(TUint32 aSid)
    {
	const FileDescription* fileDesc = 0;
	FindFile2 fileFinder(aSid, fileDesc);

  	EntryMap::iterator x = std::find_if(iEntries.begin(),
										iEntries.end(),
										fileFinder);

	if (x != iEntries.end())
	    {
		return *x->second;
	    }
	else
	    {
		throw std::runtime_error("Couldn't find specified file");
	    }
    }


void SisRegistry::SidToFileName(TUint32 aSid, std::wstring& aFileName)
    {
	if(iParamList.RegistryVersionExists())
		{
		const FileDescription* fileDesc = 0;
		FindFile2 fileFinder(aSid, fileDesc);

  		EntryMap::iterator x = std::find_if(iEntries.begin(),
											iEntries.end(),
											fileFinder);

		if (x != iEntries.end())
			{
			aFileName = fileFinder.iFileDescr->GetTarget();
			}
		else
			{
			throw std::runtime_error("Couldn't find specified file");
			}
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		aFileName = iDbHelper->SidToFileName(aSid);	
		if(aFileName.empty())
			{
			throw std::runtime_error("Couldn't find specified file");
			}
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    }


bool SisRegistry::SidExists(TUint32 aSid) const
    {
	if(iParamList.RegistryVersionExists())
		{
		const FileDescription* fileDesc = 0;
		FindFile2 fileFinder(aSid, fileDesc);

  		EntryMap::const_iterator x = std::find_if(iEntries.begin(),
												iEntries.end(),
												fileFinder);

		return x != iEntries.end();
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		return iDbHelper->SidExists(aSid);
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	return false;
    }


void SisRegistry::RemoveEntry(const TUint32 aUid, const std::wstring& aPackageName, const std::wstring& aVendorName)
    {
	if(iParamList.RegistryVersionExists())
		{
		EntryMap::iterator it = iEntries.find(aUid);
	
		while(it != iEntries.end())
	    	{
			if (aUid == it->first && !wcscmp(aPackageName.c_str(), it->second->GetPackageName().c_str()) && aVendorName == it->second->GetVendorName() )
				{
				LINFO(L"Removing package \"" << it->second->GetPackageName() 
						<< L"\" prior to re-installation");

				// Remove .reg file
				std::wstring registryPath = it->second->GetRegistryDir( iParamList.SystemDrivePath() );
				RemoveFile(registryPath + StringUtils::BuildSisRegistryFileName( it->second->GetIndex() ));
				
				// Remove .ctl file
				int drive = tolower(it->second->GetSelectedDrive());
				std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(drive);
				registryPath = it->second->GetRegistryDir( localTargetPath );
				RemoveFile(registryPath + StringUtils::BuildControllerFileName( it->second->GetIndex(), 0 ));
				
				// Remove the installed files for this SisRegistryObject
				const FileDescriptions& files = it->second->GetFileDescriptions();

				RemoveInstalledFiles(files, localTargetPath);

				// Clean up the SisRegistryObject
				delete it->second;
				iEntries.erase(it);

				return;
				}
			++it;
	    	}
    	}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{

		// the vendor name here is the unique vendore name
		TInt32 componentId = iDbHelper->GetComponentId(aUid, aPackageName, aVendorName);
		if(componentId == DbHelper::Absent)
			{
			return;
			}

		LINFO(L"Removing package \"" << aPackageName << L"\" prior to re-installation");

		// Remove .ctl file
		int drive = tolower(iDbHelper->GetSelectedDrive(componentId));
		std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(drive);
		std::wstring registryPath = GetRegistryDir( localTargetPath,componentId );
		RemoveFile(registryPath + StringUtils::BuildControllerFileName( iDbHelper->GetIndex(componentId), 0 ));
		
		// Remove the installed files for this SisRegistryObject
		const FileDescriptions& files = iDbHelper->GetFileDescriptions(componentId);

		RemoveInstalledFiles(files, localTargetPath);

		iDbHelper->RemoveEntry(componentId);

		}
	#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    }

void  SisRegistry::RemovePkg(const TUint32& aUid, bool aSkipRomStub)
    {
	if(iParamList.RegistryVersionExists())
    	{
		RemovePkgLegacy(aUid,aSkipRomStub);
    	}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		RemovePkgWithScr(aUid,aSkipRomStub);
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    }

void  SisRegistry::RemovePkgLegacy(const TUint32& aUid, bool aSkipRomStub)
	{
	EntryMap::iterator currObj = iEntries.find(aUid);
		
	if (currObj == iEntries.end())
		{
		// No SisRegistry file found!!
		std::stringstream err;
		err << " SisRegistry entry for 0x" << std::hex << aUid << L" Not found";

		throw std::runtime_error(err.str());
		}

	std::list<std::wstring> regDirFiles;
	std::wstring path = currObj->second->GetRegistryDir( iParamList.SystemDrivePath() );

    GetDirContents(path, regDirFiles);
    
	if (regDirFiles.size() == KRegDirFileCount && currObj->second->GetInRom())
		{
		// I.e. there are only .., one .reg and one .ctl file present
		// check whether it is a ROM stub object (without any upgrades)

		if (aSkipRomStub)
			{
			// Since this is only a ROM stub entry, leave it as it is;
			// to avoid regenerate it again.
			return;
			}

		std::stringstream err;
		err << "Illegal uninstall of ROM package 0x" << std::hex << aUid;
		throw InterpretSisError(err.str(), ATTEMPT_TO_UNINSTALL_ROM_PKG);
		}

	std::list<std::wstring>::iterator currFile = regDirFiles.begin();

	// Remove all the .reg and .ctl files in the system drive for this package
	while (currFile != regDirFiles.end())
	    {
		if (*currFile != L"." && *currFile != L"..")
		    {
			RemoveFile(path + *currFile);
		    }

		++currFile;
	    }

	// Remove the installed files for all packages (SA, SP, PU)
	while (currObj != iEntries.end())
		{
		// For ROM stub, the SelectedDrive = 0; as there could be ROM->SP.
		// Therefore, the rest can only apply to at least a ROM stub upgrades
		int drive = tolower(currObj->second->GetSelectedDrive());
		
		if (drive)
			{
			std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(drive);
			path = currObj->second->GetRegistryDir( localTargetPath );

			regDirFiles.clear();
			GetDirContents(path, regDirFiles);

			currFile = regDirFiles.begin();

			// Remove all the .ctl files for this package
			while (currFile != regDirFiles.end())
				{
				if (*currFile != L"." && *currFile != L"..")
					{
					RemoveFile(path + *currFile);
					}
				++currFile;
				}

			// This iterator's object has the same Uid as aUid
			const FileDescriptions& files = currObj->second->GetFileDescriptions();

			RemoveInstalledFiles(files, localTargetPath);
			}


		// Clean up the SisRegistryObject
		delete currObj->second;
		iEntries.erase(currObj);

		currObj = iEntries.find(aUid);
		}
	}

std::wstring SisRegistry::GetRegistryDir( const std::wstring& aDrivePath, TUint32 aUid ) const
	{
    std::wstring ret = StringUtils::MakePathFromSID( aDrivePath + L"/sys/install/sisregistry/", aUid );
	return ret;
	}

void SisRegistry::RemoveInstalledFiles(const FileDescriptions& aFileDes, std::wstring& aLocalPath)
	{
	FileDescriptions::const_iterator curr = aFileDes.begin();
	FileDescriptions::const_iterator end  = aFileDes.end();

	for ( ; curr != end ; ++curr)
		{
		std::wstring target((*curr)->GetTarget());
	
		ConvertToLocalPath(target, aLocalPath);
		RemoveFile(target);
		RemoveHashForFile(target, iParamList.SystemDriveLetter(), iParamList.SystemDrivePath());
		}
	}


void SisRegistry::AddRegistryEntry( const SisFile& aSis, const InstallableFiles& aFiles,
								   const InstallSISFile& aInstallSISFile )
    {
    SisRegistryObject* obj = new SisRegistryObject( aSis, aFiles, aInstallSISFile.iTargetDrive, 
		iParamList.RegistryMajorVersion(), iParamList.RegistryMinorVersion(), aInstallSISFile.iSUFlag, iConfigManager );

	// Generate .reg and .ctl file for native installation.
	// Installation based on database registration, generate only the .ctl file.
	GenerateRegistryEntry(*obj, aSis);

	if(iParamList.RegistryVersionExists())
		{	
	// Add the registry entry to the registry 
	iEntries.insert(EntryMap::value_type(obj->GetUid(),obj));
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		// if the entry already exists then remove it first
		// this is the case where an in rom application is getting 
		// overwritten by an SA type.
		// in case of db the entry is stored in the database from where first it has 
		// to be removed and then readded based on the updated registry object which is 
		// in memory. In case of file based registry update this is done is a separate
		// way, it reads the existing registry entry and store it in memory, and updates the same 
		// entry in memory and then over-writes the old registry entry with the update entry.
		TInt32 componentId = iDbHelper->GetComponentId(obj->GetUid(), obj->GetInstallType() );

		if(componentId != DbHelper::Absent)
			{
			std::wstring strStubFile = iDbHelper->GetStubFileName(componentId);
			if(strStubFile.length())
				{
				obj->SetStubFileName(strStubFile);
				}
			
			iDbHelper->RemoveEntry(componentId);
			}

		obj->SetValidationStatus(iParamList.OriginVerificationStatus());
		GenerateDbRegistryEntry(*obj, iParamList.OriginVerificationStatus());
		}
	#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	}

void SisRegistry::UpdateRegistryEntry(const SisFile& aSis,
		const InstallableFiles& aFiles,	const InstallSISFile& aInstallSISFile)
	{
	TUint32 pkg = aSis.GetPackageUid();

	SisRegistryObject* obj = GetRegistryObjectPtr(pkg);
	obj->UpgradeEntry(aSis, aFiles, aInstallSISFile.iTargetDrive, aInstallSISFile.iSUFlag, aInstallSISFile.iNonRemovable, iConfigManager);

 	// Update/Create the .reg and .ctl files
	GenerateRegistryEntry(*obj, aSis);
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	if(!iParamList.RegistryVersionExists())
		{
		TUint32 componentId = iDbHelper->GetComponentId(pkg);
		std::wstring strStubFile = iDbHelper->GetStubFileName(componentId);
		if(strStubFile.length())
			{
			obj->SetStubFileName(strStubFile);
			}
		
		// remove the application from database
		iDbHelper->RemoveEntry(componentId);

	    obj->SetValidationStatus(iParamList.OriginVerificationStatus());
		GenerateDbRegistryEntry(*obj, iParamList.OriginVerificationStatus());
		}

	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	}
	
const SisRegistryObject* SisRegistry::OwningPackage( const std::wstring& aFilename, TUint32& aUid ) const
	{
	if(iParamList.RegistryVersionExists())
		{
		// Loop through iEntries, then loop through file desc for spec UID, then return the obj
		for (EntryMap::const_iterator curr = iEntries.begin() ;
			 curr != iEntries.end() ;
			 ++curr)
			{
			 // Get the file descriptions
			 if (curr->second->GetFile(aFilename) != 0)
				 {
				 aUid = curr->first;
				 return curr->second;
				 }
			}
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		aUid = iDbHelper->GetUidFromFileName(aFilename);
		if(aUid != 0)
			{
			SisRegistryObject* sisRegistryObject = new SisRegistryObject(iConfigManager);
			if(iDbHelper->InternalizeIfExists(*sisRegistryObject,aUid))
				{
				return sisRegistryObject;
				}
			}
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	return NULL;
    }


void SisRegistry::RemoveBackupLst()
    {
	std::wstring backup = KPathToRegistry + KBackupLst;
	ConvertToLocalPath( backup, iParamList.SystemDrivePath() );

    if  ( FileExists( backup ) )
        {
		RemoveFile( backup );
        }
    }


void SisRegistry::ReadStubs( const std::wstring& aDirectory )
    {
    // Make sure the directory name is properly terminated.
    std::wstring basePath = StringUtils::EnsureDirectoryTerminated( aDirectory );

	std::list<std::wstring> stubDirs;
	GetDirContents( basePath, stubDirs );

	std::list<std::wstring>::iterator curr;
	
	for(curr = stubDirs.begin(); curr!= stubDirs.end(); ++curr)
	    {
		std::wstring stubPath ( basePath + *curr );
				
		if( !IsSisValid(stubPath) )
			continue;

		_TCHAR* fileName = const_cast<_TCHAR *>(stubPath.c_str());
		std::auto_ptr<CSISController> sisController(ReadStubController( fileName ));

		if (!sisController.get())
			{
			LERROR(L"\tCannot create CSISController for "<< stubPath);
			continue;
			}

		if (IsInstalled(sisController->UID1()))
			{
			// Has already been upgraded
			continue;
			}

		std::wstring stubFileName = L"z:";
		stubFileName.append(KPathToRomStubs);
		stubFileName.append(*curr);

		SisRegistryObject* aObj = new SisRegistryObject(*sisController, iRomManager,
														iParamList.RegistryMajorVersion(), 
														iParamList.RegistryMinorVersion(), 
														iConfigManager, stubFileName);
		GenerateStubRegistry(aObj,sisController.get());	
		    
		} // for
    }


TBool SisRegistry::IsSisValid(const std::wstring& aStubPath)
	{
	int dotIndex = aStubPath.rfind(L".");
	if(dotIndex == std::wstring::npos)
		return false;
	std::wstring extOfString = aStubPath.substr(dotIndex+1);
	std::wstring extOfStringUpper = StringUtils::ToUpper(extOfString);
	if (extOfStringUpper != L"SIS" && extOfStringUpper != L"SISX")
		return false;
	return true;
	}

void SisRegistry::GenerateStubRegistry(SisRegistryObject* aSisRegistryObject, CSISController* aSisController)
	{
	// create the directory to be used for storing reg and ctl files
	std::wstring regPath = aSisRegistryObject->GetRegistryDir( L"" );
	ConvertToLocalPath( regPath, iParamList.SystemDrivePath() );
	if ( !MakeDir( regPath ) )
		{
		throw InterpretSisError(L"Directory Creation Error - "+regPath,	DIRECTORY_CREATION_ERROR);
		}

	LINFO(L"\tAdding ROM Stub: 0x" << std::hex << aSisRegistryObject->GetUid() << L" \"" << aSisRegistryObject->GetPackageName() << L"\"");
			
	if(iParamList.RegistryVersionExists())
		{
		iEntries.insert( EntryMap::value_type( aSisRegistryObject->GetUid(), aSisRegistryObject ) );

		// Create the ROM Stub registry entries
		
		std::fstream regFile;
		std::wstring reg = regPath+KRegistryEntry;

		OpenFile( reg, regFile, std::ios::out | std::ios::binary );

		Serialiser s(regFile);
						
		// Create the .reg file:
		s << aSisRegistryObject;
			
		regFile.close();
		}
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		GenerateDbRegistryEntry(*aSisRegistryObject, iParamList.OriginVerificationStatus());
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

	// Create the .ctl file:
	std::fstream ctlFile;
	OpenFile( regPath+KControllerEntry, ctlFile, std::ios::out | std::ios::binary );

	const unsigned char* ctl = aSisController->RawBuffer();
	int  len = aSisController->BufferSize();

	ctlFile.write((const char*)&len, 4);
	ctlFile.write((char*)ctl, len);
	ctlFile.close();
	}

bool SisRegistry::IsFileWideCard(const std::wstring& aFileName)
	{
	return (aFileName.find(L'?') != std::wstring::npos || 
			aFileName.find(L'*') != std::wstring::npos || 
			#ifndef __TOOLS2_LINUX__
			aFileName[aFileName.length() - 1] == L'\\');
			#else
			aFileName[aFileName.length() - 1] == L'/');
			#endif
	}


void SisRegistry::GenerateStubRegistry()
	{
	if (!iParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet)) 
		{
		// If the -z option were used to launch the tool, then read any
		// SIS stubs that we have gathered from the emulated z:\system\install
		// location.
		if (iParamList.IsFlagSet(CParameterList::EFlagsZDriveSet)) 
			{
			std::wstring stubPathZ = KPathToRomStubs;
			ConvertToLocalPath( stubPathZ, iParamList.RomDrivePath() );
			ReadStubs( stubPathZ );
			}

		// If an additional SIS stub path were specified, we'll
		// also handle that here too.
		if (iParamList.IsFlagSet(CParameterList::EFlagsStubDirectorySet))
			{
			bool readCustomStubPath = true;
			std::wstring customStubPath = iParamList.SisStubPath();

			// But make sure we don't read the same directory twice!
			const bool haveRomDrive = iParamList.IsFlagSet(CParameterList::EFlagsZDriveSet);
			if ( haveRomDrive )
				{
				std::wstring stubPathZ = KPathToRomStubs;
				ConvertToLocalPath( stubPathZ, iParamList.RomDrivePath() );
				readCustomStubPath = ( stubPathZ != customStubPath );
				}

			if ( readCustomStubPath )
				{
				ReadStubs( customStubPath );
				}
			}
		}
	}


void SisRegistry::ReadRegistry( const std::wstring& aRegistryBasePath )
    {
    std::list<std::wstring> regDirs;
    GetDirContents( aRegistryBasePath, regDirs );
	
	std::list<std::wstring>::iterator curr = regDirs.begin();

	LINFO(L"Reconstructing SIS Registry...");
	
	for( curr = regDirs.begin(); curr != regDirs.end(); ++curr )
	    {
		struct stat s;
		const std::wstring entryPath = aRegistryBasePath + *curr;
		const int err = GetStat(entryPath.c_str(), &s);

		if ( err )
			continue;
		    
		const bool isDir = (s.st_mode & S_IFDIR) != 0;

        if (isDir && IsPackageDir(*curr))
            {
			ExtractRegistryFiles(entryPath);
            }        
	    }
    }


void SisRegistry::ExtractRegistryFiles(const std::wstring& aPath)
	{
	std::list<std::wstring> contents;
	GetDirContents(aPath, contents);
	
	for (std::list<std::wstring>::iterator c = contents.begin(); c != contents.end(); ++c)
		{
		if (c->find(L".reg",0) == std::wstring::npos)
			continue;
		
		std::fstream input;
		#ifndef __TOOLS2_LINUX__
		std::wstring filename(aPath + L"\\" + *c);
		#else
		std::wstring filename(aPath + L"/" + *c);
		#endif
		
		OpenFile(filename.c_str(), input, std::fstream::in | std::fstream::binary);
		
		if (!input.good())
			continue;
		
		SisRegistryObject* obj = new SisRegistryObject(iConfigManager);
					
		if (!obj)
			{
			throw std::runtime_error("Cannot create SisRegistryObject for " + wstring2string(filename));
			}
		Deserialiser des(input);
		
		des >> *obj;
		iEntries.insert(EntryMap::value_type(obj->GetUid(), obj));

		LINFO(L"\tExtracting: 0x" << std::hex << obj->GetUid() << L" \"" << obj->GetPackageName() << L"\"");
		
		}
	}


bool SisRegistry::IsRomStubPackage(const TUint32 aUid)
	{
	CSISController* sisController = GetStubController(aUid);

	if (sisController)
		{
		delete sisController;
		return true;
		}

	return false;
	}


void SisRegistry::GetStubFileEntries(const TUint32 aUid, std::list<std::wstring>& aFilesEntries)
	{
	std::auto_ptr<CSISController> sisController(GetStubController(aUid));

	if (!sisController.get())
		return;
		const CSISInstallBlock& installBlock =  sisController->InstallBlock();
		int fileCount = installBlock.FileCount();
		
		for (int i = 0; i < fileCount; ++i)
			{
			std::wstring target(installBlock.FileDescription(i).Target().GetString());
			aFilesEntries.push_back(target);
			}
	}


CSISController* SisRegistry::GetStubController(const TUint32 aUid)
	{
	if (iParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet)) 
		{
		return NULL;
		}
	// If the -z option were used to launch the tool, then read any
	// SIS stubs that we have gathered from the emulated Z:\System\Install
	// location.
	if (iParamList.IsFlagSet(CParameterList::EFlagsZDriveSet)) 
		{
		std::wstring stubPathZ = KPathToRomStubs;
		ConvertToLocalPath( stubPathZ, iParamList.RomDrivePath() );
		CSISController* sisController = GetStubControllerInDir( stubPathZ, aUid );

		if (sisController)
			{
			return sisController;
			}
		}

	// If an additional SIS stub path were specified, we'll
	// also handle that here too.
	if (iParamList.IsFlagSet(CParameterList::EFlagsStubDirectorySet))
		{
		bool readCustomStubPath = true;
		std::wstring customStubPath = iParamList.SisStubPath();

		// But make sure we don't read the same directory twice!
		const bool haveRomDrive = iParamList.IsFlagSet(CParameterList::EFlagsZDriveSet);
		if ( haveRomDrive )
			{
			std::wstring stubPathZ = KPathToRomStubs;
			ConvertToLocalPath( stubPathZ, iParamList.RomDrivePath() );
			readCustomStubPath = ( stubPathZ != customStubPath );
			}

		if ( readCustomStubPath )
			{
			CSISController* sisController = GetStubControllerInDir( customStubPath, aUid );

			if (sisController)
				{
				return sisController;
				}
			}
		}
	return NULL;
	}


CSISController* SisRegistry::GetStubControllerInDir( const std::wstring& aDirectory, const TUint32 aUid)
	{
    // Make sure the directory name is properly terminated.
    std::wstring basePath = StringUtils::EnsureDirectoryTerminated( aDirectory );

	std::list<std::wstring> stubDirs;
	GetDirContents( basePath, stubDirs );

	std::list<std::wstring>::iterator curr;
	for (curr = stubDirs.begin(); curr != stubDirs.end() ; ++curr)
		{
		std::wstring stubPath ( basePath + *curr );
		int dotIndex = stubPath.rfind(L".");
		if(dotIndex == std::wstring::npos)
			continue;
		
		std::wstring extOfString = stubPath.substr(dotIndex+1);

        std::wstring extOfStringUpper = StringUtils::ToUpper(extOfString);
		if (extOfStringUpper != L"SIS" && extOfStringUpper != L"SISX")
			continue;
		
		_TCHAR* fileName = const_cast<_TCHAR *>(stubPath.c_str());
		CSISController* sisController = ReadStubController( fileName );
        
		if (!sisController)
			continue;
		
		if (sisController->UID1() == aUid)
			{
			return sisController;
			}
		delete sisController;	
		}
	return NULL;
	}


CSISController* SisRegistry::ReadStubController( const wchar_t* aFileName )
	{
	const bool isSisFile = CSISContents::IsSisFile(aFileName);
	CSISController* sisController = NULL;
	
	if (isSisFile)
    	{
		LWARN(L"\tIgnoring invalid SIS stub: " << aFileName);
		return sisController;
    	}
	SisRegistryObject* aObj = NULL;
	
	TSISStream input;
	std::wstring fileName = FixPathDelimiters(aFileName);
	HANDLE file = ::MakeSISOpenFile(fileName.c_str(),GENERIC_READ,OPEN_EXISTING);
	if (INVALID_HANDLE_VALUE == file)
		{
		LERROR(L" Error : Cannot open file : " << fileName << std::endl);
		throw CSISException(CSISException::EFileProblem, std::wstring (L"cannot open ") + fileName);
		}
	bool success = input.import (file, NULL);
	::CloseHandle(file);
	if(!success)
		{
		LERROR(L" Error : Cannot read file : " << fileName << std::endl);
		throw CSISException(CSISException::EFileProblem, std::wstring (L"cannot read ") + fileName);
		}
	
	sisController = new CSISController;
	sisController->Read(input, input.length());

	return sisController;
	}

void SisRegistry::GenerateRegistryEntry(SisRegistryObject& aObj, const SisFile& aSis)
	{

	SetNextIndex(aObj);

	if(iParamList.RegistryVersionExists())
		{
		GenerateRegFile(aObj);
		}

	GenerateCtlFile(aObj, aSis);
	}

void SisRegistry::SetNextIndex(SisRegistryObject& aObj) const
	{
	TUint32 pkgUid = aObj.GetUid();
	TUint32 installType = aObj.GetInstallType();	
	int index = 0;

	if(iParamList.RegistryVersionExists())
		{
		if (installType == CSISInfo::EInstAugmentation)
			{
			// set the .reg and .ctl file index for SP installation
			index = aObj.NextSisRegistryIndex( iParamList.SystemDrivePath() );

			if (index == 0)
				{
				// No SisRegistry file found!!
				std::wstring msg = StringUtils::MakePathFromSID ( iParamList.SystemDrivePath()+L"\\", pkgUid );
				
				msg.append(L" does not have any SisRegistry file");
				
				throw std::runtime_error( wstring2string(msg) );
				}

			aObj.SetIndex( index );

			if ( !CheckSisRegistryDirPresent(iParamList.SystemDrivePath(), pkgUid) )
				{
				// c:\sys\install\sisregistry\<UID>\ directory not exist
				std::wstring msg = StringUtils::MakePathFromSID ( iParamList.SystemDrivePath()+L"\\", pkgUid );
				
				msg.append(L" directory NOT found");

				throw std::runtime_error( wstring2string(msg) );
				}
			}    
		}

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	else
		{
		if (installType == CSISInfo::EInstAugmentation)
			{
			// set .ctl file index for SP installation
			index = iDbHelper->GetIndex(pkgUid);
			
			int nextIndex = index +1;
			aObj.SetIndex( nextIndex );

			if ( !CheckSisRegistryDirPresent(iParamList.SystemDrivePath(), pkgUid) )
				{
				std::wstring msg = StringUtils::MakePathFromSID ( iParamList.SystemDrivePath()+L"\\", pkgUid );
				
				msg.append(L" directory NOT found");

				throw std::runtime_error( wstring2string(msg) );
				}
			}    
		}

	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

	}

void SisRegistry::GenerateRegFile(SisRegistryObject& aObj) const
	{
	TUint32 installType = aObj.GetInstallType();	

	std::wstring regPath = aObj.GetRegistryDir( L"" );
	ConvertToLocalPath( regPath, iParamList.SystemDrivePath() );

	if (installType == CSISInfo::EInstInstallation)
		{
		if ( !MakeDir( regPath ) )
			{
			throw InterpretSisError(L"Directory Creation Error - "+regPath, DIRECTORY_CREATION_ERROR);
			}
		}

		std::fstream regFile;
		std::wstring reg = regPath + StringUtils::BuildSisRegistryFileName(aObj.GetIndex());
		
		OpenFile( reg, regFile, std::ios::out | std::ios::binary );

		Serialiser s(regFile);
		s << aObj;

		regFile.close();
	}

void SisRegistry::GenerateCtlFile(SisRegistryObject& aObj, const SisFile& aSis) const
	{
	TUint32 pkgUid = aObj.GetUid();
	TUint32 installType = aObj.GetInstallType();	
	// Controller
	int index = 0;

	if (installType == CSISInfo::EInstPartialUpgrade)
		{
		index = aObj.NextSisControllerIndex( iParamList.SystemDrivePath() );

		if (index == 0)
			{
			// No SIS controller file found!!
			std::wstring msg = StringUtils::MakePathFromSID ( iParamList.SystemDrivePath()+L"\\", pkgUid );
		
			msg.append(L" does not have any Sis Controller file");

			throw std::runtime_error( wstring2string(msg) );
			}
		}

	// Finally, externalise the new controller file
	std::fstream ctlFile;
	std::wstring ctlPath = aObj.GetRegistryDir( L"" );
	ConvertToLocalPath( ctlPath, iParamList.SystemDrivePath() );

	if (installType == CSISInfo::EInstInstallation)
		{
		if ( !MakeDir( ctlPath ) )
			{
			throw InterpretSisError(L"Directory Creation Error - "+ctlPath, DIRECTORY_CREATION_ERROR);
			}
		}
		
	std::wstring ctrl = ctlPath + StringUtils::BuildControllerFileName( aObj.GetIndex(), index );
	

	OpenFile( ctrl, ctlFile, std::ios::out | std::ios::binary );

	const char* ctl = 0;
	int  len = 0;
	aSis.GetControllerData(ctl, len);
	ctlFile.write((const char*)&len, 4);
	ctlFile.write(ctl, len);
	ctlFile.close();

	int targetDrive = tolower(aObj.GetSelectedDrive());

	if (iConfigManager.IsTargetDriveExt(targetDrive))
		{
		// For external target drive, generate the ctl file to that drive also
		std::wstring ctlPath = aObj.GetRegistryDir( L"" );
		std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(targetDrive);

		ConvertToLocalPath( ctlPath, localTargetPath );

		if ( !MakeDir( ctlPath ) )
			{
			throw InterpretSisError(L"Directory Creation Error - "+ctlPath, DIRECTORY_CREATION_ERROR);
			}
		
		ctrl = ctlPath + StringUtils::BuildControllerFileName( aObj.GetIndex(), index );

		OpenFile( ctrl, ctlFile, std::ios::out | std::ios::binary );

		ctlFile.write((const char*)&len, 4);
		ctlFile.write(ctl, len);
		ctlFile.close();
		}
	}


int SisRegistry::GetAugmentationsNumber(TUint32 aUid)
	{
	int num = 0;
	if(iParamList.RegistryVersionExists())
		{
		EntryMap::const_iterator x = iEntries.begin();
			
		for (;x != iEntries.end(); ++x)
		    {
			if (x->first == aUid && x->second->GetIndex() != 0 )
				++num;
		    }
		}
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	else
		{
		TInt32 componentId = iDbHelper->GetComponentId(aUid);
		TInt index = iDbHelper->GetIndex(componentId);
		if( index != DbHelper::Absent )
			num = ++index;
		}
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	return num;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

std::wstring SisRegistry::FormatVersionToString( const Version& aVersion )
	{
	std::wstringstream versionText;
	versionText << aVersion.GetMajor() << L"." << aVersion.GetMinor() << L"." << aVersion.GetBuild();
	return versionText.str();	
	}

const DbHelper* SisRegistry::GetDbHelper() const
	{
	return iDbHelper;
	}

void SisRegistry::GenerateDbRegistryEntry(const SisRegistryObject& aSisRegistryObject, bool aOriginVerified)
	{
	// store the object in registry database
	XmlDetails::TScrPreProvisionDetail scrPreProvisionDetail;
	// for sis file entries the software type would always be Native.
	scrPreProvisionDetail.iSoftwareTypeName = SisRegistry::NativeSoftwareType;
	XmlDetails::TScrPreProvisionDetail::TComponent component = CreateRegistryEntry(aSisRegistryObject);	
	
	SetOriginVerification(component, aOriginVerified);	
	
	scrPreProvisionDetail.iComponents.push_back(component);
	UpdateInstallationInformation(scrPreProvisionDetail);
	}

void SisRegistry::SetOriginVerification(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, bool aOriginVerified)
	{
	// If not origin verified, set so. By default origin is verified.
	if (!aOriginVerified)
		{
		aComponent.iComponentDetails.iOriginVerified = 0;
		}
	}

void SisRegistry::UpdateInstallationInformation(XmlDetails::TScrPreProvisionDetail aScrPreProvisionDetail)
	{
	CXmlGenerator xmlGenerator;

	#ifndef __TOOLS2_LINUX__
		char* tmpFileName = tmpnam(NULL);	
	#else
		char tmpFileName[] = "/tmp/interpretsis_XXXXXX";	
		int temp_fd;
		temp_fd=mkstemp(tmpFileName); 
		fclose(fdopen(temp_fd,"w"));
	#endif

	std::wstring filename(string2wstring(tmpFileName));

	int isRomApplication = 0;
	xmlGenerator.WritePreProvisionDetails(filename , aScrPreProvisionDetail, isRomApplication);

	#ifdef __TOOLS2_LINUX__
	std::string executable = "scrtool";
	#else
	std::string executable = "scrtool.exe";
	#endif

	std::string command;
	command = executable + " -d " + GetDbPath() + " -p " + tmpFileName;

	int error = system(command.c_str());

	int err = remove(tmpFileName);
	if(err != 0)
		LERROR(L"Temporary file removal failed.");

	if(error != 0)
		{
		std::string err = "Scrtool failed to upload the database registry entry.";
		LERROR(L"Scrtool failed to upload the database registry entry.");
		throw InterpretSisError(err, DATABASE_UPDATE_FAILED);
		}
	}

std::string SisRegistry::GetDbPath()
	{
	if	( 
		(iParamList.RomLogFileNames().size() != 0 && iParamList.RomDrivePath().size() == 0 ) ||
		iParamList.IsFlagSet(CParameterList::EFlagsDisableZDriveChecksSet) 
		)
		{
		#ifndef __TOOLS2_LINUX__
		return wstring2string(iParamList.SystemDrivePath()) + "\\sys\\install\\scr\\scr.db";
		#else
		return wstring2string(iParamList.SystemDrivePath()) + "/sys/install/scr/scr.db";
		#endif
		}
	#ifndef __TOOLS2_LINUX__
		return wstring2string(iParamList.RomDrivePath()) + "\\sys\\install\\scr\\provisioned\\scr.db";
	#else
	    return wstring2string(iParamList.RomDrivePath()) + "/sys/install/scr/provisioned/scr.db";
	#endif
	
	}

std::string SisRegistry::GetEpocRoot()
	{
	const char* epocRoot = getenv("EPOCROOT");	
	if(NULL == epocRoot)
		{
		std::string err = "EPOCROOT environment variable not specified. Please specify it as part of your environment variable." \
		"It should point uptil the start of the epoc32 directory.";
		LERROR(L"EPOCROOT environment variable not specified. Please specify it as part of your environment variable." \
				"It should point uptil the start of the epoc32 directory.");
		throw InterpretSisError(err, ENV_NOT_SPECIFIED);
		}
	std::string epocRootStr(epocRoot); 
	return epocRootStr;
	}


std::wstring SisRegistry::GetGlobalId( TUint32 aUid , TInt aInstallType, std::wstring aPackageName)
	{
	wchar_t textGlobalId[20];
	
	#ifdef _MSC_VER
		swprintf(textGlobalId,L"%08x",aUid);
	#else		
		swprintf(textGlobalId,20,L"%08x",aUid);
	#endif
		

	std::wstring globalId = textGlobalId;
	
	if( aInstallType == CSISInfo::EInstAugmentation || aInstallType == CSISInfo::EInstPreInstalledPatch )
		{
		if(aPackageName.empty())
			{
			std::string err = "Package Name not specified for global id creation.";
			LERROR(L"Package Name not specified for global id creation.");
			throw InterpretSisError(err, GLOBALID_CREATION_ERROR);
			}
		globalId += L"_" + aPackageName;
		}
	return globalId;	
	}

void SisRegistry::RemovePkgWithScr(TUint32 aUid, bool aSkipRomStub)
	{
	TUint32 componentId = iDbHelper->GetComponentId(aUid);

	if ( DbHelper::Absent == componentId )
		{
		// No database entry found!!
		std::stringstream err;
		err << " SisRegistry entry for 0x" << std::hex << aUid << L" Not found";
		throw InterpretSisError(err.str(), DATABASE_ENTRY_MISSING);
		}

	std::list<std::wstring> regDirFiles;
	std::wstring path = GetRegistryDir( iParamList.SystemDrivePath(), aUid );

    GetDirContents(path, regDirFiles);

	// The directory should only consist of the .ctl file. Comparison with 
	// 2 is for the ctl file and link to the parent directory.
	if ( regDirFiles.size() == KDirFileCount && iDbHelper->GetInRom(componentId))
		{
		// I.e. there are only one .ctl file present
		// check whether it is a ROM stub object (without any upgrades)

		if (aSkipRomStub)
			{
			// Since this is only a ROM stub entry, leave it as it is;
			// to avoid regenerate it again.
			return;
			}

		std::stringstream err;
		err << "Illegal uninstall of ROM package 0x" << std::hex << aUid;
		throw InterpretSisError(err.str(), ATTEMPT_TO_UNINSTALL_ROM_PKG);
		}

	std::list<std::wstring>::iterator currFile = regDirFiles.begin();

	// Remove all .ctl files in the system drive for this package
	while (currFile != regDirFiles.end())
	    {
		if (*currFile != L"." && *currFile != L"..")
		    {
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
				int pos = 0;
				if((pos = currFile->find(L".ctl_backup", 0)) == std::wstring::npos)
			#endif
					RemoveFile(path + *currFile);
		    }

		++currFile;
	    }

	// Remove the installed files for all packages (SA, SP, PU)
	// retrieve component id for the specified uid
	std::vector<TInt32> compIdList = iDbHelper->GetComponentIdList(aUid);
	std::vector<TInt32> ::iterator compIter;
	for( compIter = compIdList.begin(); compIter != compIdList.end() ; ++compIter )
		{
		// For ROM stub, the SelectedDrive = 0; as there could be ROM->SP.
		// Therefore, the rest can only apply to at least a ROM stub upgrades
		int drive = tolower(iDbHelper->GetSelectedDrive(*compIter));
		
		if (drive)
			{
			std::wstring localTargetPath = iConfigManager.GetLocalDrivePath(drive);
			path = GetRegistryDir( localTargetPath, aUid );

			regDirFiles.clear();
			GetDirContents(path, regDirFiles);

			currFile = regDirFiles.begin();

			// Remove all the .ctl files for this package
			while (currFile != regDirFiles.end())
				{
				if (*currFile != L"." && *currFile != L"..")
					{
					#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
						int pos = 0;
						if((pos = currFile->find(L".ctl_backup", 0)) == std::wstring::npos)
					#endif
							RemoveFile(path + *currFile);
					}
				++currFile;
				}

			// This iterator's object has the same Uid as aUid
			const FileDescriptions& files = iDbHelper->GetFileDescriptions(*compIter);

			RemoveInstalledFiles(files, localTargetPath);
			
			}
		// remove the application from database
		iDbHelper->RemoveEntry(*compIter);
		}
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void SisRegistry::BackupCtl(TUint32 aUid)
	{
	std::list<std::wstring> regDirFiles;
	std::wstring path = GetRegistryDir( iParamList.SystemDrivePath(), aUid );
    GetDirContents(path, regDirFiles);
	std::list<std::wstring>::iterator currFile = regDirFiles.begin();

	std::wstring  path1 = FixPathDelimiterstap(path);
	// Backup all .ctl files in the system drive for this package
	while (currFile != regDirFiles.end())
		{
		if (*currFile != L"." && *currFile != L"..")
	   	 	{
			std::string ifile = wstring2string(path1 +*currFile);
			std::string ibackupfile(ifile);
			ibackupfile.append("_backup");

			int err=FileCopyA(ifile.c_str(),ibackupfile.c_str(),0);
			if (err != 0)
				LERROR(L"Failed to Backup .ctl ");
			}
			++currFile;
    	}
	}

void SisRegistry::RestoreCtl(TUint32 aUid, TBool& aBackupFlag)
	{
	std::list<std::wstring> regDirFiles;
	std::wstring path = GetRegistryDir( iParamList.SystemDrivePath(), aUid );
    GetDirContents(path, regDirFiles);
	std::list<std::wstring>::iterator currFile = regDirFiles.begin();

	// Restore all .ctl files in the system drive for this package
	while (currFile != regDirFiles.end())
		{
		if (*currFile != L"." && *currFile != L"..")
	    	{
			if(aBackupFlag)
				{
				int pos =0;
				if((pos = currFile->find(L".ctl_backup", 0)) == std::wstring::npos)
					{
					std::string ifile = wstring2string(path +*currFile);
					std::string ibackupfile(ifile);
					ibackupfile.append("_backup");

					RemoveFile(path + *currFile);
				 	int err = FileMoveA(ibackupfile.c_str(),ifile.c_str());
					if (err != 0)
						LERROR(L"Failed to Restore .ctl ");
					}
				}
			else
				{
				RemoveFile(path + *currFile);
				}
			}
			++currFile;
    	}
	}

void SisRegistry::RemoveCtlBackup(TUint32 aUid)
	{
	std::list<std::wstring> regDirFiles;
	std::wstring path = GetRegistryDir( iParamList.SystemDrivePath(), aUid );
    GetDirContents(path, regDirFiles);
	std::list<std::wstring>::iterator currFile = regDirFiles.begin();
	
	// Remove all .ctl backup files in the system drive for this package
	while (currFile != regDirFiles.end())
		{
		if (*currFile != L"." && *currFile != L"..")
	   		{
			std::wstring ifile(path +*currFile);
			ifile.append(L"_backup");
			
			if (FileExists(ifile))
				RemoveFile(ifile);
			}
			++currFile;
    	}
	}
#endif

TUint32 SisRegistry::GetUid(TUint32 aSid) const
	{
	return iDbHelper->GetUid(aSid);
	}

TUint32 SisRegistry::GetUid(const std::wstring& aSidFile) const
	{
	return iDbHelper->GetUid(aSidFile);
	}

bool SisRegistry::GetInRom(TUint32 aUid) const
	{
	TUint32 componentId = iDbHelper->GetComponentId(aUid);
	return iDbHelper->GetInRom(componentId);
	}
	
XmlDetails::TScrPreProvisionDetail::TComponent SisRegistry::CreateRegistryEntry(const SisRegistryObject& aSisRegistryObject)
	{

	XmlDetails::TScrPreProvisionDetail::TComponent component;

	// add registry token information
	AddSisRegistryToken(component, dynamic_cast<const SisRegistryToken&> (aSisRegistryObject));

	
	// update SisregistryObject
	AddLocalizedNames(component,aSisRegistryObject);
	
	// This flag cannot be made optional as it is used in queries
	AddComponentProperty(component, DbConstants::CompInstallType, aSisRegistryObject.GetInstallType(), IGNORE_FORMATTING);

	AddComponentDependencies(component, aSisRegistryObject);
	AddEmbeddedPackages(component, aSisRegistryObject.GetEmbeddedPackages());
	AddProperties(component, aSisRegistryObject.GetProperties());
	AddFileDescription(component, aSisRegistryObject.GetFileDescriptions());
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	AddApplicationRegistrationInfoL(component, aSisRegistryObject.GetFileDescriptions(), aSisRegistryObject.GetInRom());
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	// inROM
	if(DbConstants::KDefaultIsInRom != aSisRegistryObject.GetInRom())
		AddComponentProperty(component, DbConstants::CompInRom, aSisRegistryObject.GetInRom(), IGNORE_FORMATTING);

	// isSigned
   	if(DbConstants::KDefaultIsSigned != aSisRegistryObject.GetSigned()) 
		AddComponentProperty(component, DbConstants::CompIsSigned, aSisRegistryObject.GetSigned(), IGNORE_FORMATTING);

	// SignedBySuCert
	if(DbConstants::KDefaultIsSignedBySuCert != aSisRegistryObject.GetSignedBySuCert())
		AddComponentProperty(component, DbConstants::CompSignedBySuCert, aSisRegistryObject.GetSignedBySuCert(), IGNORE_FORMATTING);
	
	// DeletablePreinstalled
	if(DbConstants::KDefaultIsDeletablePreInstalled != aSisRegistryObject.GetPreInstalled())
		AddComponentProperty(component, DbConstants::CompDeletablePreinstalled, aSisRegistryObject.GetPreInstalled(), IGNORE_FORMATTING);

	// iTrust
	if(DbConstants::KDefaultTrustValue != aSisRegistryObject.GetTrust())
		AddComponentProperty(component, DbConstants::CompTrust, aSisRegistryObject.GetTrust(), IGNORE_FORMATTING);
	
	// iRemoveWithLastDependent
	if(DbConstants::KDefaultRemoveWithLastDependent != aSisRegistryObject.GetRemoveWithLastDependent())
		AddComponentProperty(component, DbConstants::CompIsRemoveWithLastDependent, aSisRegistryObject.GetRemoveWithLastDependent(), IGNORE_FORMATTING);

	// iTrustTimeStamp will be deprecated
	// 64 bit value
	std::wstring strTimeStamp = Utils::Int64ToWideString(aSisRegistryObject.GetTrustTimeStamp());
	AddComponentProperty(component, DbConstants::CompTrustTimeStamp, strTimeStamp, IGNORE_FORMATTING, false);

	std::wstring strStubFileName = aSisRegistryObject.GetStubFileName();
	if(strStubFileName.length())
		{
		AddComponentProperty(component, DbConstants::CompStubFileName, strStubFileName, IGNORE_FORMATTING, false);
		}
	// iTrustStatus
	AddTrustStatus(component, aSisRegistryObject.GetTrustStatus());

	// installChainIndices
	AddInstallChainIndices(component, aSisRegistryObject.GetInstallChainIndices());

	// iIsRemovable
	if(aSisRegistryObject.GetIsRemovable())
		{
		component.iComponentDetails.iIsRemovable = 1;
		}
	else
		{
		component.iComponentDetails.iIsRemovable = 0;
		}

	// iIsHidden
	if(aSisRegistryObject.GetIsHidden())
		{
		component.iComponentDetails.iIsHidden = 1;
		}
	else
		{
		component.iComponentDetails.iIsHidden = 0;
		}

	return component;
	}

void SisRegistry::AddSisRegistryToken	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
											const SisRegistryToken& aSisRegistryToken 
										)
	{
	// add sisregistrypackage details
	AddSisRegistryPackage( aComponent, dynamic_cast<const SisRegistryPackage&>(aSisRegistryToken), aSisRegistryToken.GetLanguage() );

	// add sid count
	// sids would be added later alongwith their corresponding executables.
	int sidCount = aSisRegistryToken.GetSids().size();
	if (DbConstants::KDefaultSidCount != sidCount)
		AddComponentProperty( aComponent, DbConstants::CompSidCount, sidCount, IGNORE_FORMATTING);

	if( sidCount > 0)
		{
		AddComponentProperty(aComponent, DbConstants::CompSidsPresent, ENABLE, IGNORE_FORMATTING);
		}

	AddControllerInfo(aComponent, aSisRegistryToken.GetControllerInfo());

	AddVersion(aComponent, aSisRegistryToken.GetVersion());

	if (aSisRegistryToken.GetLanguage() != DbConstants::KDefaultInstalledLanguage)
		AddComponentProperty( aComponent, DbConstants::CompLanguage, aSisRegistryToken.GetLanguage(), IGNORE_FORMATTING);

	if (aSisRegistryToken.GetSelectedDrive() != DbConstants::KDefaultInstalledDrive)
		AddComponentProperty( aComponent, DbConstants::CompSelectedDrive, aSisRegistryToken.GetSelectedDrive(), IGNORE_FORMATTING);

	}

void SisRegistry::AddSisRegistryPackage	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
											const SisRegistryPackage& aSisRegistryPackage,
											int aLanguage
										)
	{	
		
	// vendor
	AddComponentProperty( aComponent, DbConstants::CompUniqueVendorName, aSisRegistryPackage.GetVendorName(),IGNORE_FORMATTING,false);
	// uid
	std::wstringstream strPropName;
	strPropName << DbConstants::CompUid;
		
	std::wstringstream strPropValue;
	TInt32 uid = aSisRegistryPackage.GetUid();
	strPropValue << uid;

	XmlDetails::TScrPreProvisionDetail::TComponentProperty 
	compProperty = CreateComponentProperty(strPropName.str(), strPropValue.str(), true, false); 
	
	aComponent.iComponentProperties.push_back(compProperty);

	// index
	AddComponentProperty( aComponent, DbConstants::CompIndex, aSisRegistryPackage.GetIndex(),IGNORE_FORMATTING );

	// calculate the globalid for this component
	int installType = dynamic_cast<const SisRegistryObject&>(aSisRegistryPackage).GetInstallType();
	std::wstring globalId = GetGlobalId(aSisRegistryPackage.GetUid(), installType, aSisRegistryPackage.GetPackageName());
	
	aComponent.iComponentDetails.iGlobalId = globalId;
	}

void SisRegistry::AddControllerInfo	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										const std::vector<ControllerInfo*>& aControllerInfo 
									)
	{
	if(DbConstants::KDefaultControllerCount != aControllerInfo.size())
		AddComponentProperty( aComponent, DbConstants::CompControllerCount, aControllerInfo.size(), IGNORE_FORMATTING);
	
	std::vector<ControllerInfo*>::const_iterator controllerInfoIter;
	int count = 0;
	for(controllerInfoIter = aControllerInfo.begin() ; controllerInfoIter != aControllerInfo.end() ; ++controllerInfoIter )
		{
		std::wstring version = FormatVersionToString((*controllerInfoIter)->GetVersion());
		AddComponentProperty( aComponent, DbConstants::CompControllerVersion, version, count, false);

		// offset
		AddComponentProperty( aComponent, DbConstants::CompControllerOffset, (*controllerInfoIter)->GetOffset(), count );

		// add hash information
		AddHashContainer(aComponent, (*controllerInfoIter)->GetHashContainer(), count );
		++count;
		}	
	}

void SisRegistry::AddVersion	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const Version& aVersion
								)
	{
	aComponent.iComponentDetails.iVersion.iMajor = Utils::IntegerToWideString(aVersion.GetMajor());
	aComponent.iComponentDetails.iVersion.iMinor = Utils::IntegerToWideString(aVersion.GetMinor());
	aComponent.iComponentDetails.iVersion.iBuild = Utils::IntegerToWideString(aVersion.GetBuild());
	}

void SisRegistry::AddHashContainer	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										const HashContainer& aHashContainer,
										int aCount
									)
	{
	if (aHashContainer.GetHashId() != HashContainer::EHashSHA)
		AddComponentProperty( aComponent, DbConstants::CompControllerHashAlgorithm, aHashContainer.GetHashId(), aCount);
	
	int algorithmId = aHashContainer.GetHashId();
	// The property is a concatenation of the algorithm id and the hash data
	const std::string hashData = aHashContainer.GetData();
		
	std::string encodedHashData = Util::Base64Encode(hashData);	
	std::wstring wideHashData = string2wstring(encodedHashData);
	AddComponentProperty( aComponent, DbConstants::CompControllerHashData, wideHashData, aCount, true);

	}


void SisRegistry::AddLocalizedNames(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const SisRegistryObject& aSisRegistryObject)
	{
	TInt size = aSisRegistryObject.GetMatchingSupportedLanguages().size();
	if ( 0 == size )
		{
		XmlDetails::TScrPreProvisionDetail::TComponentLocalizable componentLocalizable;
		componentLocalizable.iVendor = aSisRegistryObject.GetVendorLocalizedName();
		componentLocalizable.iName = aSisRegistryObject.GetPackageName();
		componentLocalizable.iLocale = aSisRegistryObject.GetLanguage();
		aComponent.iComponentLocalizables.push_back(componentLocalizable);
		}
	else
		{
		for ( TInt i=0; i < size; i++)
			{
			XmlDetails::TScrPreProvisionDetail::TComponentLocalizable componentLocalizable;
			componentLocalizable.iName = aSisRegistryObject.GetLocalizedPackageNames()[i]->GetString();
			componentLocalizable.iLocale = aSisRegistryObject.GetMatchingSupportedLanguages().at(i);
			componentLocalizable.iVendor = aSisRegistryObject.GetLocalizedVendorNames()[i]->GetString();
			aComponent.iComponentLocalizables.push_back(componentLocalizable);
			}
		}
	}

void SisRegistry::AddComponentDependencies(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const SisRegistryObject& aSisRegistryObject)
	{
	const std::vector<SisRegistryDependency*>& dependencies  = aSisRegistryObject.GetDependencies();
	if(dependencies.size() == 0)
		return;

	std::wstring dependentGlobalId = GetGlobalId( aSisRegistryObject.GetUid(), aSisRegistryObject.GetInstallType(), aSisRegistryObject.GetPackageName() );
	
	XmlDetails::TScrPreProvisionDetail::TComponentDependency componentDependency;
	componentDependency.iDependentId = dependentGlobalId;

	
	for(std::vector<SisRegistryDependency*>::const_iterator depIter = dependencies.begin() ; depIter != dependencies.end() ; ++depIter)
		{
		XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail componentDependencyDetail;
		
		TUint32 uid = (*depIter)->GetUid();
		std::wstring supplierGlobalId = GetGlobalId(uid,CSISInfo::EInstInstallation,L"");
		componentDependencyDetail.iSupplierId = supplierGlobalId;

		// add fromVersion
		std::wstring fromVersion = FormatVersionToString((*depIter)->GetFromVersion());
		componentDependencyDetail.iFromVersion = fromVersion;

		// add toVersion
		std::wstring toVersion = FormatVersionToString((*depIter)->GetToVersion());
		componentDependencyDetail.iToVersion = toVersion;

		componentDependency.iComponentDependencyList.push_back(componentDependencyDetail);
		}
	aComponent.iComponentDependency = componentDependency;
	}		

void SisRegistry::AddEmbeddedPackages(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::vector<SisRegistryPackage*>& aEmbeddedPackages )
	{
	// EmbeddedPckgCount
	int embeddedPckgCount = aEmbeddedPackages.size();
	if (DbConstants::KDefaultEmbeddedPackageCount != embeddedPckgCount)
		AddComponentProperty(aComponent, DbConstants::CompEmbeddedPackageCount, embeddedPckgCount, IGNORE_FORMATTING );

	if( embeddedPckgCount > 0)
		{
		AddComponentProperty(aComponent, DbConstants::CompEmbeddedPackagePresent, ENABLE, IGNORE_FORMATTING );	
		}
	

	std::vector<SisRegistryPackage*>::const_iterator pckgIter;
	
	int count = 0;
	for(pckgIter = aEmbeddedPackages.begin() ; pckgIter != aEmbeddedPackages.end() ; ++pckgIter, ++count)
		{
		// package uid
		AddComponentProperty( aComponent, DbConstants::CompEmbeddedPackageUid, (*pckgIter)->GetUid(), count);
		// package name
		AddComponentProperty( aComponent, DbConstants::CompEmbeddedPackageName, (*pckgIter)->GetPackageName(), count, false);
		// vendor name
		AddComponentProperty( aComponent, DbConstants::CompEmbeddedPackageVendor, (*pckgIter)->GetVendorName(), count, false);
		// index
		AddComponentProperty( aComponent, DbConstants::CompEmbeddedPackageIndex, (*pckgIter)->GetIndex(), count);
		}

	}


void SisRegistry::AddProperties(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::vector<SisRegistryProperty*>& aProperties )
	{
	if (aProperties.size() != DbConstants::KDefaultCompPropertyCount)
		AddComponentProperty(aComponent, DbConstants::CompPropertyCount, aProperties.size(), IGNORE_FORMATTING );

	std::vector<SisRegistryProperty*>::const_iterator propIter;
	
	int count = 0;
	for(propIter = aProperties.begin() ; propIter != aProperties.end() ; ++propIter, ++count)
		{
		// key
		AddComponentProperty( aComponent, DbConstants::CompPropertyKey, (*propIter)->GetKey(), count);
		// value
		AddComponentProperty( aComponent, DbConstants::CompPropertyValue, (*propIter)->GetValue(), count);
		}
	}


void SisRegistry::AddFileDescription(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::vector<FileDescription*>& aFileDescription )
	{
	std::vector<FileDescription*>::const_iterator filedesIter;
	int wildcardFileCount = 0;
	int exeFileIndex = 0;
	for(filedesIter = aFileDescription.begin() ; filedesIter != aFileDescription.end(); ++filedesIter)
		{
		// if it has wild card characters then add it as part of componentproperties table
		if( IsFileWideCard((*filedesIter)->GetTarget()) ) 
			{
			// file target
			AddComponentProperty( aComponent, DbConstants::WCFileName, (*filedesIter)->GetTarget(), wildcardFileCount++, false);
			continue;
			}

		TInt sid = (*filedesIter)->GetSid();
		if( sid != 0 )
			{
			// add sid file format
			AddComponentProperty(aComponent, DbConstants::CompSidFileName, (*filedesIter)->GetTarget(), exeFileIndex, false);
			AddComponentProperty(aComponent, DbConstants::CompSid, sid, exeFileIndex);
			++exeFileIndex;
			}

		// add the location in the component files table
		XmlDetails::TScrPreProvisionDetail::TComponentFile componentFile;
		componentFile.iLocation = (*filedesIter)->GetTarget();

		AddFileDescriptionAsFileProperty(componentFile, *filedesIter);
		aComponent.iComponentFiles.push_back(componentFile);
		}

	if (DbConstants::KDefaultWildCardFileCount != wildcardFileCount)
		AddComponentProperty(aComponent, DbConstants::CompWildCardFileCount, wildcardFileCount, IGNORE_FORMATTING);	
	}


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void SisRegistry::AddApplicationRegistrationInfoL(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const std::vector<FileDescription*>& aFileDescription, int aInRom )
	{
	std::vector<FileDescription*>::const_iterator filedesIter;
	std::wstring iLocalFile;
	std::string RegistrationFileName;
	
	//Find Registration File from list of filelist
	for(filedesIter = aFileDescription.begin() ; filedesIter != aFileDescription.end(); ++filedesIter)
		{
		// if it has wild card characters then donot process. Continue.
		if( IsFileWideCard((*filedesIter)->GetLocalFile()) )
			{
			continue;
			}

		iLocalFile = (*filedesIter)->GetLocalFile();
		RegistrationFileName = wstring2string(iLocalFile);

		std::string iRomPath = wstring2string(iParamList.RomDrivePath());
		if(aInRom)
			{
			std::string localpath = FullNameWithoutDrive(RegistrationFileName);
			RegistrationFileName = iRomPath + localpath;
			}

		size_t found=RegistrationFileName.rfind(".rsc");
		if(found==string::npos)
			continue;

		//Return 0 for Registration file else 1
		TInt err = FindRegistrationResourceFileL(RegistrationFileName);

		if(err)
			continue;

		std::string folder;
		#ifdef __LINUX__
		const char *privateFolder = "private/10003a3f/";
		#else
		const char *privateFolder = "private\\10003a3f\\";
		#endif

		found=RegistrationFileName.find(privateFolder);

		if( found != string::npos ) 
			folder = RegistrationFileName.substr(0,found); 
		else
			folder = RegistrationFileName; 

		CAppInfoReader* appInfoReader = NULL;
		appInfoReader = CAppInfoReader::NewL(RegistrationFileName, NullUid, folder); 
		if (!appInfoReader)
			{
			std::string errMsg= "Error in Reading File. Memory Allocation Failed";
			throw CResourceFileException(errMsg);
			}
		else
			{
			TBool readSuccessful=EFalse;

			readSuccessful= appInfoReader->ReadL(aFileDescription, iRomPath, aInRom);
		
			if (readSuccessful)
				{
				CreateApplicationRegistrationInfoL(aComponent,appInfoReader);
				}
			else
				{
				delete appInfoReader;
				std::string errMsg= "Reading Resource File failed.";
				throw CResourceFileException(errMsg);
				}
			}
		delete appInfoReader;
		}
	}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

void SisRegistry::AddFileDescriptionAsFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile, 
												const FileDescription* aFileDescription
										)
	{
	// mime type
	if( !aFileDescription->GetMimeType().empty())
		{
		AddFileProperty( aComponentFile, DbConstants::FileMimeType, aFileDescription->GetMimeType(), true);
		}
	// operation
	if(aFileDescription->GetOperation() != CSISFileDescription::EOpInstall)
		{
		AddFileProperty( aComponentFile, DbConstants::FileSisFileOperation, aFileDescription->GetOperation(), false);
		}
	// operation option	
	if( aFileDescription->GetOperationOptions() )
		{
		AddFileProperty( aComponentFile, DbConstants::FileFileOperationOptions, aFileDescription->GetOperationOptions(), false);
		}

	// uncompressed length
	if( aFileDescription->GetUncompressedLength() )
		{
		AddFileProperty( aComponentFile, DbConstants::FileUncompressedLength, aFileDescription->GetUncompressedLength(), false);
		}

	// file index
	if( aFileDescription->GetIndex() )
		{
		AddFileProperty( aComponentFile, DbConstants::FileIndex, aFileDescription->GetIndex(), false);
		}

	// sid
	if( aFileDescription->GetSid() )
		{
		AddFileProperty( aComponentFile, DbConstants::FileSid, aFileDescription->GetSid(), false);
		}

	// filehash algorithm
	if (aFileDescription->GetHash().GetHashId() != HashContainer::EHashSHA )
		{
		AddFileProperty( aComponentFile, DbConstants::FileHashAlgorithm, aFileDescription->GetHash().GetHashId(), false);
		}
	// file hash data
	if(!aFileDescription->GetHash().GetData().empty())
		{
		std::string binHashData = Util::Base64Encode(aFileDescription->GetHash().GetData());
		std::wstring hashData = string2wstring(binHashData);
		AddFileProperty( aComponentFile, DbConstants::FileHashData, hashData, true);
		}
	}

void SisRegistry::AddTrustStatus	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										const TrustStatus& aTrustStatus 
									)
	{
	// iValidationStatus
	AddComponentProperty( aComponent, DbConstants::CompTrustValidationStatus, aTrustStatus.GetValidationStatus(), IGNORE_FORMATTING);
	}


void SisRegistry::AddInstallChainIndices	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
												const std::vector<TInt>& aInstallChainIndices 
											)
	{
	if (aInstallChainIndices.size() != DbConstants::KDefaultChainIndex)
		AddComponentProperty( aComponent, DbConstants::CompInstallChainIndexCount, aInstallChainIndices.size(), IGNORE_FORMATTING);
		
	std::vector<TInt>::const_iterator chainIndicesIter;
	int count = 0;
	for(chainIndicesIter = aInstallChainIndices.begin(); chainIndicesIter != aInstallChainIndices.end() ; ++chainIndicesIter)
		{
		AddComponentProperty(aComponent, DbConstants::CompInstallChainIndex, *chainIndicesIter, count++);
		}
	}

void SisRegistry::AddComponentProperty	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent,
											const std::wstring& aPropName,
											int aPropValue,
											int aCount
										)
	{
	std::wstringstream strPropName;
	
	if(aCount != IGNORE_FORMATTING)
		{
		strPropName << aPropName << aCount;
		}
	else
		{
		strPropName << aPropName;
		}
	
	std::wstringstream strPropValue;
	strPropValue << aPropValue;

	XmlDetails::TScrPreProvisionDetail::TComponentProperty 
	compProperty = CreateComponentProperty(strPropName.str(), strPropValue.str(), true, false); 
	
	aComponent.iComponentProperties.push_back(compProperty);

	}

void SisRegistry::AddComponentProperty	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent,
											const std::wstring& aPropName,
											const std::wstring& aPropValue,
											int aCount,
											bool aIsBinary
										)
	{
	std::wstringstream strPropName;
	
	if(aCount != IGNORE_FORMATTING)
		{
		strPropName << aPropName << aCount;
		}
	else
		{
		strPropName << aPropName;
		}
	
	XmlDetails::TScrPreProvisionDetail::TComponentProperty compProperty;

	if(aIsBinary)
		{
		compProperty = CreateComponentProperty(strPropName.str(), aPropValue,false, true); 
		}
	else
		{
		compProperty = CreateComponentProperty(strPropName.str(), aPropValue,false, false); 
		}
	
	aComponent.iComponentProperties.push_back(compProperty);
	}

void SisRegistry::AddFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile,
										const std::wstring& aPropName,
										const std::wstring& aPropValue,
										bool aIsBinary
									)
	{
	XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty fileProperty;
	
	fileProperty.iName = aPropName;
	fileProperty.iValue = aPropValue;
	fileProperty.iIsIntValue = !aIsBinary;
	aComponentFile.iFileProperties.push_back(fileProperty);
	}
	
void SisRegistry::AddFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile,
										const std::wstring& aPropName,
										int aPropValue,
										bool aIsBinary
									)
	{
	std::wstringstream strPropValue;
	strPropValue << aPropValue;
	AddFileProperty(aComponentFile, aPropName, strPropValue.str(), aIsBinary);
	}

XmlDetails::TScrPreProvisionDetail::TComponentProperty 
	SisRegistry::CreateComponentProperty(	const std::wstring& aPropName, const std::wstring& aPropValue, 
											bool aIsInt, bool aIsStr8Bit, int aLocale
										)
	{
	XmlDetails::TScrPreProvisionDetail::TComponentProperty componentProperty;
	
	componentProperty.iName			= aPropName;
	componentProperty.iLocale		= aLocale;
	componentProperty.iValue		= aPropValue;
	componentProperty.iIsIntValue	= aIsInt;
	componentProperty.iIsStr8Bit	= aIsStr8Bit;

	return componentProperty;
	}

#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
