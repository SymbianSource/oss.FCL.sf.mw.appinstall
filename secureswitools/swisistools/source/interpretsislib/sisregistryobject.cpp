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
#include "sisregistryobject.h"

// System includes
#include <openssl/sha.h>
#include <iostream>

// SisX Includes
#include "sisdatetime.h"
#include "sisstring.h"
#include "sisfiledata.h"
#include "sisproperties.h"
#include "sisproperty.h"
#include "sisdependency.h"
#include "sisprerequisites.h"
#include "sisinstallblock.h"
#include "sisfiledescription.h"
#include "sissupportedlanguages.h"
#include "sislanguage.h"
#include "siscapabilities.h"
#include "sisversion.h"

// User includes
#include "symbiantypes.h"
#include "is_utils.h"
#include "deserialiser.h"
#include "sisfile.h"
#include "installablefile.h"
#include "serialiser.h"
#include "hashcontainer.h"
#include "errors.h"
#include "rommanager.h"
#include "stringutils.h"

// Type definitions
typedef int TInt;
typedef bool TBool;



// from swi/sispackaGetrust.h
const TUint32 SisRegistryObject::KSisPackageCertificateChainValidatedToTrustAnchor = 200;
const TUint32 SisRegistryObject::KSisPackageBuiltIntoRom = 500;

const TInt KExecutableImageUidValue=0x1000007a;

const TUint32 KDriveZ = 1 << 25;
const TUint32 KDriveC = 1 << 2;



Deserialiser& operator>>(Deserialiser& aInput, std::vector<CSISString*>& val)
    {
    TUint32 size = 0;
    aInput>> size;
    val.resize(size);
    for (TUint32 i = 0; i < size ; ++i)
        {
         std::wstring *tmpString = new std::wstring();
         aInput >> *tmpString;
         val[i] = new CSISString(*tmpString);
         delete tmpString;
        }
    return aInput;
    }

Serialiser& operator<<(Serialiser& aOutput, std::vector<CSISString*>& val)
    {
    TUint32 size = val.size();
    aOutput << size;
    for (TUint32 i = 0; i < size ; ++i)
        {
        std::wstring tmpStr = val[i]->GetString();
        aOutput << tmpStr;
        }
    return aOutput;
    }



SisRegistryObject::SisRegistryObject (ConfigManager& aConfigManager):iConfigManager(aConfigManager)
	{
	iVendorLocalizedName = new std::wstring();
	iTrustStatus = new TrustStatus();
	}


