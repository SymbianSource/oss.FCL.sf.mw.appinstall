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
 @publishedPartner
 @released
*/
#ifndef	__PARAMETER_H__
#define	__PARAMETER_H__

#pragma warning (disable: 4786)

// System includes
#include <istream>
#include <list>
#include <vector>
#include <map>
#include <string>

// User includes
#include "logger.h"
#include "sislanguage.h"


struct InstallSISFile
	{
public:
	/**
	 * Default constructor. 
	 */
	InstallSISFile() : iFileName(L""), iTargetDrive(L'$'), iGenerateStub(false),
						iNonRemovable(false),iReadOnly(false), iNotRegister(false), iSUFlag(false) {}

	/**
	 * Constructor
	 */
	InstallSISFile(std::wstring aName, int aDrive, bool aSUFlag)
		: iFileName(aName), iTargetDrive(aDrive), iGenerateStub(false), 
			iNonRemovable(false),iReadOnly(false), iNotRegister(false), iSUFlag(aSUFlag) {}

	/**
	 * Constructor
	 */
	InstallSISFile(std::wstring aName, int aDrive, bool aGenerateStub, bool aNonRemovable,
					bool aReadOnly, bool aNotRegister, bool aSUFlag)
		: iFileName(aName), iTargetDrive(aDrive), iGenerateStub(aGenerateStub), 
			iNonRemovable(aNonRemovable), iReadOnly(aReadOnly), iNotRegister(aNotRegister), iSUFlag(aSUFlag) {}

public:
	std::wstring iFileName; // SIS file name
	int iTargetDrive;		// Target drive
	bool iGenerateStub;		// Whether to generate stub sis file for this sis or not.
	bool iNonRemovable;		// Whether this is a non-removable SIS or not.
	bool iReadOnly;		    // Whether this is a Readonly SIS or not.
	bool iNotRegister;		// Whether to register the sis or not.
	bool iSUFlag;			// Whether SU flag is set for this or not.
	};



/** 
 * Contains the parameter required by interpretsis for instalaltion/un-installation
 */
class CParameterList
	{
public:
	// Type definitions
	typedef std::list<std::wstring> OptionList;
	typedef std::list<InstallSISFile> SISFileList;

public: // Enumerations
	/**
	 * Enumeration which represents invalid arguement error.
	 */
    enum TParamException
	    {
	    EParamUnknownOption = 0,
	    EParamInvalidDir,
	    EParamInvalidSystemDrive,
        EParamMissingConfigurationFile,
        EParamUseEitherRomRofsLogsOrZDrive,
        EParamMissingSisStubFilePath,
		EParamInvalidRegistryVersion,
		EParamInstallRemoveExclusive,
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		EParamRegVersionOriginVerificationExclusive
		#endif
	    };

    /**
     * Installation/un-installation flags used by interpretsis.
     */
    enum TFlags
        {
        EFlagsNull                  = 0x0000,
        EFlagsLanguageWasSet        = 0x0001,
        EFlagsRomRofsLogFilesSet    = 0x0002,
        EFlagsCDriveSet             = 0x0004,
        EFlagsZDriveSet             = 0x0008,
        EFlagsSysDriveSet           = 0x0010,
        EFlagsStubDirectorySet      = 0x0020,
        EFlagsConfigIniSet          = 0x0040,
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		EFlagsResourceFilePathSet   = 0x0100,
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		EFlagsDisableZDriveChecksSet= 0x0080

        };

public: // Constructors & destructor
	/**
	 * Default Constructor
	 */
	CParameterList();
	/**
	 * Cleanup owned resources.
	 */
	~CParameterList();
	
public: 
	/**
	 * This function will log the error in the logger.
	 * @param aError type of error.
	 */
	static void DisplayError(TParamException aError);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Set the path which represent the system drive.
	 * @param File-system path which represents system drive
	 */
	void SetResourceFilePath(const std::string& aFilePath);
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Set the path which represent the system drive.
	 * @param File-system path which represents system drive
	 */
	void SetSystemDrive(const std::string& aFilePath);
	/**
	 * Drive letter of the system drive
	 * @param Drive letter
	 */
	void SetSystemDriveLetter(int aDriveLetter);
	/**
	 * Set the file-system path of the ROM (Z) drive.
	 * @param aFilePath path
	 */
	void SetZDrive(const std::string& aFilePath);
	/**
	 * Set the stub director.
	 * @param aFilePath Path for stubs.
	 */
	void SetStubDir(const std::string& aFilePath);
	/**
	 * Set the config file.
	 * @param aFilePath config file name
	 */
	void SetConfigFile(const std::string& aFilePath);
	/**
	 * Function will set the sis registry path.
	 */
	void SetSisRegistryVersion(const std::string& aVerString);
	/**
	 * Set the log file name.
	 * @param aLogFile Log filename
	 */
	void SetLogFile(const std::wstring& aLogFile);
	/**
	 * Set the installation language.
	 * @param aLanguage language code.
	 */
	void SetLanguage(CSISLanguage::TLanguage aLanguage);
	/**
	 * Set the installation/un-installation flag.
	 */
	void SetFlag(TFlags aFlag);
	/**
	 * Set registry's major version.
	 * @param aMajorVersion major version of registry.
	 */
	inline void SetRegistryMajorVersion(int aMajorVersion);
	/**
	 * Set registry's minor version.
	 * @param aMajorVersion minor version of registry.
	 */
	inline void SetRegistryMinorVersion(int aMinorVersion);
	/**
	 * Add a new SIS file into the installation list.
	 * @param aSISFile new sis file to be installed.
	 */
	void AddSISFile(InstallSISFile& aSISFile);
	/**
	 * Add ROM log file into the existing list.
	 */
	void AddRomLogFile(std::wstring& aRomLogFile);
	/**
	 * Empty the ROM log file list.
	 */
	void EmptyRomLogFiles();
	/**
	 * Add pkg UID to the existing list of UIDs for un-installation.
	 */
	void AddPkgUid(std::wstring& aPkgUID);
	/**
	 * Clear the package UID list.
	 */
	void EmptyPkgUids();
	/**
	 * This function will validate all the parameters supplied.
	 * Throws an exception of it encounters any illegal parameter.
	 */
	void ValidateParam();
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * This function will set the Origin Verification status of the application. 
	 * By default, verification status is set to TRUE (verified).
	 */
	void SetOriginVerificationStatus(bool aOriginVerified);
	#endif
	
	/**
	 * Retrieves the list of SIS files marked for installation
	 */
	inline const SISFileList&	FileNames() const;
	/**
	 * Retrieves the list of ROM log files.
	 */
	inline const OptionList&	RomLogFileNames() const;
	/**
	 * Retrieves the list of package UIDs marked for un-installation.
	 */
	inline const OptionList&	PkgUidsToRemove() const;
	/**
	 * Get the system drive path.
	 */
	inline const std::wstring&	SystemDrivePath() const;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Get the Resource File path.
	 */
	inline const std::wstring&	ResourceFilePath() const;
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Gets the ROM drive (Z) path in the file-system.
	 */
	inline const std::wstring&	RomDrivePath() const;
	/**
	 * Gets the config file name.
	 */
	inline const std::wstring&	ConfigFileName() const;
	/**
	 * Gets the stub path.
	 */
	inline const std::wstring&	SisStubPath() const;
	/**
	 * Retrieves the language code.
	 */
	inline CSISLanguage::TLanguage			Language() const;
	/**
	 * Retrieves the System drive letter
	 */
	inline int					SystemDriveLetter() const;
	/**
	 * Checks if a particular installation/un-installation flag is set or not.
	 * @return true if the flag is set else false.
	 */
	inline bool					IsFlagSet(TFlags aFlag) const;
	/**
	 * Returns the major version of registry
	 */
	inline int					RegistryMajorVersion() const;
	/**
	 * Returns the minor version of registry
	 */
	inline int					RegistryMinorVersion() const;
	
	/**
	 * Checks whether registry version exists or not.
	 */
	inline bool RegistryVersionExists() const; 
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Gives the OriginVerification status.
	 */
	inline bool OriginVerificationStatus() const;
	#endif
	
private:
	typedef std::map<std::string, WarnLevel> WarnMap;

	SISFileList				iFileNames;
    OptionList				iRomLogFiles;
	OptionList				iPkgUIDs;
	std::wstring			iSystemDrivePath;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	std::wstring			iResourceFilePath;
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	std::wstring			iZDrive; 
    std::wstring			iConfigFile;
    std::wstring			iSisStubPath;
	int						iSystemDriveLetter;
	CSISLanguage::TLanguage	iLanguage;
    int                     iFlags;
	int						iRegistryMajorVersion;
	int						iRegistryMinorVersion;
	static WarnMap          KString2Warning;
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	bool 					iOriginVerified;
	#endif
	};

