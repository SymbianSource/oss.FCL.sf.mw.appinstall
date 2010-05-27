/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistry - registry entry class declaration
* It is used to contain the information stored by the registry about 
* any specific native package.    
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISREGISTRYOBJECT_H__
#define __SISREGISTRYOBJECT_H__

#include <e32base.h>
#include <swi/sistruststatus.h>
#include "sispackagetrust.h"
#include <hash.h>
#include "installtypes.h"
#include "sisregistrytoken.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

class RReadStream;
class RWriteStream;
namespace Swi
{
class CApplication;
class CSisRegistryDependency;
class CSisRegistryProperty;
class CSisRegistryFileDescription;
class TSisTrustStatus;

	namespace Sis
	{
	class CController;
	class CVersion;
	}

/**
 * Contains all the information about an installed package.
 *
 * This class stores all the information about a package in the registry
 * to facilitate memory scaling problem CSisRegistryObject is a specialisation of 
 * CSisRegistryToken which in turn inherits from CSisRegistryPackage. 
 * The reason for this orgainisation is that whilst a package is the 
 * main identification element still is useful to store some general information 
 * in RAM in order to be easily accessible in general searches 
 */
class CSisRegistryObject : public CSisRegistryToken
	{
public:

	IMPORT_C static CSisRegistryObject* NewL();
	IMPORT_C static CSisRegistryObject* NewLC();

	/**
	 * Constructs a entry from a given existing .reg filename. 
	 */
	IMPORT_C static CSisRegistryObject* NewL(RReadStream& aStream);
	IMPORT_C static CSisRegistryObject* NewLC(RReadStream& aStream);
	
	virtual ~CSisRegistryObject();
	
	/**
	 * Write the object to a stream 
	 *
	 * @param aStream The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * Read the object from a stream
	 *
	 * @param aStream The stream to read from
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/**
	 * Processes an application to create a registry entry object.
	 *
	 * @param aApplication The application to process
	 * @param aController The raw controller data
	 *
	 */
	void ProcessApplicationL(const CApplication& aApplication, const TDesC8& aController);
	
	/**
	 * Upgrades the content of this object with an application content.
	 *
	 * @param aApplication The application to process
	 * @param aController The raw controller data
	 *
	 */
	void UpgradeApplicationL(const CApplication& aApplication, const TDesC8& aController);

	/**
	 * Processes a controller in rom to create a registry entry object.
	 *
	 * @param aController 		The controller object
	 * @param aControllerData	The raw controller data
	 *
	 */
	IMPORT_C void ProcessInRomControllerL(const Swi::Sis::CController& aController, const TDesC8& aControllerData);
	
	/**
	 * Provides an array of file descriptions
	 *
	 * @param aFileDescriptions The array of file descriptions.
	 *
	 */
	RPointerArray<CSisRegistryFileDescription>& FileDescriptions();

	/**
	 * Returns the install type for this package
	 *
	 * @return The install type
	 */
	Sis::TInstallType InstallType() const;

	/**
	 * Returns whether or not the package is installed on read-only media
	 *
	 * @return ETrue if any drive used by this package is read-only
	 *         EFalse otherwise
	 */
	TBool InRom() const;

	/**
	 * Returns whether or not the package was pre-installed
	 *
	 * @return ETrue if the package was pre-installed
	 *         EFalse otherwise
	 */
	TBool PreInstalled() const;
	
	/**
	 * Returns whether or not the package was pre-installed but files
	 * should still be deleted on uninstall. This will be true if and only if
	 * the swipolicy indicates that preinstalled files can be deleted, and the
	 * stub sis file used to install the package was writable at install time.
	 *
	 * @return ETrue if the package was pre-installed and files should
	 *               be deleted on uninstall.
	 *         EFalse otherwise
	 */
	TBool DeletablePreInstalled() const;
	
	/**
	 * Returns whether or not the package is removable
	 *
	 * @return ETrue if the package is removable
	 *         EFalse otherwise
	 */	
	TBool IsRemovable() const;

	/**
	 * Trust Timestamp
	 *
	 * @return The time when the trust associated with this package was
     * established
	 * @deprecated Replaced by the TSisTrustStatus member
	 */
	TTime TrustTimeStamp() const;

	/**
	 * Returns the trust level of the package
	 *
	 * @return The trust level
	 * @deprecated Replaced by the TSisTrustStatus member
	 */
	TSisPackageTrust Trust() const;

	/**
	 * Returns whether or not the package was signed
	 *
	 * @return ETrue if the package is signed
	 *         EFalse otherwise
	 */
	TBool IsSigned() const;
	
	
	inline RArray<TInt>& GetSupportedLanguageIdsArray();
	
	inline RPointerArray<HBufC>& GetLocalizedPackageNamesArray();
	inline RPointerArray<HBufC>& GetLocalizedVendorNamesArray();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	/**
	 * Returns whether the object is DRM protected
	 *
	 * @return ETrue if the package is DRM protected
	 *         EFalse otherwise
	 */
	TBool IsDrmProtected() const;