SisRegistryObject::SisRegistryObject(const SisFile& aSis,
									 const InstallableFiles& aFiles,
									 const TInt aTargetDrive,
									 const TUint16 aRegistryFileMajorVersion,
									 const TUint16 aRegistryFileMinorVersion,
									 const bool aSUFlag,
									 ConfigManager& aConfigManager)
	: SisRegistryToken(
	  	aSis.GetIndex(),
		aSis.GetVendorName(),
		aSis.GetPackageName(),
		aSis.GetPackageUid(),
		aTargetDrive,
		aSis.GetLanguage(),
		aSis.GetVersion(),
		aSis.GetControllerInfo(aRegistryFileMajorVersion,aRegistryFileMinorVersion),
		KDriveC),
	  iVendorLocalizedName(new std::wstring(aSis.GetVendorLocalName())),iConfigManager(aConfigManager)
{
	for( InstallableFiles::const_iterator curr = aFiles.begin(); curr != aFiles.end(); ++curr )
	{	
		InstallableFile* installableFile= *curr;	
		#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		FileDescription* f = new FileDescription(*installableFile->FileDescription() , installableFile->Sid(), aTargetDrive, installableFile->GetTarget());
		#else
		FileDescription* f = new FileDescription(*installableFile->FileDescription() , installableFile->Sid(), aTargetDrive, installableFile->GetTarget(),installableFile->GetLocalTarget());
		#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iFileDescriptions.push_back(f);
	}

	iTrustStatus = new TrustStatus();
	TUint64 x = 0;
	iTrustStatus->SetLastCheckDate(x);
	iTrustStatus->SetQuaratined((TUint32)0);
	iTrustStatus->SetQuaratinedDate(x);
	iTrustStatus->SetResultDate(x);
	iTrustStatus->SetRevocationStatus(TrustStatus::KOcspNotPerformed);
	iTrustStatus->SetValidationStatus(TrustStatus::KValidatedToAnchor);

	const CSISProperties::SISPropertyArray& props =	aSis.GetProperties()->Properties();

	for (int i = 0; i < props.size(); ++i)
		{
		SisRegistryProperty* p = new SisRegistryProperty(props[i]);
		iProperties.push_back(p);
		}

	const CSISArray<CSISDependency, CSISFieldRoot::ESISDependency>& deps =
		aSis.GetDependencies()->DependencyList();

	for(i = 0; i < deps.size(); ++i)
		{
			SisRegistryDependency* p = new SisRegistryDependency(deps[i]);
			iDependencies.push_back(p);
		}

	iFileMajorVersion = aRegistryFileMajorVersion;
	iFileMinorVersion = aRegistryFileMinorVersion;

	iInstallType = aSis.GetInstallType();
	iInRom = false;
	if (iInstallType == CSISInfo::EInstPreInstalledApp)
		{
		iDeletablePreInstalled = true;
		}
	else
		{
		iDeletablePreInstalled = false;
		}
	iSigned	= aSis.GetSigned();
	iTrust = KSisPackageCertificateChainValidatedToTrustAnchor;
	iRemoveWithLastDependent = 0;
	iTrustTimeStamp = 0;
	iInstallChainIndices = aSis.GetAllInstallChainIndices();
	if(iFileMajorVersion == 4)
		iIsRemovable = true;
	else
		iIsRemovable = !(aSis.GetInstallFlags() & CSISInfo::EInstFlagNonRemovable);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	iIsHidden = !(!(aSis.GetInstallFlags() & CSISInfo::EInstFlagHide));
	#endif

	Sids sids;
	GetSidsFromInstallable(aFiles, sids);
	TUint32 drives = 0;
	if (!iFileDescriptions.empty())
		drives = 1 << (aTargetDrive - 'A');

	SetSids(sids);
	SetDrives(drives);

	iSignedBySuCert = aSUFlag;

	// Set Localized Package Names and Vendor Names
	std::vector<int> matchingDeviceLanguagesArray = iConfigManager.GetMatchingSupportedLanguages();
	TInt matchingLanguageCount = matchingDeviceLanguagesArray.size();
	
	for (TInt i=0; i < matchingLanguageCount; i++)
		{
		TInt languageId = matchingDeviceLanguagesArray.at(i);
		CSISController sisController = aSis.GetController();
		TInt supportedLanguageCount = sisController.LanguageCount();
	
		for ( TInt j=0; j < supportedLanguageCount; j++ )
			{
			if ( sisController.Language(j) == languageId)
				{
				iSupportedLanguageIds.push_back(languageId);
				iLocalizedVendorNames.push_back(new CSISString(sisController.SISInfo().VendorName(j)));
				iLocalizedPackageNames.push_back(new CSISString(sisController.SISInfo().PackageName(j)));
				break;
				}
			}
		}
}

