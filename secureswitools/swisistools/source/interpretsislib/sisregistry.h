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
* SisRegistry - All operations related to the registry.
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef	SISREGISTRY_H
#define	SISREGISTRY_H

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

class DbHelper;
#include "../xmlparser/xmlparser.h"

#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <vector>
#include <string>
#include <map>
#include <list>

#include "siscontroller.h"
#include "symbiantypes.h"
#include "installablefile.h"
#include "parameterlist.h"
#include "sisregistryobject.h"


// Forward Declarations
class SisRegistryPackage;
class HashContainer;
class SisRegistryObject;
class SisFile;
class RomManager;
class ConfigManager;
class CSISController;

class SisRegistry
	{
public: // Constructor & destructor
	SisRegistry( CParameterList& aParamList, RomManager& aRomManager, ConfigManager& aConfigManager);
	~SisRegistry();

public: // API
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void BackupCtl(TUint32 aUid);
	void RestoreCtl(TUint32 aUid, TBool& aBackupFlag);
	void RemoveCtlBackup(TUint32 aUid);
#endif
	TBool IsInstalled(TUint32 aUid) const;
	const SisRegistryPackage& SidToPackage(TUint32 aSid);
	void SidToFileName(TUint32 aSid, std::wstring& aFileName);
	bool SidExists(TUint32 aSid) const;
	const SisRegistryObject& GetRegistryObject(TUint32 aUid) const;
	SisRegistryObject* GetRegistryObjectPtr(TUint32 aUid);
	void RemoveEntry(const TUint32 aUid, const std::wstring& aPackageName, const std::wstring& aVendorName);
	void RemovePkg(const TUint32& aUid, bool aDeleteRomStub = false);
	void RemovePkgLegacy(const TUint32& aUid, bool aDeleteRomStub = false);
	
	void AddRegistryEntry(const SisFile& aSis, const InstallableFiles& aFiles,
							const InstallSISFile& aInstallSISFile);
	void UpdateRegistryEntry(const SisFile& aSis, const InstallableFiles& aFiles,
							const InstallSISFile& aInstallSISFile);
	const SisRegistryObject* OwningPackage(const std::wstring& aFileName, TUint32& aUid) const;
	void RemoveBackupLst();
	bool IsRomStubPackage(const TUint32 aUid);
	void GetStubFileEntries(const TUint32 aUid, std::list<std::wstring>& aFilesEntries);
	void GenerateStubRegistry();
	int GetAugmentationsNumber(TUint32 aUid);
	CSISController* GetStubController(const TUint32 aUid);

	static const int KSisRegistryMajorVersion;
	static const int KSisRegistryMinorVersion;
	static const std::string KRegistryV40string;
	static const std::string KRegistryV50string;
	static const std::string KRegistryV51string;
	static const std::string KRegistryV52string;
	static const std::string KRegistryV53string;
	static const std::string KRegistryV54string;
	typedef std::multimap<TUint32, SisRegistryObject*> EntryMap;

private: // Internal methods
    void ReadStubs( const std::wstring& aDirectory );
    void ReadRegistry( const std::wstring& aRegistryBasePath );
	CSISController* GetStubControllerInDir( const std::wstring& aDirectory, const TUint32 aUid);
	CSISController* ReadStubController( const wchar_t* aFilename );
	void GenerateRegistryEntry(SisRegistryObject& aObj, const SisFile& aSis);
	void ExtractRegistryFiles(const std::wstring& path);
	void RemoveInstalledFiles(const FileDescriptions& aFileDes, std::wstring& aLocalPath);
	void GenerateStubRegistry(SisRegistryObject* aSisRegistryObject, CSISController* aSisController);
	void SetNextIndex(SisRegistryObject& aObj) const;
	std::wstring GetRegistryDir( const std::wstring& aDrivePath, TUint32 aUid  ) const;
	void GenerateRegFile(SisRegistryObject& aObj) const;
	void GenerateCtlFile(SisRegistryObject& aObj, const SisFile& aSis) const;

public:
	TUint32 GetUid(TUint32 aSid) const;
	TUint32 GetUid(const std::wstring& aSidFile) const;
	bool GetInRom(TUint32 aUid) const;
	std::string GetDbPath();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	const DbHelper* GetDbHelper() const;	
#endif

private: // Utility functions
    static bool IsPackageDir( const std::wstring& aString );
	void ReadSisRegistryVersion();
	void FindMarkerAndSetVersion(const std::wstring& aPath);
	TBool IsSisValid(const std::wstring& aStubPath);
	bool IsFileWideCard(const std::wstring& aFileName);

private: // Friends
	friend struct Contains;

private: // Constants
	static const std::wstring KPathToRegistry;
	static const std::wstring KPathToRomStubs;
	static const std::wstring KPathToRegistryVersionMarker;
	static const std::wstring KSisRegistryVersion;
	// SisRegistry versions
	static const std::wstring KRegistryFormat;
	static const std::wstring KRegistryV40;
	static const std::wstring KRegistryV51;
	static const std::wstring KRegistryV52;
	static const std::wstring KRegistryV53;
	static const std::wstring KRegistryV54;
	static const std::wstring KRomStubDir;
	static const std::wstring KRomRegistryVersionMarker;

private: // Data members
	std::wstring	iCDrive;
	TInt			iSystemDrive;
    CParameterList& iParamList;
    RomManager& iRomManager;
	ConfigManager& iConfigManager;
	EntryMap iEntries;


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
private:
	// internal methods for accessing database
	void RemovePkgWithScr(TUint32 aUid, bool aSkipRomStub=false);
	

private:
	XmlDetails::TScrPreProvisionDetail::TComponent CreateRegistryEntry(const SisRegistryObject& aSisRegistryObject);
	
	// specific methods for adding SisRegistryObject
	void AddLocalizedNames	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const SisRegistryObject& aSisRegistryObject
								);

	
	void AddComponentDependencies	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										const SisRegistryObject& aSisRegistryObject 
									);

	void AddEmbeddedPackages	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const std::vector<SisRegistryPackage*>& aEmbeddedPackages 
								);
	
	void AddProperties	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
							const std::vector<SisRegistryProperty*>& aProperties 
						);
	
	void AddFileDescription	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
								const std::vector<FileDescription*>& aFileDescription 
							);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void AddApplicationRegistrationInfoL	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
								const std::vector<FileDescription*>& aFileDescription,
								int aInRom 
							);