	/**
	 * Returns whether the object is hidden
	 *
	 * @return ETrue if the package is hidden
	 *         EFalse otherwise
	 */
	
	TBool IsHidden() const;
	
#endif	

	/**
	 * Returns the localized vendor name of a package
	 *
	 * @return aVendorlocalizedName Returns the localized name of the vendor
	 *
	 */
	const TDesC& VendorLocalizedName() const;

	/**
     * Given the list of all available certificate chains, the returned array 
     * will contain the indexes into that list, of chains that were used to 
     * verify the object at install time.
     *   
     * @return The indexes of chains used to verify the trustworthiness of the
     * package at install time. 
	 */
	IMPORT_C const RArray<TInt>& InstallChainIndicies() const;

	/**
     * Provides the latest trust status information for the object.
     *   
     * @see TSisTrustStatus
	 * @return The trust status for the registry object
	 */
	IMPORT_C const TSisTrustStatus& TrustStatus() const;
	
	/**
     * Updates the trust status information for the object.
     *   
     * @see TSisTrustStatus
	 * @param status The trust status for the registry object
	 */
	IMPORT_C void SetTrustStatus(const TSisTrustStatus& status);

	/**
	 * Return an array of packages that were embedded within this package.
	 *
	 * @param aDependents The array of embedded packages
	 *
	 */
	IMPORT_C void EmbeddedPackagesL(RPointerArray<CSisRegistryPackage>& aEmbedded) const;
	
	/**
	 * Returns the dependency array for this package (ie UIDs it depends on).
	 * A being "Dependent" on B means that either B is in A's dependency list
	 * or B is in A's EmbeddedPackage list
	 *
	 * @param aDependents The array of dependencies
	 *
	 */
	IMPORT_C void DependsOnL(RPointerArray<CSisRegistryDependency>& aDependencies) const;
	
    /**
	 * Returns the size of an installation
	 * 
	 * @return the sum of all installed files as TInt64
	 */		
    IMPORT_C TInt64 SizeL() const;

	/**
	 * Returns value of a property within a package
	 *
	 * @param aKey The key of the property
	 *
	 * @return the value of the property denoted by the key, 
	 *         KErrNone if value does not exist
	 */
	IMPORT_C TInt Property(TInt aKey) const;
    	
	/**
	 * Returns the description of a registered file  
	 *
	 * @return the description of a registered file 
	 */	
	IMPORT_C const CSisRegistryFileDescription& FileDescriptionL(const TDesC& aFileName) const;

	/**
	 * Returns the description of a registered file  
	 *
	 * @param aSid the uid for the installation package
	 * @return the description of a registered file 
	 */	
	IMPORT_C const CSisRegistryFileDescription& FileDescriptionL(TUid aSid) const;

	/**
	 * Returns whether current/this object depends on a specific registry object
	 * 
	 * @param aObject - a reference to the object the current may depend on  
	 *
	 * @return ETrue if dependency exists EFalse otherwise
	 */		
	IMPORT_C TBool DependsOnObject(const CSisRegistryObject& aObject) const;

	/**
	 * Returns whether this object embeds a specific registry package
	 * 
	 * @param aPackage - a reference to the package the current object may embed  
	 *
	 * @return ETrue if embeds it exists EFalse otherwise
	 */		
    IMPORT_C TBool EmbedsPackage(const CSisRegistryPackage& aPackage) const;
     
     /**
     * Sets the flag indicating whether the package will be uninstalled with the 
     * last dependent
     */
    IMPORT_C void SetRemoveWithLastDependent();

	/**
     * Returns value of the flag indicating whether the package will be uninstalled 
     * with the last dependent
     */
	IMPORT_C TInt RemoveWithLastDependent() const;
    
	/**
     * Returns whether this object is signed by a certificate trusted by the device with System Upgrade flag.
     */
    TBool IsSignedBySuCert() const;
       
    /**
     * Returns the embedded package at a given index.
     *
     * @param aIndex The index to fetch the embedded package from
     * @return The appropriate package, or NULL if the index does not exist
     */
     IMPORT_C CSisRegistryPackage* EmbeddedPackage(TInt aIndex);

    // Determines if the specified install type is a preinstalled type (PA or PP)
	static TBool IsPreinstalled(Sis::TInstallType aInstallType);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions
	friend void ScrHelperUtil::AddComponentL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::TComponentId& aCompId, CSisRegistryObject& aObject, Usif::TScrComponentOperationType aOpType);
	friend void ScrHelperUtil::GetComponentL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject);
	friend void ScrHelperUtil::GetComponentLocalizedInfoL(Usif::RSoftwareComponentRegistry& aScrSession, const Usif::TComponentId aCompId, CSisRegistryObject& aObject);
#endif

