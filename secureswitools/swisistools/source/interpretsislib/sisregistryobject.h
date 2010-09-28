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


#ifndef	SISREGISTRYOBJECT_H
#define	SISREGISTRYOBJECT_H

#include <vector>
#include <string>
#include "sisregistrytoken.h"
#include "serialisable.h"
#include "truststatus.h"
#include "filedescription.h"
#include "sisregistryproperty.h"
#include "sisregistrydependency.h"
#include "installablefile.h"
#include "siscontroller.h"
#include "configmanager.h"

typedef std::vector<FileDescription*>			FileDescriptions;
typedef std::vector<SisRegistryDependency*>		Dependencies;
typedef std::vector<SisRegistryPackage*>		Packages;
typedef std::vector<SisRegistryProperty*>		Properties;

class SisFile;
class RomManager;

/**
* @file SISREGISTRYOBJECT.H
*
* @internalComponent
* @released
*/
class SisRegistryObject : public SisRegistryToken
{
public:

	static const TUint32 KSisPackageCertificateChainValidatedToTrustAnchor;
	static const TUint32 KSisPackageBuiltIntoRom;

	SisRegistryObject (ConfigManager& aConfigManager);

	SisRegistryObject(
		const SisFile& aSis,
		const InstallableFiles& aFiles,
		const TInt aTargetDrive,
		const TUint16 aRegistryFileMajorVersion,
		const TUint16 aRegistryFileMinorVersion,
		const bool aSUFlag,
		ConfigManager& aConfigManager);

	SisRegistryObject(CSISController& aSisController,
				RomManager& aRomManager,
				const TUint16 aRegistryFileMajorVersion,
				const TUint16 aRegistryFileMinorVersion,
				ConfigManager& aConfigManager,
				const std::wstring& aStubFileName );

	virtual ~SisRegistryObject ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	const std::vector<TInt>& GetInstallChainIndices() const
	{ return iInstallChainIndices; }

	void SetInstallChainIndices(const std::vector<TInt>& aInstallChainIndices)
	{ iInstallChainIndices = aInstallChainIndices; }

	const TrustStatus& GetTrustStatus() const
	{ return *iTrustStatus; }

	void SetTrustStatus(const TrustStatus& aTrustStatus)
	{ delete iTrustStatus; iTrustStatus = new TrustStatus(aTrustStatus); }

	TInt64 GetTrustTimeStamp() const
	{ return iTrustTimeStamp; }

	void SetTrustTimeStamp(TInt64& aTrustTimeStamp)
	{ iTrustTimeStamp = aTrustTimeStamp; }

	TInt GetRemoveWithLastDependent() const
	{ return iRemoveWithLastDependent; }

	void SetRemoveWithLastDependent(TInt& aIsRemoveWithLastDependent)
	{ iRemoveWithLastDependent = aIsRemoveWithLastDependent; }

	TInt GetTrust() const
	{ return iTrust; }

	void SetTrust(TInt& aTrust)
	{ iTrust = aTrust; }

	TUint16 GetFileMinorVersion() const
	{ return iFileMinorVersion; }

	void SetFileMinorVersion(TUint16& aFileMinorVersion)
	{ iFileMinorVersion = aFileMinorVersion; }

	TUint16 GetFileMajorVersion() const
	{ return iFileMajorVersion; }

	void SetFileMajorVersion(TUint16& aFileMajorVersion)
	{ iFileMajorVersion = aFileMajorVersion; }

	TBool GetPreInstalled() const
	{ return iDeletablePreInstalled; }

	void SetPreInstalled(TBool& aPreInstalled)
	{ iDeletablePreInstalled = aPreInstalled; }

	TBool IsPreInstalled() const
	{ return (iInstallType == CSISInfo::EInstPreInstalledApp); }

	TBool GetSigned() const
	{ return iSigned; }

	void SetSigned(TBool& aSigned)
	{ iSigned = aSigned; }

	TBool GetInRom() const
	{ return iInRom; }

	void SetInRom(TBool& aInRom)
	{ iInRom = aInRom; }

	const FileDescriptions& GetFileDescriptions() const
	{ return iFileDescriptions; }

	TBool GetIsRemovable() const
	{ return iIsRemovable; }

