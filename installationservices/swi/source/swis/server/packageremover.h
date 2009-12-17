/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SWIS package remover
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef __PACKAGEREMOVER_H__
#define __PACKAGEREMOVER_H__


#include "sishelperclient.h"
#include "sisregistrysession.h"

namespace Swi
{

// Forward declaration
namespace Sis
{
class CController;
class CFileDescription;
}

class TControllerAndLevel
	{
public:
	TControllerAndLevel(TInt aLevel, Sis::CController* aController) : iLevel(aLevel), iController(aController) {};

public:
	TInt				iLevel;
	Sis::CController* 	iController;
	};

class TFileDescAndInstallType
	{
public:
	TFileDescAndInstallType(Sis::TInstallType aType, Sis::CFileDescription* aFileDesc) : iType(aType), iFileDesc(aFileDesc) {};

public:
	Sis::TInstallType		iType;
	Sis::CFileDescription* 	iFileDesc;
	};

class CUninstalledPkgEntry : public CBase
	{
public:
	enum TPackageType
	{
	    ESaPackage,       ///< Standard Application
		ESpPackage,       ///< Standard Patch (augmentation)
		EPuPackage,       ///< Partial Upgrade
		EPaPackage,       ///< Preinstalled Application
		EPpPackage        ///< Preinstalled Patch
	};

public:
	static CUninstalledPkgEntry* NewLC(const TUid& aUid, const TDesC& aPackageName,
		const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType,
		const TDesC& aSisCtlFileName, const TDesC& aAssociatedStubSisName, const TDesC8& aControllerHash);

	~CUninstalledPkgEntry();
	
	inline const TUid& Uid() const;
	
	inline const TDesC& PackageName() const;

	inline const TDesC& VendorName() const;

	inline const TVersion& Version() const;
	
	inline TPackageType PackageType() const;
	
	inline const TDesC& SisCtlFileName() const;

	inline const TDesC& AssociatedStubSisName() const;
	
	inline const TDesC8& HashController() const;
	
	void SetAssociatedStubSisNameL(const TDesC& aAssociatedStubSisName);

private:
	void ConstructL(const TUid& aUid, const TDesC& aPackageName,
		const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType, 
		const TDesC& aSisCtlFileName, const TDesC& aAssociatedStubSisName, const TDesC8& aControllerHash);

	CUninstalledPkgEntry();

private:
	TUid iUid;
	HBufC* iPackageName;
	HBufC* iVendorName;
	TVersion iVersion;
	CUninstalledPkgEntry::TPackageType iPackageType;
	HBufC* iSisCtlFileName;	// sis file or controller file path name
	HBufC* iAssociatedStubSisName; // associated stub sis file path when iSisCtlFileName contains ctl file
	HBufC8* iHashController; // hash of the Controller 
	};

inline const TUid& CUninstalledPkgEntry::Uid() const
	{
	return iUid;
	}	

inline const TDesC& CUninstalledPkgEntry::PackageName() const
	{
	return *iPackageName;
	}	

inline const TDesC& CUninstalledPkgEntry::VendorName() const
	{
	return *iVendorName;
	}	

inline const TVersion& CUninstalledPkgEntry::Version() const
	{
	return iVersion;
	}	

inline CUninstalledPkgEntry::TPackageType CUninstalledPkgEntry::PackageType() const
	{
	return iPackageType;
	}

inline const TDesC& CUninstalledPkgEntry::SisCtlFileName() const
	{
	return *iSisCtlFileName;
	}	
	
inline const TDesC& CUninstalledPkgEntry::AssociatedStubSisName() const
	{
	return *iAssociatedStubSisName;
	}
	
inline const TDesC8& CUninstalledPkgEntry::HashController() const
	{
	return *iHashController;
	}

/**
 * Implementation for querying/removing uninstalled packages. Handles interactions between
 * SISHelper, SISRegistry.
 *
 * @internalComponent
 * @released
 */
class CPackageRemover : public CBase
	{
public:

	static CPackageRemover* NewL(const RMessage2& aMessage);
	static CPackageRemover* NewLC(const RMessage2& aMessage);
	
	void ListL();
	void RemoveL();

	~CPackageRemover();

private:
	HBufC8* GetControllerFromControllerFileLC(const TDesC& aFileName);
	TBool IsAlreadyAddedL(TDesC8& aHashController, const TDesC& aFileName);
	CPackageRemover(const RMessage2& aMessage);
	void ConstructL();
	void AddIfUninstalledL(const CDir& aDirName, const TDesC& aPathName);
	HBufC8* WriteListToBufferLC();
	HBufC8* ComputeHashL(TDesC8& aController);
	
	TBool CheckFileL(TDesC& aFile);
	void ProcessControllerL();
	void ValidateAndDeleteFilesL();
	TInt DeleteFile(const TDesC& aName);
	void DeletePathIfEmpty(const TDesC& aPath);

private:

	RFs										iFs;
	RSisHelper								iSisHelper; 		// SISHelper session
	RSisRegistrySession						iSisRegistry;
	const RMessage2     					iMessage;           // Message to complete when done

	RPointerArray<CUninstalledPkgEntry>		iUninstalledPkgEntry;

	RPointerArray<TFileDescAndInstallType> 	iFileList;
	RPointerArray<TControllerAndLevel> 		iControllerList;
	Sis::CController* 						iMainController;
	HBufC8* 								iControllerData;
	TChar 									iRemovableDriveChar;
	}; 

} // namespace Swi

#endif	// __PACKAGEREMOVER_H__