#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	

	void AddFileDescriptionAsFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile, 
												const FileDescription* aFileDescription
											);

	void AddTrustStatus	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, const TrustStatus& aTrustStatus );

	void AddInstallChainIndices	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const std::vector<TInt>& aInstallChainIndices 
								);
	
	// specific methods for adding SisRegistryToken
	void AddSisRegistryToken	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const SisRegistryToken& aSisRegistryToken 
								);

	void AddControllerInfo	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
								const std::vector<ControllerInfo*>& aControllerInfo 
							);

	void AddHashContainer	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
								const HashContainer& aHashContainer,
								int aCount
							);

	void AddVersion	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
						const Version& aVersion
					);

	// specific methods for adding SisRegistryPackage
	void AddSisRegistryPackage	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
									const SisRegistryPackage& aSisRegistryPackage,
									int aLanguage
								);


	// physical schema methods
	XmlDetails::TScrPreProvisionDetail::TComponentProperty CreateComponentProperty	(	const std::wstring& aPropName, 
																						const std::wstring& aPropValue, 
																						bool aIsInt, 
																						bool aIsStr8Bit,
																						int aLocale=CSISLanguage::ELangTest 
																					);

	void AddComponentProperty	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent,
									const std::wstring& aPropName,
									int aPropValue,
									int aCount
								);

	void AddComponentProperty	(	XmlDetails::TScrPreProvisionDetail::TComponent& aComponent,
									const std::wstring& aPropName,
									const std::wstring& aPropValue,
									int aCount,
									bool aIsBinary
								);

	void AddFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile,
								const std::wstring& aPropName,
								const std::wstring& aPropValue,
								bool aIsBinary
							);

	void AddFileProperty	(	XmlDetails::TScrPreProvisionDetail::TComponentFile& aComponentFile,
								const std::wstring& aPropName,
								int aPropValue,
								bool aIsBinary
							);

	// other helper methods
	

	std::wstring FormatVersionToString( const Version& aVersion );

	std::string GetEpocRoot();

	void UpdateInstallationInformation(XmlDetails::TScrPreProvisionDetail aScrPreProvisionDetail);
	
	void GenerateDbRegistryEntry(const SisRegistryObject& aSisRegistryObject, bool aOriginVerified);

	std::wstring GetGlobalId( TUint32 aUid , TInt aInstallType, std::wstring aPackageName);

	void SetOriginVerification(XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, bool aOriginVerified);
private:
	DbHelper* iDbHelper;

	static const std::wstring NativeSoftwareType;

#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
};

#endif	/* SISREGISTRY_H */

