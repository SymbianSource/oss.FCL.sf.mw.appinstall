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

#ifndef __SISCONTENTS_H__
#define __SISCONTENTS_H__

#include "siscontrollerchecksum.h"
#include "sisdatachecksum.h"
#include "siscontroller.h"
#include "siscompressed.h"
#include "sisdata.h"
#include "version.h"

const unsigned long KUidAppDllDoc16 	= 0x10003A12;
const unsigned long KUidSISXApp 		= 0x10201A7A;
const unsigned long KUidLegacySisFile 	= 0x10000419;
const unsigned short KUidInstallCount 	= 3;

class CSISContents : public CStructure <CSISFieldRoot::ESISContents>
	{
public:
	typedef enum
		{
		EStubNone,
		EStubPreInstalled,
		EStubROM,
		EStubIllegal
		} TStub;
public:
	/**
	 * Default constructor
	 */ 
	CSISContents ();
	
	/**
	 * Parameterised constructor
	 * @param aControllerChecksum 	- Controller checksum
	 * @param aDataChecksum			- Data checksum
	 * @param aController			- List of compressed controllers
	 * @param aData					- CSISData object.
	 */
	CSISContents (	CSISControllerChecksum& 		aControllerChecksum,
					CSISDataChecksum&				aDataChecksum,
					CSISCompressed <CSISController>& aController,
					CSISData&						aData);
	/**
	 * Copy constructor
	 */ 
	CSISContents (const CSISContents& aInitialiser);

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

	/**
	 * This function will read the file and internalize its content. Provided
	 * the file is a valid SIS file. Else this function will throw relevant error.
	 * @param aFileName - SIS file name.
	 * @param size - size of the file to be read.
	 */
	void Load (const std::wstring& aFileName, TUint64* size = NULL);
	
	/**
	 * This function will create a SIS file from the class object.
	 * @param aFileName - SIS file name.
	 */
	void Save (const std::wstring& aFileName) const;
	
	/**
	 * Add a new language to the base controller.
	 * @param aLan - language to be added.
	 */ 
	void AddLanguage (const CSISLanguage::TDialect aLan);
	/**
	 * @return total number of languages present in the base controller.
	 */
	TUint32 LanguageCount () const;
	/**
	 * Checks whether there are any langauge specified for the base controller.
	 * @return true if language is specified else false.
	 */ 
	bool AreLanguagesSpecified () const;
	/**
	 * Adds a new property into the base controller.
	 * @param aKey - key of the property
	 * @param aValue - Value for the corresponding key
	 */
	void AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue);
	/**
	 * @return UID of the SIS file.
	 */
	CSISUid::TUid UID1 () const;
	/**
	 * Set the UID of for the base controller.
	 * @param aUID new UID for the SIS.
	 */
	void SetUID (const CSISUid::TUid aUID);
	/**
	 * Add package name to the existing list of package names for the base controller.
	 * @param aPackageName - package name
	 */ 
	void AddName (const std::wstring& aPackageName);
	/**
	 * Add vendor name to the existing list of vendor names for the base controller.
	 * @param aVendorName vendor name.
	 */
	void AddVendorName (const std::wstring& aVendorName);
	/**
	 * Set the unique vendor name for the base controller.
	 * @param VendorUniqueName unique vendor name.
	 */
	void SetVendorUniqueName (const std::wstring& aVendorUniqueName);
	/**
	 * Set SIS version
	 * @param aVersion new version to be set.
	 */
	void SetVersion (const TVersion& aVersion);
	/**
	 * Each SIS has n number of package dependencies and each package dependency 
	 * has m number of dependency names, where n & m >= 0. 
	 * This function will add the dependency name to the existing list of 
	 * names of the last package dependency.
	 * @param aName new dependency name
	 */
	void AddDependencyName (const std::wstring& aName);
	/**
	 * Add a new package dependency into the list of existing package dependencies.
	 * @param aUID - UID of the package
	 * @param aFrom - From version 
	 * @param aTo - To version
	 */ 
	void AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Each SIS has n number of target dependencies and each target dependency 
	 * has m number of target dependency names, where n & m >= 0. 
	 * This function will add the target dependency name to the existing list of 
	 * names of the last target dependency.
	 * @param aName new target dependency name
	 */
	void AddTargetName (const std::wstring& aName);
	/**
	 * Add a new target dependency into the list of existing tergate dependencies.
	 * @param aUID - UID of the target
	 * @param aFrom - From version 
	 * @param aTo - To version
	 */ 
	void AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo);
	/**
	 * Add new option name to the last supported option.
	 * @param aName option name
	 */
	void AddOptionName (const std::wstring& aName);
	/**
	 * Creates a new object of supported options and pushes it into the existing list.
	 */
	void AddOption ();
	/**
	 * Converts the string into installation option (only if its valid) and sets the
	 * option to last file of base controller's installblock.
	 * @param aOption installation option string.
	 * @return returns the installation option
	 */
	CSISFileDescription::TSISInstOption InterpretOption (const std::wstring& aOption);
	/**
	 * Converts the string into installation type (only if its valid) and sets the
	 * type to base controller.
	 * @param aOption installation type string.
	 * @return returns the installation type.
	 */
	CSISInfo::TSISInstallationType InterpretType (const std::wstring& aOption);
	/**
	 * Only preinstalled apps must be compressed. 
	 * @return false if the sis is preinstalled app or preinstalled patch, else true
	 */
	bool MustRejectCompression () const;
	/**
	 * Set MIME type of the last file of the base controller's install block.
	 * @param aMimeType MIME type
	 */
	void SetMimeType (const std::wstring& aMimeType);
	/**
	 * Add a new CSISIf object to the existing list in the base controller's installblock.
	 */
	void AddIf ();
	/**
	 * @return returns the last CSISIf block present in the base controller's installblock.
	 */
	const CSISIf& If () const;
	/**
	 * @return returns the last CSISIf block present in the base controller's installblock.
	 */
	CSISIf& If ();
	/**
	 * @return install block of the base controller.
	 */
	const CSISInstallBlock& InstallBlock () const;
	/**
	 * @return install block of the base controller.
	 */
	CSISInstallBlock& InstallBlock ();
	/**
	 * Reads the file and load the content into the controller's data unit.
	 * @param aFile filename of the file to be loaded.
	 * @param aSize size of the file to be read.
	 */
	TUint32 LoadFile (const std::wstring& aFile, TUint64* aSize = NULL);
	/**
	 * @return base controller
	 */
	const CSISController& Controller () const;
	/**
	 * Sets the base controller's data index.
	 * @param aIndex new index of the controller
	 */
	void SetDataIndex (const TUint32 aIndex);
	/**
	 * Adds a new data unit into the CSISData.
	 * @param aUnit new data unit object.
	 */
	TUint32 AddDataUnit (const CSISDataUnit& aUnit)	;
	/**
	 * returns the data unit referenced by the index.
	 * @param aIndex index of the data unit to be retrieved.
	 * @return data unit object
	 */
	const CSISDataUnit& DataUnit (const TUint32 aIndex) const;
	/**
	 * returns the data unit referenced by the index.
	 * @param aIndex index of the data unit to be retrieved.
	 * @return data unit object
	 */
	CSISDataUnit& DataUnit (const TUint32 aUnit);
	/**
	 * returns the data unit of the base controller
	 * @return data unit object
	 */
	const CSISDataUnit& DataUnit () const;
	/**
	 * returns the data unit of the base controller
	 * @return data unit object
	 */
	CSISDataUnit& DataUnit ();
	/**
	 * @return data unit index of the base controller
	 */
	TUint32 DataIndex () const;
	/**
	 * Adds a new install
	 * @param new installation flag.
	 */
	void AddInstallFlag (const TUint8 aFlag);
	/**
	 * Sets the logo into the base controller
	 * @param aMime MIME type of the logo file.
	 * @param aTarget destination for the logo file.
	 * @param aFileName file name of the logo file.
	 * @param aDataUnit data unit where the logo file will be added
	 */
	void SetLogo (const std::wstring& aMime , const std::wstring& aTarget, const std::wstring& aFileName, const CSISDataUnit& aDataUnit);
	/**
	 * Writes all the UIDs (UID1, UID2, UID3 and UID checksum) into the file.
	 * @param output Stream where the UIDs needs to be written.
	 */
	void OutputHeaderUids (TSISStream& output) const;
	/**
	 * Embeds the SIS file into the install block passed as argument.
	 * @param aFile - SIS file name
	 * @param aUID - UID of the SIS file.
	 * @param aInstallBlock - install block in which the SIS needs to be embeded.
	 */
	void EmbedFile (const std::wstring& aFile, const CSISUid::TUid& aUid, CSISInstallBlock&  aInstall);
	/**
	 * Embeds the SIS file into the install block passed as argument.
	 * @param aInstallBlock - install block in which the SIS needs to be embeded.
	 * @param aFile - SIS file name
	 * @param aUID - UID of the SIS file.
	 */
	void EmbedPackage (CSISInstallBlock& aInstallBlock, const std::wstring& aFile, const CSISUid::TUid& aUid);
	/**
	 * Generate Controller and Data checksum
	 */
	void PrepareCrcs ();
	/**
	 * Adds a new data unit into the list in CSISData.
	 */ 
	void SetDefaultContent ();
	/**
	 * @return Compressed sie of the last file of base controller's data unit
	 */
	TUint64 CompressedSize () const;
	/**
	 * @return uncompressed sie of the last file of base controller's data unit
	 */
	TUint64 UncompressedSize () const;
	/**
	 * Sets the version information of the SIS
	 * @param aUID uid of the SIS file. This UID will be the new UID of the SIS
	 * @param aVerion this will be the new version of the SIS.
	 * @param aType sets the installation type.
	 * @param aFlags sets the file header option (THeadOpt)
	 */
   	void SetVersionInfo (	const CSISUid::TUid aUID, const TVersion& aVersion,
							const TUint32 aType, const TUint32 aFlags);
   	/**
   	 * Retrieves the Language of the base controller pointed by language index.
   	 * @param aIndex language index
   	 * @return language
   	 */
	CSISLanguage::TLanguage Language (const TUint32 aIndex) const;
	/**
	 * This function will create a SIS file from the class object.
	 * @param aFileName - SIS file name.
	 */
	void WriteSIS (std::wstring aTargetFileName);
	/**
	 * @return CSISData 
	 */ 
	inline const CSISData& SisData() const;
	/**
	 * Controller checksum of the base controller
	 * @return checksum
	 */
	inline const CSISControllerChecksum& ControllerChecksum() const;
	/**
	 * Data checksum of the base controller
	 * @return checksum
	 */
	inline const CSISDataChecksum& DataChecksum() const;
	
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	
	