SisRegistryObject::SisRegistryObject(CSISController& aSisController,
									 RomManager& aRomManager,
									 const TUint16 aRegistryFileMajorVersion,
									 const TUint16 aRegistryFileMinorVersion,
									 ConfigManager& aConfigManager,
									 const std::wstring& aStubFileName
									 )
	: SisRegistryToken(),
	iFileMajorVersion(aRegistryFileMajorVersion),
	iFileMinorVersion(aRegistryFileMinorVersion),
	iInRom(true),
	iDeletablePreInstalled(false),
	iIsRemovable(false),
	iConfigManager(aConfigManager)
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	,iIsHidden(false)
	#endif
{
	//*** SisRegistryPackage ***//
	iStubFileName = aStubFileName;

	// UID
	const CSISInfo& info = aSisController.SISInfo();
	TUint32 uid = info.UID1();
	SetUid(uid);

	// Vendor Name
	std::wstring vendorName = aSisController.SISInfo().UniqueVendorName();
   	SetVendorName(vendorName);

	// Package Name
	std::wstring packageName;
	if (aSisController.SISInfo().PackageNameCount() > 0 )
		{
		packageName = aSisController.SISInfo().PackageName(0);
		SetPackageName(packageName);
		}
	else
		{
		std::string error = "can not retrieve package name";
		throw InvalidSis("", error, INVALID_SIS);
		}

	// Index
	TInt dataIndex = aSisController.DataIndex();
	SetIndex(dataIndex);

	//*** SisRegistryToken ***//

	// Language
	TInt lang = aSisController.Language(0);
	SetLanguage(lang);

	// Version
	const CSISVersion& sisVersion = aSisController.SISInfo().SISVersion();
	Version version = Version(sisVersion.Major(), sisVersion.Minor(), sisVersion.Build());
	SetVersion(version);

	// Controller Info
	ControllerInfo* ci = new ControllerInfo();
	ci->CalculateAndSetHash(aSisController,aRegistryFileMajorVersion,aRegistryFileMinorVersion);
	ci->SetVersion(version);
	ci->SetOffset(0);

	Controllers controllers;

	controllers.push_back(ci);

	// Embedded SIS controllers
	TControllerMap embeddedCtls;
	aSisController.InstallBlock().GetEmbeddedControllers(embeddedCtls, false);
	for (TControllerMapConstIter iter = embeddedCtls.begin(); iter != embeddedCtls.end(); ++iter)	
		{
		const CSISController& ctrl = *iter->second;
		ControllerInfo* ctlInfo = new ControllerInfo();

		ctlInfo->SetVersion(version);
		ctlInfo->CalculateAndSetHash(ctrl, aRegistryFileMajorVersion, aRegistryFileMinorVersion);
		ctlInfo->SetOffset(0);

		controllers.push_back(ctlInfo);
		}

	SetControllerInfo(controllers);

	// Drives
	SetDrives(KDriveZ);

	// Selected Drive
	int drive = 0;
	SetSelectedDrive(drive);

	//*** SisRegistryObject ***//
	iInstallType = aSisController.SISInfo().InstallationType();

	int signatureCount = aSisController.SignatureCount();
	iSigned = (signatureCount == 0)? false:true;

	TInt index = 0;
	for(index = 0; index < signatureCount; ++index)
		{
		iInstallChainIndices.push_back(index);
		}
	iTrust = KSisPackageBuiltIntoRom;
	iRemoveWithLastDependent = 0;

	iTrustTimeStamp = 0;
	iTrustStatus = new TrustStatus();
	TUint64 x = 0;
	iTrustStatus->SetLastCheckDate(x);			///< the last date a revocation check was
												///< attempted
	iTrustStatus->SetQuaratined((TUint32)0);	///< true if quarantined.
												///< Not used currently
	iTrustStatus->SetQuaratinedDate(x);			///< date entry was quarantined.
												///< Not used currently
	iTrustStatus->SetResultDate(x);				///< the last date a successful revocation
												///< check was performed
	iTrustStatus->SetRevocationStatus(TrustStatus::KOcspNotPerformed);
	iTrustStatus->SetValidationStatus(TrustStatus::KPackageInRom);

	// Properties
	const CSISProperties::SISPropertyArray& props = aSisController.Properties().Properties();
	for (int i = 0; i < props.size(); ++i)
		{
		SisRegistryProperty* p = new SisRegistryProperty(props[i]);
			iProperties.push_back(p);
		}

	// File descriptions and SIDs
	Sids aSids;
	int fileCount = aSisController.InstallBlock().FileCount();
	int value = fileCount;
	for(i = 0; i < aSisController.InstallBlock().FileCount(); ++i)
		{
		TUint32 aSid = 0;
		const CSISFileDescription& fileDesc =aSisController.InstallBlock().FileDescription(i);

		std::wstring target = std::wstring(fileDesc.Target().GetString());

			// if the target name contains wildchars, do not open to check SID, it is for eclipsing
			if ((target.find(L"?") == std::wstring::npos) || (target.find(L"*") == std::wstring::npos))
			{
				std::wstring::size_type index = target.rfind(L".");

				if (index != std::wstring::npos)
				{
					std::wstring extOfString = StringUtils::ToUpper(target.substr(index+1));

					const bool fileExists = aRomManager.RomFileExists( target );
					if  ( fileExists )
					{
						SBinarySecurityInfo info;
						const TInt err = aRomManager.ReadSecurityInfo( info, target );

						// SID is only applicable to an exe
						if (!err && (extOfString == L"EXE"))
						{
							aSid = info.iSecureId;
						}
					}
				}
				
				if(aSid)
				{
					aSids.push_back(aSid);
				}
			}
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK //Just to ignore compiler
			FileDescription* aFileDescription = new FileDescription(fileDesc, aSid, L'Z', fileDesc.Target().GetString(), fileDesc.Target().GetString());
			#else
			FileDescription* aFileDescription = new FileDescription(fileDesc, aSid, L'Z', fileDesc.Target().GetString());
			#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			iFileDescriptions.push_back(aFileDescription);
		}
		SetSids(aSids);

	// Dependencies
	const CSISPrerequisites::TDependencyList& deps = aSisController.Prerequisites().DependencyList();
	for (i = 0; i < deps.size(); ++i)
		{
		SisRegistryDependency* p = new SisRegistryDependency(deps[i]);
			iDependencies.push_back(p);
		}

	// Vendor Localized Name
	if ( aSisController.SISInfo().VendorNameCount() > 0 )
		{
		iVendorLocalizedName = new std::wstring(aSisController.SISInfo().VendorName(0));
		}
	else
	{
		std::string error = "can not retrieve localized vendor name";
		std::string x = wstring2string((std::wstring)packageName);
		throw InvalidSis(x, error, INVALID_SIS);
	}

	// Signed by SU cert
	iSignedBySuCert = false;
	
	// Set Localized Package Names and Vendor Names
	std::vector<int> matchingDeviceLanguagesArray = iConfigManager.GetMatchingSupportedLanguages();
	TInt matchingLanguageCount = matchingDeviceLanguagesArray.size();
	
	for (TInt i=0; i < matchingLanguageCount; i++)
		{
		TInt languageId = matchingDeviceLanguagesArray.at(i);
		TInt supportedLanguageCount = aSisController.LanguageCount();
	
		for ( TInt j=0; j < supportedLanguageCount; j++ )
			{
			if ( aSisController.Language(j) == languageId)
				{
				iSupportedLanguageIds.push_back(languageId);
				iLocalizedVendorNames.push_back(new CSISString(aSisController.SISInfo().VendorName(j)));
				iLocalizedPackageNames.push_back(new CSISString(aSisController.SISInfo().PackageName(j)));
				}
			}
		}	
}

