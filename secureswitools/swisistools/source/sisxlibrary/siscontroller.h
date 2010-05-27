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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISCONTROLLER_H__
#define __SISCONTROLLER_H__


#include "sisinfo.h"
#include "sissupportedoptions.h"
#include "sissupportedlanguages.h"
#include "sisprerequisites.h"
#include "sisproperties.h"
#include "sislogo.h"
#include "sisinstallblock.h"
#include "signaturecertchaindata.h"
#include "sisdataindex.h"
#include "sequence.h"
#include "version.h"


class CSISController : public CStructure <CSISFieldRoot::ESISController>
	{
public:
	typedef CSequence <CSignatureCertChainData, CSISFieldRoot::ESISSignatureCertificateChain> SignatureCertChainList;

public:
	/**
	 * Default controller
	 */
	CSISController ();
	/**
	 * Copy constructor
	 */
	CSISController (const CSISController& aInitialiser);
	
public:
	/**
	 * This function verifies the structure
	 * @param aLanguage - language
	 */ 
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Reads the file and internalize the content
	 * @param aFile Stream which contains the controller.
	 * @param aContainerSize controller size.
	 * @param aArrayType type of array. ESISUndefined if it is not an array.
	 */ 
	virtual void Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType = CSISFieldRoot::ESISUndefined);
	/**
	 * Write the content of the controller into the file.
	 * @param aFile Stream where the content needs to be written.
	 * @param aIsArrayElement bool which checks if the controller is part of an array.
	 */
	virtual void Write (TSISStream& aFile, const bool aIsArrayElement) const;
	/**
	 * @return name of the class.
	 */
	virtual std::string Name () const;
	/**
	Loads the Stub SIS Controller file. This function also checks if the file is a valid 
	Stub SIS Controller. 
	To ensure that the file reading should start from the length field of controller block 
	in the  read default type parameter is passed as CSISFieldRoot::ESISController

	@param aFileName	Stub SIS Controller file name
	*/
	void Load (const std::wstring& aFileName);
	/**
	 * Add a new language to the controller.
	 * @param aLan - language to be added.
	 */ 
	void AddLanguage (const CSISLanguage::TDialect aLan);
	/**
	 * @return total number of languages present in the controller.
	 */
	TUint32 LanguageCount () const;
	/**
	 * Checks whether there are any langauge specified for the controller.
	 * @return true if language is specified else false.
	 */ 
	bool AreLanguagesSpecified () const;
	/**
	 * Adds a new property into the controller.
	 * @param aKey - key of the property
	 * @param aValue - Value for the corresponding key
	 */
	void AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue);
	/**
	 * @return UID of the SIS file.
	 */
	CSISUid::TUid UID1 () const;
	/**
	 * Set the UID for the controller.
	 * @param aUID new UID for the SIS.
	 */
	void SetUID (const CSISUid::TUid aUID);
	/**
	 * Add package name to the existing list of package names for the controller.
	 * @param aPackageName - package name
	 */ 
	void AddName (const std::wstring& aLan)	;
	/**
	 * Add vendor name to the existing list of vendor names for the controller.
	 * @param aVendorName vendor name.
	 */
	void AddVendorName (const std::wstring& aLan);
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
	 * Adds a new install
	 * @param new installation flag.
	 */
	void AddInstallFlag(const TUint8 aFlag);
	/**
	 * Converts the string into installation option (only if its valid) and sets the
	 * option to last file of controller's installblock.
	 * @param aOption installation option string.
	 * @return returns the installation option
	 */
	CSISFileDescription::TSISInstOption InterpretOption (const std::wstring& aOption);
	/**
	 * Converts the string into installation type (only if its valid) and sets the
	 * type to controller.
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
	 * Add a new CSISIf object to the existing list in the controller's installblock.
	 */
	void AddIf ();
	/**
	 * @return returns the last CSISIf block present in the controller's installblock.
	 */
	const CSISIf& If () const;
	/**
	 * @return returns the last CSISIf block present in the controller's installblock.
	 */
	CSISIf& If ();
	/**
	 * @return install block of the controller.
	 */
	const CSISInstallBlock& InstallBlock () const;
	/**
	 * @return install block of the controller.
	 */
	CSISInstallBlock& InstallBlock ();
	/**
	 * Sets the controller's data index.
	 * @param aIndex new index of the controller
	 */
	void SetDataIndex (const TUint32 aIndex);
	/**
	 * @return data unit index of the controller
	 */
	TUint32 DataIndex () const;
	/**
	 * Sets the logo into the base controller
	 * @param aMime MIME type of the logo file.
	 * @param aTarget destination for the logo file.
	 * @param aFileName file name of the logo file.
	 * @param aDataUnit data unit where the logo file will be added
	 */
	void SetLogo (const std::wstring& aMime, const std::wstring& aTarget, const TUint32 aFileIndex, const CSISDataUnit& aDataUnit);
	/**
	 * Retrieves the instance of logo class
	 */
	inline const CSISLogo& Logo() const;
	/**
	 * Embeds the SIS file into the controller.
	 * @param aEmbed Controller to be embedded
	 * @param aIndex reserved for future use
	 */
	void Embed (class CSISController& aEmbed, const TUint32 aIndex);
	/**
	 * Set the compressed and uncompressed sizes of the logo file.
	 * @param aCompressedSize compressed size.
	 * @param aUncompressedSize uncompressed size.
	 */
	void SetLogoSizes (const TUint64 aCompressedSize, const TUint64 aUncompressedSize);
   	/**
   	 * Retrieves the Language of the controller pointed by language index.
   	 * @param aIndex language index
   	 * @return language
   	 */
	CSISLanguage::TLanguage Language (const TUint32 aIndex) const;
	/**
	 * Set the current time as the sis creation time.
	 */
	void SetNow ();
	/**
	 * Removes the most recent signature from the controller.
	 */
	void RemoveSignature ();
	/**
	 * Gets the signature count from the controller.
	 * @return signature count.
	 */
	inline int	SignatureCount() const;
	/**
	 * Returns the buffer containing controller data.
	 * @return raw controller buffer.
	 */
	const TUint8* RawBuffer () const;
	/**
	 * Buffer size which is actually read. Doesn't include data whose reading is skipped.
	 * @return buffer size.
	 */
	TUint64 BufferSize () const;
	/**
	 * Raw buffer size. The entire controller size.
	 * @return buffer size.
	 */
	TUint64 RawBufferSize () const;
	/**
	 * For calculating controller hash signature and data unit index is skipped.
	 * This function returns the length of the controller minus signature and data
	 * unit length.
	 * @return buffer size.
	 */
	TUint64 ControllerSizeForHash() const;
	/**
	 * Add a new signature chain into the controller.
	 * @param aSISSigCertChain signature chain.
	 */
	void AddSignatureChain(CSignatureCertChainData& aSISSigCertChain);
	/**
	 * Adds package entry related to this controller.
	 * @param aStream stream into which the package details need to be written.
	 * @param aVerbose If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Retrieves the CSignatureCertChainData pointed by chain index.
	 * @param aIndex index of the chain in the controller.
	 * @return CSignatureCertChainData object (signature and cert chain container)
	 */
	inline const CSignatureCertChainData& SignatureCertChain(TUint32 aIndex) const;
	/**
	 * Prerequisites for this controller
	 * @return CSISPrerequisites
	 */
	inline const CSISPrerequisites& Prerequisites() const;
	/**
	 * Properties of the controller class.
	 * @return CSISProperties
	 */
	inline const CSISProperties& Properties() const;
	/**
	 * Retrieves the CSISInfo class which holds the basic information of the sis file.
	 * @return CSISInfo
	 */
	inline const CSISInfo& SISInfo() const;
	/**
	 * Retrieves the CSISLogo class which holds the log information.
	 * @return CSISLogo
	 */
	inline const CSISLogo& SISLogo() const;
	/**
	 * Supported option count.
	 * @return option count.
	 */
	inline TUint32 SupportedOptionCount() const;
	/**
	 * Retrieve controller ID
	 */
	inline int GetControllerID() const;

	/**
	 * If this function is called then it will skip reading of some class members
	 */
	static void SetSkip ();
	