// Inline Member Functions
inline const CParameterList::SISFileList& CParameterList::FileNames() const
	{
	return iFileNames;
	}

inline const CParameterList::OptionList& CParameterList::RomLogFileNames() const
	{
	return iRomLogFiles;
	}

inline const CParameterList::OptionList& CParameterList::PkgUidsToRemove() const
	{
	return iPkgUIDs;
	}

inline const std::wstring& CParameterList::SystemDrivePath() const
	{
	return iSystemDrivePath;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline const std::wstring& CParameterList::ResourceFilePath() const
	{
	return iResourceFilePath;
	}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

inline const std::wstring& CParameterList::RomDrivePath() const
	{
	return iZDrive;
	}

inline const std::wstring& CParameterList::ConfigFileName() const
	{
	return iConfigFile;
	}

inline const std::wstring& CParameterList::SisStubPath() const
	{
	return iSisStubPath;
	}

inline CSISLanguage::TLanguage CParameterList::Language() const
	{
	return iLanguage;
	}

inline int CParameterList::SystemDriveLetter() const
	{
	return iSystemDriveLetter;
	}

inline bool CParameterList::IsFlagSet(TFlags aFlag) const
	{
    return (iFlags & aFlag)? true: false;
	}

inline int CParameterList::RegistryMajorVersion() const
	{
	return iRegistryMajorVersion;
	}

inline int CParameterList::RegistryMinorVersion() const
	{
	return iRegistryMinorVersion;
	}

inline void CParameterList::SetRegistryMajorVersion(int aMajorVersion) 
	{
	iRegistryMajorVersion = aMajorVersion; 
	}

inline void CParameterList::SetRegistryMinorVersion(int aMinorVersion) 
	{
	iRegistryMinorVersion = aMinorVersion; 
	}

inline bool CParameterList::RegistryVersionExists() const 
	{ 
	return (iRegistryMajorVersion != 0 || iRegistryMinorVersion != 0); 
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline bool CParameterList::OriginVerificationStatus() const 
	{ 
	return iOriginVerified;
	}
#endif

#endif	// __PARAMETER_H__