SisRegistryObject::~SisRegistryObject ()
	{

	delete iTrustStatus;
	TInt i;
	for (i = 0 ; i < iFileDescriptions.size() ; ++i)
		delete iFileDescriptions[i];
	for (i = 0 ; i < iProperties.size() ; ++i)
		delete iProperties[i];
	for (i = 0 ; i < iEmbeddedPackages.size() ; ++i)
		delete iEmbeddedPackages[i];
	for (i = 0 ; i < iDependencies.size() ; ++i)
		delete iDependencies[i];

	for (i = 0 ; i < iLocalizedPackageNames.size() ; ++i)
		delete iLocalizedPackageNames[i];
	for (i = 0 ; i < iLocalizedVendorNames.size() ; ++i)
		delete iLocalizedVendorNames[i];

	iSupportedLanguageIds.clear();
	delete iVendorLocalizedName;

	}

void SisRegistryObject::Internalize(Deserialiser& des)
	{
	SisRegistryToken::Internalize(des);

	des >> iFileMajorVersion
		>> iFileMinorVersion
		>> *iVendorLocalizedName
		>> iInstallType
		>> iInRom
		>> iDeletablePreInstalled
		>> iSigned
		>> iTrust
		>> iRemoveWithLastDependent
	    >> iTrustTimeStamp
		>> iDependencies
		>> iEmbeddedPackages
		>> iProperties
		>> iFileDescriptions
		>> *iTrustStatus
	    >> iInstallChainIndices;
	if(iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion > 0))
		des >> iIsRemovable;
	else	
		iIsRemovable = true;

	if(iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 3))
		des >> iSignedBySuCert;
	else
		iSignedBySuCert = false;
	if(iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 4))
		{
		des >> iSupportedLanguageIds;
		
		des >> iLocalizedPackageNames;
		
		des >> iLocalizedVendorNames;

		}
	}