private:
	void InsertMembers ();
	int DeleteDirectory (std::string& aDirectory);

private:
	// change these ? change Read and Write (for SignSIS) :
	CSISInfo				iInfo;
	CSISSupportedOptions	iSupportedOptions;
	CSISSupportedLanguages	iSupportedLanguages;
	CSISPrerequisites		iPrerequisites;
	CSISProperties			iProperties;
	CSISLogo				iLogo;
	CSISInstallBlock		iInstallBlock;
	SignatureCertChainList	iSignatures;
	CSISDataIndex			iDataIndex;
	TUint8*					iRawBuffer;
	TUint64					iRawBufferSize;
	TUint64					iBufferSize;
	/**
	 * Used distinguish between other embeded controllers.
	 * Base controllers ID will be -1 and embedded controllers
	 * ID starts with 0
	 */
	int						iControllerID; 
	static int				iLastCtlID; // Last allocated controller's ID.
	static bool				iSkip;
	};



inline CSISController::CSISController () :
		iRawBuffer (NULL),
		iRawBufferSize(0),
		iBufferSize (0),
		iLogo (false)
	{
	InsertMembers ();
	}


inline void CSISController::AddLanguage (const CSISLanguage::TDialect aLan)	
	{ 
	iSupportedLanguages.AddLanguage (aLan); 
	}