public:
	// Static member functions

	/**
	 * It calculates the checksum of the first three UIDs.
	 * 
	 * @param aUid1 - UID of the application associated with SIS Files (0x10201A7A)
	 * @param aUid2 - This UIDF is reserved for possible future use.
	 * @param aUid3 - This is the package UID which uniquely identifies a SIS file, except for
	 * 			for the case of upgrades, where both SIS files will share the same UID3.
	 * @return It returns the checksum of the UID structure in its entirety.
	 */
	static CSISUid::TUid UidChecksum (const CSISUid::TUid aUid1, const CSISUid::TUid aUid2, const CSISUid::TUid aUid3);

	/**
	 * Set the stub type if it was not already set.
	 * @param aStub stub type.
	 */
	static void SetStub (const TStub aStub);
	/**
	 * @return the stub type. returns EStubNone if the sis is not a stub.
	 */
	static CSISContents::TStub StubType();
	/**
	 * Set the stub type regardless of the previous state.
	 * @param aStub stub type.
	 */
	static void ForceSetStub(TStub aStub);
	/**
	 * Checks if the sis is a stub or not.
	 * @return true if the sis is a stub else false.
	 */
	static bool IsStub();
	/**
	 * Checks if the sis is preinstalled or not.
	 * @return true if the sis is a preinstalled app.
	 */
	static bool IsPreInstalledApp();
	/**
	 * By inspecting the file header it checks whether the sis is a SIS file or not.
	 * @param aFileName sis file name to inspect
	 * @return true if its a sis file else false. 
	 */
	static bool IsSisFile(const std::wstring& aFileName);
	/**
	 * By inspecting the file header it checks whether the sis is a SIS file or not.
	 * @param aStream stream which represents the SIS file. 
	 * @return true if its a sis file else false. 
	 */
	static bool IsSisFile(TSISStream& aStream);

