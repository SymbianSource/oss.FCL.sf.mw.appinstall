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


#ifndef	SISFILE_H
#define	SISFILE_H

#pragma warning (disable: 4786)

#include <string>

#include "symbiantypes.h"
#include "installablefile.h"
#include "version.h"
#include "sisregistrytoken.h"
#include "siscontents.h"
#include "sisinstallblock.h"
#include "sisexpression.h"
#include "sisproperty.h"
#include "sisproperties.h"

class CSISContents;
class CSISPrerequisites;
class CSISProperties;
class CSISProperty;
class ExpressionEvaluator;

typedef std::vector<TUint32>		PackageUids;
/**
* @file SISFILE.H
*
* @internalComponent
* @released
*/
class SisFile
	{
public:
	SisFile(const std::wstring& aFilename);
	~SisFile();

	TUint32 GetPackageUid() const;

	void       CheckValid() const;

	const CSISPrerequisites* GetDependencies() const;
	const CSISProperties* GetProperties() const;

	bool GetInstallableFiles(InstallableFiles& aFiles, 
							ExpressionEvaluator& aEvaluator,
							const std::wstring& aCDrivePath,
							int aInstallingDrive) const;

	void GetControllerData(const char*& aData, int& aLen) const;

	std::wstring GetVendorLocalName() const;

	std::wstring GetVendorName() const;

	std::wstring GetPackageName() const;

	TUint32 GetIndex() const;

	TUint32 GetSigned() const;
	TUint32 GetInstallType() const;
	TUint32 GetInstallFlags() const;
	void AddInstallFlags(const TUint8 aFlag) const;

	TUint32 GetLanguage() const;

	Version GetVersion() const;

	const Controllers GetControllerInfo(const TUint16 aRegistryFileMajorVersion, 
		const TUint16 aRegistryFileMinorVersion) const;

	std::vector<TInt> GetAllInstallChainIndices() const;

	PackageUids GetEmbeddedPackageUids() const;
	bool IsSupportedLanguage(TUint32 aLanguage) const;

	bool HasEmbedded() const;
	 
	void MakeSISStub(std::wstring& aFileName);
	
	const CSISController& GetController(); 
	
	void ProcessEmbeddedFileWarning(const CSISInstallBlock& aInstallBlock) const;

	bool ProcessInstallOptionsWarning(const CSISInstallBlock& aInstallBlock, std::string& aError);

private:
	void GetInstallableFiles(	InstallableFiles& aFiles, 
								const CSISInstallBlock& aInstallBlock, 
								const std::wstring& aDrivePath,
								int aInstallingDrive) const;
	
	void ProcessInstallBlock(const CSISInstallBlock& aInstallBlock,
							InstallableFiles& aFiles,
							ExpressionEvaluator& aEvaluator,
							const std::wstring& aCDrivePath,
							int aInstallingDrive) const;

private:
	CSISContents iContents;
	};
#endif	/* SISFILE_H */
