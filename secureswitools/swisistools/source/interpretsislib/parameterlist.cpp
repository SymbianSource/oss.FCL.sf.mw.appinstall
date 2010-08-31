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


#include <iostream>

#include "parameterlist.h"
#include "stringutils.h"
#include "sisregistry.h"

#include "is_utils.h"

CParameterList::CParameterList():
	iSystemDriveLetter(L'c'),
	iLanguage( CSISLanguage::ELangEnglish ),
	iFlags( EFlagsNull ),
	iRegistryMajorVersion(0),
	iRegistryMinorVersion(0)
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	,iOriginVerified(true)
	#endif	
	{	
	}

CParameterList::~CParameterList()
	{
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CParameterList::SetResourceFilePath(const std::string& aFilePath)
	{
	iResourceFilePath = StringUtils::FixPathDelimiters(aFilePath);
	if (!IsDirectory(iResourceFilePath))
		{
		throw EParamInvalidDir;
		}
	iFlags |= EFlagsResourceFilePathSet;
	}
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

void CParameterList::SetSystemDrive(const std::string& aFilePath)
	{
	iSystemDrivePath = StringUtils::FixPathDelimiters(aFilePath);
	if (!IsDirectory(iSystemDrivePath))
		{
		throw EParamInvalidDir;
		}
	iFlags |= EFlagsCDriveSet;
	}

void CParameterList::SetSystemDriveLetter(int aDriveLetter)
	{
	iSystemDriveLetter = aDriveLetter;

	if (iSystemDriveLetter < 'a' || iSystemDriveLetter > 'y')
		{
		throw EParamInvalidSystemDrive;
		}

	iFlags |= EFlagsSysDriveSet;
	}

void CParameterList::SetZDrive(const std::string& aFilePath)
	{
	iZDrive = StringUtils::FixPathDelimiters(aFilePath);

	if (!IsDirectory(iZDrive))
		throw EParamInvalidDir;

	iFlags |= EFlagsZDriveSet;
	}

void CParameterList::SetFlag(TFlags aFlag)
	{
	iFlags |= aFlag;
	}

void CParameterList::SetConfigFile(const std::string& aFilePath)
	{
	iConfigFile = StringUtils::FixPathDelimiters(aFilePath);
   
	if ( !FileExists( iConfigFile ) )
		{
		throw EParamMissingConfigurationFile;
		}
			
	iFlags |= EFlagsConfigIniSet;
	}

void CParameterList::SetStubDir(const std::string& aFilePath)
	{
	iSisStubPath = StringUtils::FixPathDelimiters(aFilePath);

	if (!IsDirectory(iSisStubPath))
		throw EParamInvalidDir;

	iFlags |= EFlagsStubDirectorySet;
	}

void CParameterList::SetLanguage(CSISLanguage::TLanguage aLanguage)
	{
	iLanguage = aLanguage;
	iFlags |= EFlagsLanguageWasSet;
	}

void CParameterList::AddSISFile(InstallSISFile& aSISFile)
	{
	iFileNames.push_back(aSISFile);
	}

void CParameterList::EmptyRomLogFiles()
	{
	iRomLogFiles.clear();
	iFlags &= ~EFlagsRomRofsLogFilesSet;
	}

void CParameterList::AddRomLogFile(std::wstring& aRomLogFile)
	{
	iRomLogFiles.push_back(aRomLogFile);
	iFlags |= EFlagsRomRofsLogFilesSet;
	}

void CParameterList::EmptyPkgUids()
	{
	iPkgUIDs.clear();
	}

void CParameterList::AddPkgUid(std::wstring& aPkgUid)
	{
	iPkgUIDs.push_back(aPkgUid);
	}

void CParameterList::SetSisRegistryVersion(const std::string& aVerString)
	{
	if (aVerString == SisRegistry::KRegistryV40string)
		{
		iRegistryMajorVersion = 4;
		iRegistryMinorVersion = 0;
		}
	else if (aVerString == SisRegistry::KRegistryV50string) // fixed DEF096906 - Added iRemoveWithLastDependant
		{
		iRegistryMajorVersion = 5;
		iRegistryMinorVersion = 0;
		}
	else if (aVerString == SisRegistry::KRegistryV51string) // introduced isRemovable flag
		{
		iRegistryMajorVersion = 5;
		iRegistryMinorVersion = 1;
		}
	else if (aVerString == SisRegistry::KRegistryV52string) // fixed DEF114705 - SIS controller incorrect
		{
		iRegistryMajorVersion = 5;
		iRegistryMinorVersion = 2;
		}
	else if (aVerString == SisRegistry::KRegistryV53string) // introduced SU cert flag
		{
		iRegistryMajorVersion = 5;
		iRegistryMinorVersion = 3;
		}
	else if (aVerString == SisRegistry::KRegistryV54string) // introduced Device Supported Languages ( Exact Match and Equivalent match )
	    {
	    iRegistryMajorVersion = 5;
	    iRegistryMinorVersion = 4;
	    }
	else
		{
		throw EParamInvalidRegistryVersion;
		}
	}

void CParameterList::ValidateParam()
	{
	if (!iFileNames.empty())
		{
		SISFileList::iterator curr = iFileNames.begin();

		// Touching up the InstallSISFile's iTargetDrive to the right System drive letter
		for (; curr != iFileNames.end(); ++curr)
			{
			if (curr->iTargetDrive == '$')
				{
				curr->iTargetDrive = iSystemDriveLetter;
				}
			}
		}

	if ( (iFlags & EFlagsDisableZDriveChecksSet) != 0)
    	{
		return;
    	}
	// Some options are mutually inclusive.
	//
	// When using -r, then -t must also be specified.
	// When using -z, -r cannot be specified.
	//
	const bool romLogs = ( iFlags & EFlagsRomRofsLogFilesSet ) != 0;
	const bool zDrive = ( iFlags & EFlagsZDriveSet ) != 0;
	const bool sisStubs = ( iFlags & EFlagsStubDirectorySet ) != 0;						

    if ( romLogs && zDrive )
        {
        // Both are not allowed
        throw EParamUseEitherRomRofsLogsOrZDrive;
        }
	else if ( romLogs && !sisStubs )
        {
        // SIS stubs are mandatory when using -R
        throw EParamMissingSisStubFilePath;
        }
	else if (!iFileNames.empty() && !iPkgUIDs.empty())
		{
		// -x and -s option are mutually exclusive
		throw EParamInstallRemoveExclusive;
		}
	}

void CParameterList::DisplayError(TParamException aError)
	{
	wchar_t* msg = NULL;
    //
	switch(aError)
		{
		case EParamInvalidDir:
			msg = L"Invalid directory(s) specified";
			break;
		case EParamInvalidSystemDrive:
			msg = L"Invalid system drive specified";
			break;
		case EParamMissingConfigurationFile:
			msg = L"Configuration file not found";
	        break;
		case EParamUseEitherRomRofsLogsOrZDrive:
			msg = L"Cannot specify both -r and -z. Use one or the other";
			break;
		case EParamMissingSisStubFilePath:
			msg = L"Must specify -t when using -r";
	        break;
		case EParamInvalidRegistryVersion:
			msg = L"Invalid SIS Registry version";
			break;
		case EParamInstallRemoveExclusive:
			msg = L"Cannot have both -s and -x options at the sametime";
			break;
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		case EParamRegVersionOriginVerificationExclusive:
			msg = L"Cannot have both reg file version (via -k or file) and -f options at the sametime";
			break;
		#endif
		case EParamUnknownOption:
		default:
			msg = L"Unknown error";
			break;
		}
	LERROR(msg << std::endl);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CParameterList::SetOriginVerificationStatus(bool aOriginVerified)
	{	
	iOriginVerified = aOriginVerified;	
	}
#endif
// End of File