private:
	void InsertMembers ();
	static TCRC UIDCheck (const void *pPtr);

private:
	// if you change these, modify ReRead
	CSISControllerChecksum			iControllerChecksum;
	CSISDataChecksum				iDataChecksum;
	CSISCompressed <CSISController>	iController;
	CSISData						iData;
	static TStub					iStub;
	};


inline CSISContents::CSISContents () :
		iControllerChecksum (false), 
		iDataChecksum (false)
	{ 
	InsertMembers (); 
	}

inline CSISContents::CSISContents ( CSISControllerChecksum& aControllerChecksum,
						CSISDataChecksum&	aDataChecksum,
							CSISCompressed <CSISController>& aController,
								CSISData&	aData):
									iControllerChecksum(aControllerChecksum),
									iDataChecksum(aDataChecksum),
									iController(aController),
									iData(aData)
	{
	InsertMembers ();
	}


inline void CSISContents::AddLanguage (const CSISLanguage::TDialect aLan)		
	{ 
	iController.Content ().AddLanguage (aLan); 
	}


inline TUint32 CSISContents::LanguageCount () const								
	{ 
	return iController.Content ().LanguageCount (); 
	}


inline bool CSISContents::AreLanguagesSpecified () const							
	{ 
	return iController.Content ().AreLanguagesSpecified (); 
	}


