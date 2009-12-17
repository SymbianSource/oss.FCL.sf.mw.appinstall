/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef __SISINFO_H__
#define __SISINFO_H__


#include "stringarray.h"
#include "sisuid.h"
#include "sisversion.h"
#include "sisdatetime.h"
#include "version.h"

class CSISInfo : public CStructure <CSISFieldRoot::ESISInfo>
	{
public:
	/**
	 * Installatation type enumeration
	 */
	typedef enum 
		{
		EInstInstallation,
		EInstAugmentation,
		EInstPartialUpgrade,
		EInstPreInstalledApp,
		EInstPreInstalledPatch,
		EInstIllegal
		} TSISInstallationType;

	/**
	 * Installation flags. Considered to be of 8 bit length, and 
	 * bitwise OR-ed to give more than one option. So the values
	 * must be of 2 power n, where n => 0 to 7.
	 */
	typedef enum
		{
		EInstFlagShutdownApps	= 1<<0,
		EInstFlagNonRemovable	= 1<<1,
		EInstFlagROMUpgrade		= 1<<2,
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		EInstFlagHide			= 1<<3
		#endif
		} TInstallFlags;
		
public:
	/**
	 * Default constructor
	 */
	CSISInfo ();
	/**
	 * Copy constructor
	 */
	CSISInfo (const CSISInfo& aInitialiser);
	
public:
	/**
	 * This function verifies the structure
	 * @param aLanguage - language
	 */ 
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * @return name of the class.
	 */
	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	/**
	 * @return UID of the SIS file.
	 */
	CSISUid::TUid UID1 () const;
	/**
	 * Set the UID of the SIS.
	 * @param aUID new UID for the SIS.
	 */
	void SetUID (const CSISUid::TUid aUID);
	/**
	 * Add package name to the existing list of package names for the SIS.
	 * @param aPackageName - package name
	 */ 
	void AddName (const std::wstring& aLan);
	/**
	 * Add vendor name to the existing list of vendor names for the controller.
	 * @param aVendorName vendor name.
	 */
	void AddVendorName (const std::wstring& aVendor);
	/**
	 * Set the unique vendor name for the controller.
	 * @param VendorUniqueName unique vendor name.
	 */
	void SetVendorUniqueName (const std::wstring& aVendorUniqueName);
	/**
	 * Set SIS version
	 * @param aVersion new version to be set.
	 */
	void SetVersion (const TVersion& aVersion);
	/**
	 * @return UID of the SIS file.
	 */
	const CSISUid& uid () const;
	/**
	 * @return total number of languages present in the controller.
	 */
	TUint32 LanguageCount () const;
	/**
	 * Converts the string into installation type (only if its valid) and sets the
	 * type to controller.
	 * @param aOption installation type string.
	 * @return returns the installation type.
	 */
	TSISInstallationType InterpretType (const std::wstring& aOption);
	/**
	 * Only preinstalled apps must be compressed. 
	 * @return false if the sis is preinstalled app or preinstalled patch, else true
	 */
	bool MustRejectCompression () const;
	/**
	 * Adds a new install
	 * @param aFlag new installation flag.
	 */
	void AddFlag(const TUint8 aFlag);
	/**
	 * Set the current time as the sis creation time.
	 */
	void SetNow ();
	/**
	 * Adds package entry related to sis info.
	 * @param aStream stream into which the package details need to be written.
	 * @param aVerbose If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * @return Vendor Name Count
	 */
	inline TUint32 VendorNameCount() const;
	/**
	 * @return Package Name Count
	 */
	inline TUint32 PackageNameCount() const;
	/**
	 * Retrieves the vendor name pointed by index.
	 * @param index index of the vendor name.
	 * @return vendor Name 
	 */
	inline std::wstring VendorName(TUint32 index = 0) const;
	/**
	 * @return UNique vendor Name 
	 */
	inline std::wstring UniqueVendorName() const;
	/**
	 * Retrieves the package name pointed by index.
	 * @param index index of the package name.
	 * @return Package Name 
	 */
	inline std::wstring PackageName(TUint32 index = 0) const;
	/**
	 * @return installation type.
	 */
	inline TSISInstallationType InstallationType() const;
	/**
	 * @return Installation flag
	 */
	inline TInstallFlags InstallationFlag() const;
	/**
	 * @return version of the sis.
	 */
	inline const CSISVersion& SISVersion() const;

private:
	void InsertMembers ();

private:
	CSISUid				iUid;
	CSISString			iVendorUniqueName;
	CSISArrayOfStrings	iNames;
	CSISArrayOfStrings	iVendorNames;
	CSISVersion			iVersion;
	CSISDateTime		iCreationTime;
	CSISUInt8			iInstallType;
	CSISUInt8			iInstallFlags;
	};


inline CSISInfo::CSISInfo ()
	{
	InsertMembers ();
	}


inline CSISUid::TUid CSISInfo::UID1 () const
	{ 
	return iUid.UID1 (); 
	}


inline void CSISInfo::SetUID (const CSISUid::TUid aUID)
	{ 
	iUid.SetUID (aUID); 
	}


inline void CSISInfo::AddName (const std::wstring& aLan)
	{ 
	iNames.Push (CSISString (aLan)); 
	}


inline void CSISInfo::AddVendorName (const std::wstring& aVendor)
	{ 
	iVendorNames.Push (CSISString (aVendor)); 
	}


inline void CSISInfo::SetVersion (const TVersion& aVersion)
	{ 
	iVersion.SetVersion (aVersion); 
	}


inline const CSISUid& CSISInfo::uid () const
	{ 
	return iUid; 
	}


inline TUint32 CSISInfo::LanguageCount () const
	{ 
	return iNames.size (); 
	}

#ifdef GENERATE_ERRORS
inline void CSISInfo::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iInstallType = rand ();
		}
	}
#endif // GENERATE_ERRORS

inline void CSISInfo::SetVendorUniqueName (const std::wstring& aVendorUniqueName)
	{
	iVendorUniqueName = aVendorUniqueName;
	}

inline bool  CSISInfo::MustRejectCompression () const
	{
	return ((iInstallType != EInstPreInstalledApp) && (iInstallType != EInstPreInstalledPatch));
	}

inline void CSISInfo::AddFlag(const TUint8 aFlag)
	{
	iInstallFlags |= aFlag;
	}

inline void CSISInfo::SetNow ()
	{
	iCreationTime.SetNow ();
	}

inline std::wstring CSISInfo::VendorName(TUint32 index) const
	{
	assert(iVendorNames.size() > index);
	return iVendorNames[index].GetString();
	}

inline std::wstring CSISInfo::UniqueVendorName() const
	{
	return iVendorUniqueName.GetString();
	}

inline std::wstring CSISInfo::PackageName(TUint32 index) const
	{
	assert(iNames.size() > index);
	return iNames[index].GetString();
	}

inline CSISInfo::TSISInstallationType CSISInfo::InstallationType() const
	{
	return (TSISInstallationType)(int)iInstallType;
	}

inline CSISInfo::TInstallFlags CSISInfo::InstallationFlag() const
	{
	return (TInstallFlags)(int)iInstallFlags;
	}

inline const CSISVersion& CSISInfo::SISVersion() const
	{
	return iVersion;
	}

inline TUint32 CSISInfo::VendorNameCount() const
	{
	return iVendorNames.size();
	}

inline TUint32 CSISInfo::PackageNameCount() const
	{
	return iNames.size();
	}

#endif // __SISINFO_H__