private:
	void ConstructL();
	void ConstructL(RReadStream& aReadStream);
	TInt UserSelectedLanguageIndexL(const CApplication& aApplication) const;
	
	void AddFilesL(const CApplication& aApplication);
	void RemoveFilesL(const CApplication& aApplication);
	void UpdateEmbeddedAppsL(const CApplication& aApplication);
    void StorePropertiesL(const Sis::CController& aController);
	void StoreDependenciesL(const Sis::CController& aController);
	
	void UpdateDrivesL();
	void StoreControllerInfoL(const CMessageDigest& aMsgDigest, const Sis::CVersion& aVersion);
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/// Converts file version 1 trust level into the new trust status.
	void ConvertTrustToTrustStatus(const TSisPackageTrust& trust, 
	    TSisTrustStatus& trustStatus) const; 
	
	// Updates the localized vendor name and package name as per the current language on 
	// the device. 
	void UpdateLocalizedInfoL();
#endif
	void PopulateLocalizedPackageAndVendorNamesL(const CApplication& aApplication);
	
	
private:
	HBufC* iVendorLocalizedName;
	Sis::TInstallType iInstallType;
	
	// The UIDs of the packages the entry is dependent on
	RPointerArray<CSisRegistryDependency> iDependencies;
	
	// The UIDs of embedded packages
	RPointerArray<CSisRegistryPackage> iEmbeddedPackages;
	RPointerArray<CSisRegistryProperty> iProperties;
	
	TInt iOwnedFileDescriptions;
	RPointerArray<CSisRegistryFileDescription> iFileDescriptions;

	TBool iInRom; ///< ETrue if the entry is stored in ROM (Z drive)
	TBool iSigned;

	TBool iSignedBySuCert; ////< trust level and upgrade of SA+RU signed by SUCERT

	// ETrue if files were preinstalled but are to be deleted on uninstall.
	TBool iDeletablePreInstalled;
	
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	TUint16 iFileMajorVersion;
	TUint16 iFileMinorVersion;
	#endif
	
	// for future extensions 
	TSisPackageTrust iTrust;        ///< trust value, will be deprecated
	//if a package embeds this package is removed, this flag is set to ETrue
	TInt iRemoveWithLastDependent;
	TTime iTrustTimeStamp;          ///< Timestamp of the trust calculation, 
	                                ///< will be deprecated.
	
	TSisTrustStatus iTrustStatus;  ///< Trust status object indicating SWI
	                                ///< trust level.
	RArray<TInt> iInstallChainIndices;  ///< Set of certificate chains used
	                                    ///< at install time.
	
	// Array of language Id's that are common to the languages in mentioned
	// in SIS file and device supported languages
	RArray<TInt> iSupportedLanguageIds;

	// Localized packagename info in supported languages
	RPointerArray<HBufC> iLocalizedPackageNames;
	
	// Localized vendorname info in supported languages
	RPointerArray<HBufC> iLocalizedVendorNames;
	
	
	// Is the package removable                    
	TBool iIsRemovable;
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Is the package DRM protected
	TBool iIsDrmProtected; 	
	
	// Is the package is hidden for normal users
	TBool iIsHidden;
	#endif
	};
	

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline const TDesC& CSisRegistryObject::VendorLocalizedName() const
	{
	return *iVendorLocalizedName;	
	}
#endif
inline TBool CSisRegistryObject::IsSigned() const
	{
	return iSigned;	
	}	

inline TSisPackageTrust CSisRegistryObject::Trust() const
	{
	return iTrust;
	}

inline TTime CSisRegistryObject::TrustTimeStamp() const
	{
	return iTrustTimeStamp;	
	}

inline TBool CSisRegistryObject::IsRemovable() const
	{
	return iIsRemovable;
	}

inline TBool CSisRegistryObject::IsSignedBySuCert() const
	{
	return 	iSignedBySuCert;
	}

// Determines if the specified install type is a preinstalled type (PA or PP)
inline TBool CSisRegistryObject::IsPreinstalled(Sis::TInstallType aInstallType)
	{
	return (aInstallType == Sis::EInstPreInstalledApp || aInstallType == Sis::EInstPreInstalledPatch)? 
			ETrue:EFalse;
	}	

inline RPointerArray<CSisRegistryFileDescription>& CSisRegistryObject::FileDescriptions()
	{
	return iFileDescriptions;
	}
	
inline Sis::TInstallType CSisRegistryObject::InstallType() const
	{
	return iInstallType;
	}

inline TBool CSisRegistryObject::InRom() const
	{
	return iInRom;
	}

inline TBool CSisRegistryObject::PreInstalled() const
	{
	return IsPreinstalled(iInstallType);
	}
	
inline TBool CSisRegistryObject::DeletablePreInstalled() const
	{
	return iDeletablePreInstalled;
	}
	
inline RPointerArray<HBufC>& CSisRegistryObject::GetLocalizedPackageNamesArray()
	{
	return  iLocalizedPackageNames;
	}

inline RArray<TInt>&  CSisRegistryObject::GetSupportedLanguageIdsArray()
	{
	return  iSupportedLanguageIds;
	}

inline RPointerArray<HBufC>&  CSisRegistryObject::GetLocalizedVendorNamesArray()
	{
	return  iLocalizedVendorNames;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline TBool CSisRegistryObject::IsDrmProtected() const
	{
	return iIsDrmProtected;
	}
	
inline TBool CSisRegistryObject::IsHidden() const
	{
	return iIsHidden;
	}	

#endif	

	} // namespace
#endif