inline void CSISContents::AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue)
	{ 
	iController.Content ().AddProperty (aKey, aValue); 
	}


inline CSISUid::TUid CSISContents::UID1 () const									
	{ 
	return iController.Content ().UID1 (); 
	}


inline void CSISContents::SetUID (const CSISUid::TUid aUID)						
	{ 
	iController.Content ().SetUID (aUID); 
	}


inline void CSISContents::AddName (const std::wstring& aLan)						
	{ 
	iController.Content ().AddName (aLan); 
	}


inline void CSISContents::AddVendorName (const std::wstring& aLan)				
	{ 
	iController.Content ().AddVendorName (aLan); 
	}


inline void CSISContents::SetVersion (const TVersion& aVersion)
	{ 
	iController.Content ().SetVersion (aVersion); 
	}


inline void CSISContents::AddDependencyName (const std::wstring& aName)		
	{ 
	iController.Content ().AddDependencyName (aName); 
	}


inline void CSISContents::AddTargetName (const std::wstring& aName)				
	{ 
	iController.Content ().AddTargetName (aName); 
	}


inline void CSISContents::AddOptionName (const std::wstring& aName)				
	{ 
	iController.Content ().AddOptionName (aName); 
	}


inline void CSISContents::AddOption ()											
	{ 
	iController.Content ().AddOption (); 
	}


inline CSISFileDescription::TSISInstOption CSISContents::InterpretOption (const std::wstring& aOption)
	{ 
	return iController.Content ().InterpretOption (aOption); 
	}


inline CSISInfo::TSISInstallationType CSISContents::InterpretType (const std::wstring& aOption)
	{
	return iController.Content ().InterpretType (aOption); 
	}


inline bool CSISContents::MustRejectCompression () const
	{
	return iController.Content ().MustRejectCompression (); 
	}


inline void CSISContents::SetMimeType (const std::wstring& aMimeType)			
	{ 
	iController.Content ().SetMimeType (aMimeType); 
	}


inline void CSISContents::AddIf ()											
	{ 
	iController.Content ().AddIf (); 
	}


inline const CSISIf& CSISContents::If () const								
	{ 
	return iController.Content ().If (); 
	}


inline CSISIf& CSISContents::If ()											
	{ 
	return iController.Content ().If (); 
	}


inline const CSISInstallBlock& CSISContents::InstallBlock () const			
	{ 
	return iController.Content ().InstallBlock (); 
	}


inline CSISInstallBlock& CSISContents::InstallBlock ()							
	{ 
	return iController.Content ().InstallBlock (); 
	}


inline TUint32 CSISContents::LoadFile (const std::wstring& aFile, TUint64* aSize)			
	{ 
	return DataUnit ().LoadFile (aFile, aSize); 
	}


inline const CSISController& CSISContents::Controller () const				
	{ 
	return iController.Content (); 
	}