void SisRegistryObject::Externalize(Serialiser& ser)
	{
	SisRegistryToken::Externalize(ser);
	ser << iFileMajorVersion
		<< iFileMinorVersion
		<< *iVendorLocalizedName
		<< iInstallType
		<< iInRom
		<< iDeletablePreInstalled
		<< iSigned
		<< iTrust
		<< iRemoveWithLastDependent
		<< iTrustTimeStamp
		<< iDependencies
		<< iEmbeddedPackages
		<< iProperties
		<< iFileDescriptions
		<< *iTrustStatus
		<< iInstallChainIndices;
	if(iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion > 0))
		ser << iIsRemovable;

	if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 3))
		ser << iSignedBySuCert;
	if (iFileMajorVersion > 5 || (iFileMajorVersion == 5 && iFileMinorVersion >= 4))
		{
		ser << iSupportedLanguageIds;
		
		ser << iLocalizedPackageNames;
		
		ser << iLocalizedVendorNames;

		}
	}


FileDescription const * SisRegistryObject::GetFile(const std::wstring& aFile) const
{
 	FileDescriptions::const_iterator end = iFileDescriptions.end();
	for (FileDescriptions::const_iterator curr = iFileDescriptions.begin() ;
		 curr != end; ++curr)
		{
			 if (  FoldedCompare((*curr)->GetTarget(), aFile) == 0)
			 {
				 return *curr;
			 }
		}
	return 0;
}

FileDescription const * SisRegistryObject::GetFile(TUint32 aSid) const
{
	FileDescriptions::const_iterator end = iFileDescriptions.end();
	for (FileDescriptions::const_iterator curr = iFileDescriptions.begin() ;
		 curr != end; ++curr)
		{
			 if (  (*curr)->GetSid() == aSid)
			 {
				 return *curr;
			 }
		}
	return 0;
}


std::wstring SisRegistryObject::GetRegistryDir( const std::wstring& aDrivePath ) const
{
    const TUint32 uid = GetUid();
    std::wstring ret = StringUtils::MakePathFromSID( aDrivePath + L"/sys/install/sisregistry/", uid );
	return ret;
}

int SisRegistryObject::NextSisRegistryIndex(const std::wstring& aDrivePath) const
{
    const TUint32 uid = GetUid();
    std::wstring regPath = GetRegistryDir(aDrivePath);

	for (int index = 0; index < 0xFFFFFFFF; index++)
	{
		std::wstring fileName = regPath + StringUtils::BuildSisRegistryFileName( index );

		if ( !FileExists(fileName) )
		{
			// return the next available index
			return index;
		}
	}

	return 0;
}

int SisRegistryObject::NextSisControllerIndex(const std::wstring& aDrivePath) const
{
    const TUint32 uid = GetUid();
    std::wstring regPath = GetRegistryDir(aDrivePath);

	for (int index = 0; index < 0xFFFFFFFF; index++)
	{
		std::wstring fileName = regPath + StringUtils::BuildControllerFileName(GetIndex(), index);

		if ( !FileExists(fileName) )
		{
			// return the next available index
			return index;
		}
	}

	return 0;
}