	void SetIsRemovable(TBool& aIsRemovable)
	{ iIsRemovable = aIsRemovable; }

	TBool GetSignedBySuCert() const
	{ return iSignedBySuCert; }

	void SetSignedBySuCert(TBool& aSignedBySuCert)
	{ iSignedBySuCert = aSignedBySuCert; }

	FileDescription const * GetFile(const std::wstring& aFile) const;
	FileDescription const * GetFile(TUint32 aSid) const;

	void SetFileDescriptions(const FileDescriptions& aFileDescriptions);

	const Properties& GetProperties() const
	{ return iProperties; }

	void SetProperties(const Properties& aProperties);

	const Packages& GetEmbeddedPackages() const
	{ return iEmbeddedPackages; }

	void SetEmbeddedPackages(const Packages& aEmbeddedPackages);

	const Dependencies& GetDependencies() const
	{ return iDependencies; }

	void SetDependencies(const Dependencies& aDependencies);

	TUint32 GetInstallType() const
	{ return iInstallType; }

	void SetInstallType(TUint32& aInstallType)
	{ iInstallType = aInstallType; }

	const std::wstring& GetVendorLocalizedName() const
	{ return *iVendorLocalizedName; }

	void SetVendorLocalizedName(const std::wstring& aVendorLocalizedName)
	{ *iVendorLocalizedName = aVendorLocalizedName; }

	const std::wstring& GetStubFileName() const
	{ return iStubFileName;	}

	void SetStubFileName(const std::wstring& aStubFileName)
	{ iStubFileName = aStubFileName; }

	const std::vector<TInt>& GetMatchingSupportedLanguages() const
	{ return iSupportedLanguageIds; }

	const std::vector<CSISString*>& GetLocalizedPackageNames() const
	{ return iLocalizedPackageNames; }
	
	const std::vector<CSISString*>& GetLocalizedVendorNames() const
	{ return iLocalizedVendorNames; }
	
	// Utility functions

	std::wstring GetRegistryDir(const std::wstring& aDrivePath) const;
	int NextSisRegistryIndex(const std::wstring& aDrivePath) const;
	int NextSisControllerIndex(const std::wstring& aDrivePath) const;

	// Update the entry for a PU installation
	void UpgradeEntry(const SisFile& aSis, const InstallableFiles& aFiles, 
					TInt aInstallDrive, const bool aSUFlag, const bool aIsRemovable, ConfigManager& aConfigManager);
					
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void SetValidationStatus(bool aOriginVerificationStatus);
	
	TBool GetIsHidden() const
	{ return iIsHidden; }

	void SetIsHidden(TBool& aIsHidden)
	{ iIsHidden = aIsHidden; }
	#endif
	
private:

	// PU installations
	void UpdateDrives();
	void AddFiles(const InstallableFiles& aFiles);
	void RemoveFiles(const InstallableFiles& aFiles);

private:

	std::wstring*                             iVendorLocalizedName;
	TUint32                                   iInstallType;
	Dependencies						      iDependencies;
	Packages						          iEmbeddedPackages;
	Properties						          iProperties;
	FileDescriptions						  iFileDescriptions;
	TBool                                     iInRom;
	TBool                                     iSigned;
	TBool									  iSignedBySuCert;
	TBool                                     iDeletablePreInstalled;
	TUint16                                   iFileMajorVersion;
	TUint16                                   iFileMinorVersion;
	TInt                                    iTrust;
	TInt                                    iRemoveWithLastDependent;
	TInt64                                    iTrustTimeStamp;
	TrustStatus*                              iTrustStatus;
	std::vector<TInt>                       iInstallChainIndices;
	TBool									  iIsRemovable;
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TBool									iIsHidden;
	#endif
	std::wstring                             iStubFileName;
	// Array of language Id's that are common to the languages in mentioned
	// in SIS file and device supported languages
	std::vector<TInt> iSupportedLanguageIds;

	// Localized packagename info in supported languages
	std::vector<CSISString*> iLocalizedPackageNames;
	
	// Localized vendorname info in supported languages
	std::vector<CSISString*> iLocalizedVendorNames;
	
	ConfigManager& iConfigManager;
};


#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryObject*>& val);

#endif

#endif	/* SISREGISTRYOBJECT_H */