inline void CSISContents::SetDataIndex (const TUint32 aIndex)					
	{ 
	iController.Content ().SetDataIndex (aIndex); 
	}


inline TUint32 CSISContents::AddDataUnit (const CSISDataUnit& aUnit)			
	{ 
	return iData.AddDataUnit (aUnit); 
	}


inline TUint32 CSISContents::DataIndex () const									
	{ 
	return iController.Content ().DataIndex (); 
	}


inline const CSISDataUnit& CSISContents::DataUnit (const TUint32 aIndex) const 
	{ 
	return iData.DataUnit (aIndex); 
	}


inline CSISDataUnit& CSISContents::DataUnit (const TUint32 aIndex) 
	{ 
	return iData.DataUnit (aIndex); 
	}


inline const CSISDataUnit& CSISContents::DataUnit () const 
	{ 
	return DataUnit (DataIndex ()); 
	}


inline CSISDataUnit& CSISContents::DataUnit () 
	{ 
	return DataUnit (DataIndex ()); 
	}



inline void CSISContents::AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iController.Content ().AddDependency (aUID, aFrom, aTo);
	}

inline void CSISContents::AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iController.Content ().AddTarget (aUID, aFrom, aTo);
	}

inline void CSISContents::SetLogo (const std::wstring& aMime, const std::wstring& aTarget, const std::wstring& aFileName, const CSISDataUnit& aDataUnit)
	{
	TUint64 size = 0;
	const TUint32 fileIndex = LoadFile (aFileName, &size);
	iController.Content ().SetLogoSizes (CompressedSize(), size); 
	iController.Content ().SetLogo (aMime, aTarget, fileIndex, aDataUnit);
	}

inline std::string CSISContents::Name () const
	{
	return "Content";
	}

inline void CSISContents::SetDefaultContent ()
	{
	iData.SetDefaultContent ();
	}


inline TUint64 CSISContents::CompressedSize () const
	{
	return DataUnit ().CompressedSize ();
	}

inline TUint64 CSISContents::UncompressedSize () const
	{
	return DataUnit ().UncompressedSize ();
	}

inline void CSISContents::SetVersionInfo (	const CSISUid::TUid aUID, const TVersion& aVersion,
											const TUint32 aType, const TUint32 aFlags)
	{
 	SetUID (aUID);
	SetVersion (aVersion);
	CSISFieldRoot::SetHeaderOption (static_cast <CSISFieldRoot::THeadOpt> (aFlags));
	if (	CSISFieldRoot::IsDebugOptionSet (CSISFieldRoot::EDbgCompress) &&
			MustRejectCompression ())
		{
		SISLogger::Log(L"Compression ignored.\n");
		}
	}

inline void CSISContents::SetVendorUniqueName (const std::wstring& aVendorUniqueName)
	{
	iController.Content ().SetVendorUniqueName (aVendorUniqueName);
	}

inline CSISLanguage::TLanguage CSISContents::Language (const TUint32 aIndex) const
	{
	return iController.Content ().Language (aIndex);
	}

inline void CSISContents::SetStub (const TStub aStub)
	{
	if (iStub == EStubNone)
		{
		iStub = aStub;
		}
	}

inline CSISContents::TStub CSISContents::StubType()
	{
	return iStub;
	}

inline void CSISContents::ForceSetStub (const TStub aStub)
	{
	iStub = aStub;
	}
	
inline bool CSISContents::IsStub()
	{
	if (iStub == EStubROM)
		{
		return true;	
		}
	else
		{	
		return false;
		}
	}

inline bool CSISContents::IsPreInstalledApp()
	{
	if (iStub == EStubPreInstalled)
		{
		return true;	
		}
	else
		{	
		return false;
		}
	}

inline void CSISContents::AddInstallFlag(const TUint8 aFlag)
	{
	iController.Content ().AddInstallFlag(aFlag);
	}

inline const CSISData& CSISContents::SisData() const
	{
	return iData;
	}

inline const CSISControllerChecksum& CSISContents::ControllerChecksum() const
	{
	return	iControllerChecksum;
	}

inline const CSISDataChecksum& CSISContents::DataChecksum() const
	{
	return iDataChecksum;
	}


#endif // __SISCONTENTS_H__