void SisRegistryObject::UpgradeEntry(const SisFile& aSis, const InstallableFiles& aFiles, TInt aInstallDrive, const bool aSUFlag, const bool aIsRemovable, ConfigManager& aConfigManager)
{
	// Version
	iVersion = aSis.GetVersion();
	
	// Controller Info. Update the controller info using the same SIS Registry version as the base package
	StoreControllerInfo(aSis.GetControllerInfo(iFileMajorVersion,iFileMinorVersion));

	iInstallType = aSis.GetInstallType();

	iSelectedDrive = toupper(aInstallDrive);

	// With interpretsis, SisFile is default to true
	iSigned = aSis.GetSigned();

	iSignedBySuCert = aSUFlag;

	if(aIsRemovable)
		iIsRemovable = false;
	iTrust = KSisPackageCertificateChainValidatedToTrustAnchor;

	iTrustTimeStamp = 0;

	TUint64 x = 0;
	iTrustStatus->SetLastCheckDate(x);
	iTrustStatus->SetQuaratined((TUint32)0);
	iTrustStatus->SetQuaratinedDate(x);
	iTrustStatus->SetResultDate(x);
	iTrustStatus->SetRevocationStatus(TrustStatus::KOcspNotPerformed);
	iTrustStatus->SetValidationStatus(TrustStatus::KValidatedToAnchor);

	iRemoveWithLastDependent = 0;

	std::vector<TInt> indices = aSis.GetAllInstallChainIndices();

 	std::vector<TInt>::const_iterator end = indices.end();
	int size = indices.size();
	for (std::vector<TInt>::const_iterator curr = indices.begin() ;
		 curr != end; ++curr)
		{
		int w = *curr;
		iInstallChainIndices.push_back(*curr);
		}

	// Properties
	TInt i = 0;
	for (i = 0 ; i < iProperties.size() ; ++i)
		delete iProperties[i];

	const CSISProperties::SISPropertyArray& props = aSis.GetProperties()->Properties();

	for(i = 0; i < props.size(); ++i)
		{
		SisRegistryProperty* p = new SisRegistryProperty(props[i]);
		iProperties.push_back(p);
		}

	// Dependencies
	for (i = 0 ; i < iDependencies.size() ; ++i)
		delete iDependencies[i];

	const CSISPrerequisites::TDependencyList& deps = aSis.GetDependencies()->DependencyList();

	for(i = 0; i < deps.size(); ++i)
		{
		SisRegistryDependency* p = new SisRegistryDependency(deps[i]);
		iDependencies.push_back(p);
		}

	RemoveFiles(aFiles);
	AddFiles(aFiles);
	UpdateDrives();
	
	// Set Localized Package Names and Vendor Names
	std::vector<int> matchingDeviceLanguagesArray = iConfigManager.GetMatchingSupportedLanguages();
	TInt matchingLanguageCount = matchingDeviceLanguagesArray.size();
	
	for (TInt i=0; i < matchingLanguageCount; i++)
		{
		TInt languageId = matchingDeviceLanguagesArray.at(i);
		CSISController sisController = aSis.GetController();
		TInt supportedLanguageCount = sisController.LanguageCount();
	
		for ( TInt j=0; j < supportedLanguageCount; j++ )
			{
			if ( sisController.Language(j) == languageId)
				{
				iSupportedLanguageIds.push_back(languageId);
				iLocalizedVendorNames.push_back(new CSISString(sisController.SISInfo().VendorName(j)));
				iLocalizedPackageNames.push_back(new CSISString(sisController.SISInfo().PackageName(j)));
				break;
				}
			}
		}

}

void SisRegistryObject::UpdateDrives()
{
	TUint32 drives = 0;
 	FileDescriptions::const_iterator end = iFileDescriptions.end();
	for (FileDescriptions::const_iterator curr = iFileDescriptions.begin() ;
		 curr != end; ++curr)
		{
		std::wstring target = (*curr)->GetTarget();
		// At this stage, '!' has already been assigned, so no need to worry about it...
		if (StringUtils::StartsWithDrive(target))
			{
			drives |= 1 << (target.at(0) - 'A');
			}
		}
	SetDrives(drives);
}