inline TUint32 CSISController::LanguageCount () const						
	{ 
	return iSupportedLanguages.LanguageCount (); 
	}


inline bool CSISController::AreLanguagesSpecified () const					
	{ 
	return iSupportedLanguages.AreLanguagesSpecified (); 
	}


inline void CSISController::AddProperty (const CSISProperty::TKey aKey, const CSISProperty::TValue aValue)
	{ 
	iProperties.AddProperty (aKey, aValue); 
	}


inline CSISUid::TUid CSISController::UID1 () const								
	{ 
	return iInfo.UID1 (); 
	}


inline void CSISController::SetUID (const CSISUid::TUid aUID)					
	{ 
	iInfo.SetUID (aUID); 
	}


inline void CSISController::AddName (const std::wstring& aLan)					
	{ 
	iInfo.AddName (aLan); 
	}


inline void CSISController::AddVendorName (const std::wstring& aLan)				
	{ 
	iInfo.AddVendorName (aLan); 
	}


inline void CSISController::SetVersion (const TVersion& aVersion)
	{ 
	iInfo.SetVersion (aVersion); 
	}


inline void CSISController::AddDependencyName (const std::wstring& aName)			
	{ 
	iPrerequisites.AddDependencyName (aName); 
	}


inline void CSISController::AddTargetName (const std::wstring& aName)				
	{ 
	iPrerequisites.AddTargetName (aName); 
	}


inline void CSISController::AddOptionName (const std::wstring& aName)				
	{ 
	iSupportedOptions.AddOptionName (aName); 
	}


inline void CSISController::AddOption ()										
	{ 
	iSupportedOptions.AddOption (); 
	}


inline CSISFileDescription::TSISInstOption CSISController::InterpretOption (const std::wstring& aOption)
	{ 
	return iInstallBlock.InterpretOption (aOption); 
	}


inline CSISInfo::TSISInstallationType CSISController::InterpretType (const std::wstring& aOption)
	{
	return iInfo.InterpretType (aOption); 
	}


inline bool CSISController::MustRejectCompression () const
	{
	return iInfo.MustRejectCompression (); 
	}


inline void CSISController::SetMimeType (const std::wstring& aMimeType)			
	{ 
	iInstallBlock.SetMimeType (aMimeType); 
	}


inline void CSISController::AddIf ()												
	{ 
	iInstallBlock.AddIf (); 
	}


inline const CSISIf& CSISController::If () const									
	{ 
	return iInstallBlock.If (); 
	}