void SisRegistryObject::AddFiles(const InstallableFiles& aFiles)
{
	for(InstallableFiles::const_iterator curr = aFiles.begin(); curr != aFiles.end(); ++curr )
	{	
		InstallableFile* installableFile= *curr;
		TUint32 sid = installableFile->Sid();
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		FileDescription* f = new FileDescription(*installableFile->FileDescription() , sid, iSelectedDrive, installableFile->GetTarget(),installableFile->GetLocalTarget());
		#else
		FileDescription* f = new FileDescription(*installableFile->FileDescription() , sid, iSelectedDrive, installableFile->GetTarget());
		#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iFileDescriptions.push_back(f);

		// If the SID is there already there is no reason to add it again
		if (installableFile->IsExe())
		{
			if (std::find(iSids.begin(), iSids.end(), sid) == iSids.end())
			{
			iSids.push_back(sid);
			}
		}
	}
}

void SisRegistryObject::RemoveFiles(const InstallableFiles& aFiles)
{
	// Check the new files to determine which files we need to remove prior to installing them
	for(InstallableFiles::const_iterator curr = aFiles.begin(); curr != aFiles.end(); ++curr )
	{	
		InstallableFile* installableFile= *curr;
		TUint32 sid = installableFile->Sid();
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		FileDescription f = FileDescription(*installableFile->FileDescription(), sid,
											iSelectedDrive, installableFile->GetTarget(),installableFile->GetLocalTarget());
		#else
		FileDescription f = FileDescription(*installableFile->FileDescription(), sid,
										iSelectedDrive, installableFile->GetTarget());
		#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		
		FileDescriptions::const_iterator end = iFileDescriptions.end();
		for (FileDescriptions::iterator curr2 = iFileDescriptions.begin();
			curr2 != end; ++curr2)
		{
			if (f.GetTarget() == (*curr2)->GetTarget())
			{
				// If the file to remove is an .exe, remove it's associated SID 
				if (sid)
				{
					Sids::iterator i = std::find(iSids.begin(), iSids.end(), sid);		
					if (i != iSids.end())
					{
						iSids.erase(i);
					}
				}
			
				delete *curr2;
				iFileDescriptions.erase(curr2);
				break;
			}
		}
	}
}

void SisRegistryObject::SetFileDescriptions(const FileDescriptions& aFileDescriptions)
	{
	iFileDescriptions.resize(aFileDescriptions.size());
	FileDescriptions::const_iterator scurr = aFileDescriptions.begin();
	FileDescriptions::const_iterator end   = aFileDescriptions.end();

	FileDescriptions::iterator dcurr = iFileDescriptions.begin();

	while (scurr != end)
		{
		*dcurr++ = new FileDescription(**scurr++);
		}
	}

void SisRegistryObject::SetProperties(const Properties& aProperties)
	{
	iProperties.resize(aProperties.size());
	Properties::const_iterator scurr = aProperties.begin();
	Properties::const_iterator end   = aProperties.end();

	Properties::iterator dcurr = iProperties.begin();

	while (scurr != end)
		{
		*dcurr++ = new SisRegistryProperty(**scurr++);
		}
}

void SisRegistryObject::SetEmbeddedPackages(const Packages& aEmbeddedPackages)
	{
	iEmbeddedPackages.resize(aEmbeddedPackages.size());
	Packages::const_iterator scurr = aEmbeddedPackages.begin();
	Packages::const_iterator end   = aEmbeddedPackages.end();

	Packages::iterator dcurr = iEmbeddedPackages.begin();

	while (scurr != end)
		{
		*dcurr++ = new SisRegistryPackage(**scurr++);
		}
	}

void SisRegistryObject::SetDependencies(const Dependencies& aDependencies)
	{
	iDependencies.resize(aDependencies.size());
	Dependencies::const_iterator scurr = aDependencies.begin();
	Dependencies::const_iterator end   = aDependencies.end();

	Dependencies::iterator dcurr = iDependencies.begin();

	while (scurr != end)
		{
		*dcurr++ = new SisRegistryDependency(**scurr++);
		}
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void SisRegistryObject::SetValidationStatus(bool aOriginVerificationStatus)
	{
	// Set the validation status based on the input parameter preference.
	if(aOriginVerificationStatus)
		{
		iTrustStatus->SetValidationStatus(TrustStatus::KValidatedToAnchor);
		}
	else
		{
		iTrustStatus->SetValidationStatus(TrustStatus::KValidationStatusUnknown);
		}
	}
#endif