inline CSISIf& CSISController::If ()												
	{ 
	return iInstallBlock.If (); 
	}


inline const CSISInstallBlock& CSISController::InstallBlock () const				
	{ 
	return iInstallBlock; 
	}


inline CSISInstallBlock& CSISController::InstallBlock ()							
	{ 
	return iInstallBlock; 
	}


inline void CSISController::SetDataIndex (const TUint32 aIndex)					
	{ 
	iDataIndex.SetIndex (aIndex);
	}


inline TUint32 CSISController::DataIndex () const									
	{ 
	return iDataIndex.Index ();
	}


inline void CSISController::SetLogo (const std::wstring& aMime, const std::wstring& aTarget, const TUint32 aFileIndex, const CSISDataUnit& aDataUnit)
	{
	iLogo.Set (aMime, aTarget, aFileIndex, aDataUnit); 
	}

inline const CSISLogo& CSISController::Logo() const
	{
	return iLogo;
	}


inline void CSISController::AddDependency (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iPrerequisites.AddDependency (aUID, aFrom, aTo);
	}

inline void CSISController::AddTarget (const CSISUid::TUid aUID, const TVersion& aFrom, const TVersion& aTo)
	{
	iPrerequisites.AddTarget (aUID, aFrom, aTo);
	}

inline void CSISController::Embed (class CSISController& aEmbed, const TUint32 aOffset)
	{
	iInstallBlock.Embed (aEmbed, aOffset);
	}

inline void CSISController::SetLogoSizes (const TUint64 aCompressedSize, const TUint64 aUncompressedSize)
	{
	iLogo.SetSizes (aCompressedSize, aUncompressedSize);
	}

inline void CSISController::Verify (const TUint32 aLanguages) const
	{
	CStructure <CSISFieldRoot::ESISController>::Verify (LanguageCount ());
#if defined (SIGNSIS)
	for (int index = 0; index < iSignatures.size (); index++)
		{
		
		iSignatures [index].VerifySignature (this, PostHeaderPos ());
			
		}
#endif // SIGNSIS
	}

inline void CSISController::SetVendorUniqueName (const std::wstring& aVendorUniqueName)
	{
	iInfo.SetVendorUniqueName (aVendorUniqueName);
	}

inline CSISLanguage::TLanguage CSISController::Language (const TUint32 aIndex) const
	{
	return iSupportedLanguages.Language (aIndex);
	}

inline std::string CSISController::Name () const
	{
	return "Controller";
	}

inline void CSISController::SetSkip ()
	{
	iSkip = true;
	}

inline const TUint8* CSISController::RawBuffer () const
	{
	assert (iRawBuffer);
	return iRawBuffer;
	}

inline TUint64 CSISController::BufferSize () const
	{
	assert (iBufferSize);
	return iBufferSize;
	}

inline TUint64 CSISController::RawBufferSize () const
	{
	assert (iRawBufferSize);
	return iRawBufferSize;
	}

inline void CSISController::AddInstallFlag(const TUint8 aFlag)
	{
	iInfo.AddFlag(aFlag);
	}

inline void CSISController::SetNow ()
	{
	iInfo.SetNow();
	}

inline int CSISController::SignatureCount () const
	{
	return iSignatures.size();
	}

inline const CSignatureCertChainData& CSISController::SignatureCertChain(TUint32 aIndex) const
	{
	assert(aIndex < iSignatures.size());
	return iSignatures[aIndex];
	}

inline const CSISPrerequisites& CSISController::Prerequisites() const
	{
	return iPrerequisites;
	}

inline const CSISProperties& CSISController::Properties() const
	{
	return iProperties;
	}

inline const CSISInfo& CSISController::SISInfo() const
	{
	return iInfo;
	}

inline const CSISLogo& CSISController::SISLogo() const
	{
	return iLogo;
	}

inline TUint32 CSISController::SupportedOptionCount() const
	{
	return iSupportedOptions.SupportedOptionCount();
	}

inline int CSISController::GetControllerID() const
	{
	return iControllerID;
	}

#endif // __SISCONTROLLER_H__

